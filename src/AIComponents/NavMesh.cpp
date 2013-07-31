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
{
	// TODO Auto-generated constructor stub
}

NavMesh::NavMesh(SMARTPTR(NavMeshTemplate)tmpl):
	mGeom(0),
	mCtx(new BuildContext),
	mMeshName("")
{
	CHECKEXISTENCE(GameAIManager::GetSingletonPtr(),
			"NavMesh::NavMesh: invalid GameAIManager")
	mTmpl = tmpl;
}

NavMesh::~NavMesh()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	delete mCtx;
#ifdef ELY_DEBUG
	//delete the DebugDrawers
	delete mDD;
	delete mDDM;
#endif
}

const ComponentFamilyType NavMesh::familyType() const
{
	return mTmpl->familyType();
}

const ComponentType NavMesh::componentType() const
{
	return mTmpl->componentType();
}

bool NavMesh::initialize()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	bool result = true;
	//set NavMesh parameters (store internally for future use)
	//
	std::string navMeshtypeStr = mTmpl->parameter(std::string("navmesh_type"));
	if (navMeshtypeStr == "tile")
	{
		mNavMeshTypeEnum = TILE;
	}
	else if (navMeshtypeStr == "obstacle")
	{
		mNavMeshTypeEnum = OBSTACLE;
	}
	else
	{
		mNavMeshTypeEnum = SOLO;
	}
	mAutoSetup = (
			mTmpl->parameter(std::string("auto_setup"))
					== std::string("false") ? false : true);
	std::string movType = mTmpl->parameter(std::string("mov_type"));
	if (movType == "kinematic")
	{
		CHECKEXISTENCE(GamePhysicsManager::GetSingletonPtr(),
				"NavMesh::NavMesh: invalid GamePhysicsManager")
		mMovType = KINEMATIC;
	}
#ifdef WITHCHARACTER
	else if (movType == "character")
	{
		CHECKEXISTENCE(GamePhysicsManager::GetSingletonPtr(),
				"NavMesh::NavMesh: invalid GamePhysicsManager")
		mMovType = CHARACTER;
	}
#endif
	else
	{
		mMovType = RECAST;
	}
	mNavMeshSettings.m_cellSize = (float) strtof(
			mTmpl->parameter(std::string("cell_size")).c_str(), NULL);
	mNavMeshSettings.m_cellHeight = (float) strtof(
			mTmpl->parameter(std::string("cell_height")).c_str(), NULL);
	mNavMeshSettings.m_agentHeight = (float) strtof(
			mTmpl->parameter(std::string("agent_height")).c_str(), NULL);
	mNavMeshSettings.m_agentRadius = (float) strtof(
			mTmpl->parameter(std::string("agent_radius")).c_str(), NULL);
	mNavMeshSettings.m_agentMaxClimb = (float) strtof(
			mTmpl->parameter(std::string("agent_max_climb")).c_str(), NULL);
	mNavMeshSettings.m_agentMaxSlope = (float) strtof(
			mTmpl->parameter(std::string("agent_max_slope")).c_str(), NULL);
	mNavMeshSettings.m_regionMinSize = (float) strtof(
			mTmpl->parameter(std::string("region_min_size")).c_str(), NULL);
	mNavMeshSettings.m_regionMergeSize = (float) strtof(
			mTmpl->parameter(std::string("region_merge_size")).c_str(), NULL);
	mNavMeshSettings.m_monotonePartitioning = (
			mTmpl->parameter(std::string("monotone_partitioning"))
					== std::string("true") ? true : false);
	mNavMeshSettings.m_edgeMaxLen = (float) strtof(
			mTmpl->parameter(std::string("edge_max_len")).c_str(), NULL);
	mNavMeshSettings.m_edgeMaxError = (float) strtof(
			mTmpl->parameter(std::string("edge_max_error")).c_str(), NULL);
	mNavMeshSettings.m_vertsPerPoly = (float) strtof(
			mTmpl->parameter(std::string("verts_per_poly")).c_str(), NULL);
	mNavMeshSettings.m_detailSampleDist = (float) strtof(
			mTmpl->parameter(std::string("detail_sample_dist")).c_str(), NULL);
	mNavMeshSettings.m_detailSampleMaxError = (float) strtof(
			mTmpl->parameter(std::string("detail_sample_max_error")).c_str(),
			NULL);
	//nav mesh tile
	mNavMeshTileSettings.m_buildAllTiles = (
			mTmpl->parameter(std::string("build_all_tiles"))
					== std::string("true") ? true : false);
	mNavMeshTileSettings.m_maxTiles = (float) strtof(
			mTmpl->parameter(std::string("max_tiles")).c_str(), NULL);
	mNavMeshTileSettings.m_maxPolysPerTile = (float) strtof(
			mTmpl->parameter(std::string("max_polys_per_tile")).c_str(), NULL);
	mNavMeshTileSettings.m_tileSize = (float) strtof(
			mTmpl->parameter(std::string("tile_size")).c_str(), NULL);
	//area-flags-cost settings
	mAreaFlagsCostXmlParam = mTmpl->parameterList(std::string("area_flags_cost"));
	//crowd include/exclude flags
	mCrowdIncludeFlagsXmlParam = mTmpl->parameter(std::string("crowd_include_flags"));
	mCrowdExcludeFlagsXmlParam = mTmpl->parameter(std::string("crowd_exclude_flags"));
	//convex volumes
	mConvexVolumeXmlParam = mTmpl->parameterList(std::string("convex_volume"));
	//off mesh connections
	mOffMeshConnectionXmlParam = mTmpl->parameterList(std::string("offmesh_connection"));
	//
	return result;
}

