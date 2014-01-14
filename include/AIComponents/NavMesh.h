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
 * \file /Ely/include/AIComponents/NavMesh.h
 *
 * \date 23/giu/2013 (18:51:03)
 * \author consultit
 */

#ifndef NAVMESH_H_
#define NAVMESH_H_

#include "RecastNavigation/NavMeshType.h"
#include "RecastNavigation/InputGeom.h"
#include "RecastNavigation/DebugInterfaces.h"
#include "RecastNavigation/CrowdTool.h"
#include "ObjectModel/Component.h"
#include "CrowdAgent.h"
#include <DetourCrowd.h>
#include <DetourTileCache.h>
#include <nodePath.h>
#include <conditionVar.h>

namespace ely
{

class NavMeshTemplate;

/**
 * \brief Component implementing dtNavMesh from Recast Navigation library.
 *
 * \see https://code.google.com/p/recastnavigation
 * 		http://digestingduck.blogspot.it
 * 		https://groups.google.com/forum/?fromgroups#!forum/recastnavigation
 *
 * This component should be used only in association to stationary
 * (i.e. is_steady=true) "Scene" components.\n
 * \note convex volumes and off mesh connections points are are given wrt
 * the scaled owner object node path.
 *
 * XML Param(s):
 * - "navmesh_type"					|single|"solo" (values: solo|tile|obstacle)
 * - "auto_setup"					|single|"true"
 * - "cell_size"					|single|"0.3"
 * - "cell_height"					|single|"0.2"
 * - "agent_height"					|single|"2.0"
 * - "agent_radius"					|single|"0.6"
 * - "agent_max_climb"				|single|"0.9"
 * - "agent_max_slope"				|single|"45.0"
 * - "region_min_size"				|single|"8"
 * - "region_merge_size"			|single|"20"
 * - "monotone_partitioning"		|single|"false"
 * - "edge_max_len"					|single|"12.0"
 * - "edge_max_error"				|single|"1.3"
 * - "verts_per_poly"				|single|"6.0"
 * - "detail_sample_dist"			|single|"6.0"
 * - "detail_sample_max_error"		|single|"1.0"
 * - "build_all_tiles"				|single|"false"
 * - "max_tiles"					|single|"128"
 * - "max_polys_per_tile"			|single|"32768"
 * - "tile_size"					|single|"32"
 * - "area_flags_cost"				|multiple|no default (each one specified as "area@flag1[:flag2...:flagN]@cost" note: flags are or-ed)
 * - "crowd_include_flags"			|single|no default (specified as "flag1[:flag2...:flagN]" note: flags are or-ed)
 * - "crowd_exclude_flags"			|single|no default (specified as "flag1[:flag2...:flagN]" note: flags are or-ed)
 * - "convex_volume"				|multiple|no default (each one specified as "x1,y1,z1[:x2,y2,z2...:xN,yN,zN]@area_type")
 * - "offmesh_connection"			|multiple|no default (each one specified as	"xB,yB,zB:xE,yE,zE@bidirectional")
 *
 * \note parts inside [] are optional.\n
 */
class NavMesh: public Component
{
protected:
	friend class NavMeshTemplate;

	virtual void reset();
	virtual bool initialize();
	virtual void onAddToObjectSetup();
	virtual void onRemoveFromObjectCleanup();
	virtual void onAddToSceneSetup();
	virtual void onRemoveFromSceneCleanup();

public:
	NavMesh();
	NavMesh(SMARTPTR(NavMeshTemplate)tmpl);
	virtual ~NavMesh();

	virtual ComponentFamilyType familyType() const;
	virtual ComponentType componentType() const;

	/**
	 * \brief Updates position/orientation of crowd agents.
	 *
	 * Will be called automatically by an ai manager update.
	 * @param data The custom data.
	 */
	virtual void update(void* data);

	///Helper typedefs.
	//convex volume
	typedef std::list<LPoint3f> PointList;
	typedef std::pair<PointList,int> PointListArea;
	//off mesh connection
	typedef std::pair<LPoint3f, LPoint3f> PointPair;
	typedef std::pair<PointPair,bool> PointPairBidir;

