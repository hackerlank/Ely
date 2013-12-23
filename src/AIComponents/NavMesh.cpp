/*
 *   This file is part of Ely.
 *
 *   Ely is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   Ely is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with Ely.  If not, see <http://www.gnu.org/licenses/>.
 */
/**
 * \file /Ely/src/AIComponents/NavMesh.cpp
 *
 * \date 23/giu/2013 (18:51:03)
 * \author consultit
 */

#include "AIComponents/NavMesh.h"
#include "AIComponents/NavMeshTemplate.h"
#include "AIComponents/RecastNavigation/NavMeshType_Solo.h"
#include "AIComponents/RecastNavigation/NavMeshType_Tile.h"
#include "AIComponents/RecastNavigation/NavMeshType_Obstacle.h"
#include "AIComponents/RecastNavigation/ConvexVolumeTool.h"
#include "AIComponents/RecastNavigation/OffMeshConnectionTool.h"
#include "AIComponents/RecastNavigation/CrowdTool.h"
#include "ObjectModel/Object.h"
#include "ObjectModel/ObjectTemplateManager.h"
#include "Game/GameAIManager.h"
#include "Game/GamePhysicsManager.h"

namespace ely
{

NavMesh::NavMesh()
#ifdef ELY_THREAD
:mAsyncSetupVar(mAsyncSetupMutex), mAsyncSetupComplete(true)
#endif
{
	// TODO Auto-generated constructor stub
}

NavMesh::NavMesh(SMARTPTR(NavMeshTemplate)tmpl)
#ifdef ELY_THREAD
:mAsyncSetupVar(mAsyncSetupMutex), mAsyncSetupComplete(true)
#endif
{
	CHECK_EXISTENCE_DEBUG(GameAIManager::GetSingletonPtr(),
			"NavMesh::NavMesh: invalid GameAIManager")

	mTmpl = tmpl;
	reset();
}

NavMesh::~NavMesh()
{
}

ComponentFamilyType NavMesh::familyType() const
{
	return mTmpl->familyType();
}

ComponentType NavMesh::componentType() const
{
	return mTmpl->componentType();
}

bool NavMesh::initialize()
{
	bool result = true;
	//set NavMesh parameters (store internally for future use)
	//navmesh type
	std::string navMeshtypeStr = mTmpl->parameter(std::string("navmesh_type"));
	if (navMeshtypeStr == std::string("tile"))
	{
		mNavMeshTypeEnum = TILE;
	}
	else if (navMeshtypeStr == std::string("obstacle"))
	{
		mNavMeshTypeEnum = OBSTACLE;
	}
	else
	{
		mNavMeshTypeEnum = SOLO;
	}
	//auto setup
	mAutoSetup = (
			mTmpl->parameter(std::string("auto_setup"))
					== std::string("false") ? false : true);
	//mov type
	std::string movType = mTmpl->parameter(std::string("mov_type"));
	if (movType == std::string("kinematic"))
	{
		CHECK_EXISTENCE_DEBUG(GamePhysicsManager::GetSingletonPtr(),
				"NavMesh::initialize: invalid GamePhysicsManager")
		mMovType = RECAST_KINEMATIC;
	}
	else
	{
		mMovType = RECAST;
	}
	//
	float value;
	int valueInt;
	//cell size
	value = strtof(mTmpl->parameter(std::string("cell_size")).c_str(), NULL);
	mNavMeshSettings.m_cellSize = (value >= 0.0 ? value : -value);
	//cell height
	value = strtof(mTmpl->parameter(std::string("cell_height")).c_str(), NULL);
	mNavMeshSettings.m_cellHeight = (value >= 0.0 ? value : -value);
	//agent height
	value = strtof(mTmpl->parameter(std::string("agent_height")).c_str(), NULL);
	mNavMeshSettings.m_agentHeight = (value >= 0.0 ? value : -value);
	//agent radius
	value = strtof(mTmpl->parameter(std::string("agent_radius")).c_str(), NULL);
	mNavMeshSettings.m_agentRadius = (value >= 0.0 ? value : -value);
	//agent max climb
	value = strtof(mTmpl->parameter(std::string("agent_max_climb")).c_str(),
	NULL);
	mNavMeshSettings.m_agentMaxClimb = (value >= 0.0 ? value : -value);
	//agent max slope
	value = strtof(mTmpl->parameter(std::string("agent_max_slope")).c_str(),
	NULL);
	mNavMeshSettings.m_agentMaxSlope = (value >= 0.0 ? value : -value);
	//region min size
	value = strtof(mTmpl->parameter(std::string("region_min_size")).c_str(),
	NULL);
	mNavMeshSettings.m_regionMinSize = (value >= 0.0 ? value : -value);
	//region merge size
	value = strtof(mTmpl->parameter(std::string("region_merge_size")).c_str(),
	NULL);
	mNavMeshSettings.m_regionMergeSize = (value >= 0.0 ? value : -value);
	//monotone partitioning
	mNavMeshSettings.m_monotonePartitioning = (
			mTmpl->parameter(std::string("monotone_partitioning"))
					== std::string("true") ? true : false);
	//edge max len
	value = strtof(mTmpl->parameter(std::string("edge_max_len")).c_str(), NULL);
	mNavMeshSettings.m_edgeMaxLen = (value >= 0.0 ? value : -value);
	//edge max error
	value = strtof(mTmpl->parameter(std::string("edge_max_error")).c_str(),
	NULL);
	mNavMeshSettings.m_edgeMaxError = (value >= 0.0 ? value : -value);
	//verts per poly
	value = strtof(mTmpl->parameter(std::string("verts_per_poly")).c_str(),
	NULL);
	mNavMeshSettings.m_vertsPerPoly = (value >= 0.0 ? value : -value);
	//detail sample dist
	value = strtof(mTmpl->parameter(std::string("detail_sample_dist")).c_str(),
	NULL);
	mNavMeshSettings.m_detailSampleDist = (value >= 0.0 ? value : -value);
	//detail sample max error
	value = strtof(
			mTmpl->parameter(std::string("detail_sample_max_error")).c_str(),
			NULL);
	mNavMeshSettings.m_detailSampleMaxError = (value >= 0.0 ? value : -value);
	//build all tiles
	mNavMeshTileSettings.m_buildAllTiles = (
			mTmpl->parameter(std::string("build_all_tiles"))
					== std::string("true") ? true : false);
	//max tiles
	valueInt = strtol(mTmpl->parameter(std::string("max_tiles")).c_str(), NULL,
			0);
	mNavMeshTileSettings.m_maxTiles = (valueInt >= 0 ? valueInt : -valueInt);
	//max polys per tile
	valueInt = strtol(
			mTmpl->parameter(std::string("max_polys_per_tile")).c_str(), NULL,
			0);
	mNavMeshTileSettings.m_maxPolysPerTile = (
			valueInt >= 0 ? valueInt : -valueInt);
	//tile size
	value = strtof(mTmpl->parameter(std::string("tile_size")).c_str(), NULL);
	mNavMeshTileSettings.m_tileSize = (value >= 0.0 ? value : -value);
	//area-flags-cost settings
	mAreaFlagsCostXmlParam = mTmpl->parameterList(
			std::string("area_flags_cost"));
	//crowd include/exclude flags
	mCrowdIncludeFlagsParam = mTmpl->parameter(
			std::string("crowd_include_flags"));
	mCrowdExcludeFlagsParam = mTmpl->parameter(
			std::string("crowd_exclude_flags"));
	//convex volumes
	mConvexVolumesParam = mTmpl->parameterList(std::string("convex_volume"));
	//off mesh connections
	mOffMeshConnectionsParam = mTmpl->parameterList(
			std::string("offmesh_connection"));
	//
	return result;
}

void NavMesh::onAddToObjectSetup()
{
	//setup the build context
	mCtx = new BuildContext;
}

void NavMesh::onRemoveFromObjectCleanup()
{
	//cleanup NavMesh
	doNavMeshCleanup();

	//delete old model mesh
	delete mGeom;

	//remove all handled CrowdAgents (if any)
	std::list<SMARTPTR(CrowdAgent)> crowdAgents = mCrowdAgents;
	std::list<SMARTPTR(CrowdAgent)>::const_iterator iter;
	for (iter = crowdAgents.begin(); iter != crowdAgents.end();
			++iter)
	{
		doRemoveCrowdAgentFromUpdateList(*iter);
		doRemoveCrowdAgentFromRecastUpdate(*iter);
	}

#ifdef ELY_DEBUG
	if (not mDebugCamera.is_empty())
	{
		//set the recast debug camera to empty node path
		mDebugCamera = NodePath();
		//remove the recast debug node path
		mDebugNodePath.remove_node();
	}
	//delete the DebugDrawers
	delete mDD;
	delete mDDM;
#endif

#ifdef ELY_THREAD
	//remove the task chain on which navMeshAsyncSetup() has run
	AsyncTaskManager::get_global_ptr()->remove_task_chain(mTaskChainName);
#endif

	//delete build context
	delete mCtx;
	//
	reset();
}

void NavMesh::onAddToSceneSetup()
{
#ifdef ELY_THREAD
	//add the task chain on which navMeshAsyncSetup() will be running
	mTaskChainName = mComponentId + "-taskChain";
	AsyncTaskManager::get_global_ptr()->make_task_chain(mTaskChainName);
	AsyncTaskManager::get_global_ptr()->
			find_task_chain(mTaskChainName)->set_num_threads(1);
	AsyncTaskManager::get_global_ptr()->
				find_task_chain(mTaskChainName)->set_frame_sync(false);
#endif

	//set mOwnerObject's parent node path as reference
	mReferenceNP = mOwnerObject->getNodePath().get_parent();

#ifdef ELY_DEBUG
	//reset the DebugDrawers
	mDD = NULL;
	mDDM = NULL;
	mDebugRenderData.clear();
	mDebugRenderTask.clear();
	//set the recast debug node path as child of mReferenceNP node path
	//set the recast debug camera to the first child of "camera" node path
	SMARTPTR(Object) cameraDebug = ObjectTemplateManager::GetSingletonPtr()->
			getCreatedObject("camera");
	if (cameraDebug)
	{
		//add a debug node path
		mDebugNodePath = mReferenceNP.attach_new_node(
				"RecastDebugNodePath_" + COMPONENT_STANDARD_NAME);
		mDebugNodePath.set_bin("fixed", 10);
		//by default mDebugNodePath is hidden
		mDebugNodePath.hide();
		//
		mDebugCamera = cameraDebug->getNodePath().get_child(0);
	}
#endif

	///1: get the input from xml
	std::list<std::string>::iterator iterStr;
	///get area ability flags and cost (for crowd)
	NavMeshPolyAreaFlags flagsAreaTable;
	flagsAreaTable.clear();
	NavMeshPolyAreaCost costAreaTable;
	costAreaTable.clear();
	for (iterStr = mAreaFlagsCostXmlParam.begin();
			iterStr != mAreaFlagsCostXmlParam.end(); ++iterStr)
	{
		//any "area_flags" string is a "compound" one, i.e. could have the form:
		// "area@flag1|flag2...|flagN@cost"
		std::vector<std::string> areaFlagsCostStr = parseCompoundString(
				*iterStr, '@');
		//check only if there is a triple
		if (areaFlagsCostStr.size() == 3)
		{
			//default area: NAVMESH_POLYAREA_GROUND (== 0)
			int area = (
					not areaFlagsCostStr[0].empty() ?
							strtol(areaFlagsCostStr[0].c_str(), NULL, 0) :
							NAVMESH_POLYAREA_GROUND);
			//iterate over flags
			std::vector<std::string> flags = parseCompoundString(
					areaFlagsCostStr[1], ':');
			std::vector<std::string>::const_iterator iterF;
			//default flag: NAVMESH_POLYFLAGS_WALK (== 0x01)
			int oredFlags = (flags.size() == 0 ? 0x01 : 0x0);
			for (iterF = flags.begin(); iterF != flags.end(); ++iterF)
			{
				int flag = (
						not (*iterF).empty() ?
								strtol((*iterF).c_str(), NULL, 0) :
								NAVMESH_POLYFLAGS_WALK);
				//or flag
				oredFlags |= flag;
			}
			//add area with corresponding ored ability flags
			flagsAreaTable[area] = oredFlags;

			//default cost: 1.0
			float cost = strtof(areaFlagsCostStr[2].c_str(), NULL);
			if (cost <= 0.0)
			{
				cost = 1.0;
			}
			//add area with corresponding cost
			costAreaTable[area] = cost;
		}
	}
	///get crowd include & exclude flags
	std::vector<std::string>::const_iterator iterIEFStr;
	std::vector<std::string> ieFlagsStr;
	//1:iterate over include flags
	ieFlagsStr = parseCompoundString(mCrowdIncludeFlagsParam, ':');
	//default include flag: NAVMESH_POLYFLAGS_WALK (== 0x01)
	int includeFlags = (ieFlagsStr.empty() ? NAVMESH_POLYFLAGS_WALK : 0x0);
	for (iterIEFStr = ieFlagsStr.begin(); iterIEFStr != ieFlagsStr.end();
			++iterIEFStr)
	{
		int flag = (
				not (*iterIEFStr).empty() ?
						strtol((*iterIEFStr).c_str(), NULL, 0) :
						NAVMESH_POLYFLAGS_WALK);
		//or flag
		includeFlags |= flag;
	}
	//2:iterate over exclude flags
	ieFlagsStr = parseCompoundString(mCrowdExcludeFlagsParam, ':');
	//default exclude flag: NAVMESH_POLYFLAGS_DISABLED (== 0x10)
	int excludeFlags = (ieFlagsStr.empty() ? NAVMESH_POLYFLAGS_DISABLED : 0x0);
	for (iterIEFStr = ieFlagsStr.begin(); iterIEFStr != ieFlagsStr.end();
			++iterIEFStr)
	{
		int flag = (
				not (*iterIEFStr).empty() ?
						strtol((*iterIEFStr).c_str(), NULL, 0) :
						NAVMESH_POLYFLAGS_DISABLED);
		//or flag
		excludeFlags |= flag;
	}
	///get convex volumes
	std::list<PointListArea> convexVolumes;
	for (iterStr = mConvexVolumesParam.begin();
			iterStr != mConvexVolumesParam.end(); ++iterStr)
	{
		//any "convex_volume" string is a "compound" one, i.e. could have the form:
		// "x1,y1,z1:x2,y2,z2...:xN,yN,zN@area_type"
		std::vector<std::string> pointsAreaTypeStr = parseCompoundString(
				*iterStr, '@');
		//check only if there is (at least) a pair
		if (pointsAreaTypeStr.size() < 2)
		{
			continue;
		}
		//an empty point sequence is ignored
		if (pointsAreaTypeStr[0].empty())
		{
			continue;
		}
		//
		int areaType = (
				not pointsAreaTypeStr[1].empty() ?
						strtol(pointsAreaTypeStr[1].c_str(), NULL, 0) :
						NAVMESH_POLYAREA_GROUND);
		//iterate over points
		PointList pointList;
		std::vector<std::string> pointsStr = parseCompoundString(
				pointsAreaTypeStr[0], ':');
		std::vector<std::string>::const_iterator iterP;
		for (iterP = pointsStr.begin(); iterP != pointsStr.end(); ++iterP)
		{
			std::vector<std::string> posStr = parseCompoundString(*iterP, ',');
			LPoint3f point = LPoint3f::zero();
			for (unsigned int i = 0; (i < 3) and (i < posStr.size()); ++i)
			{
				point[i] = strtof(posStr[i].c_str(), NULL);
			}
			//insert the point to the list
			pointList.push_back(point);
		}
		//insert convex volume to the list
		convexVolumes.push_back(PointListArea(pointList, areaType));
	}
	///get off mesh connections
	std::list<PointPairBidir> offMeshConnections;
	for (iterStr = mOffMeshConnectionsParam.begin();
			iterStr != mOffMeshConnectionsParam.end(); ++iterStr)
	{
		//any "offmesh_connection" string is a "compound" one, i.e. has the form:
		// "xB,yB,zB:xE,yE,zE@bidirectional", with bidirectional=true by default.
		std::vector<std::string> pointPairBidirStr = parseCompoundString(
				*iterStr, '@');
		//check only if there is (at least) a pair
		if (pointPairBidirStr.size() < 2)
		{
			continue;
		}
		//an empty point pair is ignored
		if (pointPairBidirStr[0].empty())
		{
			continue;
		}
		bool bidir = (
				std::string(pointPairBidirStr[1]) == std::string("false") ? false : true);
		//iterate over the first 2 points
		//each point defaults to LPoint3f::zero()
		PointPair pointPair(LPoint3f::zero(), LPoint3f::zero());
		std::vector<std::string> pointsStr = parseCompoundString(
				pointPairBidirStr[0], ':');
		std::vector<std::string>::const_iterator iterPStr;
		int k;
		for (k = 0, iterPStr = pointsStr.begin();
				k < 2 and (iterPStr != pointsStr.end()); ++k, ++iterPStr)
		{
			std::vector<std::string> posStr = parseCompoundString(*iterPStr,
					',');
			LPoint3f point = LPoint3f::zero();
			for (unsigned int i = 0; (i < 3) and (i < posStr.size()); ++i)
			{
				point[i] = strtof(posStr[i].c_str(), NULL);
			}
			//insert the point to the pair
			switch (k)
			{
			case 0:
				pointPair.first = point;
				break;
			case 1:
				pointPair.second = point;
				break;
			default:
				break;
			}
		}
		//insert off mesh connection to the list
		offMeshConnections.push_back(PointPairBidir(pointPair, bidir));
	}
	///2: add settings for nav mesh
	///set nav mesh type enum: already done
	///set mov type: already done
	///set nav mesh settings: already done
	///set nav mesh tile settings: already done
	///set area ability flags
	mPolyAreaFlags = flagsAreaTable;
	///set area cost (for crowd)
	mPolyAreaCost = costAreaTable;
	///set crowd include & exclude flags
	mCrowdIncludeFlags = includeFlags;
	mCrowdExcludeFlags = excludeFlags;
	///set convex volumes
	mConvexVolumes = convexVolumes;
	///set off mesh connections
	mOffMeshConnections = offMeshConnections;
	//setup nav mesh if auto setup is true
	if (mAutoSetup)
	{
		///3: do real nav mesh setup
		doNavMeshSetup();
	}
}

void NavMesh::onRemoveFromSceneCleanup()
{
	//remove from AI manager update
	GameAIManager::GetSingletonPtr()->removeFromAIUpdate(this);

#ifdef ELY_THREAD
	HOLD_MUTEX(mAsyncSetupMutex)

	//Note: at this point mDestroying is true so no more
	//NavMeshAsyncSetup will be called.
	//Wait for all NavMeshAsyncSetup calls to finish their execution
	while (mAsyncSetupCallCounter > 0)
	{
		//notify one NavMeshAsyncSetup to finish its execution
		mAsyncSetupVar.notify();
		//wait for more executions to finish
		mAsyncSetupVar.wait();
	}
#endif
}

NavMesh::Result NavMesh::navMeshSetup()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	//return if destroying
	RETURN_ON_ASYNC_COND(mDestroying, NavMesh::Result::DESTROYING)

