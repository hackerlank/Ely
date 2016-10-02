/**
 * \file gameAIManager.cxx
 *
 * \date 2016-09-17
 * \author consultit
 */

#include "gameAIManager.h"

#include "osSteerPlugIn.h"
#include "osSteerVehicle.h"
#include "rnCrowdAgent.h"
#include "rnNavMesh.h"
#include "asyncTaskManager.h"
#include "bamFile.h"

/**
 *
 */
GameAIManager::GameAIManager(int taskSort, const NodePath& root,
		const CollideMask& mask):
		mReferenceNP(NodePath("ReferenceNode")),
		mTaskSort(taskSort),
		mRoot(root),
		mMask(mask),
		mCollisionHandler(NULL),
		mPickerRay(NULL),
		mCTrav(NULL),
		mReferenceDebugNP(NodePath("ReferenceDebugNode")),
		mReferenceDebug2DNP(NodePath("ReferenceDebugNode2D")),
		mRef(0)
{
	PRINT_DEBUG("GameAIManager::GameAIManager: creating the singleton manager.");

	mSteerPlugIns.clear();
	mSteerPlugInsParameterTable.clear();
	mSteerVehicles.clear();
	mSteerVehiclesParameterTable.clear();
	mObstacles.first().clear();
	mObstacles.second().clear();
	set_parameters_defaults(STEERPLUGIN);
	set_parameters_defaults(STEERVEHICLE);
	//
	mNavMeshes.clear();
	mNavMeshesParameterTable.clear();
	mCrowdAgents.clear();
	mCrowdAgentsParameterTable.clear();
	set_parameters_defaults(NAVMESH);
	set_parameters_defaults(CROWDAGENT);
	//
	mUpdateData.clear();
	mUpdateTask.clear();
	//
	if (!mRoot.is_empty())
	{
		mCTrav = new CollisionTraverser();
		mCollisionHandler = new CollisionHandlerQueue();
		mPickerRay = new CollisionRay();
		PT(CollisionNode)pickerNode = new CollisionNode(string("GameAIManager::pickerNode"));
		pickerNode->add_solid(mPickerRay);
		pickerNode->set_from_collide_mask(mMask);
		pickerNode->set_into_collide_mask(BitMask32::all_off());
		mCTrav->add_collider(mRoot.attach_new_node(pickerNode),
				mCollisionHandler);
	}
#ifdef ELY_DEBUG
	mOSDD = NULL;
	if (!mRoot.is_empty())
	{
		//create new DebugDrawer
		mOSDD = new ossup::DebugDrawPanda3d(mRoot);
	}
	mRNDD = NULL;
	if (!mRoot.is_empty())
	{
		//create new DebugDrawer
		mRNDD = new DebugDrawPrimitives(mRoot);
	}
#endif //ELY_DEBUG
}

/**
 *
 */
GameAIManager::~GameAIManager()
{
	PRINT_DEBUG("GameAIManager::~GameAIManager: destroying the singleton manager.");

	//stop any default update
	stop_default_update();
	{
		//destroy all OSSteerVehicles
		PTA(PT(OSSteerVehicle))::iterator iterC = mSteerVehicles.begin();
		while (iterC != mSteerVehicles.end())
		{
			//\see http://stackoverflow.com/questions/596162/can-you-remove-elements-from-a-stdlist-while-iterating-through-it
			//give a chance to OSSteerVehicle to cleanup itself before being destroyed.
			(*iterC)->do_finalize();
			//remove the OSSteerVehicles from the inner list (and from the update task)
			iterC = mSteerVehicles.erase(iterC);
		}

		//destroy all OSSteerPlugIns
		PTA(PT(OSSteerPlugIn))::iterator iterN = mSteerPlugIns.begin();
		while (iterN != mSteerPlugIns.end())
		{
			//\see http://stackoverflow.com/questions/596162/can-you-remove-elements-from-a-stdlist-while-iterating-through-it
			//give a chance to OSSteerVehicle to cleanup itself before being destroyed.
			(*iterN)->do_finalize();
			//remove the OSSteerVehicles from the inner list (and from the update task)
			iterN = mSteerPlugIns.erase(iterN);
		}
	}
	{
		//destroy all RNCrowdAgents
		PTA(PT(RNCrowdAgent))::iterator iterC = mCrowdAgents.begin();
		while (iterC != mCrowdAgents.end())
		{
			//\see http://stackoverflow.com/questions/596162/can-you-remove-elements-from-a-stdlist-while-iterating-through-it
			//give a chance to RNCrowdAgent to cleanup itself before being destroyed.
			(*iterC)->do_finalize();
			//remove the RNCrowdAgents from the inner list (and from the update task)
			iterC = mCrowdAgents.erase(iterC);
		}

		//destroy all RNNavMeshes
		PTA(PT(RNNavMesh))::iterator iterN = mNavMeshes.begin();
		while (iterN != mNavMeshes.end())
		{
			//\see http://stackoverflow.com/questions/596162/can-you-remove-elements-from-a-stdlist-while-iterating-through-it
			//give a chance to RNCrowdAgent to cleanup itself before being destroyed.
			(*iterN)->do_finalize();
			//remove the RNCrowdAgents from the inner list (and from the update task)
			iterN = mNavMeshes.erase(iterN);
		}
	}
	//clear parameters' tables
	mSteerPlugInsParameterTable.clear();
	mSteerVehiclesParameterTable.clear();
	mNavMeshesParameterTable.clear();
	mCrowdAgentsParameterTable.clear();
	//
	delete mCTrav;

#ifdef ELY_DEBUG
	if (mOSDD)
	{
		delete mOSDD;
		mOSDD = NULL;
	}
	if (mRNDD)
	{
		delete mRNDD;
		mRNDD = NULL;
	}
#endif //ELY_DEBUG
}