void NavMesh::onAddToObjectSetup()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	//add only for a not empty object node path
	if (mOwnerObject->getNodePath().is_empty())
	{
		return;
	}

	//setup event callbacks if any
	setupEvents();
	//register event callbacks if any
	registerEventCallbacks();
}

void NavMesh::onAddToSceneSetup()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	//add only for a not empty object node path
	if (mOwnerObject->getNodePath().is_empty())
	{
		return;
	}

	//set mOwnerObject's parent node path as reference
	mReferenceNP = mOwnerObject->getNodePath().get_parent();

#ifdef ELY_DEBUG
	//set the recast debug node path as child of mReferenceNP node path
	//set the recast debug camera to the first child of "camera" node path
	SMARTPTR(Object) cameraDebug = ObjectTemplateManager::GetSingletonPtr()->
			getCreatedObject("camera");
	if (cameraDebug)
	{
		mDebugNodePath = mReferenceNP.attach_new_node("RecastDebugNodePath");
		mDebugNodePath.set_bin("fixed", 10);
		//by default mDebugNodePath is hidden
		mDebugNodePath.hide();
		//
		mDebugCamera = cameraDebug->getNodePath().get_child(0);
	}
	//create the DebugDrawers
	mDD = new DebugDrawPanda3d(mDebugNodePath);
	mDDM = new DebugDrawMeshDrawer(mDebugNodePath, mDebugCamera);
