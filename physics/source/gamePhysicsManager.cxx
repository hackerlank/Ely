/**
 * \file gamePhysicsManager.cxx
 *
 * \date 2016-10-09
 * \author consultit
 */

#include "asyncTaskManager.h"
#include "bamFile.h"
#include "nodePathCollection.h"
#include "bulletSphereShape.h"
#include "bulletPlaneShape.h"
#include "bulletBoxShape.h"
#include "bulletCylinderShape.h"
#include "bulletCapsuleShape.h"
#include "bulletConeShape.h"
#include "bulletHeightfieldShape.h"
#include "bulletTriangleMesh.h"
#include "bulletTriangleMeshShape.h"
#include "gamePhysicsManager.h"
#include "throw_event.h"

#include "btRigidBody.h"
//#include "btSoftBody.h"
//#include "btGhost.h"

/**
 *
 */
GamePhysicsManager::GamePhysicsManager(int taskSort, const NodePath& root,
		const CollideMask& mask):
		mTaskSort(taskSort),
		mReferenceNP(NodePath("ReferenceNode")),
		mMask(mask),
		mRef(0)
{
	PRINT_DEBUG("GamePhysicsManager::GamePhysicsManager: creating the singleton manager.");

	mRigidBodies.clear();
	mRigidBodiesParameterTable.clear();
//	mSoftBodies.clear(); xxx
//	mSoftBodiesParameterTable.clear();
//	mGhosts.clear();
//	mGhostsParameterTable.clear();
	set_parameters_defaults(RIGIDBODY);
	set_parameters_defaults(SOFTBODY);
	set_parameters_defaults(GHOST);
	//
	mUpdateData.clear();
	mUpdateTask.clear();
	//
	mBulletWorld = new BulletWorld();
	mBulletWorld->set_gravity(0.0, 0.0, -9.81);
	//set default collision notify data
	mCollisionNotify.mEnable = false;
	mCollisionNotify.mFrequency = 30.0;
	//clear the colliding pair set
	mCollidingNodePairs.clear();
	//get a reference to collision dispatcher (for collision management)
	mCollisionDispatcher = static_cast<btCollisionDispatcher*>(mBulletWorld->get_dispatcher());
#ifdef ELY_DEBUG
	// set up Bullet Debug Renderer (disabled by default)
	mBulletDebugNodePath = NodePath(new BulletDebugNode("BulletDebugNP"));
#endif
}

/**
 *
 */
GamePhysicsManager::~GamePhysicsManager()
{
	PRINT_DEBUG("GamePhysicsManager::~GamePhysicsManager: destroying the singleton manager.");

	//stop any default update
	stop_default_update();
	{
		//destroy all BTRigidBodys
		PTA(PT(BTRigidBody))::iterator iterA = mRigidBodies.begin();
		while (iterA != mRigidBodies.end())
		{
			//\see http://stackoverflow.com/questions/596162/can-you-remove-elements-from-a-stdlist-while-iterating-through-it
			//give a chance to BTRigidBody to cleanup itself before being destroyed.
			(*iterA)->do_finalize();
			//remove the BTRigidBodys from the inner list (&& from the update task)
			iterA = mRigidBodies.erase(iterA);
		}

//		//destroy all BTSoftBodys xxx
//		PTA(PT(BTSoftBody))::iterator iterB = mSoftBodies.begin();
//		while (iterB != mSoftBodies.end())
//		{
//			//\see http://stackoverflow.com/questions/596162/can-you-remove-elements-from-a-stdlist-while-iterating-through-it
//			//give a chance to BTSoftBody to cleanup itself before being destroyed.
//			(*iterB)->do_finalize();
//			//remove the BTSoftBodys from the inner list (&& from the update task)
//			iterB = mSoftBodies.erase(iterB);
//		}

//		//destroy all BTGhosts xxx
//		PTA(PT(BTGhost))::iterator iterC = mGhosts.begin();
//		while (iterC != mGhosts.end())
//		{
//			//\see http://stackoverflow.com/questions/596162/can-you-remove-elements-from-a-stdlist-while-iterating-through-it
//			//give a chance to BTGhost to cleanup itself before being destroyed.
//			(*iterC)->do_finalize();
//			//remove the BTGhosts from the inner list (&& from the update task)
//			iterC = mGhosts.erase(iterC);
//		}
	}
	//clear parameters' tables
	mRigidBodiesParameterTable.clear();
//	mSoftBodiesParameterTable.clear(); xxx
//	mGhostsParameterTable.clear(); xxx
}

/**
 * Creates a BTRigidBody with a given (mandatory && not empty) name.
 * Returns a NodePath to the new BTRigidBody,or an empty NodePath with the
 * ET_fail error type set on error.
 */
NodePath GamePhysicsManager::create_rigid_body(const string& name)
{
	nassertr_always(!name.empty(), NodePath::fail())

	PT(BTRigidBody) newRigidBody = new BTRigidBody(name);
	nassertr_always(newRigidBody /*&& (!mWin.is_null())xxx*/, NodePath::fail())

	// set reference node
	newRigidBody->mReferenceNP = mReferenceNP;
	// reparent to reference node && set "this" NodePath
	NodePath np = mReferenceNP.attach_new_node(newRigidBody);
	// initialize the new RigidBody (could use mReferenceNP, mThisNP, mWin)
	newRigidBody->do_initialize();

	// add the new RigidBody to the inner list (&& to the update task)
	mRigidBodies.push_back(newRigidBody);
	//
	return np;
}

/**
 * Destroys a BTRigidBody.
 * Returns false on error.
 */