/**
 * Creates a OSSteerPlugIn.
 * Returns a NodePath to the new OSSteerPlugIn,or an empty NodePath with the
 * ET_fail error type set on error.
 */
NodePath GameAIManager::create_steer_plug_in()
{
	PT(OSSteerPlugIn) newSteerPlugIn = new OSSteerPlugIn();
	nassertr_always(newSteerPlugIn, NodePath::fail())

	// set reference nodes
	newSteerPlugIn->mReferenceNP = mReferenceNP;
	newSteerPlugIn->mReferenceDebugNP = mReferenceDebugNP;
	newSteerPlugIn->mReferenceDebug2DNP = mReferenceDebug2DNP;
	// initialize the new SteerPlugIn
	newSteerPlugIn->do_initialize();

	// add the new SteerPlugIn to the inner list (and to the update task)
	mSteerPlugIns.push_back(newSteerPlugIn);
	// reparent to reference node
	NodePath np = mReferenceNP.attach_new_node(newSteerPlugIn);
	//
	return np;
}

/**
 * Destroys a OSSteerPlugIn.
 * Returns false on error.
 */
bool GameAIManager::destroy_steer_plug_in(NodePath steerPlugInNP)
{
	CONTINUE_IF_ELSE_R(
			steerPlugInNP.node()->is_of_type(OSSteerPlugIn::get_class_type()),
			false)

	PT(OSSteerPlugIn)steerPlugIn = DCAST(OSSteerPlugIn, steerPlugInNP.node());
	SteerPlugInList::iterator iter = find(mSteerPlugIns.begin(),
			mSteerPlugIns.end(), steerPlugIn);
	CONTINUE_IF_ELSE_R(iter != mSteerPlugIns.end(), false)

	//give a chance to OSSteerPlugIn to cleanup itself before being destroyed.
	steerPlugIn->do_finalize();

	//remove the OSSteerPlugIn from the inner list (and from the update task)
	mSteerPlugIns.erase(iter);
	//
	return true;
}

/**
 * Gets an OSSteerPlugIn by index, or NULL on error.
 */
PT(OSSteerPlugIn) GameAIManager::get_steer_plug_in(int index) const
{
	nassertr_always((index >= 0) && (index < (int ) mSteerPlugIns.size()),
			NULL)

	return mSteerPlugIns[index];
}

/**
 * Creates a OSSteerVehicle with a given (mandatory and not empty) name.
 * Returns a NodePath to the new OSSteerVehicle,or an empty NodePath with the
 * ET_fail error type set on error.
 */
NodePath GameAIManager::create_steer_vehicle(const string& name)
{
	nassertr_always(!name.empty(), NodePath::fail())

	PT(OSSteerVehicle)newSteerVehicle = new OSSteerVehicle(name);
	nassertr_always(newSteerVehicle, NodePath::fail())

	// set reference node
	newSteerVehicle->mReferenceNP = mReferenceNP;
	//initialize the new SteerVehicle
	newSteerVehicle->do_initialize();

	//add the new SteerVehicle to the inner list
	mSteerVehicles.push_back(newSteerVehicle);
	// reparent to reference node and set "this" NodePath
	newSteerVehicle->mThisNP = mReferenceNP.attach_new_node(newSteerVehicle);
	//
	return newSteerVehicle->mThisNP;
}