	struct Result: public Component::Result
	{
		Result(int value):Component::Result(value)
		{
		}
		enum
		{
			NAVMESHTYPE_NULL = COMPONENT_RESULT_END + 1,
			NAVMESH_RESULT_END
		};
	};

	/**
	 * \brief NavMesh related methods.
	 */
	///@{
	//SOLO TILE OBSTACLE
	void setNavMeshTypeEnum(NavMeshTypeEnum typeEnum);
	NavMeshTypeEnum getNavMeshTypeEnum() const;
	void setNavMeshSettings(const NavMeshSettings& settings);
	NavMeshSettings getNavMeshSettings() const;
	std::list<SMARTPTR(CrowdAgent)> getCrowdAgents();
	//TILE OBSTACLE
	void setNavMeshTileSettings(const NavMeshTileSettings& settings);
	NavMeshTileSettings getNavMeshTileSettings() const;
	Result getTilePos(const LPoint3f& pos, int& tx, int& ty);
	//TILE
	Result buildTile(const LPoint3f& pos);
	Result removeTile(const LPoint3f& pos);
	Result buildAllTiles();
	Result removeAllTiles();
	//OBSTACLE
	dtTileCache* getTileCache();
	Result addObstacle(SMARTPTR(Object) object);
	Result removeObstacle(SMARTPTR(Object) object);
	Result clearAllObstacles();
	///@}

	/**
	 * \brief Recast nav mesh related methods.
	 */
	///@{
	InputGeom* getRecastInputGeom() const;
	dtNavMesh* getRecastNavMesh() const;
	dtNavMeshQuery* getRecastNavMeshQuery() const;
	dtCrowd* getRecastCrowd() const;
	float getRecastAgentRadius() const;
	float getRecastAgentHeight() const;
	float getRecastAgentClimb() const;
	LVecBase3f getRecastBoundsMin() const;
	LVecBase3f getRecastBoundsMax() const;
	///@}

	/**
	 * \brief Adds a CrowdAgent component to the dtCrowd handling
	 * mechanism.
	 *
	 * If CrowdAgent belongs to any NavMesh it is not added.\n
	 * @param crowdAgent The CrowdAgent to add.
	 * @return Result::OK on successful addition, various error conditions otherwise.
	 */
	Result addCrowdAgent(SMARTPTR(CrowdAgent)crowdAgent);

	/**
	 * \brief Removes a CrowdAgent component from the dtCrowd handling
	 * mechanism.
	 *
	 * If CrowdAgent doesn't belong to any NavMesh it is not removed.\n
	 * @param crowdAgent The CrowdAgent to remove.
	 * @return Result::OK on successful removal, various error conditions otherwise.
	 */
	Result removeCrowdAgent(SMARTPTR(CrowdAgent)crowdAgent);

	///@{
	///CrowdAgents' settings setters.
	Result setCrowdAgentParams(SMARTPTR(CrowdAgent)crowdAgent,
			const dtCrowdAgentParams& params);
	Result setCrowdAgentTarget(SMARTPTR(CrowdAgent)crowdAgent,
			const LPoint3f& moveTarget);
	Result setCrowdAgentVelocity(SMARTPTR(CrowdAgent)crowdAgent,
			const LVector3f& moveVelocity);
	///@}

	/**
	 * \brief Sets up NavMesh to be ready for CrowdAgents handling.
	 *
	 * This method can be called at startup or during program execution
	 * repeatedly.\n
	 * Before calling this method NavMeshType, MovType, NavMeshSettings
	 * and NavMeshTileSettings can be set up.\n
	 * The other settings: area types, area ability flags, area cost,
	 * crowd include/exclude flags, convex volumes and off mesh connections,
	 * can be setup only at construction time.
	 */
	Result navMeshSetup();

	/**
	 * \brief Clean up NavMesh.
	 */
	Result navMeshCleanup();