#endif

	//setup nav mesh if auto setup is true
	if(mAutoSetup)
	{
		///1: get the input from xml
		std::list<std::string>::iterator iterStr;
		///get area ability flags and cost (for crowd)
		NavMeshPolyAreaFlags flagsAreaTable;
		flagsAreaTable.clear();
		NavMeshPolyAreaCost costAreaTable;
		costAreaTable.clear();
		for (iterStr = mAreaFlagsCostXmlParam.begin(); iterStr != mAreaFlagsCostXmlParam.end();
				++iterStr)
		{
			//any "area_flags" string is a "compound" one, i.e. could have the form:
			// "area@flag1|flag2...|flagN@cost"
			std::vector<std::string> areaFlagsCostStr = parseCompoundString(*iterStr,
					'@');
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
						areaFlagsCostStr[1], '|');
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
				float cost = (float) strtof(areaFlagsCostStr[2].c_str(), NULL);
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
		ieFlagsStr = parseCompoundString(mCrowdIncludeFlagsXmlParam, '|');
		//default include flag: NAVMESH_POLYFLAGS_WALK (== 0x01)
		int includeFlags = (ieFlagsStr.empty() ? NAVMESH_POLYFLAGS_WALK : 0x0);
		for (iterIEFStr = ieFlagsStr.begin(); iterIEFStr != ieFlagsStr.end(); ++iterIEFStr)
		{
			int flag = (
					not (*iterIEFStr).empty() ?
							strtol((*iterIEFStr).c_str(), NULL, 0) :
							NAVMESH_POLYFLAGS_WALK);
			//or flag
			includeFlags |= flag;
		}
		//2:iterate over exclude flags
		ieFlagsStr = parseCompoundString(mCrowdExcludeFlagsXmlParam, '|');
		//default exclude flag: NAVMESH_POLYFLAGS_DISABLED (== 0x10)
		int excludeFlags = (ieFlagsStr.empty() ? NAVMESH_POLYFLAGS_DISABLED : 0x0);
		for (iterIEFStr = ieFlagsStr.begin(); iterIEFStr != ieFlagsStr.end(); ++iterIEFStr)
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
		for (iterStr = mConvexVolumeXmlParam.begin(); iterStr != mConvexVolumeXmlParam.end();
				++iterStr)
		{
			//any "convex_volume" string is a "compound" one, i.e. could have the form:
			// "x1,y1,z1&x2,y2,z2...&xN,yN,zN@area_type"
			std::vector<std::string> pointsAreaTypeStr = parseCompoundString(*iterStr,
					'@');
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
					pointsAreaTypeStr[0], '&');
			std::vector<std::string>::const_iterator iterP;
			for (iterP = pointsStr.begin(); iterP != pointsStr.end(); ++iterP)
			{
				std::vector<std::string> posStr = parseCompoundString(
						*iterP, ',');
				LPoint3f point = LPoint3f::zero();
				for (unsigned int i = 0;
						(i < 3) and (i < posStr.size()); ++i)
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
		for (iterStr = mOffMeshConnectionXmlParam.begin(); iterStr != mOffMeshConnectionXmlParam.end();
				++iterStr)
		{
			//any "offmesh_connection" string is a "compound" one, i.e. has the form:
			// "xB,yB,zB&xE,yE,zE@bidirectional", with bidirectional=true by default.
			std::vector<std::string> pointPairBidirStr = parseCompoundString(*iterStr,
					'@');
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
					std::string(pointPairBidirStr[1]) == "false" ? false : true);
			//iterate over the first 2 points
			//each point defaults to LPoint3f::zero()
			PointPair pointPair(LPoint3f::zero(), LPoint3f::zero());
			std::vector<std::string> pointsStr = parseCompoundString(
					pointPairBidirStr[0], '&');
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
		setFlagsAreaTable(flagsAreaTable);
		///set area cost (for crowd)
		setCostAreaTable(costAreaTable);
		///set crowd include & exclude flags
		setCrowdIncludeExcludeFlags(includeFlags, excludeFlags);
		///set convex volumes
		setConvexVolumes(convexVolumes);
		///set off mesh connections
		setOffMeshConnections(offMeshConnections);
		///3: setup nav mesh
		navMeshSetup();
	}
}