	//do the real setup
	doNavMeshSetup();
	//
	return Result::OK;
}

void NavMesh::doNavMeshSetup()
{
	//(re)-create the task for executing navMeshAsyncSetup()
	//the task is executed only once and then removed from AsyncTaskManager
	mUpdateData.clear();
	mUpdateTask.clear();
	mUpdateData = new TaskInterface<NavMesh>::TaskData(this,
			&NavMesh::navMeshAsyncSetup);
	mUpdateTask = new GenericAsyncTask(mComponentId + "NavMesh::navMeshAsyncSetup",
			&TaskInterface<NavMesh>::taskFunction,
			reinterpret_cast<void*>(mUpdateData.p()));
	//set sort/priority
	mUpdateTask->set_sort(0);
	mUpdateTask->set_priority(0);
	//Add the task for updating the controlled object
#ifdef ELY_THREAD
	//add the task to the task chain.
	mUpdateTask->set_task_chain(mTaskChainName);
	{
		HOLD_MUTEX(mAsyncSetupMutex)

		++mAsyncSetupCallCounter;
	}
#endif
	//Adds mUpdateTask to the active queue.
	AsyncTaskManager::get_global_ptr()->add(mUpdateTask);
}

NavMesh::Result NavMesh::navMeshCleanup()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	//return if destroying
	RETURN_ON_ASYNC_COND(mDestroying, Result::DESTROYING)

	//return if NavMesh has not been setup yet
	RETURN_ON_COND(not mNavMeshType, Result::NAVMESHTYPE_NULL)

	//do real cleanup
	doNavMeshCleanup();
	//
	return Result::OK;
}