bool GamePhysicsManager::destroy_rigid_body(NodePath rigidBodyNP)
{
	CONTINUE_IF_ELSE_R(
			rigidBodyNP.node()->is_of_type(BTRigidBody::get_class_type()),
			false)

	PT(BTRigidBody)rigid_body = DCAST(BTRigidBody, rigidBodyNP.node());
	RigidBodyList::iterator iter = find(mRigidBodies.begin(),
			mRigidBodies.end(), rigid_body);
	CONTINUE_IF_ELSE_R(iter != mRigidBodies.end(), false)

	// give a chance to BTRigidBody to cleanup itself before being destroyed.
	rigid_body->do_finalize();
	//remove the BTRigidBody from the inner list (&& from the update task)
	mRigidBodies.erase(iter);
	//
	return true;
}

/**
 * Gets an BTRigidBody by index, or NULL on error.
 */
PT(BTRigidBody) GamePhysicsManager::get_rigid_body(int index) const
{
	nassertr_always((index >= 0) && (index < (int ) mRigidBodies.size()),
			NULL)

	return mRigidBodies[index];
}

/**
 * Creates a BTSoftBody with a given (mandatory && not empty) name.
 * Returns a NodePath to the new BTSoftBody,or an empty NodePath with the
 * ET_fail error type set on error.
 */
//NodePath GamePhysicsManager::create_soft_body(const string& name) xxx
//{
//	nassertr_always(!name.empty(), NodePath::fail())
//
//	PT(BTSoftBody)newSoftBody = new BTSoftBody(name);
//	nassertr_always(newSoftBody /*&& (!mWin.is_null()) xxx*/, NodePath::fail())
//
//	// set reference node
//	newSoftBody->mReferenceNP = mReferenceNP;
//	// reparent to reference node && set "this" NodePath
//	NodePath np = mReferenceNP.attach_new_node(newSoftBody);
//	// set the reference graphic window.
////	newSoftBody->mWin = mWin;xxx
//	// initialize the new Driver (could use mReferenceNP, mThisNP, mWin)
//	newSoftBody->do_initialize();
//
//	// add the new Driver to the inner list (&& to the update task)
//	mSoftBodies.push_back(newSoftBody);
//	//
//	return np;
//}

/**
 * Destroys a BTSoftBody.
 * Returns false on error.
 */
//bool GamePhysicsManager::destroy_soft_body(NodePath softBodyNP) xxx
//{
//	CONTINUE_IF_ELSE_R(
//			softBodyNP.node()->is_of_type(BTSoftBody::get_class_type()),
//			false)
//
//	PT(BTSoftBody)soft_body = DCAST(BTSoftBody, softBodyNP.node());
//	SoftBodyList::iterator iter = find(mSoftBodies.begin(),
//			mSoftBodies.end(), soft_body);
//	CONTINUE_IF_ELSE_R(iter != mSoftBodies.end(), false)
//
//	// give a chance to BTSoftBody to cleanup itself before being destroyed.
//	soft_body->do_finalize();
//	// reset the reference graphic window. xxx
//	soft_body->mWin.clear();
//	//remove the BTSoftBody from the inner list (&& from the update task)
//	mSoftBodies.erase(iter);
//	//
//	return true;
//}

/**
 * Gets an BTSoftBody by index, or NULL on error.
 */
//PT(BTSoftBody) GamePhysicsManager::get_soft_body(int index) const xxx
//{
//	nassertr_always((index >= 0) && (index < (int ) mSoftBodies.size()),
//			NULL)
//
//	return mSoftBodies[index];
//}

/**
 * Creates a BTGhost with a given (mandatory && not empty) name.
 * Returns a NodePath to the new BTGhost,or an empty NodePath with the
 * ET_fail error type set on error.
 */
//NodePath GamePhysicsManager::create_ghost(const string& name) xxx
//{
//	nassertr_always(!name.empty(), NodePath::fail())
//
//	PT(BTGhost)newGhost = new BTGhost(name);
//	nassertr_always(newGhost /*&& (!mWin.is_null()xxx)*/, NodePath::fail())
//
//	// set reference node
//	newGhost->mReferenceNP = mReferenceNP;
//	// reparent to reference node && set "this" NodePath
//	NodePath np = mReferenceNP.attach_new_node(newGhost);
//	// set the reference graphic window.
////	newGhost->mWin = mWin;xxx
//	// initialize the new Driver (could use mReferenceNP, mThisNP, mWin)
//	newGhost->do_initialize();
//
//	// add the new Driver to the inner list (&& to the update task)
//	mGhosts.push_back(newGhost);
//	//
//	return np;
//}

/**
 * Destroys a BTGhost.
 * Returns false on error.
 */
//bool GamePhysicsManager::destroy_ghost(NodePath ghostNP) xxx
//{
//	CONTINUE_IF_ELSE_R(
//			ghostNP.node()->is_of_type(BTGhost::get_class_type()),
//			false)
//
//	PT(BTGhost)ghost = DCAST(BTGhost, ghostNP.node());
//	GhostList::iterator iter = find(mGhosts.begin(),
//			mGhosts.end(), ghost);
//	CONTINUE_IF_ELSE_R(iter != mGhosts.end(), false)
//
//	// give a chance to BTGhost to cleanup itself before being destroyed.
//	ghost->do_finalize();
//	// reset the reference graphic window. xxx
//	ghost->mWin.clear();
//	//remove the BTGhost from the inner list (&& from the update task)
//	mGhosts.erase(iter);
//	//
//	return true;
//}

/**
 * Gets an BTGhost by index, or NULL on error.
 */
//PT(BTGhost) GamePhysicsManager::get_ghost(int index) const xxx
//{
//	nassertr_always((index >= 0) && (index < (int ) mGhosts.size()),
//			NULL)
//
//	return mGhosts[index];
//}

/**
 * Sets a multi-valued parameter to a multi-value overwriting the existing one(s).
 */