	/**
	 * \name NavMeshType reference getter & conversion function.
	 */
	///@{
	NavMeshType& getNavMeshType();
	operator NavMeshType&();
	///@}

#ifdef ELY_DEBUG
	/**
	 * \brief Gets a reference to the Recast Debug node.
	 * @return The Recast Debug node.
	 */
	NodePath getDebugNodePath() const;
	/**
	 * \brief Enables/disables debugging.
	 * @param enable True to enable, false to disable.
	 */
	Result debug(bool enable);
#endif

private:
	///Current underlying NavMeshType.
	NavMeshType* mNavMeshType;
	///Current mesh type.
	NavMeshTypeEnum mNavMeshTypeEnum;
	///Input geometry.
	InputGeom* mGeom;
	///Build context.
	BuildContext* mCtx;
	///The mesh name.
	std::string mMeshName;
	///The reference node path (read only after creation).
	NodePath mReferenceNP;
	///NavMeshSettings from template.
	NavMeshSettings mNavMeshSettings;
	///NavMeshTileSettings from template.
	NavMeshTileSettings mNavMeshTileSettings;
	///Area with flags and cost xml settings.
	std::list<std::string> mAreaFlagsCostXmlParam;
	///Area types with ability flags settings.
	NavMeshPolyAreaFlags mPolyAreaFlags;
	///Area types with cost settings.
	NavMeshPolyAreaCost mPolyAreaCost;
	///Crowd include & exclude flags settings.
	std::string mCrowdIncludeFlagsParam, mCrowdExcludeFlagsParam;
	int mCrowdIncludeFlags, mCrowdExcludeFlags;
	///Convex volumes.
	std::list<std::string> mConvexVolumesParam;
	std::list<PointListArea> mConvexVolumes;
	///Off mesh connections.
	std::list<std::string> mOffMeshConnectionsParam;
	std::list<PointPairBidir> mOffMeshConnections;
	///Auto setup: true (default) if navigation mesh
	///is to be setup during component creation (specifically
	///when the owner object is added to scene), false if it
	///will be built manually during program execution.
	bool mAutoSetup;
	/// Obstacles table
	std::map<SMARTPTR(Object), dtObstacleRef> mObstacles;
	/**
	 * \brief Crowd related data.
	 */
	///@{
	///The CrowdAgent components handled by this NavMesh.
	std::list<SMARTPTR(CrowdAgent)> mCrowdAgents;
	///@}

	/**
	 * \brief Loads the mesh from a model node path.
	 *
	 * \note: the model's parent node path is the reference wrt
	 * any calculation are performed.
	 * @param model The model's node path.
	 * @return True if successful, false otherwise.
	 */
	bool doLoadModelMesh(NodePath model);

	/**
	 * \brief Creates the navigation mesh type for the loaded model mesh.
	 * @param navMeshType The navigation mesh type.
	 * @param navMeshTypeEnum The navigation mesh enum type.
	 */
	void doCreateNavMeshType(NavMeshType* navMeshType);

	/**
	 * \brief Builds the navigation mesh for the loaded model mesh.
	 * @return True if successful, false otherwise.
	 */
	bool doBuildNavMesh();

	/**
	 * \brief Actually sets up NavMesh to be ready for CrowdAgents handling.
	 */
	void doNavMeshSetup();

	/**
	 * \brief Actually cleans up NavMesh.
	 */
	void doNavMeshCleanup();

	/**
	 * \brief Adds CrowdAgent to update list.
	 */
	void doAddCrowdAgentToUpdateList(SMARTPTR(CrowdAgent)crowdAgent);

	/**
	 * \brief Adds CrowdAgent to recast update.
	 */
	bool doAddCrowdAgentToRecastUpdate(SMARTPTR(CrowdAgent)crowdAgent);

	/**
	 * \brief Removes CrowdAgent from update list.
	 */
	void doRemoveCrowdAgentFromUpdateList(SMARTPTR(CrowdAgent)crowdAgent);

	/**
	 * \brief Removes CrowdAgent from recast update.
	 */
	void doRemoveCrowdAgentFromRecastUpdate(SMARTPTR(CrowdAgent)crowdAgent);