void NavMesh::doNavMeshCleanup()
{
	//reset NavMeshTypeTool
	mNavMeshType->setTool(NULL);

	//delete old navigation mesh type
	delete mNavMeshType;
	mNavMeshType = NULL;
}

AsyncTask::DoneStatus NavMesh::navMeshAsyncSetup(GenericAsyncTask* task)
{
#ifdef ELY_THREAD
	HOLD_MUTEX(mAsyncSetupMutex)

	//check condition var
	while (not mAsyncSetupComplete)
	{
		//another Async Setup is going on: wait
		mAsyncSetupVar.wait();
	}
#endif

	//remove from AI manager update
	GameAIManager::GetSingletonPtr()->removeFromAIUpdate(this);

	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	//return if destroying
	RETURN_ON_ASYNC_COND(mDestroying, AsyncTask::DS_done)

#ifdef ELY_THREAD
	//we can effectively begin setup
	mAsyncSetupComplete = false;
#endif

	//setup navigation mesh, otherwise the same
	//operations must be performed by program.
	PRINT_DEBUG(
			"'" <<mOwnerObject->objectId() << "'::'" << mComponentId << "'::navMeshSetup");

	///don't load model mesh more than once
	if (not mGeom)
	{
		///load the mesh from the owner node path
		if (not doLoadModelMesh(mOwnerObject->getNodePath()))
		{
			throw GameException(
					"NavMesh::navMeshSetup: cannot load mesh model");
		}
	}

	///set up the type of navigation mesh
	switch (mNavMeshTypeEnum)
	{
	case SOLO:
	{
		doCreateNavMeshType(new NavMeshType_Solo());
		//set navigation mesh settings
		mNavMeshType->setNavMeshSettings(mNavMeshSettings);
	}
		break;
	case TILE:
	{
		doCreateNavMeshType(new NavMeshType_Tile());
		//set navigation mesh settings
		mNavMeshType->setNavMeshSettings(mNavMeshSettings);
		//set navigation mesh tile settings
		dynamic_cast<NavMeshType_Tile*>(mNavMeshType)->setTileSettings(
				mNavMeshTileSettings);
	}
		break;
	case OBSTACLE:
	{
		doCreateNavMeshType(new NavMeshType_Obstacle());
		//set navigation mesh settings
		mNavMeshType->setNavMeshSettings(mNavMeshSettings);
		//set navigation mesh tile settings...
		//	evaluate m_maxTiles & m_maxPolysPerTile from m_tileSize
		const float* bmin = getRecastInputGeom()->getMeshBoundsMin();
		const float* bmax = getRecastInputGeom()->getMeshBoundsMax();
		int gw = 0, gh = 0;
		rcCalcGridSize(bmin, bmax, mNavMeshSettings.m_cellSize, &gw, &gh);
		const int ts = (int) mNavMeshTileSettings.m_tileSize;
		const int tw = (gw + ts - 1) / ts;
		const int th = (gh + ts - 1) / ts;
		//	Max tiles and max polys affect how the tile IDs are calculated.
		// 	There are 22 bits available for identifying a tile and a polygon.
		int tileBits = rcMin((int) ilog2(nextPow2(tw * th)), 14);
		if (tileBits > 14)
		{
			tileBits = 14;
		}
		int polyBits = 22 - tileBits;
		mNavMeshTileSettings.m_maxTiles = 1 << tileBits;
		mNavMeshTileSettings.m_maxPolysPerTile = 1 << polyBits;
		//...effectively
		dynamic_cast<NavMeshType_Obstacle*>(mNavMeshType)->setTileSettings(
				mNavMeshTileSettings);
	}
		break;
	default:
		break;
	}

	///set recast areas' flags table
	mNavMeshType->setFlagsAreaTable(mPolyAreaFlags);

	///set recast convex volumes
	ConvexVolumeTool* cvTool = new ConvexVolumeTool();
	mNavMeshType->setTool(cvTool);
	std::list<PointListArea>::iterator iterPLA;
	for (iterPLA = mConvexVolumes.begin(); iterPLA != mConvexVolumes.end();
			++iterPLA)
	{
		PointList points = iterPLA->first;
		//check if there are at least 3 points for
		//complete a convex volume
		if (points.size() < 3)
		{
			continue;
		}
		int area = iterPLA->second;
		if (area < 0)
		{
			area = NAVMESH_POLYAREA_GROUND;
		}

		//1: set recast area type
		cvTool->setAreaType(area);

		//2: iterate, compute and insert recast points
		PointList::const_iterator iterP;
		float recastPos[3];
		for (iterP = points.begin(); iterP != points.end(); ++iterP)
		{
			//point is given wrt mOwnerObject node path but
			//it has to be wrt mReferenceNP
			LPoint3f refPos = mReferenceNP.get_relative_point(
					mOwnerObject->getNodePath(), *iterP);
			//insert convex volume point
			LVecBase3fToRecast(refPos, recastPos);
			mNavMeshType->getTool()->handleClick(NULL, recastPos, false);
		}
		//re-insert the last point (to close convex volume)
		mNavMeshType->getTool()->handleClick(NULL, recastPos, false);
	}
	mNavMeshType->setTool(NULL);

	///set recast off mesh connections
	OffMeshConnectionTool* omcTool = new OffMeshConnectionTool();
	mNavMeshType->setTool(omcTool);
	std::list<PointPairBidir>::iterator iterPPB;
	for (iterPPB = mOffMeshConnections.begin();
			iterPPB != mOffMeshConnections.end(); ++iterPPB)
	{
		PointPair pointPair = iterPPB->first;
		bool bidir = iterPPB->second;

		//1: set recast connection bidir
		omcTool->setBidir(bidir);

		//2: iterate, compute and insert recast points
		//points are given wrt mOwnerObject node path but
		//they have to be wrt mReferenceNP
		float recastPos[3];
		LPoint3f refPos;
		//compute and insert first recast point
		refPos = mReferenceNP.get_relative_point(mOwnerObject->getNodePath(),
				pointPair.first);
		LVecBase3fToRecast(refPos, recastPos);
		mNavMeshType->getTool()->handleClick(NULL, recastPos, false);
		//compute and insert second recast point
		refPos = mReferenceNP.get_relative_point(mOwnerObject->getNodePath(),
				pointPair.second);
		LVecBase3fToRecast(refPos, recastPos);
		mNavMeshType->getTool()->handleClick(NULL, recastPos, false);
	}
	mNavMeshType->setTool(NULL);

	///build navigation mesh effectively
	doBuildNavMesh();

	///set recast crowd
	CrowdTool* crowdTool = new CrowdTool();
	mNavMeshType->setTool(crowdTool);

	///set recast areas' costs
	dtQueryFilter* filter =
			crowdTool->getState()->getCrowd()->getEditableFilter();
	NavMeshPolyAreaCost::const_iterator iterAC;
	for (iterAC = mPolyAreaCost.begin(); iterAC != mPolyAreaCost.end();
			++iterAC)
	{
		filter->setAreaCost((*iterAC).first, (*iterAC).second);
	}

	///set recast crowd include & exclude flags
	crowdTool->getState()->getCrowd()->getEditableFilter()->setIncludeFlags(
			mCrowdIncludeFlags);
	crowdTool->getState()->getCrowd()->getEditableFilter()->setExcludeFlags(
			mCrowdExcludeFlags);

	///<this code is executed only when in manual setup:
	///add to recast previously added CrowdAgents.
	//mCrowdAgents could be modified during iteration so use this pattern:
	std::list<SMARTPTR(CrowdAgent)>::iterator iterCrowdAgents = mCrowdAgents.begin();
	while (iterCrowdAgents != mCrowdAgents.end())
	{
		SMARTPTR(CrowdAgent) crowdAgent = *iterCrowdAgents;
		//NavMesh object updates CrowdAgents pos/vel wrt its reference node path
		LPoint3f pos;
		if (mReferenceNP
				!= crowdAgent->getOwnerObject()->getNodePath().get_parent())
		{
			//the CrowdAgent owner object is reparented to the NavMesh
			//object reference node path
			pos = crowdAgent->getOwnerObject()->getNodePath().get_pos(
					mReferenceNP);
			crowdAgent->getOwnerObject()->getNodePath().reparent_to(
					mReferenceNP);
			crowdAgent->getOwnerObject()->getNodePath().set_pos(pos);
		}
		else
		{
			pos = crowdAgent->getOwnerObject()->getNodePath().get_pos();
		}
		//get recast p (y-up)
		float p[3];
		LVecBase3fToRecast(pos, p);
		//all crowd agent have the same dimensions: those
		//registered into the current mNavMeshType
		dtCrowdAgentParams ap = crowdAgent->getParams();
		ap.radius = mNavMeshType->getNavMeshSettings().m_agentRadius;
		ap.height = mNavMeshType->getNavMeshSettings().m_agentHeight;
		//add recast agent and set the index of the crowd agent
		crowdAgent->mAgentIdx = crowdTool->getState()->addAgent(p, &ap);
		if (crowdAgent->mAgentIdx == -1)
		{
			///\see http://stackoverflow.com/questions/596162/can-you-remove-elements-from-a-stdlist-while-iterating-through-it
			iterCrowdAgents = mCrowdAgents.erase(iterCrowdAgents);
			continue;
		}
		//set the mov type of the crowd agent
		crowdAgent->mMovType = mMovType;
		//reset events' sending
		crowdAgent->mCrowdAgentStartSent = false;
		crowdAgent->mCrowdAgentStopSent = true;
		//set physics parameters
		crowdAgent->mMaxError = mNavMeshType->getNavMeshSettings().m_agentHeight;
		crowdAgent->mDeltaRayOrig = LVector3f(0, 0, crowdAgent->mMaxError);
		crowdAgent->mDeltaRayDown = LVector3f(0, 0, -10 * crowdAgent->mMaxError);
		crowdAgent->mHitResult = BulletClosestHitRayResult::empty();
		///update move target
		float target[3];
		LVecBase3fToRecast(crowdAgent->mMoveTarget, target);
		crowdTool->getState()->setMoveTarget(crowdAgent->mAgentIdx, target);
		///update move velocity (if length != 0)
		if(length(crowdAgent->mMoveVelocity) > 0.0)
		{
			float velocity[3];
			LVecBase3fToRecast(crowdAgent->mMoveVelocity, velocity);
			crowdTool->getState()->setMoveVelocity(crowdAgent->mAgentIdx, velocity);
		}
		//increment iterator
		++iterCrowdAgents;
	}
	///>

#ifdef ELY_DEBUG
	//delete old DebugDrawers (if any)
	delete mDD;
	delete mDDM;
	//create new DebugDrawers
	mDD = new DebugDrawPanda3d(mDebugNodePath);
	mDDM = new DebugDrawMeshDrawer(mDebugNodePath, mDebugCamera);
	///create the task for executing debug render
	//the task is executed only once and then removed from AsyncTaskManager
	mDebugRenderData.clear();
	mDebugRenderTask.clear();
	mDebugRenderData = new TaskInterface<NavMesh>::TaskData(this,
			&NavMesh::debugStaticRenderTask);
	mDebugRenderTask = new GenericAsyncTask(
			mComponentId + "NavMesh::debugRender",
			&TaskInterface<NavMesh>::taskFunction,
			reinterpret_cast<void*>(mDebugRenderData.p()));
	//set sort/priority
	mDebugRenderTask->set_sort(0);
	mDebugRenderTask->set_priority(0);
	//Add the task for updating the controlled object
#ifdef ELY_THREAD
	//add the task to the task chain.
	mDebugRenderTask->set_task_chain("default");
#endif
	//Adds mUpdateTask to the active queue.
	AsyncTaskManager::get_global_ptr()->add(mDebugRenderTask);
#endif //ELY_DEBUG

	//Add to the AI manager update
	GameAIManager::GetSingletonPtr()->addToAIUpdate(this);

#ifdef ELY_THREAD
	//Async Setup is complete
	mAsyncSetupComplete = true;
	--mAsyncSetupCallCounter;
	//notify one thread that Async Setup is complete
	mAsyncSetupVar.notify();
#endif

	//
	return AsyncTask::DS_done;
}