/**
 * Destroys a OSSteerVehicle.
 * Returns false on error.
 */
bool GameAIManager::destroy_steer_vehicle(NodePath steerVehicleNP)
{
	CONTINUE_IF_ELSE_R(
			steerVehicleNP.node()->is_of_type(OSSteerVehicle::get_class_type()),
			false)

	PT(OSSteerVehicle)steerVehicle = DCAST(OSSteerVehicle, steerVehicleNP.node());
	SteerVehicleList::iterator iter = find(mSteerVehicles.begin(),
			mSteerVehicles.end(), steerVehicle);
	CONTINUE_IF_ELSE_R(iter != mSteerVehicles.end(), false)

	//give a chance to OSSteerVehicle to cleanup itself before being destroyed.
	steerVehicle->do_finalize();
	//remove the OSSteerVehicle from the inner list (and from the update task)
	mSteerVehicles.erase(iter);
	//
	return true;
}

/**
 * Gets an OSSteerVehicle by index, or NULL on error.
 */
PT(OSSteerVehicle) GameAIManager::get_steer_vehicle(int index) const
{
	nassertr_always((index >= 0) && (index < (int ) mSteerVehicles.size()),
			NULL)

	return mSteerVehicles[index];
}

/**
 * Creates a RNNavMesh.
 * Returns a NodePath to the new RNNavMesh,or an empty NodePath with the ET_fail
 * error type set on error.
 */
NodePath GameAIManager::create_nav_mesh()
{
	PT(RNNavMesh) newNavMesh = new RNNavMesh();
	nassertr_always(newNavMesh, NodePath::fail())

	// set reference nodes
	newNavMesh->mReferenceNP = mReferenceNP;
	newNavMesh->mReferenceDebugNP = mReferenceDebugNP;
	// initialize the new NavMesh
	newNavMesh->do_initialize();

	// add the new NavMesh to the inner list (and to the update task)
	mNavMeshes.push_back(newNavMesh);
	// reparent to reference node
	NodePath np = mReferenceNP.attach_new_node(newNavMesh);
	//
	return np;
}

/**
 * Destroys a RNNavMesh.
 * Returns false on error.
 */
bool GameAIManager::destroy_nav_mesh(NodePath navMeshNP)
{
	CONTINUE_IF_ELSE_R(navMeshNP.node()->is_of_type(RNNavMesh::get_class_type()),
			false)

	PT(RNNavMesh) navMesh = DCAST(RNNavMesh, navMeshNP.node());
	NavMeshList::iterator iter = find(mNavMeshes.begin(), mNavMeshes.end(),
			navMesh);
	CONTINUE_IF_ELSE_R(iter != mNavMeshes.end(), false)

	//give a chance to NavMesh to cleanup itself before being destroyed.
	navMesh->do_finalize();

	//remove the NavMesh from the inner list (and from the update task)
	mNavMeshes.erase(iter);
	//
	return true;
}

/**
 * Gets an RNNavMesh by index, or NULL on error.
 */
PT(RNNavMesh) GameAIManager::get_nav_mesh(int index) const
{
	nassertr_always((index >= 0) && (index < (int ) mNavMeshes.size()),
			NULL);

	return mNavMeshes[index];
}

/**
 * Creates a RNCrowdAgent with a given (mandatory and not empty) name.
 * Returns a NodePath to the new RNCrowdAgent,or an empty NodePath with the
 * ET_fail error type set on error.
 */
NodePath GameAIManager::create_crowd_agent(const string& name)
{
	nassertr_always(!name.empty(), NodePath::fail())

	PT(RNCrowdAgent)newCrowdAgent = new RNCrowdAgent(name);
	nassertr_always(newCrowdAgent, NodePath::fail())

	// set reference node
	newCrowdAgent->mReferenceNP = mReferenceNP;
	//initialize the new CrowdAgent
	newCrowdAgent->do_initialize();

	//add the new CrowdAgent to the inner list
	mCrowdAgents.push_back(newCrowdAgent);
	// reparent to reference node and set "this" NodePath
	newCrowdAgent->mThisNP = mReferenceNP.attach_new_node(newCrowdAgent);
	//
	return newCrowdAgent->mThisNP;
}

/**
 * Destroys a RNCrowdAgent.
 * Returns false on error.
 */