	/**
	 * \brief Inline helper: only called by other methods.
	 */
	void doSetCrowdAgentOtherSettings(
			SMARTPTR(CrowdAgent)crowdAgent, CrowdTool* crowdTool);

	///@{
	///A task data to do asynchronous NavMesh setup.
	SMARTPTR(TaskInterface<NavMesh>::TaskData) mUpdateData;
	SMARTPTR(AsyncTask) mUpdateTask;
	AsyncTask::DoneStatus navMeshAsyncSetup(GenericAsyncTask* task);
#ifdef ELY_THREAD
	std::string mTaskChainName;
	ConditionVar mAsyncSetupVar;
	Mutex mAsyncSetupMutex;
	int mAsyncSetupCallCounter;
	bool mAsyncSetupComplete;
#endif
	///@}

#ifdef ELY_DEBUG
	/// Recast debug node path.
	NodePath mDebugNodePath;
	/// Recast debug camera.
	NodePath mDebugCamera;
	/// DebugDrawers.
	DebugDrawPanda3d* mDD;
	DebugDrawMeshDrawer* mDDM;
	///Enable Draw update
	bool mEnableDrawUpdate;
	/// Debug render with DebugDrawPanda3d.
	void doDebugStaticRender();
	///@{
	///A task data to do asynchronous debug render.
	SMARTPTR(TaskInterface<NavMesh>::TaskData) mDebugRenderData;
	SMARTPTR(AsyncTask) mDebugRenderTask;
	AsyncTask::DoneStatus debugStaticRenderTask(GenericAsyncTask* task);
	///@}
#endif

	///TypedObject semantics: hardcoded
public:
	static TypeHandle get_class_type()
	{
		return _type_handle;
	}
	static void init_type()
	{
		Component::init_type();
		register_type(_type_handle, "NavMesh", Component::get_class_type());
	}
	virtual TypeHandle get_type() const
	{
		return get_class_type();
	}
	virtual TypeHandle force_init_type()
	{
		init_type();
		return get_class_type();
	}

private:
	static TypeHandle _type_handle;

};

///inline definitions

inline void NavMesh::reset()
{
	//
	mNavMeshType = NULL;
	mNavMeshTypeEnum = SOLO;
	mGeom = NULL;
	mCtx = NULL;
	mMeshName.clear();
	mReferenceNP = NodePath();
	mNavMeshSettings = NavMeshSettings();
	mNavMeshTileSettings = NavMeshTileSettings();
	mAreaFlagsCostXmlParam.clear();
	mPolyAreaFlags.clear();
	mPolyAreaCost.clear();
	mCrowdIncludeFlagsParam.clear();
	mCrowdExcludeFlagsParam.clear();
	mCrowdIncludeFlags = mCrowdExcludeFlags = 0;
	mConvexVolumesParam.clear();
	mConvexVolumes.clear();
	mOffMeshConnectionsParam.clear();
	mOffMeshConnections.clear();
	mAutoSetup = true;
	mObstacles.clear();
	mCrowdAgents.clear();
	mUpdateData.clear();
	mUpdateTask.clear();
#ifdef ELY_THREAD
	mTaskChainName.clear();
	mAsyncSetupCallCounter = 0;
#endif

#ifdef ELY_DEBUG
	mDebugNodePath = NodePath();
	mDebugCamera = NodePath();
	mDD = NULL;
	mDDM = NULL;
	mEnableDrawUpdate = false;
	mDebugRenderData.clear();
	mDebugRenderTask.clear();
#endif
}

inline void NavMesh::setNavMeshTypeEnum(NavMeshTypeEnum typeEnum)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	//return if async-setup is not complete
	RETURN_ON_ASYNC_COND(not mAsyncSetupComplete,)

	mNavMeshTypeEnum = typeEnum;
}

inline NavMeshTypeEnum NavMesh::getNavMeshTypeEnum() const
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	//return if async-setup is not complete
	RETURN_ON_ASYNC_COND(not mAsyncSetupComplete, NavMeshType_NONE)

	return mNavMeshTypeEnum;
}