#ifdef ELY_DEBUG
void NavMesh::doDebugStaticRender()
{
	//debug render with DebugDrawPanda3d
	mDD->reset();
	mNavMeshType->handleRender(*mDD);
	mNavMeshType->getInputGeom()->drawConvexVolumes(mDD);
	mNavMeshType->getInputGeom()->drawOffMeshConnections(mDD, true);
}

AsyncTask::DoneStatus NavMesh::debugStaticRenderTask(GenericAsyncTask* task)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	//return if destroying
	RETURN_ON_ASYNC_COND(mDestroying, AsyncTask::DS_done)

	//return if async-setup is not complete (and abort attempt)
	RETURN_ON_ASYNC_COND(not mAsyncSetupComplete, AsyncTask::DS_done)

	doDebugStaticRender();
	//
	return AsyncTask::DS_done;
}
#endif

NavMesh::Result NavMesh::getTilePos(const LPoint3f& pos, int& tx, int& ty)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	//return if destroying
	RETURN_ON_ASYNC_COND(mDestroying, Result::DESTROYING)

	//return if NavMesh has not been setup yet
	RETURN_ON_COND(not mNavMeshType, Result::NAVMESHTYPE_NULL)

	float recastPos[3];
	LVecBase3fToRecast(pos, recastPos);
	if (mNavMeshTypeEnum == TILE)
	{
		dynamic_cast<NavMeshType_Tile*>(mNavMeshType)->getTilePos(recastPos, tx,
				ty);
	}
	else if (mNavMeshTypeEnum == OBSTACLE)
	{
		dynamic_cast<NavMeshType_Obstacle*>(mNavMeshType)->getTilePos(recastPos,
				tx, ty);
	}
	//
	return Result::OK;
}