void NavMesh::navMeshSetup()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	//setup navigation mesh, otherwise the same
	//operations must be performed by program.
	PRINT("'" <<getOwnerObject()->objectId()
			<< "'::'" << mComponentId << "'::navMeshSetup");

	///load the mesh from the owner node path
	loadModelMesh(getOwnerObject()->getNodePath());

	///set up the type of navigation mesh
	switch (getNavMeshTypeEnum())
	{
	case SOLO:
	{
		setNavMeshType(new NavMeshType_Solo(), SOLO);
		//set navigation mesh settings
		NavMeshSettings settings = getNavMeshSettings();
		getNavMeshType()->setNavMeshSettings(settings);
	}
		break;
	case TILE:
	{
		setNavMeshType(new NavMeshType_Tile(), TILE);
		//set navigation mesh settings
		NavMeshSettings settings = getNavMeshSettings();
		getNavMeshType()->setNavMeshSettings(settings);
		//set navigation mesh tile settings
		NavMeshTileSettings tileSettings =
				dynamic_cast<NavMeshType_Tile*>(getNavMeshType())->getTileSettings();
		dynamic_cast<NavMeshType_Tile*>(getNavMeshType())->setTileSettings(
				tileSettings);
	}
		break;
	case OBSTACLE:
	{
		setNavMeshType(new NavMeshType_Obstacle(), OBSTACLE);
		//set navigation mesh settings
		NavMeshSettings settings = getNavMeshSettings();
		getNavMeshType()->setNavMeshSettings(settings);
		//set navigation mesh tile settings...
		NavMeshTileSettings tileSettings =
				dynamic_cast<NavMeshType_Obstacle*>(getNavMeshType())->getTileSettings();
		//	evaluate m_maxTiles & m_maxPolysPerTile from m_tileSize
		const float* bmin = getRecastInputGeom()->getMeshBoundsMin();
		const float* bmax = getRecastInputGeom()->getMeshBoundsMax();
		int gw = 0, gh = 0;
		rcCalcGridSize(bmin, bmax, settings.m_cellSize, &gw, &gh);
		const int ts = (int) tileSettings.m_tileSize;
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
		tileSettings.m_maxTiles = 1 << tileBits;
		tileSettings.m_maxPolysPerTile = 1 << polyBits;
		//...effectively
		dynamic_cast<NavMeshType_Obstacle*>(getNavMeshType())->setTileSettings(
				tileSettings);
	}
		break;
	default:
		break;
	}

	///set recast areas' flags table
	getNavMeshType()->setFlagsAreaTable(getFlagsAreaTable());

	///set recast convex volumes
	ConvexVolumeTool* cvTool = new ConvexVolumeTool();
	setTool(cvTool);
	std::list<PointListArea>::iterator iterPLA;
	std::list<PointListArea> convexVolumes = getConvexVolumes();
	for (iterPLA = convexVolumes.begin(); iterPLA != convexVolumes.end();
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
		if(area < 0)
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
			LPoint3f refPos = getReferenceNP().get_relative_point(
					getOwnerObject()->getNodePath(), *iterP);
			//insert convex volume point
			LVecBase3fToRecast(refPos, recastPos);
			getTool()->handleClick(NULL, recastPos, false);
		}
		//re-insert the last point (to close convex volume)
		getTool()->handleClick(NULL, recastPos, false);
	}
	setTool(NULL);

	///set recast off mesh connections
	OffMeshConnectionTool* omcTool = new OffMeshConnectionTool();
	setTool(omcTool);
	std::list<PointPairBidir>::iterator iterPPB;
	std::list<PointPairBidir> offMeshConnections = getOffMeshConnections();
	for (iterPPB = offMeshConnections.begin(); iterPPB != offMeshConnections.end();
			++iterPPB)
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
		refPos = getReferenceNP().get_relative_point(
				getOwnerObject()->getNodePath(), pointPair.first);
		LVecBase3fToRecast(refPos, recastPos);
		getTool()->handleClick(NULL, recastPos, false);
		//compute and insert second recast point
		refPos = getReferenceNP().get_relative_point(
				getOwnerObject()->getNodePath(), pointPair.second);
		LVecBase3fToRecast(refPos, recastPos);
		getTool()->handleClick(NULL, recastPos, false);
	}
	setTool(NULL);

	///build navigation mesh effectively
	buildNavMesh();

	///set recast crowd
	CrowdTool* crowdTool = new CrowdTool();
	setTool(crowdTool);

	///set recast areas' costs
	dtQueryFilter* filter =
			crowdTool->getState()->getCrowd()->getEditableFilter();
	NavMeshPolyAreaCost::const_iterator iterAC;
	NavMeshPolyAreaCost costAreTable = getCostAreaTable();
	for (iterAC = costAreTable.begin(); iterAC != costAreTable.end();
			++iterAC)
	{
		filter->setAreaCost((*iterAC).first, (*iterAC).second);
	}

	///set recast crowd include & exclude flags
	int includeFlags, excludeFlags;
	getCrowdIncludeExcludeFlags(includeFlags, excludeFlags);
	crowdTool->getState()->getCrowd()->getEditableFilter()->setIncludeFlags(
			includeFlags);
	crowdTool->getState()->getCrowd()->getEditableFilter()->setExcludeFlags(
			excludeFlags);

	///Add to the AI manager update
	GameAIManager::GetSingletonPtr()->addToAIUpdate(this);

