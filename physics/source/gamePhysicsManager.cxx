/**
 * \file gamePhysicsManager.cxx
 *
 * \date 2016-10-09
 * \author consultit
 */

#include "asyncTaskManager.h"
#include "graphicsWindow.h"
#include "bamFile.h"
#include "gamePhysicsManager.h"

#include "btRigidBody.h"
#include "btSoftBody.h"

/**
 *
 */
GamePhysicsManager::GamePhysicsManager(PT(GraphicsWindow) win, int taskSort, const NodePath& root,
		const CollideMask& mask):
		mWin(win),
		mTaskSort(taskSort),
		mReferenceNP(NodePath("ReferenceNode")),
		mRoot(root),
		mMask(mask),
		mCollisionHandler(NULL),
		mPickerRay(NULL),
		mCTrav(NULL),
		mRef(0)
{
	PRINT_DEBUG("GamePhysicsManager::GamePhysicsManager: creating the singleton manager.");

	mRigidBodies.clear();
	mRigidBodiesParameterTable.clear();
	mSoftBodies.clear();
	mSoftBodiesParameterTable.clear();
	mGhosts.clear();
	mGhostsParameterTable.clear();
	set_parameters_defaults(RIGIDBODY);
	set_parameters_defaults(SOFTBODY);
	set_parameters_defaults(GHOST);
	//
	mUpdateData.clear();
	mUpdateTask.clear();
	//
	if (!mRoot.is_empty()) //xxx
	{
		mCTrav = new CollisionTraverser();
		mCollisionHandler = new CollisionHandlerQueue();
		mPickerRay = new CollisionRay();
		PT(CollisionNode)pickerNode = new CollisionNode(string("GamePhysicsManager::pickerNode"));
		pickerNode->add_solid(mPickerRay);
		pickerNode->set_from_collide_mask(mMask);
		pickerNode->set_into_collide_mask(BitMask32::all_off());
		mCTrav->add_collider(mRoot.attach_new_node(pickerNode),
				mCollisionHandler);
	}
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
			//remove the BTRigidBodys from the inner list (and from the update task)
			iterA = mRigidBodies.erase(iterA);
		}

		//destroy all BTSoftBodys
		PTA(PT(BTSoftBody))::iterator iterB = mSoftBodies.begin();
		while (iterB != mSoftBodies.end())
		{
			//\see http://stackoverflow.com/questions/596162/can-you-remove-elements-from-a-stdlist-while-iterating-through-it
			//give a chance to BTSoftBody to cleanup itself before being destroyed.
			(*iterB)->do_finalize();
			//remove the BTSoftBodys from the inner list (and from the update task)
			iterB = mSoftBodies.erase(iterB);
		}

		//destroy all BTGhosts
		PTA(PT(BTGhost))::iterator iterC = mGhosts.begin();
		while (iterC != mGhosts.end())
		{
			//\see http://stackoverflow.com/questions/596162/can-you-remove-elements-from-a-stdlist-while-iterating-through-it
			//give a chance to BTGhost to cleanup itself before being destroyed.
			(*iterC)->do_finalize();
			//remove the BTGhosts from the inner list (and from the update task)
			iterC = mGhosts.erase(iterC);
		}
	}
	//clear parameters' tables
	mRigidBodiesParameterTable.clear();
	mSoftBodiesParameterTable.clear();
	mGhostsParameterTable.clear();
	//
	delete mCTrav;
}

/**
 * Creates a BTRigidBody with a given (mandatory and not empty) name.
 * Returns a NodePath to the new BTRigidBody,or an empty NodePath with the
 * ET_fail error type set on error.
 */