NavMesh::Result NavMesh::buildTile(const LPoint3f& pos)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	//return if destroying
	RETURN_ON_ASYNC_COND(mDestroying, Result::DESTROYING)

	//return if NavMesh has not been setup yet
	RETURN_ON_COND(not mNavMeshType, Result::NAVMESHTYPE_NULL)

	if (mNavMeshTypeEnum == TILE)
	{
		float recastPos[3];
		LVecBase3fToRecast(pos, recastPos);
		dynamic_cast<NavMeshType_Tile*>(mNavMeshType)->buildTile(recastPos);
		PRINT_DEBUG("'" << getOwnerObject()->objectId() << "'::'"
				<< mComponentId << "'::buildTile : " << pos);
#ifdef ELY_DEBUG
		doDebugStaticRender();
#endif
	}
	//
	return Result::OK;
}

NavMesh::Result NavMesh::removeTile(const LPoint3f& pos)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	//return if destroying
	RETURN_ON_ASYNC_COND(mDestroying, Result::DESTROYING)

	//return if NavMesh has not been setup yet
	RETURN_ON_COND(not mNavMeshType, Result::NAVMESHTYPE_NULL)

	if (mNavMeshTypeEnum == TILE)
	{
		float recastPos[3];
		LVecBase3fToRecast(pos, recastPos);
		dynamic_cast<NavMeshType_Tile*>(mNavMeshType)->removeTile(recastPos);
		PRINT_DEBUG("'" << getOwnerObject()->objectId() << "'::'"
				<< mComponentId << "'::removeTile : " << pos);
#ifdef ELY_DEBUG
		doDebugStaticRender();
#endif
	}
	//
	return Result::OK;
}

NavMesh::Result NavMesh::buildAllTiles()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	//return if destroying
	RETURN_ON_ASYNC_COND(mDestroying, Result::DESTROYING)

	//return if NavMesh has not been setup yet
	RETURN_ON_COND(not mNavMeshType, Result::NAVMESHTYPE_NULL)

	if (mNavMeshTypeEnum == TILE)
	{
		dynamic_cast<NavMeshType_Tile*>(mNavMeshType)->buildAllTiles();
#ifdef ELY_DEBUG
		doDebugStaticRender();
#endif
	}
	//
	return Result::OK;
}

