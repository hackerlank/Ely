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
#include "ObjectModel/Component.h"
#include "CrowdAgent.h"
#include <DetourCrowd.h>
#include <DetourTileCache.h>
#include <nodePath.h>

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
 * - "navmesh_type"					|single|"solo" (solo|tile|obstacle)
 * - "auto_setup"					|single|"true"
 * - "mov_type"						|single|"recast" (recast|kinematic|character)
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
 * - "area_flags_cost"				|multiple|no default (each specified as
 * "area@flag1|flag2...|flagN@cost")
 * - "crowd_include_flags"			|single|no default (specified as "flag1|flag2...|flagN")
 * - "crowd_exclude_flags"			|single|no default (specified as "flag1|flag2...|flagN")
 * - "convex_volume"				|multiple|no default (each specified as
 * 	"x1,y1,z1&x2,y2,z2...&xN,yN,zN@area_type")
 * - "offmesh_connection"			|multiple|no default (each specified as
 * 	"xB,yB,zB&xE,yE,zE@bidirectional")
 */
class NavMesh: public Component
{
protected:
	friend class Object;
	friend class NavMeshTemplate;

	virtual bool initialize();
	virtual void onAddToObjectSetup();
	virtual void onAddToSceneSetup();

public:
	NavMesh();
	NavMesh(SMARTPTR(NavMeshTemplate)tmpl);
	virtual ~NavMesh();

	virtual ComponentFamilyType familyType() const;
	virtual ComponentType componentType() const;

	/**
	 * \brief Updates position/orientation of crowd agents.
	 *
	 * Will be called automatically by an control manager update.
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

	/**
	 * \brief NavMesh related methods.
	 */
	///@{
	//SOLO TILE OBSTACLE
	void setNavMeshType(NavMeshTypeEnum typeEnum);
	NavMeshTypeEnum getNavMeshType() const;
	void setMovType(AgentMovType movType);
	AgentMovType getMovType() const;
	void setNavMeshSettings(const NavMeshSettings& settings);
	NavMeshSettings getNavMeshSettings() const;
	std::list<SMARTPTR(CrowdAgent)> getCrowdAgents();
	//TILE OBSTACLE
	void setNavMeshTileSettings(const NavMeshTileSettings& settings);
	NavMeshTileSettings getNavMeshTileSettings() const;
	void getTilePos(const LPoint3f& pos, int& tx, int& ty);
	//TILE
	void buildTile(const LPoint3f& pos);
	void removeTile(const LPoint3f& pos);
	void buildAllTiles();
	void removeAllTiles();
	//OBSTACLE
	dtTileCache* getTileCache();
	void addObstacle(SMARTPTR(Object) object);
	void removeObstacle(SMARTPTR(Object) object);
	void clearAllObstacles();
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
	 * \brief Adds an object owning a CrowdAgent component to the dtCrowd handling
	 * mechanism.
	 * @param crowdAgentObject The CrowdAgent object to add.
	 * @return True if CrowdAgent was added to recast update, false otherwise.
	 */
	bool addCrowdAgent(SMARTPTR(Object)crowdAgentObject);

	/**
	 * \brief Removes an object owning a CrowdAgent component from the dtCrowd handling
	 * mechanism.
	 * @param crowdAgentObject The CrowdAgent object to add.
	 */
	void removeCrowdAgent(SMARTPTR(Object)crowdAgentObject);