void GamePhysicsManager::set_parameter_values(PhysicsType type, const string& paramName,
		const ValueList<string>& paramValues)
{
	pair<ParameterTableIter, ParameterTableIter> iterRange;

	if (type == RIGIDBODY)
	{
		//find from mParameterTable the paramName's values to be overwritten
		iterRange = mRigidBodiesParameterTable.equal_range(paramName);
		//...&& erase them
		mRigidBodiesParameterTable.erase(iterRange.first, iterRange.second);
		//insert the new values
		for (int idx = 0; idx < paramValues.size(); ++idx)
		{
			mRigidBodiesParameterTable.insert(
					ParameterNameValue(paramName, paramValues[idx]));
		}
		return;
	}
//	if (type == SOFTBODY) xxx
//	{
//		//find from mParameterTable the paramName's values to be overwritten
//		iterRange = mSoftBodiesParameterTable.equal_range(paramName);
//		//...&& erase them
//		mSoftBodiesParameterTable.erase(iterRange.first, iterRange.second);
//		//insert the new values
//		for (int idx = 0; idx < paramValues.size(); ++idx)
//		{
//			mSoftBodiesParameterTable.insert(
//					ParameterNameValue(paramName, paramValues[idx]));
//		}
//		return;
//	}
//	if (type == GHOST) xxx
//	{
//		//find from mParameterTable the paramName's values to be overwritten
//		iterRange = mGhostsParameterTable.equal_range(paramName);
//		//...&& erase them
//		mGhostsParameterTable.erase(iterRange.first, iterRange.second);
//		//insert the new values
//		for (int idx = 0; idx < paramValues.size(); ++idx)
//		{
//			mGhostsParameterTable.insert(
//					ParameterNameValue(paramName, paramValues[idx]));
//		}
//		return;
//	}
}

/**
 * Gets the multiple values of a (actually set) parameter.
 */
ValueList<string> GamePhysicsManager::get_parameter_values(PhysicsType type,
		const string& paramName) const
{
	ValueList<string> strList;
	ParameterTableConstIter iter;
	pair<ParameterTableConstIter, ParameterTableConstIter> iterRange;

	if (type == RIGIDBODY)
	{
		iterRange = mRigidBodiesParameterTable.equal_range(paramName);
		if (iterRange.first != iterRange.second)
		{
			for (iter = iterRange.first; iter != iterRange.second; ++iter)
			{
				strList.add_value(iter->second);
			}
		}
		return strList;
	}
//	if (type == SOFTBODY) xxx
//	{
//		iterRange = mSoftBodiesParameterTable.equal_range(paramName);
//		if (iterRange.first != iterRange.second)
//		{
//			for (iter = iterRange.first; iter != iterRange.second; ++iter)
//			{
//				strList.add_value(iter->second);
//			}
//		}
//		return strList;
//	}
//	if (type == GHOST) xxx
//	{
//		iterRange = mGhostsParameterTable.equal_range(paramName);
//		if (iterRange.first != iterRange.second)
//		{
//			for (iter = iterRange.first; iter != iterRange.second; ++iter)
//			{
//				strList.add_value(iter->second);
//			}
//		}
//		return strList;
//	}
	//
	return strList;
}

/**
 * Sets a multi/single-valued parameter to a single value overwriting the existing one(s).
 */
void GamePhysicsManager::set_parameter_value(PhysicsType type, const string& paramName,
		const string& value)
{
	ValueList<string> valueList;
	valueList.add_value(value);
	set_parameter_values(type, paramName, valueList);
}

/**
 * Gets a single value (i.e. the first one) of a parameter.
 */
string GamePhysicsManager::get_parameter_value(PhysicsType type,
		const string& paramName) const
{
	ValueList<string> valueList = get_parameter_values(type, paramName);
	return (valueList.size() != 0 ? valueList[0] : string(""));
}

/**
 * Gets a list of the names of the parameters actually set.
 */
ValueList<string> GamePhysicsManager::get_parameter_name_list(PhysicsType type) const
{
	ValueList<string> strList;
	ParameterTableIter iter;
	ParameterTable tempTable;

	if (type == RIGIDBODY)
	{
		tempTable = mRigidBodiesParameterTable;
		for (iter = tempTable.begin(); iter != tempTable.end(); ++iter)
		{
			string name = (*iter).first;
			if (!strList.has_value(name))
			{
				strList.add_value(name);
			}
		}
		return strList;
	}
//	if (type == SOFTBODY) xxx
//	{
//		tempTable = mSoftBodiesParameterTable;
//		for (iter = tempTable.begin(); iter != tempTable.end(); ++iter)
//		{
//			string name = (*iter).first;
//			if (!strList.has_value(name))
//			{
//				strList.add_value(name);
//			}
//		}
//		return strList;
//	}
//	if (type == GHOST) xxx
//	{
//		tempTable = mGhostsParameterTable;
//		for (iter = tempTable.begin(); iter != tempTable.end(); ++iter)
//		{
//			string name = (*iter).first;
//			if (!strList.has_value(name))
//			{
//				strList.add_value(name);
//			}
//		}
//		return strList;
//	}
	//
	return strList;
}

/**
 * Sets all parameters to their default values (if any).
 * \note: After reading objects from bam files, the objects' creation parameters
 * which reside in the manager, are reset to their default values.
 */
