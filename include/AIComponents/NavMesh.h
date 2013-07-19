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
#include <DetourCrowd.h>
#include <DetourTileCache.h>
#include "ObjectModel/Component.h"
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
 * (i.e. is_steady=true) Model components.
 *
 * XML Param(s):
 * - "navmesh_type"					|single|"solo" (solo|tile|obstacle)
 * - "auto_build"					|single|"true"
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
 */
class NavMesh: public Component
{
public:
	NavMesh();
	NavMesh(SMARTPTR(NavMeshTemplate)tmpl);
	virtual ~NavMesh();

	const virtual ComponentFamilyType familyType() const;
	const virtual ComponentType componentType() const;

	virtual bool initialize();
	virtual void onAddToObjectSetup();
	virtual void onAddToSceneSetup();

	/**
	 * \brief Navigation mesh related methods.
	 */
	///@{
	//SOLO TILE OBSTACLE
	NavMeshType* getNavMeshType();
	NavMeshTypeEnum getNavMeshTypeEnum();
	InputGeom* getInputGeom();
	dtNavMesh* getNavMesh();
	dtNavMeshQuery* getNavMeshQuery();
	dtCrowd* getCrowd();
	float getAgentRadius();
	float getAgentHeight();
	float getAgentClimb();
	LVecBase3f getBoundsMin();
	LVecBase3f getBoundsMax();
	NavMeshSettings getNavMeshSettings();
	void setNavMeshSettings(const NavMeshSettings& settings);
	//TILE OBSTACLE
	NavMeshTileSettings getNavMeshTileSettings();
	void setNavMeshTileSettings(const NavMeshTileSettings& settings);
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
	 * \brief Loads the mesh from a model node path.
	 *
	 * \note: the model's parent node path is the reference wrt
	 * any calculation are performed.
	 * @param model The model's node path.
	 * @return True if successful, false otherwise.
	 */
	bool loadModelMesh(NodePath model);

	/**
	 * \brief Sets up the type of navigation mesh for the loaded model mesh.
	 * @param navMeshType The type of navigation mesh.
	 * @param navMeshTypeEnum The type of navigation mesh enum.
	 */
	void setupNavMesh(NavMeshType* navMeshType, NavMeshTypeEnum navMeshTypeEnum=SOLO);

	/**
	 * \brief Builds the navigation mesh for the loaded model mesh.
	 * @return True if successful, false otherwise.
	 */
	bool buildNavMesh();

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
	/// Input geometry.
	InputGeom* mGeom;
	/// Build context.
	BuildContext* mCtx;
	/// The mesh name.
	std::string mMeshName;
	/// The reference node path;
	NodePath mReferenceNP;
	///@{
	/// Current mesh type.
	NavMeshTypeEnum mNavMeshTypeEnum;
	NavMeshType* mNavMeshType;
	///@}
	/// NavMeshSettings from template.
	NavMeshSettings mNavMeshSettings;
	/// NavMeshTileSettings from template.
	NavMeshTileSettings mNavMeshTileSettings;
	///@{
	/// Auto build: true (default) if navigation mesh
	/// is to be built when owner object is added to scene,
	/// false if it will be built manually by program.
	/// \note Manual build is necessary when the owner object has
	/// children objects and an overall navigation mesh should
	/// consider them too: parents objects are (created and)
	/// added to scene before their children, so an overall
	/// navigation mesh can be built only after hierarchies
	/// between objects have been already established, i.e.
	/// after world creation; typically this kind of navigation
	/// mesh is built (manually) during object initialization.
	bool mAutoBuild;
	/// Obstacles table
	std::map<SMARTPTR(Object), dtObstacleRef> mObstacles;
	///@}
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

} // namespace ely

#endif /* NAVMESH_H_ */