	/**
	 * \brief Recast crowd agents related methods.
	 */
	///@{
	void updateParams(int agentIdx,	const dtCrowdAgentParams& agentParams);
	void updateMoveTarget(int agentIdx,	const LPoint3f& pos);
	void updateMoveVelocity(int agentIdx, const LVector3f& vel);
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
	void navMeshSetup();

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
	void debug(bool enable);
#endif

private:
	///Input geometry.
	InputGeom* mGeom;
	///Build context.
	BuildContext* mCtx;
	///The mesh name.
	std::string mMeshName;
	///The reference node path (read only after creation).
	NodePath mReferenceNP;
	///@{
	///Current mesh type.
	NavMeshTypeEnum mNavMeshTypeEnum;
	NavMeshType* mNavMeshType;
	///@}
	///The movement type.
	AgentMovType mMovType;
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
	std::string mCrowdIncludeFlagsXmlParam, mCrowdExcludeFlagsXmlParam;
	int mCrowdIncludeFlags, mCrowdExcludeFlags;
	///Convex volumes.
	std::list<std::string> mConvexVolumeXmlParam;
	std::list<PointListArea> mConvexVolumes;
	///Off mesh connections.
	std::list<std::string> mOffMeshConnectionXmlParam;
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
	bool loadModelMesh(NodePath model);

	/**
	 * \brief Creates the navigation mesh type for the loaded model mesh.
	 * @param navMeshType The navigation mesh type.
	 * @param navMeshTypeEnum The navigation mesh enum type.
	 */
	void createNavMeshType(NavMeshType* navMeshType);

	/**
	 * \brief Builds the navigation mesh for the loaded model mesh.
	 * @return True if successful, false otherwise.
	 */
	bool buildNavMesh();

#ifdef ELY_DEBUG
	/// Recast debug node path.
	NodePath mDebugNodePath;
	/// Recast debug camera.
	NodePath mDebugCamera;
	/// DebugDrawers.
	DebugDrawPanda3d* mDD;
	DebugDrawMeshDrawer* mDDM;
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

inline void NavMesh::setNavMeshType(NavMeshTypeEnum typeEnum)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	mNavMeshTypeEnum = typeEnum;
}

inline NavMeshTypeEnum NavMesh::getNavMeshType() const
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mNavMeshTypeEnum;
}

inline void NavMesh::setMovType(AgentMovType movType)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	mMovType = movType;
}

inline AgentMovType NavMesh::getMovType() const
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mMovType;
}

inline void NavMesh::setNavMeshSettings(const NavMeshSettings& settings)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	mNavMeshSettings = settings;
}

inline NavMeshSettings NavMesh::getNavMeshSettings() const
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mNavMeshSettings;
}

inline void NavMesh::setNavMeshTileSettings(const NavMeshTileSettings& settings)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	mNavMeshTileSettings = settings;
}

inline NavMeshTileSettings NavMesh::getNavMeshTileSettings() const
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mNavMeshTileSettings;
}

inline std::list<SMARTPTR(CrowdAgent)> NavMesh::getCrowdAgents()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mCrowdAgents;
}

inline InputGeom* NavMesh::getRecastInputGeom() const
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mGeom;
}

inline dtNavMesh* NavMesh::getRecastNavMesh() const
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mNavMeshType->getNavMesh();
}

inline dtNavMeshQuery* NavMesh::getRecastNavMeshQuery() const
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mNavMeshType->getNavMeshQuery();
}

inline dtCrowd* NavMesh::getRecastCrowd() const
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mNavMeshType->getCrowd();
}

inline float NavMesh::getRecastAgentRadius() const
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mNavMeshType->getAgentRadius();
}

inline float NavMesh::getRecastAgentHeight() const
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mNavMeshType->getAgentHeight();
}

inline float NavMesh::getRecastAgentClimb() const
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mNavMeshType->getAgentClimb();
}

inline LVecBase3f NavMesh::getRecastBoundsMin() const
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	CHECKEXISTENCE(mGeom,
			"NavMesh::getBoundsMin: invalid InputGeom")
	return RecastToLVecBase3f(mGeom->getMeshBoundsMin());
}

inline LVecBase3f NavMesh::getRecastBoundsMax() const
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	CHECKEXISTENCE(mGeom,
			"NavMesh::getBoundsMax: invalid InputGeom")
	return RecastToLVecBase3f(mGeom->getMeshBoundsMax());
}

} // namespace ely

#endif /* NAVMESH_H_ */