#ifdef ELY_DEBUG
	mDD->reset();
	getNavMeshType()->handleRender(*mDD);
	getNavMeshType()->getInputGeom()->drawConvexVolumes(mDD);
	getNavMeshType()->getInputGeom()->drawOffMeshConnections(mDD, true);
#endif

}

InputGeom* NavMesh::getRecastInputGeom()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mGeom;
}

dtNavMesh* NavMesh::getRecastNavMesh()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mNavMeshType->getNavMesh();
}

dtNavMeshQuery* NavMesh::getRecastNavMeshQuery()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mNavMeshType->getNavMeshQuery();
}

dtCrowd* NavMesh::getRecastCrowd()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mNavMeshType->getCrowd();
}

float NavMesh::getRecastAgentRadius()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mNavMeshType->getAgentRadius();
}

float NavMesh::getRecastAgentHeight()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mNavMeshType->getAgentHeight();
}

float NavMesh::getRecastAgentClimb()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mNavMeshType->getAgentClimb();
}

LVecBase3f NavMesh::getRecastBoundsMin()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	CHECKEXISTENCE(mGeom,
			"NavMesh::getBoundsMin: invalid InputGeom")
	return RecastToLVecBase3f(mGeom->getMeshBoundsMin());
}

LVecBase3f NavMesh::getRecastBoundsMax()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	CHECKEXISTENCE(mGeom,
			"NavMesh::getBoundsMax: invalid InputGeom")
	return RecastToLVecBase3f(mGeom->getMeshBoundsMax());
}

void NavMesh::setNavMeshTypeEnum(NavMeshTypeEnum typeEnum)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	mNavMeshTypeEnum = typeEnum;
}

NavMeshTypeEnum NavMesh::getNavMeshTypeEnum()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mNavMeshTypeEnum;
}

NavMeshType* NavMesh::getNavMeshType()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mNavMeshType;
}

AgentMovType NavMesh::getMovType()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mMovType;
}

void NavMesh::setMovType(AgentMovType movType)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	mMovType = movType;
}

void NavMesh::setNavMeshSettings(const NavMeshSettings& settings)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	mNavMeshSettings = settings;
}

NavMeshSettings NavMesh::getNavMeshSettings()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mNavMeshSettings;
}

void NavMesh::setTool(NavMeshTypeTool* tool)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	mNavMeshType->setTool(tool);
}

NavMeshTypeTool* NavMesh::getTool()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mNavMeshType->getTool();
}

void NavMesh::setFlagsAreaTable(const NavMeshPolyAreaFlags& flagsAreaTable)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	mPolyAreaFlags = flagsAreaTable;
}

NavMeshPolyAreaFlags NavMesh::getFlagsAreaTable()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mPolyAreaFlags;
}

void NavMesh::setCostAreaTable(const NavMeshPolyAreaCost& costAreaTable)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	mPolyAreaCost = costAreaTable;
}

NavMeshPolyAreaCost NavMesh::getCostAreaTable()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mPolyAreaCost;
}

void NavMesh::setCrowdIncludeExcludeFlags(int includeFlags, int excludeFlags)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	mCrowdIncludeFlags = includeFlags;
	mCrowdExcludeFlags = excludeFlags;
}