NavMesh::Result NavMesh::removeAllTiles()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	//return if destroying
	RETURN_ON_ASYNC_COND(mDestroying, Result::DESTROYING)

	//return if NavMesh has not been setup yet
	RETURN_ON_COND(not mNavMeshType, Result::NAVMESHTYPE_NULL)

	if (mNavMeshTypeEnum == TILE)
	{
		dynamic_cast<NavMeshType_Tile*>(mNavMeshType)->removeAllTiles();
#ifdef ELY_DEBUG
		doDebugStaticRender();
#endif
	}
	//
	return Result::OK;
}

dtTileCache* NavMesh::getTileCache()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	//return if destroying
	RETURN_ON_ASYNC_COND(mDestroying, NULL)

	//return if async-setup is not complete
	RETURN_ON_ASYNC_COND(not mAsyncSetupComplete, NULL)

	//return if NavMesh has not been setup yet
	RETURN_ON_COND(not mNavMeshType, NULL)

	//
	RETURN_ON_COND(mNavMeshTypeEnum == OBSTACLE,
			dynamic_cast<NavMeshType_Obstacle*>(mNavMeshType)->getTileCache())
	//
	return NULL;
}

NavMesh::Result NavMesh::addObstacle(SMARTPTR(Object)object)
{
	RETURN_ON_COND(not object, Result::ERROR)

	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	//return if destroying
	RETURN_ON_ASYNC_COND(mDestroying, Result::DESTROYING)

	//return if NavMesh has not been setup yet
	RETURN_ON_COND(not mNavMeshType, Result::NAVMESHTYPE_NULL)

	if ((mNavMeshTypeEnum == OBSTACLE) and
			(not mReferenceNP.is_empty()) and object)
	{
		//get obstacle dimensions
		NodePath objectNP = object->getNodePath();
		LPoint3f min_point, max_point;
		objectNP.calc_tight_bounds(min_point, max_point);
		float dX = max_point.get_x() - min_point.get_x();
		float dY = max_point.get_y() - min_point.get_y();
		float radius = sqrt(pow(dX,2) + pow(dY,2)) / 2.0;
		float heigth = max_point.get_z() - min_point.get_z();
		//calculate pos wrt reference node path
		LPoint3f pos = objectNP.get_pos(mReferenceNP);
		//add detour obstacle
		dtObstacleRef obstacleRef;
		float recastPos[3];
		LVecBase3fToRecast(pos, recastPos);
		dtTileCache* tileCache =
		dynamic_cast<NavMeshType_Obstacle*>(mNavMeshType)->getTileCache();
		tileCache->addObstacle(recastPos, radius, heigth, &obstacleRef);
		//update tile cache
		tileCache->update(0, mNavMeshType->getNavMesh());
		//add to the obstacles table
		mObstacles[object] = obstacleRef;
		PRINT_DEBUG("'" << getOwnerObject()->objectId() << "'::'"
		<< mComponentId << "'::addObstacle: '" << object->objectId()
		<< "' at pos: " << pos);
#ifdef ELY_DEBUG
		doDebugStaticRender();
#endif
	}
	//
	return Result::OK;
}

NavMesh::Result NavMesh::removeObstacle(SMARTPTR(Object)object)
{
	RETURN_ON_COND(not object, Result::ERROR)

	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	//return if destroying
	RETURN_ON_ASYNC_COND(mDestroying, Result::DESTROYING)

	//return if NavMesh has not been setup yet
	RETURN_ON_COND(not mNavMeshType, Result::NAVMESHTYPE_NULL)

	if ((mNavMeshTypeEnum == OBSTACLE) and
	(not mReferenceNP.is_empty()) and object)
	{
		//get obstacle ref
		dtObstacleRef obstacleRef = mObstacles[object];
		//dtObstacleRef cannot be zero
		RETURN_ON_COND(obstacleRef == 0, Result::ERROR)

		//remove recast obstacle
		dtTileCache* tileCache =
		dynamic_cast<NavMeshType_Obstacle*>(mNavMeshType)->getTileCache();
		tileCache->removeObstacle(obstacleRef);
		//update tile cache
		tileCache->update(0, mNavMeshType->getNavMesh());
		//remove from obstacle table
		mObstacles.erase(object);
		PRINT_DEBUG("'" << getOwnerObject()->objectId() << "'::'"
		<< mComponentId << "'::removeObstacle: '" << object->objectId() << "'");
#ifdef ELY_DEBUG
		doDebugStaticRender();
#endif
	}
	//
	return Result::OK;
}

NavMesh::Result NavMesh::clearAllObstacles()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	//return if destroying
	RETURN_ON_ASYNC_COND(mDestroying, Result::DESTROYING)

	//return if NavMesh has not been setup yet
	RETURN_ON_COND(not mNavMeshType, Result::NAVMESHTYPE_NULL)

	if (mNavMeshTypeEnum == OBSTACLE)
	{
		dynamic_cast<NavMeshType_Obstacle*>(mNavMeshType)->clearAllTempObstacles();
		PRINT_DEBUG("'" << getOwnerObject()->objectId() << "'::'"
				<< mComponentId << "'::clearAllObstacles");
#ifdef ELY_DEBUG
		doDebugStaticRender();
#endif
	}
	//
	return Result::OK;
}

bool NavMesh::doLoadModelMesh(NodePath model)
{
	bool result = true;
	mGeom = new InputGeom;
	mMeshName = model.get_name();
	//
	if ((not mGeom) or (not mGeom->loadMesh(mCtx, NULL, model, mReferenceNP)))
	{
		delete mGeom;
		mGeom = NULL;
#ifdef ELY_DEBUG
		mCtx->dumpLog("Geom load log %s:", mMeshName.c_str());
#endif
		result = false;
	}
	return result;
}

void NavMesh::doCreateNavMeshType(NavMeshType* navMeshType)
{
	//delete old navigation mesh type
	delete mNavMeshType;
	//set the navigation mesh type
	mNavMeshType = navMeshType;
	//set rcContext
	mNavMeshType->setContext(mCtx);
	//handle Mesh Changed
	mNavMeshType->handleMeshChanged(mGeom);
}

bool NavMesh::doBuildNavMesh()
{
#ifdef ELY_DEBUG
	mCtx->resetLog();
#endif
	//build navigation mesh
	bool result = mNavMeshType->handleBuild();
#ifdef ELY_DEBUG
	mCtx->dumpLog("Build log %s:", mMeshName.c_str());
#endif
	return result;
}

