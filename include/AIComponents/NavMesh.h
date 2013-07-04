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

#include "RecastNavigation/InputGeom.h"
#include "RecastNavigation/DebugInterfaces.h"
#include "ObjectModel/Component.h"
#include <windowFramework.h>
#include <nodePath.h>

namespace ely
{

/**
 * \brief NavMesh polygon area types.
 *
 * These are just area types to use consistent values across the nav mesh.
 * The use should specify these base on his needs.
 */
enum NavMeshPolyAreas
{
	NAVMESH_POLYAREA_GROUND,
	NAVMESH_POLYAREA_WATER,
	NAVMESH_POLYAREA_ROAD,
	NAVMESH_POLYAREA_DOOR,
	NAVMESH_POLYAREA_GRASS,
	NAVMESH_POLYAREA_JUMP,
	//the last is a sentinel
	NAVMESH_POLYAREA_END
};

/**
 * \brief NavMesh polygon flags.
 */
enum NavMeshPolyFlags
{
	NAVMESH_POLYFLAGS_WALK = 0x01,		// Ability to walk (ground, grass, road)
	NAVMESH_POLYFLAGS_SWIM = 0x02,		// Ability to swim (water).
	NAVMESH_POLYFLAGS_DOOR = 0x04,		// Ability to move through doors.
	NAVMESH_POLYFLAGS_JUMP = 0x08,		// Ability to jump.
	NAVMESH_POLYFLAGS_DISABLED = 0x10,		// Disabled polygon
	NAVMESH_POLYFLAGS_ALL = 0xffff	// All abilities.
};

/**
 * \brief NavMesh polygon area flags.
 */
struct NavMeshSettings
{
	float mCellSize;
	float mCellHeight;
	float mAgentHeight;
	float mAgentRadius;
	float mAgentMaxClimb;
	float mAgentMaxSlope;
	float mRegionMinSize;
	float mRegionMergeSize;
	bool mMonotonePartitioning;
	float mEdgeMaxLen;
	float mEdgeMaxError;
	float mVertsPerPoly;
	float mDetailSampleDist;
	float mDetailSampleMaxError;
};

class NavMeshTemplate;

/**
 * \brief Component implementing dtNavMesh from Recast Navigation library.
 *
 * \see https://code.google.com/p/recastnavigation
 * 		http://digestingduck.blogspot.it
 * 		https://groups.google.com/forum/?fromgroups#!forum/recastnavigation
 *
 * This is a ...
 *
 * XML Param(s):
 * - "param1"  						|single|"true"
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
	 * \brief Updates position/orientation of the controlled object.
	 *
	 * Will be called automatically by an control manager update.
	 * @param data The custom data.
	 */
	virtual void update(void* data);

	/**
	 * \brief Getters/setters.
	 */
	///@{
	InputGeom* getInputGeom();
	dtNavMesh* getNavMesh();
	dtNavMeshQuery* getNavMeshQuery();
	dtCrowd* getCrowd();
	float getAgentRadius();
	float getAgentHeight();
	float getAgentClimb();
	void setNavMeshSettings(const NavMeshSettings& settings);
	NavMeshSettings getNavMeshSettings();
	float* getBoundsMin();
	float* getBoundsMax();
	///@}

#ifdef ELY_DEBUG
	/**
	 * \brief Gets a reference to the Recast Debug node.
	 * @return The Recast Debug node.
	 */
	NodePath getDebugNodePath() const;

	/**
	 * \brief Initializes debugging.
	 */
	void initDebug(WindowFramework* windowFramework);
	/**
	 * \brief Enables/disables debugging.
	 * @param enable True to enable, false to disable.
	 */
	void debug(bool enable);
#endif

private:
	/// Input geometry.
	InputGeom* mGeom;
	/// Detour navigation mesh.
	dtNavMesh* mNavMesh;
	/// Detour navigation mesh query.
	dtNavMeshQuery* mNavQuery;
	/// Crowd core class.
	dtCrowd* mCrowd;
	/// Build context.
	BuildContext* mCtx;
#ifdef ELY_DEBUG
	/// Recast debug node path.
	NodePath mRecastDebugNodePath;
	/// Panda3d debug draw implementation.
	DebugDrawPanda3d mDebugDraw;
#endif
	/**
	 * \brief Nav mesh settings data.
	 */
	///@{
	float mCellSize;
	float mCellHeight;
	float mAgentHeight;
	float mAgentRadius;
	float mAgentMaxClimb;
	float mAgentMaxSlope;
	float mRegionMinSize;
	float mRegionMergeSize;
	bool mMonotonePartitioning;
	float mEdgeMaxLen;
	float mEdgeMaxError;
	float mVertsPerPoly;
	float mDetailSampleDist;
	float mDetailSampleMaxError;
	///@}

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