bool GameAIManager::destroy_crowd_agent(NodePath crowdAgentNP)
{
	CONTINUE_IF_ELSE_R(
			crowdAgentNP.node()->is_of_type(RNCrowdAgent::get_class_type()),
			false)

	PT(RNCrowdAgent)crowdAgent = DCAST(RNCrowdAgent, crowdAgentNP.node());
	CrowdAgentList::iterator iter = find(mCrowdAgents.begin(),
			mCrowdAgents.end(), crowdAgent);
	CONTINUE_IF_ELSE_R(iter != mCrowdAgents.end(), false)

	//give a chance to CrowdAgent to cleanup itself before being destroyed.
	crowdAgent->do_finalize();
	//remove the CrowdAgent from the inner list (and from the update task)
	mCrowdAgents.erase(iter);
	//
	return true;
}

/**
 * Gets an RNCrowdAgent by index, or NULL on error.
 */
PT(RNCrowdAgent) GameAIManager::get_crowd_agent(int index) const
{
	nassertr_always((index >= 0) && (index < (int ) mCrowdAgents.size()),
			NULL);

	return mCrowdAgents[index];
}

/**
 * Sets a multi-valued parameter to a multi-value overwriting the existing one(s).
 */
void GameAIManager::set_parameter_values(AIType type, const string& paramName,
		const ValueList<string>& paramValues)
{
	pair<ParameterTableIter, ParameterTableIter> iterRange;

	if (type == STEERPLUGIN)
	{
		//find from mParameterTable the paramName's values to be overwritten
		iterRange = mSteerPlugInsParameterTable.equal_range(paramName);
		//...and erase them
		mSteerPlugInsParameterTable.erase(iterRange.first, iterRange.second);
		//insert the new values
		for (int idx = 0; idx < paramValues.size(); ++idx)
		{
			mSteerPlugInsParameterTable.insert(
					ParameterNameValue(paramName, paramValues[idx]));
		}
		return;
	}
	if (type == STEERVEHICLE)
	{
		//find from mParameterTable the paramName's values to be overwritten
		iterRange = mSteerVehiclesParameterTable.equal_range(paramName);
		//...and erase them
		mSteerVehiclesParameterTable.erase(iterRange.first, iterRange.second);
		//insert the new values
		for (int idx = 0; idx < paramValues.size(); ++idx)
		{
			mSteerVehiclesParameterTable.insert(
					ParameterNameValue(paramName, paramValues[idx]));
		}
		return;
	}
	if (type == NAVMESH)
	{
		//find from mParameterTable the paramName's values to be overwritten
		iterRange = mNavMeshesParameterTable.equal_range(paramName);
		//...and erase them
		mNavMeshesParameterTable.erase(iterRange.first, iterRange.second);
		//insert the new values
		for (int idx = 0; idx < paramValues.size(); ++idx)
		{
			mNavMeshesParameterTable.insert(
					ParameterNameValue(paramName, paramValues[idx]));
		}
		return;
	}
	if (type == CROWDAGENT)
	{
		//find from mParameterTable the paramName's values to be overwritten
		iterRange = mCrowdAgentsParameterTable.equal_range(paramName);
		//...and erase them
		mCrowdAgentsParameterTable.erase(iterRange.first, iterRange.second);
		//insert the new values
		for (int idx = 0; idx < paramValues.size(); ++idx)
		{
			mCrowdAgentsParameterTable.insert(
					ParameterNameValue(paramName, paramValues[idx]));
		}
		return;
	}
}

/**
 * Gets the multiple values of a (actually set) parameter.
 */