void NavMesh::getCrowdIncludeExcludeFlags(int& includeFlags, int& excludeFlags)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	includeFlags = mCrowdIncludeFlags;
	excludeFlags = mCrowdExcludeFlags;
}

void NavMesh::setNavMeshTileSettings(const NavMeshTileSettings& settings)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	mNavMeshTileSettings = settings;
}

NavMeshTileSettings NavMesh::getNavMeshTileSettings()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mNavMeshTileSettings;
}

void NavMesh::setConvexVolumes(const std::list<PointListArea>& convexVolumes)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	mConvexVolumes = convexVolumes;
}

std::list<NavMesh::PointListArea> NavMesh::getConvexVolumes()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mConvexVolumes;
}

void NavMesh::setOffMeshConnections(
		const std::list<PointPairBidir>& offMeshConnections)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	mOffMeshConnections = offMeshConnections;
}

std::list<NavMesh::PointPairBidir> NavMesh::getOffMeshConnections()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mOffMeshConnections;
}

NodePath NavMesh::getReferenceNP()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mReferenceNP;
}

void NavMesh::getTilePos(const LPoint3f& pos, int& tx, int& ty)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

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
}

void NavMesh::buildTile(const LPoint3f& pos)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	if (mNavMeshTypeEnum == TILE)
	{
		float recastPos[3];
		LVecBase3fToRecast(pos, recastPos);
		dynamic_cast<NavMeshType_Tile*>(mNavMeshType)->buildTile(recastPos);
		PRINT("'" << getOwnerObject()->objectId() << "'::'"
				<< mComponentId << "'::buildTile : " << pos);
#ifdef ELY_DEBUG
		mDD->reset();
		mNavMeshType->handleRender(*mDD);
#endif
	}
}

void NavMesh::removeTile(const LPoint3f& pos)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	if (mNavMeshTypeEnum == TILE)
	{
		float recastPos[3];
		LVecBase3fToRecast(pos, recastPos);
		dynamic_cast<NavMeshType_Tile*>(mNavMeshType)->removeTile(recastPos);
		PRINT("'" << getOwnerObject()->objectId() << "'::'"
				<< mComponentId << "'::removeTile : " << pos);
#ifdef ELY_DEBUG
		mDD->reset();
		mNavMeshType->handleRender(*mDD);
#endif
	}
}

void NavMesh::buildAllTiles()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	if (mNavMeshTypeEnum == TILE)
	{
		dynamic_cast<NavMeshType_Tile*>(mNavMeshType)->buildAllTiles();
#ifdef ELY_DEBUG
		mDD->reset();
		mNavMeshType->handleRender(*mDD);
#endif
	}
}

void NavMesh::removeAllTiles()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	if (mNavMeshTypeEnum == TILE)
	{
		dynamic_cast<NavMeshType_Tile*>(mNavMeshType)->removeAllTiles();
#ifdef ELY_DEBUG
		mDD->reset();
		mNavMeshType->handleRender(*mDD);
#endif
	}
}

dtTileCache* NavMesh::getTileCache()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	if (mNavMeshTypeEnum == OBSTACLE)
	{
		return dynamic_cast<NavMeshType_Obstacle*>(mNavMeshType)->getTileCache();
	}
	return NULL;
}

void NavMesh::addObstacle(SMARTPTR(Object)object)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

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
		PRINT("'" << getOwnerObject()->objectId() << "'::'"
		<< mComponentId << "'::addObstacle: '" << object->objectId()
		<< "' at pos: " << pos);
#ifdef ELY_DEBUG
		mDD->reset();
		mNavMeshType->handleRender(*mDD);
#endif
	}
}

