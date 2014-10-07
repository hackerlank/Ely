//
// Copyright (c) 2009-2010 Mikko Mononen memon@inside.org
//
// This software is provided 'as-is', without any express or implied
// warranty.  In no event will the authors be held liable for any damages
// arising from the use of this software.
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.
//

#ifndef RECASTSAMPLE_H
#define RECASTSAMPLE_H

#include "InputGeom.h"
#include "DebugInterfaces.h"
#include <DetourNavMeshQuery.h>
#ifndef WITHCHARACTER
#	include <DetourCrowd.h>
#else
#	include <DetourCrowdPhysics.h>
#endif

namespace ely
{

/// Tool types enum.
enum NavMeshTypeToolEnum
{
	TOOL_NONE = 0,
	TOOL_TILE_EDIT,
	TOOL_TILE_HIGHLIGHT,
	TOOL_TEMP_OBSTACLE,
	TOOL_NAVMESH_TESTER,
	TOOL_NAVMESH_PRUNE,
	TOOL_OFFMESH_CONNECTION,
	TOOL_CONVEX_VOLUME,
	TOOL_CROWD,
	MAX_TOOLS
};

/**
 * \brief NavMesh type enum.
 */
enum NavMeshTypeEnum
{
	SOLO, TILE, OBSTACLE, NavMeshType_NONE
};

/**
 * \brief NavMesh polygon area types.
 *
 * These are just area types to use consistent values across the nav mesh.
 * The use should specify these base on his needs.
 */
enum NavMeshPolyAreasEnum
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
enum NavMeshPolyFlagsEnum
{
	NAVMESH_POLYFLAGS_WALK = 0x01,		// Ability to walk (ground, grass, road)
	NAVMESH_POLYFLAGS_SWIM = 0x02,		// Ability to swim (water).
	NAVMESH_POLYFLAGS_DOOR = 0x04,		// Ability to move through doors.
	NAVMESH_POLYFLAGS_JUMP = 0x08,		// Ability to jump.
	NAVMESH_POLYFLAGS_DISABLED = 0x10,		// Disabled polygon
	NAVMESH_POLYFLAGS_ALL = 0xffff	// All abilities.
};
/**
 * \brief NavMesh partition type.
 */
enum NavMeshPartitionType
{
	NAVMESH_PARTITION_WATERSHED,
	NAVMESH_PARTITION_MONOTONE,
	NAVMESH_PARTITION_LAYERS,
};

///Table giving for each area the corresponding (or'ed) flags.
typedef std::map<int,int> NavMeshPolyAreaFlags;
///Table giving for each area the corresponding cost (for dtCrowd).
typedef std::map<int,float> NavMeshPolyAreaCost;

struct NavMeshTypeTool
{
	NavMeshTypeTool()
	{
	}
	virtual ~NavMeshTypeTool()
	{
	}
	virtual int type() = 0;
	virtual void init(class NavMeshType* sample) = 0;
	virtual void reset() = 0;
	virtual void handleClick(const float* s, const float* p, bool shift) = 0;
	virtual void handleRender(duDebugDraw& dd) = 0;
	virtual void handleStep() = 0;
	virtual void handleToggle() = 0;
	virtual void handleUpdate(const float dt) = 0;

};

struct NavMeshTypeToolState
{
	NavMeshTypeToolState()
	{
	}
	virtual ~NavMeshTypeToolState()
	{
	}
	virtual void init(class NavMeshType* sample) = 0;
	virtual void reset() = 0;
	virtual void handleRender(duDebugDraw& dd) = 0;
	virtual void handleUpdate(const float dt) = 0;

};

/**
 * \brief NavMesh settings.
 */
struct NavMeshSettings
{
	float m_cellSize;
	float m_cellHeight;
	float m_agentHeight;
	float m_agentRadius;
	float m_agentMaxClimb;
	float m_agentMaxSlope;
	float m_regionMinSize;
	float m_regionMergeSize;
	float m_edgeMaxLen;
	float m_edgeMaxError;
	float m_vertsPerPoly;
	float m_detailSampleDist;
	float m_detailSampleMaxError;
	int m_partitionType;
};

/**
 * \brief NavMesh tile settings.
 */
struct NavMeshTileSettings
{
	bool m_buildAllTiles;
	int m_maxTiles;
	int m_maxPolysPerTile;
	float m_tileSize;
};

class NavMeshType
{
protected:
	class InputGeom* m_geom;
	class dtNavMesh* m_navMesh;
	class dtNavMeshQuery* m_navQuery;
	class dtCrowd* m_crowd;

	unsigned char m_navMeshDrawFlags;

	float m_cellSize;
	float m_cellHeight;
	float m_agentHeight;
	float m_agentRadius;
	float m_agentMaxClimb;
	float m_agentMaxSlope;
	float m_regionMinSize;
	float m_regionMergeSize;
	float m_edgeMaxLen;
	float m_edgeMaxError;
	float m_vertsPerPoly;
	float m_detailSampleDist;
	float m_detailSampleMaxError;
	int m_partitionType;

	NavMeshTypeTool* m_tool;
	NavMeshTypeToolState* m_toolStates[MAX_TOOLS];

	BuildContext* m_ctx;

	NavMeshPolyAreaFlags m_flagsAreaTable;

public:
	NavMeshType();
	virtual ~NavMeshType();

	void setContext(BuildContext* ctx)
	{
		m_ctx = ctx;
	}

	void setTool(NavMeshTypeTool* tool);
	NavMeshTypeTool* getTool();
	NavMeshTypeToolState* getToolState(int type)
	{
		return m_toolStates[type];
	}
	void setToolState(int type, NavMeshTypeToolState* s)
	{
		m_toolStates[type] = s;
	}

	virtual void handleClick(const float* s, const float* p, bool shift);
	virtual void handleToggle();
	virtual void handleRender(duDebugDraw& dd);
	virtual void handleMeshChanged(class InputGeom* geom);
	virtual bool handleBuild();
	virtual void handleUpdate(const float dt);

	virtual class InputGeom* getInputGeom()
	{
		return m_geom;
	}
	virtual class dtNavMesh* getNavMesh()
	{
		return m_navMesh;
	}
	virtual class dtNavMeshQuery* getNavMeshQuery()
	{
		return m_navQuery;
	}
	virtual class dtCrowd* getCrowd()
	{
		return m_crowd;
	}
	virtual float getAgentRadius()
	{
		return m_agentRadius;
	}
	virtual float getAgentHeight()
	{
		return m_agentHeight;
	}
	virtual float getAgentClimb()
	{
		return m_agentMaxClimb;
	}
	void setNavMeshSettings(const NavMeshSettings& settings);
	NavMeshSettings getNavMeshSettings();
	void resetNavMeshSettings();

	virtual const float* getBoundsMin();
	virtual const float* getBoundsMax();

	inline unsigned char getNavMeshDrawFlags() const
	{
		return m_navMeshDrawFlags;
	}
	inline void setNavMeshDrawFlags(unsigned char flags)
	{
		m_navMeshDrawFlags = flags;
	}

	void updateToolStates(const float dt);
	void initToolStates(NavMeshType* sample);
	void resetToolStates();
	void renderToolStates(duDebugDraw& dd);

	void setFlagsAreaTable(const NavMeshPolyAreaFlags& flagsAreaTable)
	{
		m_flagsAreaTable = flagsAreaTable;
	}

};

} // namespace ely

#endif // RECASTSAMPLE_H