ValueList<string> GameAIManager::get_parameter_values(AIType type,
		const string& paramName) const
{
	ValueList<string> strList;
	ParameterTableConstIter iter;
	pair<ParameterTableConstIter, ParameterTableConstIter> iterRange;

	if (type == STEERPLUGIN)
	{
		iterRange = mSteerPlugInsParameterTable.equal_range(paramName);
		if (iterRange.first != iterRange.second)
		{
			for (iter = iterRange.first; iter != iterRange.second; ++iter)
			{
				strList.add_value(iter->second);
			}
		}
		return strList;
	}
	if (type == STEERVEHICLE)
	{
		iterRange = mSteerVehiclesParameterTable.equal_range(paramName);
		if (iterRange.first != iterRange.second)
		{
			for (iter = iterRange.first; iter != iterRange.second; ++iter)
			{
				strList.add_value(iter->second);
			}
		}
		return strList;
	}
	if (type == NAVMESH)
	{
		iterRange = mNavMeshesParameterTable.equal_range(paramName);
		if (iterRange.first != iterRange.second)
		{
			for (iter = iterRange.first; iter != iterRange.second; ++iter)
			{
				strList.add_value(iter->second);
			}
		}
		return strList;
	}
	if (type == CROWDAGENT)
	{
		iterRange = mCrowdAgentsParameterTable.equal_range(paramName);
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
void GameAIManager::set_parameter_value(AIType type, const string& paramName,
		const string& value)
{
	ValueList<string> valueList;
	valueList.add_value(value);
	set_parameter_values(type, paramName, valueList);
}

/**
 * Gets a single value (i.e. the first one) of a parameter.
 */
string GameAIManager::get_parameter_value(AIType type,
		const string& paramName) const
{
	ValueList<string> valueList = get_parameter_values(type, paramName);
	return (valueList.size() != 0 ? valueList[0] : string(""));
}

/**
 * Gets a list of the names of the parameters actually set.
 */
ValueList<string> GameAIManager::get_parameter_name_list(AIType type) const
{
	ValueList<string> strList;
	ParameterTableIter iter;
	ParameterTable tempTable;

	if (type == STEERPLUGIN)
	{
		tempTable = mSteerPlugInsParameterTable;
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
	if (type == STEERVEHICLE)
	{
		tempTable = mSteerVehiclesParameterTable;
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
	if (type == NAVMESH)
	{
		tempTable = mNavMeshesParameterTable;
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
	if (type == CROWDAGENT)
	{
		tempTable = mCrowdAgentsParameterTable;
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
void GameAIManager::set_parameters_defaults(AIType type)
{
	if (type == STEERPLUGIN)
	{
		///mSteerPlugInsParameterTable must be the first cleared
		mSteerPlugInsParameterTable.clear();
		//sets the (mandatory) parameters to their default values:
		mSteerPlugInsParameterTable.insert(
				ParameterNameValue("plugin_type", "one_turning"));
		mSteerPlugInsParameterTable.insert(
				ParameterNameValue("pathway",
						"0.0,0.0,0.0:1E-4,1E-4,1E-4$0.0$false"));
		return;
	}
	if (type == STEERVEHICLE)
	{
		///mSteerVehiclesParameterTable must be the first cleared
		mSteerVehiclesParameterTable.clear();
		//sets the (mandatory) parameters to their default values:
		mSteerVehiclesParameterTable.insert(
				ParameterNameValue("vehicle_type", "one_turning"));
		mSteerVehiclesParameterTable.insert(
				ParameterNameValue("external_update", "false"));
		mSteerVehiclesParameterTable.insert(
				ParameterNameValue("mov_type", "opensteer"));
		mSteerVehiclesParameterTable.insert(
				ParameterNameValue("up_axis_fixed", "false"));
		mSteerVehiclesParameterTable.insert(
				ParameterNameValue("up_axis_fixed_mode", "light"));
		mSteerVehiclesParameterTable.insert(ParameterNameValue("mass", "1.0"));
		mSteerVehiclesParameterTable.insert(ParameterNameValue("speed", "0.0"));
		mSteerVehiclesParameterTable.insert(
				ParameterNameValue("max_force", "0.1"));
		mSteerVehiclesParameterTable.insert(
				ParameterNameValue("max_speed", "1.0"));
		mSteerVehiclesParameterTable.insert(
				ParameterNameValue("radius", "1.0"));
		mSteerVehiclesParameterTable.insert(
				ParameterNameValue("path_pred_time", "3.0"));
		mSteerVehiclesParameterTable.insert(
				ParameterNameValue("obstacle_min_time_coll", "4.5"));
		mSteerVehiclesParameterTable.insert(
				ParameterNameValue("neighbor_min_time_coll", "3.0"));
		mSteerVehiclesParameterTable.insert(
				ParameterNameValue("neighbor_min_sep_dist", "1.0"));
		mSteerVehiclesParameterTable.insert(
				ParameterNameValue("separation_max_dist", "5.0"));
		mSteerVehiclesParameterTable.insert(
				ParameterNameValue("separation_cos_max_angle", "-0.707"));
		mSteerVehiclesParameterTable.insert(
				ParameterNameValue("alignment_max_dist", "7.5"));
		mSteerVehiclesParameterTable.insert(
				ParameterNameValue("alignment_cos_max_angle", "0.7"));
		mSteerVehiclesParameterTable.insert(
				ParameterNameValue("cohesion_max_dist", "9.0"));
		mSteerVehiclesParameterTable.insert(
				ParameterNameValue("cohesion_cos_max_angle", "-0.15"));
		mSteerVehiclesParameterTable.insert(
				ParameterNameValue("pursuit_max_pred_time", "20.0"));
		mSteerVehiclesParameterTable.insert(
				ParameterNameValue("evasion_max_pred_time", "20.0"));
		mSteerVehiclesParameterTable.insert(
				ParameterNameValue("target_speed", "1.0"));
		return;
	}
	if (type == NAVMESH)
	{
		///mNavMeshesParameterTable must be the first cleared
		mNavMeshesParameterTable.clear();
		//sets the (mandatory) parameters to their default values:
		mNavMeshesParameterTable.insert(
				ParameterNameValue("navmesh_type", "solo"));
		mNavMeshesParameterTable.insert(ParameterNameValue("cell_size", "0.3"));
		mNavMeshesParameterTable.insert(
				ParameterNameValue("cell_height", "0.2"));
		mNavMeshesParameterTable.insert(
				ParameterNameValue("agent_height", "2.0"));
		mNavMeshesParameterTable.insert(
				ParameterNameValue("agent_radius", "0.6"));
		mNavMeshesParameterTable.insert(
				ParameterNameValue("agent_max_climb", "0.9"));
		mNavMeshesParameterTable.insert(
				ParameterNameValue("agent_max_slope", "45.0"));
		mNavMeshesParameterTable.insert(
				ParameterNameValue("region_min_size", "8"));
		mNavMeshesParameterTable.insert(
				ParameterNameValue("region_merge_size", "20"));
		mNavMeshesParameterTable.insert(
				ParameterNameValue("partition_type", "watershed"));
		mNavMeshesParameterTable.insert(
				ParameterNameValue("edge_max_len", "12.0"));
		mNavMeshesParameterTable.insert(
				ParameterNameValue("edge_max_error", "1.3"));
		mNavMeshesParameterTable.insert(
				ParameterNameValue("verts_per_poly", "6.0"));
		mNavMeshesParameterTable.insert(
				ParameterNameValue("detail_sample_dist", "6.0"));
		mNavMeshesParameterTable.insert(
				ParameterNameValue("detail_sample_max_error", "1.0"));
		//nav mesh tile
		mNavMeshesParameterTable.insert(
				ParameterNameValue("build_all_tiles", "false"));
		mNavMeshesParameterTable.insert(ParameterNameValue("max_tiles", "128"));
		mNavMeshesParameterTable.insert(
				ParameterNameValue("max_polys_per_tile", "32768"));
		mNavMeshesParameterTable.insert(ParameterNameValue("tile_size", "32"));
		//area flags cost
		//NAVMESH_POLYAREA_GROUND@NAVMESH_POLYFLAGS_WALK@1.0
		mNavMeshesParameterTable.insert(
				ParameterNameValue("area_flags_cost", "0@0x01@1.0"));
		//NAVMESH_POLYAREA_WATER@NAVMESH_POLYFLAGS_SWIM@10.0
		mNavMeshesParameterTable.insert(
				ParameterNameValue("area_flags_cost", "1@0x02@10.0"));
		//NAVMESH_POLYAREA_ROAD@NAVMESH_POLYFLAGS_WALK@1.0
		mNavMeshesParameterTable.insert(
				ParameterNameValue("area_flags_cost", "2@0x01@1.0"));
		//NAVMESH_POLYAREA_DOOR@NAVMESH_POLYFLAGS_WALK:NAVMESH_POLYFLAGS_DOOR@1.0
		mNavMeshesParameterTable.insert(
				ParameterNameValue("area_flags_cost", "3@0x01:0x04@1.0"));
		//NAVMESH_POLYAREA_GRASS@NAVMESH_POLYFLAGS_WALK@2.0
		mNavMeshesParameterTable.insert(
				ParameterNameValue("area_flags_cost", "4@0x01@2.0"));
		//NAVMESH_POLYAREA_JUMP@NAVMESH_POLYFLAGS_JUMP@1.5
		mNavMeshesParameterTable.insert(
				ParameterNameValue("area_flags_cost", "5@0x08@1.5"));
		//crowd include flags = NAVMESH_POLYFLAGS_ALL ^ NAVMESH_POLYFLAGS_DISABLED = 0xffef
		mNavMeshesParameterTable.insert(
				ParameterNameValue("crowd_include_flags", "0xffef"));
		//crowd exclude flags = NAVMESH_POLYFLAGS_DISABLED = 0x10
		mNavMeshesParameterTable.insert(
				ParameterNameValue("crowd_exclude_flags", "0x10"));
		return;
	}
	if (type == CROWDAGENT)
	{
		///mCrowdAgentsParameterTable must be the first cleared
		mCrowdAgentsParameterTable.clear();
		//sets the (mandatory) parameters to their default values:
		mCrowdAgentsParameterTable.insert(
				ParameterNameValue("add_to_navmesh", ""));
		mCrowdAgentsParameterTable.insert(
				ParameterNameValue("mov_type", "recast"));
		mCrowdAgentsParameterTable.insert(
				ParameterNameValue("move_target", "0.0,0.0,0.0"));
		mCrowdAgentsParameterTable.insert(
				ParameterNameValue("move_velocity", "0.0,0.0,0.0"));
		mCrowdAgentsParameterTable.insert(
				ParameterNameValue("max_acceleration", "8.0"));
		mCrowdAgentsParameterTable.insert(
				ParameterNameValue("max_speed", "3.5"));
		mCrowdAgentsParameterTable.insert(
				ParameterNameValue("collision_query_range", "12.0"));
		mCrowdAgentsParameterTable.insert(
				ParameterNameValue("path_optimization_range", "30.0"));
		mCrowdAgentsParameterTable.insert(
				ParameterNameValue("separation_weight", "2.0"));
		mCrowdAgentsParameterTable.insert(
				ParameterNameValue("update_flags", "0x1b"));
		mCrowdAgentsParameterTable.insert(
				ParameterNameValue("obstacle_avoidance_type", "3"));
		mCrowdAgentsParameterTable.insert(
				ParameterNameValue("ray_mask", "all_on"));
		return;
	}
}

/**
 * Updates ai objects.
 *
 * Will be called automatically in a task.
 */
AsyncTask::DoneStatus GameAIManager::update(GenericAsyncTask* task)
{
	float dt = ClockObject::get_global_clock()->get_dt();

#ifdef TESTING
	dt = 0.016666667; //60 fps
#endif

	// call all OSSteerPlugIns' update functions, passing delta time
	for (PTA(PT(OSSteerPlugIn))::size_type index = 0;
			index < mSteerPlugIns.size(); ++index)
	{
		mSteerPlugIns[index]->update(dt);
	}
	// call all RNNavMeshes' update functions, passing delta time
	for (PTA(PT(RNNavMesh))::size_type index = 0;
			index < mNavMeshes.size(); ++index)
	{
		mNavMeshes[index]->update(dt);
	}
	//
	return AsyncTask::DS_cont;
}


/**
 * Adds a task to repeatedly call ai updates.
 */
void GameAIManager::start_default_update()
{
	//create the task for updating AI objects
	mUpdateData = new TaskInterface<GameAIManager>::TaskData(this,
			&GameAIManager::update);
	mUpdateTask = new GenericAsyncTask(string("GameAIManager::update"),
			&TaskInterface<GameAIManager>::taskFunction,
			reinterpret_cast<void*>(mUpdateData.p()));
	mUpdateTask->set_sort(mTaskSort);
	//Adds mUpdateTask to the active queue.
	AsyncTaskManager::get_global_ptr()->add(mUpdateTask);
}

/**
 * Removes a task to repeatedly call ai updates.
 */
void GameAIManager::stop_default_update()
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
 * Returns the obstacle's settings with the specified unique reference (>0).
 * Returns OSObstacleSettings::ref == a negative number on error.
 */
OSObstacleSettings GameAIManager::get_obstacle_settings(int ref) const
{
	OSObstacleSettings settings = OSObstacleSettings();
	settings.set_ref(AI_ERROR);
	CONTINUE_IF_ELSE_R(ref > 0, settings)

	// find settings by ref
	pvector<ObstacleAttributes>& obstacleAttrs =
			const_cast<GlobalObstacles&>(mObstacles).second();
	pvector<ObstacleAttributes>::iterator iter;
	for (iter = obstacleAttrs.begin(); iter != obstacleAttrs.end(); ++iter)
	{
		if ((*iter).first().get_ref() == ref)
		{
			settings = (*iter).first();
			break;
		}
	}
	//
	return settings;
}

/**
 * Returns the NodePath of the obstacle with the specified unique reference (>0).
 * Return an empty NodePath with the ET_fail error type set on error.
 */
NodePath GameAIManager::get_obstacle_by_ref(int ref) const
{
	NodePath obstacleNP = NodePath::fail();
	CONTINUE_IF_ELSE_R(ref > 0, obstacleNP)

	// find settings by ref
	pvector<ObstacleAttributes>& obstacleAttrs =
			const_cast<GlobalObstacles&>(mObstacles).second();
	pvector<ObstacleAttributes>::iterator iter;
	for (iter = obstacleAttrs.begin(); iter != obstacleAttrs.end(); ++iter)
	{
		if ((*iter).first().get_ref() == ref)
		{
			obstacleNP = (*iter).second();
			break;
		}
	}
	return obstacleNP;
}

/**
 * Gets bounding dimensions of a model NodePath.
 * Puts results into the out parameters: modelDims, modelDeltaCenter and returns
 * modelRadius.
 * - modelDims = absolute dimensions of the model
 * - modelCenter + modelDeltaCenter = origin of coordinate system
 * - modelRadius = radius of the containing sphere
 */
float GameAIManager::get_bounding_dimensions(NodePath modelNP,
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
Pair<bool,float> GameAIManager::get_collision_height(const LPoint3f& rayOrigin,
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
 * Draws the specified primitive, given the points, the color (RGBA) and point's size.
 */
void GameAIManager::debug_draw_primitive(OSDebugDrawPrimitives primitive,
		const ValueList<LPoint3f>& points, const LVecBase4f color, float size)
{
#ifdef ELY_DEBUG
	mOSDD->begin((ossup::DrawMeshDrawer::DrawPrimitive)primitive, size);
	// calculate the real point list size
	unsigned int realSize;
	switch (primitive)
	{
	case OS_POINTS:
		realSize = points.size();
		break;
	case OS_LINES:
		realSize = points.size() - (points.size() % 2);
		break;
	case OS_TRIS:
		realSize = points.size() - (points.size() % 3);
		break;
	case OS_QUADS:
		realSize = points.size() - (points.size() % 4);
		break;
	default:
		break;
	}
	for (unsigned int i = 0; i < realSize; ++i)
	{
		mOSDD->vertex(points[i], color);
	}
	mOSDD->end();
#endif //ELY_DEBUG
}

/**
 * Draws the specified primitive, given the points, the color (RGBA) and point's size.
 */
void GameAIManager::debug_draw_primitive(RNDebugDrawPrimitives primitive,
		const ValueList<LPoint3f>& points, const LVecBase4f color, float size)
{
#ifdef ELY_DEBUG
	mRNDD->begin((duDebugDrawPrimitives) primitive, size);
	// calculate the real point list size
	unsigned int realSize;
	switch (primitive)
	{
	case RN_POINTS:
		realSize = points.size();
		break;
	case RN_LINES:
		realSize = points.size() - (points.size() % 2);
		break;
	case RN_TRIS:
		realSize = points.size() - (points.size() % 3);
		break;
	case RN_QUADS:
		realSize = points.size() - (points.size() % 4);
		break;
	default:
		break;
	}
	for (unsigned int i = 0; i < realSize; ++i)
	{
		mRNDD->vertex(points[i], color);
	}
	mRNDD->end();
#endif //ELY_DEBUG
}

/**
 * Erases all primitives drawn until now.
 */
void GameAIManager::debug_draw_reset(OSDebugDrawPrimitives)
{
#ifdef ELY_DEBUG
	mOSDD->reset();
#endif //ELY_DEBUG
}

/**
 * Erases all primitives drawn until now.
 */
void GameAIManager::debug_draw_reset(RNDebugDrawPrimitives)
{
#ifdef ELY_DEBUG
	mRNDD->reset();
#endif //ELY_DEBUG
}

/**
 * Writes to a bam file the entire collections of ai objects and related
 * geometries (i.e. models' NodePaths)
 */
bool GameAIManager::write_to_bam_file(const string& fileName)
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
				<< "SUCCESS: all ai object collections were written to "
				<< fileName << endl;
	}
	else
	{
		cerr << errorReport << endl;
	}
	return errorReport.empty();
}

/**
 * Reads from a bam file the entire hierarchy of ai objects and related
 * geometries (i.e. models' NodePaths)
 */
bool GameAIManager::read_from_bam_file(const string& fileName)
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
		cout << "SUCCESS: all ai objects were read from "
				<< fileName << endl;
	}
	else
	{
		cerr << errorReport << endl;
	}
	return errorReport.empty();
}

//TypedObject semantics: hardcoded
TypeHandle GameAIManager::_type_handle;