void GamePhysicsManager::set_parameters_defaults(PhysicsType type)
{
	if (type == RIGIDBODY)
	{
		///mRigidBodiesParameterTable must be the first cleared
		mRigidBodiesParameterTable.clear();
		//sets the (mandatory) parameters to their default values:
		mRigidBodiesParameterTable.insert(
				ParameterNameValue("body_type", "dynamic"));
		mRigidBodiesParameterTable.insert(
				ParameterNameValue("body_mass", "1.0"));
		mRigidBodiesParameterTable.insert(
				ParameterNameValue("body_friction", "0.8"));
		mRigidBodiesParameterTable.insert(
				ParameterNameValue("body_restitution", "0.1"));
		mRigidBodiesParameterTable.insert(
				ParameterNameValue("shape_type", "sphere"));
		mRigidBodiesParameterTable.insert(
				ParameterNameValue("shape_size", "medium"));
		mRigidBodiesParameterTable.insert(
				ParameterNameValue("collide_mask", "all_on"));
		mRigidBodiesParameterTable.insert(
				ParameterNameValue("shape_height", "1.0"));
		mRigidBodiesParameterTable.insert(
				ParameterNameValue("shape_up", "z"));
		mRigidBodiesParameterTable.insert(
				ParameterNameValue("shape_scale_w", "1.0"));
		mRigidBodiesParameterTable.insert(
				ParameterNameValue("shape_scale_d", "1.0"));
		return;
	}
//	if (type == SOFTBODY) xxx
//	{
//		///mSoftBodiesParameterTable must be the first cleared
//		mSoftBodiesParameterTable.clear();
//		//sets the (mandatory) parameters to their default values:
//		mSoftBodiesParameterTable.insert(ParameterNameValue("enabled", "true"));
//		mSoftBodiesParameterTable.insert(ParameterNameValue("forward", "enabled"));
//		mSoftBodiesParameterTable.insert(ParameterNameValue("backward", "enabled"));
//		mSoftBodiesParameterTable.insert(ParameterNameValue("head_limit", "false@0.0"));
//		mSoftBodiesParameterTable.insert(ParameterNameValue("head_left", "enabled"));
//		mSoftBodiesParameterTable.insert(ParameterNameValue("head_right", "enabled"));
//		mSoftBodiesParameterTable.insert(ParameterNameValue("pitch_limit", "false@0.0"));
//		mSoftBodiesParameterTable.insert(ParameterNameValue("pitch_up", "enabled"));
//		mSoftBodiesParameterTable.insert(ParameterNameValue("pitch_down", "enabled"));
//		mSoftBodiesParameterTable.insert(ParameterNameValue("strafe_left", "enabled"));
//		mSoftBodiesParameterTable.insert(ParameterNameValue("strafe_right", "enabled"));
//		mSoftBodiesParameterTable.insert(ParameterNameValue("up", "enabled"));
//		mSoftBodiesParameterTable.insert(ParameterNameValue("down", "enabled"));
//		mSoftBodiesParameterTable.insert(ParameterNameValue("mouse_move", "disabled"));
//		mSoftBodiesParameterTable.insert(ParameterNameValue("mouse_head", "disabled"));
//		mSoftBodiesParameterTable.insert(ParameterNameValue("mouse_pitch", "disabled"));
//		mSoftBodiesParameterTable.insert(ParameterNameValue("speed_key", "shift"));
//		mSoftBodiesParameterTable.insert(ParameterNameValue("inverted_translation", "false"));
//		mSoftBodiesParameterTable.insert(ParameterNameValue("inverted_rotation", "false"));
//		mSoftBodiesParameterTable.insert(ParameterNameValue("max_linear_speed", "5.0"));
//		mSoftBodiesParameterTable.insert(ParameterNameValue("max_angular_speed", "5.0"));
//		mSoftBodiesParameterTable.insert(ParameterNameValue("linear_accel", "5.0"));
//		mSoftBodiesParameterTable.insert(ParameterNameValue("angular_accel", "5.0"));
//		mSoftBodiesParameterTable.insert(ParameterNameValue("linear_friction", "0.1"));
//		mSoftBodiesParameterTable.insert(ParameterNameValue("angular_friction", "0.1"));
//		mSoftBodiesParameterTable.insert(ParameterNameValue("stop_threshold", "0.01"));
//		mSoftBodiesParameterTable.insert(ParameterNameValue("fast_factor", "5.0"));
//		mSoftBodiesParameterTable.insert(ParameterNameValue("sens_x", "0.2"));
//		mSoftBodiesParameterTable.insert(ParameterNameValue("sens_y", "0.2"));
//		return;
//	}
//	if (type == GHOST) xxx
//	{
//		///mGhostsParameterTable must be the first cleared
//		mGhostsParameterTable.clear();
//		//sets the (mandatory) parameters to their default values:
//		mGhostsParameterTable.insert(ParameterNameValue("enabled", "true"));
//		mGhostsParameterTable.insert(ParameterNameValue("forward", "enabled"));
//		mGhostsParameterTable.insert(ParameterNameValue("backward", "enabled"));
//		mGhostsParameterTable.insert(ParameterNameValue("head_limit", "false@0.0"));
//		mGhostsParameterTable.insert(ParameterNameValue("head_left", "enabled"));
//		mGhostsParameterTable.insert(ParameterNameValue("head_right", "enabled"));
//		mGhostsParameterTable.insert(ParameterNameValue("pitch_limit", "false@0.0"));
//		mGhostsParameterTable.insert(ParameterNameValue("pitch_up", "enabled"));
//		mGhostsParameterTable.insert(ParameterNameValue("pitch_down", "enabled"));
//		mGhostsParameterTable.insert(ParameterNameValue("strafe_left", "enabled"));
//		mGhostsParameterTable.insert(ParameterNameValue("strafe_right", "enabled"));
//		mGhostsParameterTable.insert(ParameterNameValue("up", "enabled"));
//		mGhostsParameterTable.insert(ParameterNameValue("down", "enabled"));
//		mGhostsParameterTable.insert(ParameterNameValue("mouse_move", "disabled"));
//		mGhostsParameterTable.insert(ParameterNameValue("mouse_head", "disabled"));
//		mGhostsParameterTable.insert(ParameterNameValue("mouse_pitch", "disabled"));
//		mGhostsParameterTable.insert(ParameterNameValue("speed_key", "shift"));
//		mGhostsParameterTable.insert(ParameterNameValue("inverted_translation", "false"));
//		mGhostsParameterTable.insert(ParameterNameValue("inverted_rotation", "false"));
//		mGhostsParameterTable.insert(ParameterNameValue("max_linear_speed", "5.0"));
//		mGhostsParameterTable.insert(ParameterNameValue("max_angular_speed", "5.0"));
//		mGhostsParameterTable.insert(ParameterNameValue("linear_accel", "5.0"));
//		mGhostsParameterTable.insert(ParameterNameValue("angular_accel", "5.0"));
//		mGhostsParameterTable.insert(ParameterNameValue("linear_friction", "0.1"));
//		mGhostsParameterTable.insert(ParameterNameValue("angular_friction", "0.1"));
//		mGhostsParameterTable.insert(ParameterNameValue("stop_threshold", "0.01"));
//		mGhostsParameterTable.insert(ParameterNameValue("fast_factor", "5.0"));
//		mGhostsParameterTable.insert(ParameterNameValue("sens_x", "0.2"));
//		mGhostsParameterTable.insert(ParameterNameValue("sens_y", "0.2"));
//		return;
//	}
}