inline void NavMesh::setNavMeshSettings(const NavMeshSettings& settings)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	//return if async-setup is not complete
	RETURN_ON_ASYNC_COND(not mAsyncSetupComplete,)

	mNavMeshSettings = settings;
}

inline NavMeshSettings NavMesh::getNavMeshSettings() const
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	//return if async-setup is not complete
	RETURN_ON_ASYNC_COND(not mAsyncSetupComplete, NavMeshSettings())

	return mNavMeshSettings;
}

inline void NavMesh::setNavMeshTileSettings(const NavMeshTileSettings& settings)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	//return if async-setup is not complete
	RETURN_ON_ASYNC_COND(not mAsyncSetupComplete,)

	mNavMeshTileSettings = settings;
}

inline NavMeshTileSettings NavMesh::getNavMeshTileSettings() const
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	//return if async-setup is not complete
	RETURN_ON_ASYNC_COND(not mAsyncSetupComplete, NavMeshTileSettings())

	return mNavMeshTileSettings;
}

inline std::list<SMARTPTR(CrowdAgent)> NavMesh::getCrowdAgents()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	//return if async-setup is not complete
	RETURN_ON_ASYNC_COND(not mAsyncSetupComplete, std::list<SMARTPTR(CrowdAgent)>())

	return mCrowdAgents;
}

inline InputGeom* NavMesh::getRecastInputGeom() const
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	//return if async-setup is not complete
	RETURN_ON_ASYNC_COND(not mAsyncSetupComplete, NULL)

	return mGeom;
}

inline dtNavMesh* NavMesh::getRecastNavMesh() const
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	//return if async-setup is not complete
	RETURN_ON_ASYNC_COND(not mAsyncSetupComplete, NULL)

	return (mNavMeshType ? mNavMeshType->getNavMesh() : NULL);
}

inline dtNavMeshQuery* NavMesh::getRecastNavMeshQuery() const
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	//return if async-setup is not complete
	RETURN_ON_ASYNC_COND(not mAsyncSetupComplete, NULL)

	return (mNavMeshType ? mNavMeshType->getNavMeshQuery() : NULL);
}

inline dtCrowd* NavMesh::getRecastCrowd() const
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	//return if async-setup is not complete
	RETURN_ON_ASYNC_COND(not mAsyncSetupComplete, NULL)

	return (mNavMeshType ? mNavMeshType->getCrowd() : NULL);
}

inline float NavMesh::getRecastAgentRadius() const
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	//return if async-setup is not complete
	RETURN_ON_ASYNC_COND(not mAsyncSetupComplete, 0.0)

	return (mNavMeshType ? mNavMeshType->getAgentRadius() : 0.0);
}

inline float NavMesh::getRecastAgentHeight() const
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	//return if async-setup is not complete
	RETURN_ON_ASYNC_COND(not mAsyncSetupComplete, 0.0)

	return (mNavMeshType ? mNavMeshType->getAgentHeight() : 0.0);
}

inline float NavMesh::getRecastAgentClimb() const
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	//return if async-setup is not complete
	RETURN_ON_ASYNC_COND(not mAsyncSetupComplete, 0.0)

	return (mNavMeshType ? mNavMeshType->getAgentClimb() : 0.0);
}

inline LVecBase3f NavMesh::getRecastBoundsMin() const
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	//return if async-setup is not complete
	RETURN_ON_ASYNC_COND(not mAsyncSetupComplete, LVecBase3f::zero())

	return (mGeom ?
			RecastToLVecBase3f(mGeom->getMeshBoundsMin()) : LVecBase3f::zero());
}

inline LVecBase3f NavMesh::getRecastBoundsMax() const
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	//return if async-setup is not complete
	RETURN_ON_ASYNC_COND(not mAsyncSetupComplete, LVecBase3f::zero())

	return (mGeom ?
			RecastToLVecBase3f(mGeom->getMeshBoundsMax()) : LVecBase3f::zero());
}

inline NavMeshType& NavMesh::getNavMeshType()
{
	return *mNavMeshType;
}

inline NavMesh::operator NavMeshType&()
{
	return *mNavMeshType;
}

} // namespace ely

#endif /* NAVMESH_H_ */