NavMesh::Result NavMesh::addCrowdAgent(SMARTPTR(CrowdAgent)crowdAgent)
{
	RETURN_ON_COND(not crowdAgent,false)

	bool result;
	//lock (guard) the crowdAgent NavMesh mutex
	HOLD_REMUTEX(crowdAgent->mNavMeshMutex)
	{
		//lock (guard) the mutex
		HOLD_REMUTEX(mMutex)

		//return if destroying
		RETURN_ON_ASYNC_COND(mDestroying, Result::DESTROYING)

		{
			//lock (guard) the crowdAgent mutex
			HOLD_REMUTEX(crowdAgent->mMutex)

			//return if crowdAgent is destroying or belongs to some mesh
			RETURN_ON_ASYNC_COND(crowdAgent->mDestroying, Result::Result::ERROR)
			RETURN_ON_COND(crowdAgent->mNavMesh, Result::ERROR)

			//do real adding to update list
			doAddCrowdAgentToUpdateList(crowdAgent);

			//return if NavMesh has not been setup yet
			RETURN_ON_COND(not mNavMeshType, Result::NAVMESHTYPE_NULL)

			//do real adding to recast update
			result = doAddCrowdAgentToRecastUpdate(crowdAgent);
			//check if adding to recast was successful
			if (not result)
			{
				//remove CrowdAgent from update too (if previously added)
				mCrowdAgents.erase(
				find(mCrowdAgents.begin(), mCrowdAgents.end(),
						crowdAgent));
			}
		}
	}
	//
	return (result = true ? Result::OK:Result::ERROR);
}

void NavMesh::doAddCrowdAgentToUpdateList(SMARTPTR(CrowdAgent)crowdAgent)
{
	//add to update list
	std::list<SMARTPTR(CrowdAgent)>::iterator iterCA;
	//check if crowdAgent has not been already added
	iterCA = find(mCrowdAgents.begin(), mCrowdAgents.end(), crowdAgent);
	if(iterCA == mCrowdAgents.end())
	{
		//CrowdAgent needs to be added
		//set CrowdAgent's NavMesh owner object
		crowdAgent->mNavMesh = this;
		//add CrowdAgent
		mCrowdAgents.push_back(crowdAgent);
	}
}

bool NavMesh::doAddCrowdAgentToRecastUpdate(SMARTPTR(CrowdAgent)crowdAgent)
{
	//there is a crowd tool because the recast nav mesh
	//has been completely setup
	//check if crowdAgent has not been already added to recast
	CrowdTool* crowdTool = dynamic_cast<CrowdTool*>(mNavMeshType->getTool());
	if(crowdAgent->mAgentIdx == -1)
	{
		//NavMesh object updates CrowdAgents pos/vel wrt its reference node path
		LPoint3f pos;
		bool needToReparent = (mReferenceNP !=
				crowdAgent->getOwnerObject()->getNodePath().get_parent());
		if(needToReparent)
		{
			//pos is wrt object reference node path
			pos = crowdAgent->getOwnerObject()->getNodePath().get_pos(mReferenceNP);
		}
		else
		{
			pos = crowdAgent->getOwnerObject()->getNodePath().get_pos();
		}
		//get recast p (y-up)
		float p[3];
		LVecBase3fToRecast(pos, p);
		//all crowd agent have the same dimensions: those
		//registered into the current mNavMeshType
		dtCrowdAgentParams ap = crowdAgent->mAgentParams;
		ap.radius = mNavMeshType->getNavMeshSettings().m_agentRadius;
		ap.height = mNavMeshType->getNavMeshSettings().m_agentHeight;
		//add recast agent and set the index of the crowd agent
		crowdAgent->mAgentIdx = crowdTool->getState()->addAgent(p, &ap);
		if(crowdAgent->mAgentIdx == -1)
		{
			//agent has not been added to recast
			return false;
		}
		//agent has been added to recast
		//update the (possibly) modified params
		crowdAgent->mAgentParams = ap;
		//reparent if needed
		if (needToReparent)
		{
			//the CrowdAgent owner object is reparented to the NavMesh
			//object reference node path
			crowdAgent->getOwnerObject()->getNodePath().reparent_to(mReferenceNP);
			crowdAgent->getOwnerObject()->getNodePath().set_pos(pos);
		}
		//set the mov type of the crowd agent
		crowdAgent->mMovType = mMovType;
		//reset events' sending
		crowdAgent->mCrowdAgentStartSent = false;
		crowdAgent->mCrowdAgentStopSent = true;
		//set physics parameters
		crowdAgent->mMaxError = mNavMeshType->getNavMeshSettings().m_agentHeight;
		crowdAgent->mDeltaRayOrig = LVector3f(0, 0, crowdAgent->mMaxError);
		crowdAgent->mDeltaRayDown = LVector3f(0, 0, -10*crowdAgent->mMaxError);
		crowdAgent->mHitResult = BulletClosestHitRayResult::empty();
		///update move target
		float target[3];
		LVecBase3fToRecast(crowdAgent->mMoveTarget, target);
		crowdTool->getState()->setMoveTarget(crowdAgent->mAgentIdx, target);
		///update move velocity
		if(length(crowdAgent->mMoveVelocity) > 0.0)
		{
			float velocity[3];
			LVecBase3fToRecast(crowdAgent->mMoveVelocity, velocity);
			crowdTool->getState()->setMoveVelocity(crowdAgent->mAgentIdx, velocity);
		}
	}
	//agent has been added to recast
	return true;
}

NavMesh::Result NavMesh::removeCrowdAgent(SMARTPTR(CrowdAgent)crowdAgent)
{
	RETURN_ON_COND(not crowdAgent, Result::ERROR)

	//lock (guard) the crowdAgent NavMesh mutex
	HOLD_REMUTEX(crowdAgent->mNavMeshMutex)
	{
		//lock (guard) the mutex
		HOLD_REMUTEX(mMutex)

		//return if destroying
		RETURN_ON_ASYNC_COND(mDestroying, Result::DESTROYING)

		{
			//lock (guard) the crowdAgent mutex
			HOLD_REMUTEX(crowdAgent->mMutex)

			//return if crowdAgent is destroying or doesn't belong to any mesh
			RETURN_ON_ASYNC_COND(crowdAgent->mDestroying, Result::Result::ERROR)
			RETURN_ON_COND(not crowdAgent->mNavMesh, Result::ERROR)

			//remove from update list
			doRemoveCrowdAgentFromUpdateList(crowdAgent);

			//return if NavMesh has not been setup yet
			RETURN_ON_COND(not mNavMeshType, Result::NAVMESHTYPE_NULL)

			//remove from recast update
			doRemoveCrowdAgentFromRecastUpdate(crowdAgent);
		}
	}
	//
	return Result::OK;
}

void NavMesh::doRemoveCrowdAgentFromUpdateList(SMARTPTR(CrowdAgent)crowdAgent)
{
	//remove from update list
	std::list<SMARTPTR(CrowdAgent)>::iterator iterCA;
	//check if CrowdAgent has been already removed or not
	iterCA = find(mCrowdAgents.begin(), mCrowdAgents.end(), crowdAgent);
	if(iterCA != mCrowdAgents.end())
	{
		//CrowdAgent needs to be removed
		mCrowdAgents.erase(iterCA);
		//set CrowdAgent NavMesh reference to NULL
		crowdAgent->mNavMesh.clear();
	}
}

void NavMesh::doRemoveCrowdAgentFromRecastUpdate(SMARTPTR(CrowdAgent)crowdAgent)
{
	//there is a crowd tool because the recast nav mesh
	//has been completely setup
	//and check if crowdAgent has been already added to recast
	CrowdTool* crowdTool = dynamic_cast<CrowdTool*>(mNavMeshType->getTool());
	if (crowdAgent->mAgentIdx != -1)
	{
		//remove recast agent
		crowdTool->getState()->removeAgent(crowdAgent->mAgentIdx);
		//set the index of the crowd agent to -1
		crowdAgent->mAgentIdx = -1;
	}
}