/**
 * Updates physics objects.
 *
 * Will be called automatically in a task.
 */
AsyncTask::DoneStatus GamePhysicsManager::update(GenericAsyncTask* task)
{
	float dt = ClockObject::get_global_clock()->get_dt();

#ifdef TESTING
	dt = 0.016666667; //60 fps
#endif

	// call all BTRigidBodys' update functions, passing delta time
	for (PTA(PT(BTRigidBody))::size_type index = 0;
			index < mRigidBodies.size(); ++index)
	{
		mRigidBodies[index]->update(dt);
	}
//	// call all BTSoftBodys' update functions, passing delta time xxx
//	for (PTA(PT(BTSoftBody))::size_type index = 0;
//			index < mSoftBodies.size(); ++index)
//	{
//		mSoftBodies[index]->update(dt);
//	}
//	// call all BTGhosts' update functions, passing delta time xxx
//	for (PTA(PT(BTGhost))::size_type index = 0;
//			index < mGhosts.size(); ++index)
//	{
//		mGhosts[index]->update(dt);
//	}
	//
	int maxSubSteps;
	// do physics step simulation
	// timeStep < maxSubSteps * fixedTimeStep (=1/60.0=0.016666667) -->
	// supposing a minimum of 6,666666667 fps, we have a maximum timeStep of
	// 0.15 secs so: maxSubSteps <= 60 * 0.15 = 9
	if (dt < 0.016666667)
	{
		maxSubSteps = 1;
	}
	else if (dt < 0.033333333)
	{
		maxSubSteps = 2;
	}
	else if (dt < 0.05)
	{
		maxSubSteps = 3;
	}
	else if (dt < 0.066666668)
	{
		maxSubSteps = 4;
	}
	else if (dt < 0.083333335)
	{
		maxSubSteps = 5;
	}
	else if (dt < 0.100000002)
	{
		maxSubSteps = 6;
	}
	else if (dt < 0.116666669)
	{
		maxSubSteps = 7;
	}
	else if (dt < 0.133333336)
	{
		maxSubSteps = 8;
	}
	else
	{
		maxSubSteps = 9;
	}
	mBulletWorld->do_physics(dt, maxSubSteps);

	//notify collisions
	if (mCollisionNotify.mEnable)
	{
		//update general count:
		//only actual colliding object pairs have their counts updated,
		//while just stopped to collide object pairs will be erased from the set
		++mCollisionNotify.mCount;

		//elaborate current colliding object pair list
		if (mCollisionDispatcher->getNumManifolds() > 0)
		{
			//update elapsed time
			mCollisionNotify.mTimeElapsed +=
					ClockObject::get_global_clock()->get_dt();
			// iterate through all of the manifolds in the dispatcher
			for (int i = 0; i < mCollisionDispatcher->getNumManifolds(); ++i)
			{
				// get the manifold
				btPersistentManifold* pManifold =
						mCollisionDispatcher->getManifoldByIndexInternal(i);

				// ignore manifolds that have
				// no contact points.
				if (pManifold->getNumContacts() > 0)
				{
					// get the two rigid bodies' panda nodes involved in the collision
					PandaNode *node0 =
							(PandaNode *) static_cast<const btRigidBody*>(pManifold->getBody0())->getUserPointer();
					PandaNode *node1 =
							(PandaNode *) static_cast<const btRigidBody*>(pManifold->getBody1())->getUserPointer();
					//insert a default: check of equality is done only on CollidingNodePair::mPnode member
					pair<pset<CollidingNodePair>::iterator, bool> res =
							mCollidingNodePairs.insert(
									CollidingNodePair(node0, node1));
					if (res.second)
					{
						//this is a "new" colliding object pair
						//event name: <CollidingObjectType1>_<CollidingObjectType2>_Collision
						string objectType0 = node0->get_type().get_name();
						string objectType1 = node1->get_type().get_name();
						//alphabetically compare
						if (objectType0 < objectType1)
						{
							(res.first)->mCollidingNodePairData->mEventName =
									objectType0 + "_" + objectType1
											+ "_Collision";
							(res.first)->mCollidingNodePairData->mEventParameters[0] =
									EventParameter(/*physicsComponent0 xxx*/node0);
							(res.first)->mCollidingNodePairData->mEventParameters[1] =
									EventParameter(/*physicsComponent1 xxx*/node1);
						}
						else
						{
							(res.first)->mCollidingNodePairData->mEventName =
									objectType1 + "_" + objectType0
											+ "_Collision";
							(res.first)->mCollidingNodePairData->mEventParameters[0] =
									EventParameter(/*physicsComponent1 xxx*/node1);
							(res.first)->mCollidingNodePairData->mEventParameters[1] =
									EventParameter(/*physicsComponent0 xxx*/node0);
						}
						//throw the event
						throw_event(
								(res.first)->mCollidingNodePairData->mEventName,
								(res.first)->mCollidingNodePairData->mEventParameters[0],
								(res.first)->mCollidingNodePairData->mEventParameters[1]);
					}
					else
					{
						//this is an "old" colliding object pair
						if (mCollisionNotify.mTimeElapsed
								>= mCollisionNotify.mPeriod)
						{
							//throw the event
							throw_event(
									(res.first)->mCollidingNodePairData->mEventName,
									(res.first)->mCollidingNodePairData->mEventParameters[0],
									(res.first)->mCollidingNodePairData->mEventParameters[1]);
						}
					}
					//update count flag
					(res.first)->mCollidingNodePairData->mCount =
							mCollisionNotify.mCount;
				}
			}
			//update elapsed time
			if (mCollisionNotify.mTimeElapsed >= mCollisionNotify.mPeriod)
			{
				mCollisionNotify.mTimeElapsed -= mCollisionNotify.mPeriod;
			}
		}
		else
		{
			mCollisionNotify.mTimeElapsed = 0.0;
		}

		//erase just stopped to collide object pairs (which have not the count flag updated)
		for (pset<CollidingNodePair>::iterator i =
				mCollidingNodePairs.begin(); i != mCollidingNodePairs.end();)
		{
			//check if it has a previous count
			if (i->mCollidingNodePairData->mCount != (int) mCollisionNotify.mCount)
			{
				//throw the "off" event
				throw_event(i->mCollidingNodePairData->mEventName + "Off",
						i->mCollidingNodePairData->mEventParameters[0],
						i->mCollidingNodePairData->mEventParameters[1]);
				//erase the object
				mCollidingNodePairs.erase(i++);
			}
			else
			{
				++i;
			}
		}
	}

	//
	return AsyncTask::DS_cont;
}