void NavMesh::removeObstacle(SMARTPTR(Object)object)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	if ((mNavMeshTypeEnum == OBSTACLE) and
	(not mReferenceNP.is_empty()) and object)
	{
		//get obstacle ref
		dtObstacleRef obstacleRef = mObstacles[object];
		if (obstacleRef == 0)
		{
			//dtObstacleRef cannot be zero
			return;
		}
		//remove recast obstacle
		dtTileCache* tileCache =
		dynamic_cast<NavMeshType_Obstacle*>(mNavMeshType)->getTileCache();
		tileCache->removeObstacle(obstacleRef);
		//update tile cache
		tileCache->update(0, mNavMeshType->getNavMesh());
		//remove from obstacle table
		mObstacles.erase(object);
		PRINT("'" << getOwnerObject()->objectId() << "'::'"
		<< mComponentId << "'::removeObstacle: '" << object->objectId() << "'");
#ifdef ELY_DEBUG
		mDD->reset();
		mNavMeshType->handleRender(*mDD);
#endif
	}
}

void NavMesh::clearAllObstacles()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	if (mNavMeshTypeEnum == OBSTACLE)
	{
		dynamic_cast<NavMeshType_Obstacle*>(mNavMeshType)->clearAllTempObstacles();
		PRINT("'" << getOwnerObject()->objectId() << "'::'"
				<< mComponentId << "'::clearAllObstacles");
#ifdef ELY_DEBUG
		mDD->reset();
		mNavMeshType->handleRender(*mDD);
#endif
	}
}

bool NavMesh::loadModelMesh(NodePath model)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	bool result = true;
	mGeom = new InputGeom;
	mMeshName = model.get_name();
	//
	if (not mGeom->loadMesh(mCtx, NULL, model, mReferenceNP))
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

void NavMesh::setNavMeshType(NavMeshType* navMeshType,
		NavMeshTypeEnum navMeshTypeEnum)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	//set the navigation mesh type
	mNavMeshType = navMeshType;
	mNavMeshTypeEnum = navMeshTypeEnum;
	//set rcContext
	mNavMeshType->setContext(mCtx);
	//handle Mesh Changed
	mNavMeshType->handleMeshChanged(mGeom);
}

bool NavMesh::buildNavMesh()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

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

void NavMesh::addCrowdAgent(SMARTPTR(Object)crowdAgentObject)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	SMARTPTR(CrowdAgent)crowdAgent =
			DCAST(CrowdAgent,crowdAgentObject->getComponent(componentType()));
	std::list<SMARTPTR(CrowdAgent)>::iterator iterCA;
	iterCA = find(mCrowdAgents.begin(), mCrowdAgents.end(), crowdAgent);
	if(iterCA != mCrowdAgents.end())
	{
		//set the mov type of the crowd agent
		crowdAgent->setMovType(mMovType);
		mCrowdAgents.push_back(crowdAgent);
	}
}

void NavMesh::removeCrowdAgent(SMARTPTR(Object)crowdAgentObject)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	SMARTPTR(CrowdAgent)crowdAgent =
			DCAST(CrowdAgent,crowdAgentObject->getComponent(componentType()));
	std::list<SMARTPTR(CrowdAgent)>::iterator iterCA;
	iterCA = find(mCrowdAgents.begin(), mCrowdAgents.end(), crowdAgent);
	if(iterCA != mCrowdAgents.end())
	{
		//set the mov type of the crowd agent to default (RECAST)
		crowdAgent->setMovType(RECAST);
		mCrowdAgents.erase(iterCA);
	}
}

void NavMesh::update(void* data)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	float dt = *(reinterpret_cast<float*>(data));

#ifdef TESTING
	dt = 0.016666667; //60 fps
#endif

}

#ifdef ELY_DEBUG

NodePath NavMesh::getDebugNodePath() const
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mDebugNodePath;
}

void NavMesh::debug(bool enable)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	if (mDebugNodePath.is_empty())
	{
		return;
	}
	if (enable)
	{
		if (mDebugNodePath.is_hidden())
		{
			mDebugNodePath.show();
		}
	}
	else
	{
		if (not mDebugNodePath.is_hidden())
		{
			mDebugNodePath.hide();
		}
	}
}

#endif

//TypedObject semantics: hardcoded
TypeHandle NavMesh::_type_handle;
}  // namespace ely