NavMesh::Result NavMesh::setCrowdAgentParams(SMARTPTR(CrowdAgent)crowdAgent,
		const dtCrowdAgentParams& params)
{
	RETURN_ON_COND(not crowdAgent, Result::ERROR)

	//lock (guard) the crowdAgent NavMesh mutex
	HOLD_REMUTEX(crowdAgent->mNavMeshMutex)
	{
		//return if crowdAgent doesn't belong to this mesh
		RETURN_ON_COND(crowdAgent->mNavMesh != this, Result::ERROR)

		//lock (guard) the mutex
		HOLD_REMUTEX(mMutex)

		//return if destroying or NavMeshType has not been setup yet
		RETURN_ON_ASYNC_COND(mDestroying, Result::DESTROYING)
		RETURN_ON_COND(not mNavMeshType, Result::NAVMESHTYPE_NULL)

		{
			//lock (guard) the crowdAgent mutex
			HOLD_REMUTEX(crowdAgent->mMutex)

			//return if crowdAgent is destroying
			RETURN_ON_ASYNC_COND(crowdAgent->mDestroying, Result::Result::ERROR)

			//there is a crowd tool because the recast nav mesh
			//has been completely setup
			//check if crowdAgent has been already added to recast
			if (crowdAgent->mAgentIdx != -1)
			{
				//second parameter should be the parameter to update
				int agentIdx = crowdAgent->mAgentIdx;
				dtCrowdAgentParams ap = params;
				//all crowd agent have the same dimensions: those
				//registered into the current mNavMeshType
				ap.radius = mNavMeshType->getNavMeshSettings().m_agentRadius;
				ap.height = mNavMeshType->getNavMeshSettings().m_agentHeight;
				dynamic_cast<CrowdTool*>(mNavMeshType->getTool())->
				getState()->getCrowd()->updateAgentParameters(crowdAgent->mAgentIdx, &ap);
			}
			crowdAgent->mAgentParams = params;
		}
	}
	//
	return Result::OK;
}

NavMesh::Result NavMesh::setCrowdAgentTarget(SMARTPTR(CrowdAgent)crowdAgent,
		const LPoint3f& moveTarget)
{
	RETURN_ON_COND(not crowdAgent, Result::ERROR)

	//lock (guard) the crowdAgent NavMesh mutex
	HOLD_REMUTEX(crowdAgent->mNavMeshMutex)
	{
		//return if crowdAgent doesn't belong to this mesh
		RETURN_ON_COND(crowdAgent->mNavMesh != this, Result::ERROR)

		//lock (guard) the mutex
		HOLD_REMUTEX(mMutex)

		//return if destroying or NavMeshType has not been setup yet
		RETURN_ON_ASYNC_COND(mDestroying, Result::DESTROYING)
		RETURN_ON_COND(not mNavMeshType, Result::NAVMESHTYPE_NULL)

		{
			//lock (guard) the crowdAgent mutex
			HOLD_REMUTEX(crowdAgent->mMutex)

			//return if crowdAgent is destroying
			RETURN_ON_ASYNC_COND(crowdAgent->mDestroying, Result::Result::ERROR)

			//there is a crowd tool because the recast nav mesh
			//has been completely setup
			//check if crowdAgent has been already added to recast
			if (crowdAgent->mAgentIdx != -1)
			{
				float p[3];
				LVecBase3fToRecast(moveTarget, p);
				dynamic_cast<CrowdTool*>(mNavMeshType->getTool())->
				getState()->setMoveTarget(crowdAgent->mAgentIdx, p);
			}
			crowdAgent->mMoveTarget = moveTarget;
		}
	}
	//
	return Result::OK;
}

NavMesh::Result NavMesh::setCrowdAgentVelocity(SMARTPTR(CrowdAgent)crowdAgent,
		const LVector3f& moveVelocity)
{
	RETURN_ON_COND(not crowdAgent, Result::ERROR)

	//lock (guard) the crowdAgent NavMesh mutex
	HOLD_REMUTEX(crowdAgent->mNavMeshMutex)
	{
		//return if crowdAgent doesn't belong to this mesh
		RETURN_ON_COND(crowdAgent->mNavMesh != this, Result::ERROR)

		//lock (guard) the mutex
		HOLD_REMUTEX(mMutex)

		//return if destroying or NavMeshType has not been setup yet
		RETURN_ON_ASYNC_COND(mDestroying, Result::DESTROYING)
		RETURN_ON_COND(not mNavMeshType, Result::NAVMESHTYPE_NULL)

		{
			//lock (guard) the crowdAgent mutex
			HOLD_REMUTEX(crowdAgent->mMutex)

			//return if crowdAgent is destroying
			RETURN_ON_ASYNC_COND(crowdAgent->mDestroying, Result::Result::ERROR)

			//there is a crowd tool because the recast nav mesh
			//has been completely setup
			//check if crowdAgent has been already added to recast
			if (crowdAgent->mAgentIdx != -1)
			{
				float v[3];
				LVecBase3fToRecast(moveVelocity, v);
				dynamic_cast<CrowdTool*>(mNavMeshType->getTool())->
				getState()->setMoveVelocity(crowdAgent->mAgentIdx, v);
			}
			crowdAgent->mMoveVelocity = moveVelocity;
		}
	}
	//
	return Result::OK;
}

void NavMesh::update(void* data)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	float dt = *(reinterpret_cast<float*>(data));

#ifdef TESTING
	dt = 0.016666667; //60 fps
#endif

	//update is done only when there is a crowd tool
	CrowdTool* crowdTool = dynamic_cast<CrowdTool*>(mNavMeshType->getTool());
	dtCrowd* crowd = crowdTool->getState()->getCrowd();

	//update crowd agents' pos/vel
	mNavMeshType->handleUpdate(dt);

	std::list<SMARTPTR(CrowdAgent)>::const_iterator iter;
	//post-update all agent positions
	for (iter = mCrowdAgents.begin(); iter != mCrowdAgents.end();
			++iter)
	{
		int agentIdx = (*iter)->mAgentIdx;
		//give CrowdAgent chance to update its pos/vel
		const float* vel = crowd->getAgent(agentIdx)->vel;
		const float* pos = crowd->getAgent(agentIdx)->npos;
		(*iter)->doUpdatePosDir(dt, RecastToLVecBase3f(pos),
				RecastToLVecBase3f(vel));
	}
	//
#ifdef ELY_DEBUG
	if (mEnableDrawUpdate)
	{
		mDDM->reset();
		mNavMeshType->renderToolStates(*mDDM);
	}
#endif
}

#ifdef ELY_DEBUG
NodePath NavMesh::getDebugNodePath() const
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	//return if async-setup is not complete
	RETURN_ON_ASYNC_COND(not mAsyncSetupComplete, NodePath())

	return mDebugNodePath;
}

NavMesh::Result NavMesh::debug(bool enable)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	//return if destroying
	RETURN_ON_ASYNC_COND(mDestroying, Result::DESTROYING)

	//return if mDebugNodePath is empty
	RETURN_ON_COND(mDebugNodePath.is_empty(), Result::ERROR)

	if (enable)
	{
		if (mDebugNodePath.is_hidden())
		{
			mDebugNodePath.show();
			mEnableDrawUpdate = true;
		}
	}
	else
	{
		if (not mDebugNodePath.is_hidden())
		{
			mDebugNodePath.hide();
			mEnableDrawUpdate = false;
		}
	}
	//
	return Result::OK;
}
#endif

//TypedObject semantics: hardcoded
TypeHandle NavMesh::_type_handle;

}  // namespace ely