/**
 * Adds a task to repeatedly call physics updates.
 */
void GamePhysicsManager::start_default_update()
{
	//create the task for updating AI objects
	mUpdateData = new TaskInterface<GamePhysicsManager>::TaskData(this,
			&GamePhysicsManager::update);
	mUpdateTask = new GenericAsyncTask(string("GamePhysicsManager::update"),
			&TaskInterface<GamePhysicsManager>::taskFunction,
			reinterpret_cast<void*>(mUpdateData.p()));
	mUpdateTask->set_sort(mTaskSort);
	//Adds mUpdateTask to the active queue.
	AsyncTaskManager::get_global_ptr()->add(mUpdateTask);
}

/**
 * Removes a task to repeatedly call physics updates.
 */
void GamePhysicsManager::stop_default_update()
{
	if (mUpdateTask)
	{
		AsyncTaskManager::get_global_ptr()->remove(mUpdateTask);
	}
	//
	mUpdateData.clear();
	mUpdateTask.clear();
}

/**
 * Creates a wrapping Bullet shape for a given model node path.\n
 * If automaticShaping is true this method builds a shape based on a tight
 * bounding box around the model, with the shape's tightness controlled by the
 * shapeSize parameter.\n
 * If automaticShaping is false shape is built according to this scheme:
 * - BulletSphereShape(dim1)
 * - BulletPlaneShape(LVector3f(dim1, dim2, dim3), dim4)
 * - BulletBoxShape(LVector3f(dim1, dim2, dim3))
 * - BulletCylinderShape(dim1, dim2, upAxis)
 * - BulletCapsuleShape(dim1, dim2, upAxis)
 * - BulletConeShape(dim1, dim2, upAxis)
 * - BulletHeightfieldShape(heightfieldFile, dim1, upAxis)
 * - BulletTriangleMeshShape (dynamic)
 * Parameters are:
 * - modelNP: the model node path.
 * - shapeType: the shape type.
 * - shapeSize: the shape size, i.e. its tightness around the model.
 * - modelDims: returns the model bounding box dimensions for each axis
 * (out parameter).
 * - modelDeltaCenter: returns the middle point of the model bounding box
 * (out parameter).
 * - modelRadius: returns the radius of the model bounding box
 * (out parameter).
 * - dim1: returns/sets the first shape parameter (in or out parameter).
 * - dim2: returns/sets the second shape parameter (in or out parameter).
 * - dim3: returns/sets the third shape parameter (in or out parameter).
 * - dim4: returns/sets the fourth shape parameter (in or out parameter).
 * - automaticShaping: if true the dimXs are output parameters, otherwise input
 * parameter.
 * - upAxis: the up axis.
 * - heightfieldFile The height field file (HEIGHTFIELD shape).
 * - dynamic: if true the shape is dynamic (TRIANGLEMESH shape).
 * Returns the created BulletShape, or NULL on error.
 */