NodePath GamePhysicsManager::create_rigid_body(const string& name)
{
	nassertr_always(!name.empty(), NodePath::fail())

	PT(BTRigidBody) newRigidBody = new BTRigidBody(name);
	nassertr_always(newRigidBody && (!mWin.is_null()), NodePath::fail())

	// set reference node
	newRigidBody->mReferenceNP = mReferenceNP;
	// reparent to reference node and set "this" NodePath
	NodePath np = mReferenceNP.attach_new_node(newRigidBody);
	// set the reference graphic window. xxx
	newRigidBody->mWin = mWin;
	// initialize the new RigidBody (could use mReferenceNP, mThisNP, mWin)
	newRigidBody->do_initialize();

	// add the new RigidBody to the inner list (and to the update task)
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
	// reset the reference graphic window. xxx
	rigid_body->mWin.clear();
	//remove the BTRigidBody from the inner list (and from the update task)
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
 * Creates a BTSoftBody with a given (mandatory and not empty) name.
 * Returns a NodePath to the new BTSoftBody,or an empty NodePath with the
 * ET_fail error type set on error.
 */
NodePath GamePhysicsManager::create_soft_body(const string& name)
{
	nassertr_always(!name.empty(), NodePath::fail())

	PT(BTSoftBody)newSoftBody = new BTSoftBody(name);
	nassertr_always(newSoftBody && (!mWin.is_null()), NodePath::fail())

	// set reference node
	newSoftBody->mReferenceNP = mReferenceNP;
	// reparent to reference node and set "this" NodePath
	NodePath np = mReferenceNP.attach_new_node(newSoftBody);
	// set the reference graphic window. xxx
	newSoftBody->mWin = mWin;
	// initialize the new Driver (could use mReferenceNP, mThisNP, mWin)
	newSoftBody->do_initialize();

	// add the new Driver to the inner list (and to the update task)
	mSoftBodies.push_back(newSoftBody);
	//
	return np;
}

/**
 * Destroys a BTSoftBody.
 * Returns false on error.
 */
bool GamePhysicsManager::destroy_soft_body(NodePath softBodyNP)
{
	CONTINUE_IF_ELSE_R(
			softBodyNP.node()->is_of_type(BTSoftBody::get_class_type()),
			false)

	PT(BTSoftBody)soft_body = DCAST(BTSoftBody, softBodyNP.node());
	SoftBodyList::iterator iter = find(mSoftBodies.begin(),
			mSoftBodies.end(), soft_body);
	CONTINUE_IF_ELSE_R(iter != mSoftBodies.end(), false)

	// give a chance to BTSoftBody to cleanup itself before being destroyed.
	soft_body->do_finalize();
	// reset the reference graphic window. xxx
	soft_body->mWin.clear();
	//remove the BTSoftBody from the inner list (and from the update task)
	mSoftBodies.erase(iter);
	//
	return true;
}

/**
 * Gets an BTSoftBody by index, or NULL on error.
 */
PT(BTSoftBody) GamePhysicsManager::get_soft_body(int index) const
{
	nassertr_always((index >= 0) && (index < (int ) mSoftBodies.size()),
			NULL)

	return mSoftBodies[index];
}

/**
 * Creates a BTGhost with a given (mandatory and not empty) name.
 * Returns a NodePath to the new BTGhost,or an empty NodePath with the xxx
 * ET_fail error type set on error.
 */
NodePath GamePhysicsManager::create_ghost(const string& name)
{
	nassertr_always(!name.empty(), NodePath::fail())

	PT(BTGhost)newGhost = new BTGhost(name);
	nassertr_always(newGhost && (!mWin.is_null()), NodePath::fail())

	// set reference node
	newGhost->mReferenceNP = mReferenceNP;
	// reparent to reference node and set "this" NodePath
	NodePath np = mReferenceNP.attach_new_node(newGhost);
	// set the reference graphic window. xxx
	newGhost->mWin = mWin;
	// initialize the new Driver (could use mReferenceNP, mThisNP, mWin)
	newGhost->do_initialize();

	// add the new Driver to the inner list (and to the update task)
	mGhosts.push_back(newGhost);
	//
	return np;
}

/**
 * Destroys a BTGhost.
 * Returns false on error.
 */
bool GamePhysicsManager::destroy_ghost(NodePath ghostNP)
{
	CONTINUE_IF_ELSE_R(
			ghostNP.node()->is_of_type(BTGhost::get_class_type()),
			false)

	PT(BTGhost)ghost = DCAST(BTGhost, ghostNP.node());
	GhostList::iterator iter = find(mGhosts.begin(),
			mGhosts.end(), ghost);
	CONTINUE_IF_ELSE_R(iter != mGhosts.end(), false)

	// give a chance to BTGhost to cleanup itself before being destroyed.
	ghost->do_finalize();
	// reset the reference graphic window. xxx
	ghost->mWin.clear();
	//remove the BTGhost from the inner list (and from the update task)
	mGhosts.erase(iter);
	//
	return true;
}

/**
 * Gets an BTGhost by index, or NULL on error.
 */
PT(BTGhost) GamePhysicsManager::get_ghost(int index) const
{
	nassertr_always((index >= 0) && (index < (int ) mGhosts.size()),
			NULL)

	return mGhosts[index];
}

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
		//...and erase them
		mRigidBodiesParameterTable.erase(iterRange.first, iterRange.second);
		//insert the new values
		for (int idx = 0; idx < paramValues.size(); ++idx)
		{
			mRigidBodiesParameterTable.insert(
					ParameterNameValue(paramName, paramValues[idx]));
		}
		return;
	}
	if (type == SOFTBODY)
	{
		//find from mParameterTable the paramName's values to be overwritten
		iterRange = mSoftBodiesParameterTable.equal_range(paramName);
		//...and erase them
		mSoftBodiesParameterTable.erase(iterRange.first, iterRange.second);
		//insert the new values
		for (int idx = 0; idx < paramValues.size(); ++idx)
		{
			mSoftBodiesParameterTable.insert(
					ParameterNameValue(paramName, paramValues[idx]));
		}
		return;
	}
	if (type == GHOST)
	{
		//find from mParameterTable the paramName's values to be overwritten
		iterRange = mGhostsParameterTable.equal_range(paramName);
		//...and erase them
		mGhostsParameterTable.erase(iterRange.first, iterRange.second);
		//insert the new values
		for (int idx = 0; idx < paramValues.size(); ++idx)
		{
			mGhostsParameterTable.insert(
					ParameterNameValue(paramName, paramValues[idx]));
		}
		return;
	}
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
	if (type == SOFTBODY)
	{
		iterRange = mSoftBodiesParameterTable.equal_range(paramName);
		if (iterRange.first != iterRange.second)
		{
			for (iter = iterRange.first; iter != iterRange.second; ++iter)
			{
				strList.add_value(iter->second);
			}
		}
		return strList;
	}
	if (type == GHOST)
	{
		iterRange = mGhostsParameterTable.equal_range(paramName);
		if (iterRange.first != iterRange.second)
		{
			for (iter = iterRange.first; iter != iterRange.second; ++iter)
			{
				strList.add_value(iter->second);
			}
		}
		return strList;
	}
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
	if (type == SOFTBODY)
	{
		tempTable = mSoftBodiesParameterTable;
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
	if (type == GHOST)
	{
		tempTable = mGhostsParameterTable;
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
		mRigidBodiesParameterTable.insert(ParameterNameValue("enabled", "true"));
		mRigidBodiesParameterTable.insert(ParameterNameValue("backward", "true"));
		mRigidBodiesParameterTable.insert(ParameterNameValue("fixed_relative_position", "true"));
		mRigidBodiesParameterTable.insert(ParameterNameValue("friction", "1.0"));
		mRigidBodiesParameterTable.insert(ParameterNameValue("fixed_look_at", "true"));
		mRigidBodiesParameterTable.insert(ParameterNameValue("mouse_move", "false"));
		mRigidBodiesParameterTable.insert(ParameterNameValue("mouse_head", "false"));
		mRigidBodiesParameterTable.insert(ParameterNameValue("mouse_pitch", "false"));
		mRigidBodiesParameterTable.insert(ParameterNameValue("head_left", "enabled"));
		mRigidBodiesParameterTable.insert(ParameterNameValue("head_right", "enabled"));
		mRigidBodiesParameterTable.insert(ParameterNameValue("pitch_up", "enabled"));
		mRigidBodiesParameterTable.insert(ParameterNameValue("pitch_down", "enabled"));
		mRigidBodiesParameterTable.insert(ParameterNameValue("sens_x", "0.2"));
		mRigidBodiesParameterTable.insert(ParameterNameValue("sens_y", "0.2"));
		mRigidBodiesParameterTable.insert(ParameterNameValue("inverted_rotation", "false"));
		return;
	}
	if (type == SOFTBODY)
	{
		///mSoftBodiesParameterTable must be the first cleared
		mSoftBodiesParameterTable.clear();
		//sets the (mandatory) parameters to their default values:
		mSoftBodiesParameterTable.insert(ParameterNameValue("enabled", "true"));
		mSoftBodiesParameterTable.insert(ParameterNameValue("forward", "enabled"));
		mSoftBodiesParameterTable.insert(ParameterNameValue("backward", "enabled"));
		mSoftBodiesParameterTable.insert(ParameterNameValue("head_limit", "false@0.0"));
		mSoftBodiesParameterTable.insert(ParameterNameValue("head_left", "enabled"));
		mSoftBodiesParameterTable.insert(ParameterNameValue("head_right", "enabled"));
		mSoftBodiesParameterTable.insert(ParameterNameValue("pitch_limit", "false@0.0"));
		mSoftBodiesParameterTable.insert(ParameterNameValue("pitch_up", "enabled"));
		mSoftBodiesParameterTable.insert(ParameterNameValue("pitch_down", "enabled"));
		mSoftBodiesParameterTable.insert(ParameterNameValue("strafe_left", "enabled"));
		mSoftBodiesParameterTable.insert(ParameterNameValue("strafe_right", "enabled"));
		mSoftBodiesParameterTable.insert(ParameterNameValue("up", "enabled"));
		mSoftBodiesParameterTable.insert(ParameterNameValue("down", "enabled"));
		mSoftBodiesParameterTable.insert(ParameterNameValue("mouse_move", "disabled"));
		mSoftBodiesParameterTable.insert(ParameterNameValue("mouse_head", "disabled"));
		mSoftBodiesParameterTable.insert(ParameterNameValue("mouse_pitch", "disabled"));
		mSoftBodiesParameterTable.insert(ParameterNameValue("speed_key", "shift"));
		mSoftBodiesParameterTable.insert(ParameterNameValue("inverted_translation", "false"));
		mSoftBodiesParameterTable.insert(ParameterNameValue("inverted_rotation", "false"));
		mSoftBodiesParameterTable.insert(ParameterNameValue("max_linear_speed", "5.0"));
		mSoftBodiesParameterTable.insert(ParameterNameValue("max_angular_speed", "5.0"));
		mSoftBodiesParameterTable.insert(ParameterNameValue("linear_accel", "5.0"));
		mSoftBodiesParameterTable.insert(ParameterNameValue("angular_accel", "5.0"));
		mSoftBodiesParameterTable.insert(ParameterNameValue("linear_friction", "0.1"));
		mSoftBodiesParameterTable.insert(ParameterNameValue("angular_friction", "0.1"));
		mSoftBodiesParameterTable.insert(ParameterNameValue("stop_threshold", "0.01"));
		mSoftBodiesParameterTable.insert(ParameterNameValue("fast_factor", "5.0"));
		mSoftBodiesParameterTable.insert(ParameterNameValue("sens_x", "0.2"));
		mSoftBodiesParameterTable.insert(ParameterNameValue("sens_y", "0.2"));
		return;
	}
	if (type == GHOST)
	{
		///mGhostsParameterTable must be the first cleared
		mGhostsParameterTable.clear();
		//sets the (mandatory) parameters to their default values:
		mGhostsParameterTable.insert(ParameterNameValue("enabled", "true"));
		mGhostsParameterTable.insert(ParameterNameValue("forward", "enabled"));
		mGhostsParameterTable.insert(ParameterNameValue("backward", "enabled"));
		mGhostsParameterTable.insert(ParameterNameValue("head_limit", "false@0.0"));
		mGhostsParameterTable.insert(ParameterNameValue("head_left", "enabled"));
		mGhostsParameterTable.insert(ParameterNameValue("head_right", "enabled"));
		mGhostsParameterTable.insert(ParameterNameValue("pitch_limit", "false@0.0"));
		mGhostsParameterTable.insert(ParameterNameValue("pitch_up", "enabled"));
		mGhostsParameterTable.insert(ParameterNameValue("pitch_down", "enabled"));
		mGhostsParameterTable.insert(ParameterNameValue("strafe_left", "enabled"));
		mGhostsParameterTable.insert(ParameterNameValue("strafe_right", "enabled"));
		mGhostsParameterTable.insert(ParameterNameValue("up", "enabled"));
		mGhostsParameterTable.insert(ParameterNameValue("down", "enabled"));
		mGhostsParameterTable.insert(ParameterNameValue("mouse_move", "disabled"));
		mGhostsParameterTable.insert(ParameterNameValue("mouse_head", "disabled"));
		mGhostsParameterTable.insert(ParameterNameValue("mouse_pitch", "disabled"));
		mGhostsParameterTable.insert(ParameterNameValue("speed_key", "shift"));
		mGhostsParameterTable.insert(ParameterNameValue("inverted_translation", "false"));
		mGhostsParameterTable.insert(ParameterNameValue("inverted_rotation", "false"));
		mGhostsParameterTable.insert(ParameterNameValue("max_linear_speed", "5.0"));
		mGhostsParameterTable.insert(ParameterNameValue("max_angular_speed", "5.0"));
		mGhostsParameterTable.insert(ParameterNameValue("linear_accel", "5.0"));
		mGhostsParameterTable.insert(ParameterNameValue("angular_accel", "5.0"));
		mGhostsParameterTable.insert(ParameterNameValue("linear_friction", "0.1"));
		mGhostsParameterTable.insert(ParameterNameValue("angular_friction", "0.1"));
		mGhostsParameterTable.insert(ParameterNameValue("stop_threshold", "0.01"));
		mGhostsParameterTable.insert(ParameterNameValue("fast_factor", "5.0"));
		mGhostsParameterTable.insert(ParameterNameValue("sens_x", "0.2"));
		mGhostsParameterTable.insert(ParameterNameValue("sens_y", "0.2"));
		return;
	}
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
	// call all BTSoftBodys' update functions, passing delta time
	for (PTA(PT(BTSoftBody))::size_type index = 0;
			index < mSoftBodies.size(); ++index)
	{
		mSoftBodies[index]->update(dt);
	}
	// call all BTGhosts' update functions, passing delta time
	for (PTA(PT(BTGhost))::size_type index = 0;
			index < mGhosts.size(); ++index)
	{
		mGhosts[index]->update(dt);
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
 * Gets bounding dimensions of a model NodePath.
 * Puts results into the out parameters: modelDims, modelDeltaCenter and returns
 * modelRadius.
 * - modelDims = absolute dimensions of the model
 * - modelCenter + modelDeltaCenter = origin of coordinate system
 * - modelRadius = radius of the containing sphere
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
 * Throws a ray downward (-z direction default) from rayOrigin.
 * If collisions are found returns a Pair<bool,float> == (true, height),
 * with height equal to the z-value of the first one.
 * If collisions are not found returns a Pair<bool,float> == (false, 0.0).
 */
Pair<bool,float> GamePhysicsManager::get_collision_height(const LPoint3f& rayOrigin,
		const NodePath& space) const
{
	//traverse downward starting at rayOrigin
	mPickerRay->set_direction(LVecBase3f(0.0, 0.0, -1.0));
	mPickerRay->set_origin(rayOrigin);
	mCTrav->traverse(mRoot);
	if (mCollisionHandler->get_num_entries() > 0)
	{
		mCollisionHandler->sort_entries();
		CollisionEntry *entry0 =
				mCollisionHandler->get_entry(0);
		LPoint3f target = entry0->get_surface_point(space);
		float collisionHeight = target.get_z();
		return Pair<bool,float>(true, collisionHeight);
	}
	//
	return Pair<bool,float>(false, 0.0);
}

/**
 * Writes to a bam file the entire collections of physics objects and related
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
 * Reads from a bam file the entire hierarchy of physics objects and related
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

//TypedObject semantics: hardcoded
TypeHandle GamePhysicsManager::_type_handle;
