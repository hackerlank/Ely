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
#include "ObjectModel/Object.h"
#include "ObjectModel/ObjectTemplateManager.h"
#include "Game/GameAIManager.h"

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
	mAutoBuild = (
			mTmpl->parameter(std::string("auto_build"))
					== std::string("false") ? false : true);
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

#ifdef ELY_DEBUG
	//set the recast debug node path as child of "render" node path
	//set the recast debug camera to the first child of "camera" node path
	SMARTPTR(Object) renderDebug = ObjectTemplateManager::GetSingletonPtr()->
			getCreatedObject("render");
	SMARTPTR(Object) cameraDebug = ObjectTemplateManager::GetSingletonPtr()->
			getCreatedObject("camera");
	if (renderDebug and cameraDebug)
	{
		mDebugNodePath = renderDebug->getNodePath().attach_new_node("RecastDebugNodePath");
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

	//build navigation mesh if auto build is true
	if (mAutoBuild)
	{
		PRINT("'" <<getOwnerObject()->objectId()
				<< "'::'" << mComponentId << "'::onAddToSceneSetup");
		//load the mesh from the owner node path
		loadModelMesh(mOwnerObject->getNodePath());
		//set up the type of navigation mesh
		switch (mNavMeshTypeEnum)
		{
		case SOLO:
			setupNavMesh(new NavMeshType_Solo(), SOLO);
			//set navigation mesh settings
			setNavMeshSettings(mNavMeshSettings);
			break;
		case TILE:
		{
			setupNavMesh(new NavMeshType_Tile(), TILE);
			//set navigation mesh settings
			setNavMeshSettings(mNavMeshSettings);
			//set navigation mesh tile settings
			setNavMeshTileSettings(mNavMeshTileSettings);
		}
			break;
		case OBSTACLE:
		{
			setupNavMesh(new NavMeshType_Obstacle(), OBSTACLE);
			//set navigation mesh settings
			setNavMeshSettings(mNavMeshSettings);
			//set navigation mesh tile settings...
			//	evaluate m_maxTiles & m_maxPolysPerTile from m_tileSize
			const float* bmin = getInputGeom()->getMeshBoundsMin();
			const float* bmax = getInputGeom()->getMeshBoundsMax();
			int gw = 0, gh = 0;
			rcCalcGridSize(bmin, bmax, mNavMeshSettings.m_cellSize, &gw, &gh);
			const int ts = (int) mNavMeshTileSettings.m_tileSize;
			const int tw = (gw + ts - 1) / ts;
			const int th = (gh + ts - 1) / ts;
			//	Max tiles and max polys affect how the tile IDs are calculated.
			// 	There are 22 bits available for identifying a tile and a polygon.
			int tileBits = rcMin((int)ilog2(nextPow2(tw*th)), 14);
			if (tileBits > 14) tileBits = 14;
			int polyBits = 22 - tileBits;
			mNavMeshTileSettings.m_maxTiles = 1 << tileBits;
			mNavMeshTileSettings.m_maxPolysPerTile = 1 << polyBits;
			//...effectively
			setNavMeshTileSettings(mNavMeshTileSettings);
		}
			break;
		default:
			break;
		}

		///TODO
		//set convex volume tool
		//set off mesh connection tool

		//build navigation mesh effectively
		buildNavMesh();

#ifdef ELY_DEBUG
		mDD->reset();
		mNavMeshType->handleRender(*mDD);
#endif
	}
}

NavMeshType* NavMesh::getNavMeshType()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mNavMeshType;
}

NavMeshTypeEnum NavMesh::getNavMeshTypeEnum()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mNavMeshTypeEnum;
}

InputGeom* NavMesh::getInputGeom()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mGeom;
}

dtNavMesh* NavMesh::getNavMesh()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mNavMeshType->getNavMesh();
}

dtNavMeshQuery* NavMesh::getNavMeshQuery()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mNavMeshType->getNavMeshQuery();
}

dtCrowd* NavMesh::getCrowd()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mNavMeshType->getCrowd();
}

float NavMesh::getAgentRadius()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mNavMeshType->getAgentRadius();
}

float NavMesh::getAgentHeight()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mNavMeshType->getAgentHeight();
}

float NavMesh::getAgentClimb()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mNavMeshType->getAgentClimb();
}

LVecBase3f NavMesh::getBoundsMin()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	CHECKEXISTENCE(mGeom,
			"NavMesh::getBoundsMin: invalid InputGeom")
	return RecastToLVecBase3f(mGeom->getMeshBoundsMin());
}

LVecBase3f NavMesh::getBoundsMax()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	CHECKEXISTENCE(mGeom,
			"NavMesh::getBoundsMax: invalid InputGeom")
	return RecastToLVecBase3f(mGeom->getMeshBoundsMax());
}

void NavMesh::setNavMeshSettings(const NavMeshSettings& settings)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	mNavMeshType->setNavMeshSettings(settings);
}

NavMeshSettings NavMesh::getNavMeshSettings()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mNavMeshType->getNavMeshSettings();
}

void NavMesh::setNavMeshTileSettings(const NavMeshTileSettings& settings)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	if (mNavMeshTypeEnum == TILE)
	{
		dynamic_cast<NavMeshType_Tile*>(mNavMeshType)->setTileSettings(
				settings);
	}
	else if (mNavMeshTypeEnum == OBSTACLE)
	{
		dynamic_cast<NavMeshType_Obstacle*>(mNavMeshType)->setTileSettings(
				settings);
	}
}

NavMeshTileSettings NavMesh::getNavMeshTileSettings()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	NavMeshTileSettings settings;
	if(mNavMeshTypeEnum == TILE)
	{
		settings = dynamic_cast<NavMeshType_Tile*>(mNavMeshType)
				->getTileSettings();
	}
	else if (mNavMeshTypeEnum == OBSTACLE)
	{
		settings = dynamic_cast<NavMeshType_Obstacle*>(mNavMeshType)
						->getTileSettings();
	}
	return settings;
}

void NavMesh::getTilePos(const LPoint3f& pos, int& tx, int& ty)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	float recastPos[3];
	LVecBase3fToRecast(pos, recastPos);
	if(mNavMeshTypeEnum == TILE)
	{
		dynamic_cast<NavMeshType_Tile*>(mNavMeshType)
				->getTilePos(recastPos, tx, ty);
	}
	else if (mNavMeshTypeEnum == OBSTACLE)
	{
		dynamic_cast<NavMeshType_Obstacle*>(mNavMeshType)
				->getTilePos(recastPos, tx, ty);
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

void NavMesh::addObstacle(SMARTPTR(Object) object)
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

void NavMesh::removeObstacle(SMARTPTR(Object) object)
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
	//get model's parent node path as reference
	mReferenceNP = model.get_parent();
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

void NavMesh::setupNavMesh(NavMeshType* navMeshType,
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