PT(BulletShape)GamePhysicsManager::create_shape(NodePath modelNP,
		ShapeType shapeType, ShapeSize shapeSize, LVecBase3f& modelDims,
		LVector3f& modelDeltaCenter, float& modelRadius,
		float& dim1, float& dim2, float& dim3, float& dim4,
		bool automaticShaping, BulletUpAxis upAxis,
		const Filename& heightfieldFile, bool dynamic)
{
	// create the current shape
	PT(BulletShape) collisionShape = NULL;
	NodePathCollection geomNodes;
	//some preliminary check
	if (modelNP.is_empty())
	{
		//a Bullet shape is requested without an associated model:
		//force automaticShaping to false
		automaticShaping = false;
	}
	else
	{
		// check if there are some GeomNode
		geomNodes = modelNP.find_all_matches("**/+GeomNode");
		if (geomNodes.is_empty())
		{
			if(modelNP.node()->is_of_type(GeomNode::get_class_type()))
			{
				geomNodes.add_path(modelNP);
			}
		}
		// check if automatic shaping is to be on/off
		if (! geomNodes.is_empty())
		{
			// get the bounding dimensions of object node path,
			// that should represents a model
			modelRadius = get_bounding_dimensions(modelNP, modelDims,
					modelDeltaCenter);
		}
		else
		{
			//a Bullet shape is requested without GeomNodes:
			//force automaticShaping to false
			automaticShaping = false;
		}
	}
	//
	switch (shapeType)
	{
		case SPHERE:
		if (automaticShaping)
		{
			//modify radius
			dim1 = modelRadius;
		}
		collisionShape = new BulletSphereShape(dim1);
		break;
		case PLANE:
		if (automaticShaping)
		{
			//reset dims
			dim1 = 0.0;
			dim2 = 0.0;
			dim3 = 0.0;
			//compute normal
			if (upAxis == X_up)
			{
				dim1 = 1.0;
			}
			else if (upAxis == Y_up)
			{
				dim2 = 1.0;
			}
			else
			{
				dim3 = 1.0;
			}
			//modify d
			dim4 = 0.0;
		}
		collisionShape = new BulletPlaneShape(
				LVector3f(dim1, dim2, dim3).normalized(), dim4);
		break;
		case BOX:
		if (automaticShaping)
		{
			//modify half dimensions
			dim1 = modelDims.get_x() / 2.0;
			dim2 = modelDims.get_y() / 2.0;
			dim3 = modelDims.get_z() / 2.0;
		}
		collisionShape = new BulletBoxShape(LVector3f(dim1, dim2, dim3));
		break;
		case CYLINDER:
		if (automaticShaping)
		{
			//modify radius && height
			if (upAxis == X_up)
			{
				dim1 = do_get_dim(shapeSize, modelDims.get_y(), modelDims.get_z());
				dim2 = modelDims.get_x();
			}
			else if (upAxis == Y_up)
			{
				dim1 = do_get_dim(shapeSize, modelDims.get_x(), modelDims.get_z());
				dim2 = modelDims.get_y();
			}
			else
			{
				dim1 = do_get_dim(shapeSize, modelDims.get_x(), modelDims.get_y());
				dim2 = modelDims.get_z();
			}
		}
		collisionShape = new BulletCylinderShape(dim1, dim2, upAxis);
		break;
		case CAPSULE:
		if (automaticShaping)
		{
			//modify radius && height
			if (upAxis == X_up)
			{
				dim1 = do_get_dim(shapeSize, modelDims.get_y(), modelDims.get_z());
				dim2 = modelDims.get_x() - 2 * dim1;
			}
			else if (upAxis == Y_up)
			{
				dim1 = do_get_dim(shapeSize, modelDims.get_x(), modelDims.get_z());
				dim2 = modelDims.get_y() - 2 * dim1;
			}
			else
			{
				dim1 = do_get_dim(shapeSize, modelDims.get_x(), modelDims.get_y());
				dim2 = modelDims.get_z() - 2 * dim1;
			}
		}
		if (dim2 <= 0.0)
		{
			dim2 = 0.0;
		}
		collisionShape = new BulletCapsuleShape(dim1, dim2, upAxis);
		break;
		case CONE:
		if (automaticShaping)
		{
			//modify radius && height
			if (upAxis == X_up)
			{
				dim1 = do_get_dim(shapeSize, modelDims.get_y(), modelDims.get_z());
				dim2 = modelDims.get_x();
			}
			else if (upAxis == Y_up)
			{
				dim1 = do_get_dim(shapeSize, modelDims.get_x(), modelDims.get_z());
				dim2 = modelDims.get_y();
			}
			else
			{
				dim1 = do_get_dim(shapeSize, modelDims.get_x(), modelDims.get_y());
				dim2 = modelDims.get_z();
			}
		}
		collisionShape = new BulletConeShape(dim1, dim2, upAxis);
		break;
		case HEIGHTFIELD:
		collisionShape = new BulletHeightfieldShape(PNMImage(heightfieldFile), 1.0, upAxis);
		break;
		case TRIANGLEMESH:
		{
			//see: https://www.panda3d.org/forums/viewtopic.php?t=13981
			BulletTriangleMesh* triMesh = new BulletTriangleMesh();
			//add geoms from geomNodes to the mesh
			CPT(TransformState) modelTS = modelNP.get_transform();
			for (int i = 0; i < geomNodes.get_num_paths(); ++i)
			{
				PT(GeomNode) geomNode = DCAST(GeomNode,
						geomNodes.get_path(i).node());
				GeomNode::Geoms geoms = geomNode->get_geoms();
				for (int j = 0; j < geoms.get_num_geoms(); ++j)
				{
					triMesh->add_geom(geoms.get_geom(j), true, modelTS.p());
				}
			}
			collisionShape = new BulletTriangleMeshShape(triMesh, dynamic);
		}
		break;
		//
		default:
		break;
	}
	//
	return collisionShape;
}

/**
 * Gets bounding dimensions of a model NodePath.
 * Puts results into the out parameters: modelDims, modelDeltaCenter && returns
 * modelRadius.
 * - modelDims = absolute dimensions of the model
 * - modelCenter + modelDeltaCenter = origin of coordinate system
 * - modelRadius = radius of the containing sphere
 */
/**
 * \brief Calculates geometric characteristics of a GeomNode.
 *
 * It takes a NodePath, (supposedly) referring to a GeomNode, &&
 * calculates a tight AABB surrounding it, hence sets the
 * related dimensions into mModelDims, mModelCenter, mModelRadius
 * member variables.\n
 * \note Remember that AABB takes into account model rotation/orientation
 * so to get a bounding box of the model into the reference pose, no
 * rotation/orientation should be applied to the model node path.\n
 *
 * @param modelNP The model node path.
 * @param modelDims Returns the model AABB dimensions for each axis.
 * @param modelDeltaCenter Returns -(MAXP + minP)/2.0 (MAXP/minP=max/min point
 * of the model AABB).
 * @param modelRadius Returns the radius of the model AABB.
 */
float GamePhysicsManager::get_bounding_dimensions(NodePath modelNP,
		LVecBase3f& modelDims, LVector3f& modelDeltaCenter) const
{
	//get "tight" dimensions of model
	LPoint3f minP, maxP;
	modelNP.calc_tight_bounds(minP, maxP);
	//
	LVecBase3 delta = maxP - minP;
	LVector3f deltaCenter = -(minP + delta / 2.0);
	//
	modelDims.set(abs(delta.get_x()), abs(delta.get_y()), abs(delta.get_z()));
	modelDeltaCenter.set(deltaCenter.get_x(), deltaCenter.get_y(),
			deltaCenter.get_z());
	float modelRadius = max(max(modelDims.get_x(), modelDims.get_y()),
			modelDims.get_z()) / 2.0;
	return modelRadius;
}

/**
 * Computes the desired dimension given the shape size.
 * Parameters are:
 * - shapeSize: the shape size.
 * - d1: first dimension.
 * - d2: second dimension.
 * Returns the desired dimension.
 * \note Internal use only.
 */
float GamePhysicsManager::do_get_dim(ShapeSize shapeSize, float d1, float d2)
{
	float dim;
	if (shapeSize == MINIMUN)
	{
		dim = min(d1, d2) / 2.0;
	}
	else if (shapeSize == MAXIMUM)
	{
		dim = max(d1, d2) / 2.0;
	}
	else
	{
		dim = (d1 + d2) / 4.0;
	}
	//
	return dim;
}

/**
 * Throws a ray downward (-z direction default) from fromPos.
 * If collisions are found returns a Pair<bool,float> == (true, height), with
 * height equal to the z-value of the first one.
 * If collisions are not found returns a Pair<bool,float> == (false, 0.0).
 */
Pair<bool,float> GamePhysicsManager::get_collision_height(const LPoint3f& fromPos)
{
	//cast a ray downward starting at fromPos
	LPoint3f toPos = LPoint3f(fromPos.get_x(), fromPos.get_y(), -FLT_MAX);
	BulletClosestHitRayResult hitResult = mBulletWorld->ray_test_closest(
			fromPos, toPos, mMask);
	if (hitResult.has_hit())
	{
		return Pair<bool, float>(true, hitResult.get_hit_pos().get_z());
	}
	//
	return Pair<bool, float>(false, 0.0);
}

void GamePhysicsManager::enable_collision_notify(EventThrown event, /*const string& eventName, xxx*/
		float eventFreq)
{
	//some checks
//	RETURN_ON_COND(eventName == string(""),) xxx

	if (eventFreq <= 0.0)
	{
		eventFreq = 30.0;
	}

	ThrowEventData eventData;
	eventData.mEnable = true;
//	eventData.mEventName = eventName; xxx
	eventData.mFrequency = eventFreq;
	switch (event)
	{
	case COLLISIONNOTIFY:
		if(mCollisionNotify.mEnable != eventData.mEnable)
		{
			mCollisionNotify = eventData;
			mCollisionNotify.mTimeElapsed = 0;
		}
		break;
	default:
		break;
	}
}

/**
 * Writes to a bam file the entire collections of physics objects && related
 * geometries (i.e. models' NodePaths)
 */
bool GamePhysicsManager::write_to_bam_file(const string& fileName)
{
	string errorReport;
	// write to bam file
	BamFile outBamFile;
	if (outBamFile.open_write(Filename(fileName)))
	{
		cout << "Current system Bam version: "
				<< outBamFile.get_current_major_ver() << "."
				<< outBamFile.get_current_minor_ver() << endl;
		// just write the reference node
		if (!outBamFile.write_object(mReferenceNP.node()))
		{
			errorReport += string("Error writing ") + mReferenceNP.get_name()
					+ string(" node in ") + fileName + string("\n");
		}
		// close the file
		outBamFile.close();
	}
	else
	{
		errorReport += string("\nERROR: cannot open ") + fileName;
	}
	//check
	if (errorReport.empty())
	{
		cout
				<< "SUCCESS: all physics object collections were written to "
				<< fileName << endl;
	}
	else
	{
		cerr << errorReport << endl;
	}
	return errorReport.empty();
}

/**
 * Reads from a bam file the entire hierarchy of physics objects && related
 * geometries (i.e. models' NodePaths)
 */
bool GamePhysicsManager::read_from_bam_file(const string& fileName)
{
	string errorReport;
	//read from bamFile
	BamFile inBamFile;
	if (inBamFile.open_read(Filename(fileName)))
	{
		cout << "Current system Bam version: "
				<< inBamFile.get_current_major_ver() << "."
				<< inBamFile.get_current_minor_ver() << endl;
		cout << "Bam file version: " << inBamFile.get_file_major_ver() << "."
				<< inBamFile.get_file_minor_ver() << endl;
		// just read the reference node
		TypedWritable* reference = inBamFile.read_object();
		if (reference)
		{
			//resolve pointers
			if (!inBamFile.resolve())
			{
				errorReport += string("Error resolving pointers in ") + fileName
						+ string("\n");
			}
		}
		else
		{
			errorReport += string("Error reading ") + fileName + string("\n");
		}
		// close the file
		inBamFile.close();
		// restore reference node
		mReferenceNP = NodePath::any_path(DCAST(PandaNode, reference));
	}
	else
	{
		errorReport += string("\nERROR: cannot open ") + fileName;
	}
	//check
	if (errorReport.empty())
	{
		cout << "SUCCESS: all physics objects were read from "
				<< fileName << endl;
	}
	else
	{
		cerr << errorReport << endl;
	}
	return errorReport.empty();
}


void GamePhysicsManager::init_debug()
{
#ifdef ELY_DEBUG
	mBulletDebugNodePath.reparent_to(mReferenceNP);
	PT(BulletDebugNode) bulletDebugNode =
			DCAST(BulletDebugNode,mBulletDebugNodePath.node());
	mBulletWorld->set_debug_node(bulletDebugNode);
	bulletDebugNode->show_wireframe(true);
	bulletDebugNode->show_constraints(true);
	bulletDebugNode->show_bounding_boxes(false);
	bulletDebugNode->show_normals(false);
	mBulletDebugNodePath.hide();
#endif //ELY_DEBUG
}

void GamePhysicsManager::debug(bool enable)
{
#ifdef ELY_DEBUG
	if (enable)
	{
		if (mBulletDebugNodePath.is_hidden())
		{
			mBulletDebugNodePath.show();
		}
	}
	else
	{
		if (! mBulletDebugNodePath.is_hidden())
		{
			mBulletDebugNodePath.hide();
		}
	}
#endif //ELY_DEBUG
}

//TypedObject semantics: hardcoded
TypeHandle GamePhysicsManager::_type_handle;
