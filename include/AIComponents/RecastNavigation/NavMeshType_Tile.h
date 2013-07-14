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

#ifndef RECASTSAMPLETILEMESH_H
#define RECASTSAMPLETILEMESH_H

#include "AIComponents/RecastNavigation/NavMeshType.h"
#include <DetourNavMesh.h>
#include <Recast.h>

namespace ely
{

class NavMeshType_Tile: public NavMeshType
{
protected:
	bool m_keepInterResults;
	bool m_buildAll;
	float m_totalBuildTimeMs;

	unsigned char* m_triareas;
	rcHeightfield* m_solid;
	rcCompactHeightfield* m_chf;
	rcContourSet* m_cset;
	rcPolyMesh* m_pmesh;
	rcPolyMeshDetail* m_dmesh;
	rcConfig m_cfg;

	enum DrawMode
	{
		DRAWMODE_NAVMESH,
		DRAWMODE_NAVMESH_TRANS,
		DRAWMODE_NAVMESH_BVTREE,
		DRAWMODE_NAVMESH_NODES,
		DRAWMODE_NAVMESH_PORTALS,
		DRAWMODE_NAVMESH_INVIS,
		DRAWMODE_MESH,
		DRAWMODE_VOXELS,
		DRAWMODE_VOXELS_WALKABLE,
		DRAWMODE_COMPACT,
		DRAWMODE_COMPACT_DISTANCE,
		DRAWMODE_COMPACT_REGIONS,
		DRAWMODE_REGION_CONNECTIONS,
		DRAWMODE_RAW_CONTOURS,
		DRAWMODE_BOTH_CONTOURS,
		DRAWMODE_CONTOURS,
		DRAWMODE_POLYMESH,
		DRAWMODE_POLYMESH_DETAIL,
		MAX_DRAWMODE
	};

	DrawMode m_drawMode;

	int m_maxTiles;
	int m_maxPolysPerTile;
	float m_tileSize;

	unsigned int m_tileCol;
	float m_tileBmin[3];
	float m_tileBmax[3];
	float m_tileBuildTime;
	float m_tileMemUsage;
	int m_tileTriCount;

	unsigned char* buildTileMesh(const int tx, const int ty, const float* bmin,
			const float* bmax, int& dataSize);

	void cleanup();

	void saveAll(const char* path, const dtNavMesh* mesh);
	dtNavMesh* loadAll(const char* path);

public:
	NavMeshType_Tile(NodePath renderDebug = NodePath());
	virtual ~NavMeshType_Tile();

	virtual void handleRender();
	virtual void handleMeshChanged(class InputGeom* geom);
	virtual bool handleBuild();

	void setTileSettings(const NavMeshTileSettings& settings);
	NavMeshTileSettings getTileSettings();

	void getTilePos(const float* pos, int& tx, int& ty);

	void buildTile(const float* pos);
	void removeTile(const float* pos);
	void buildAllTiles();
	void removeAllTiles();
};

class NavMeshTileTool: public NavMeshTypeTool
{
	NavMeshType_Tile* m_sample;
	float m_hitPos[3];
	bool m_hitPosSet;
	float m_agentRadius;

public:

	NavMeshTileTool() :
			m_sample(0), m_hitPosSet(false), m_agentRadius(0)
	{
		m_hitPos[0] = m_hitPos[1] = m_hitPos[2] = 0;
	}

	virtual ~NavMeshTileTool()
	{
	}

	virtual void init(NavMeshType* sample)
	{
		m_sample = (NavMeshType_Tile*) sample;
	}

	virtual void reset()
	{
	}

	virtual void handleMenu()
	{
//		imguiLabel("Create Tiles");
//		if (imguiButton("Create All"))
//		{
//			if (m_sample)
//				m_sample->buildAllTiles();
//		}
//		if (imguiButton("Remove All"))
//		{
//			if (m_sample)
//				m_sample->removeAllTiles();
//		}
	}

	virtual void handleClick(const float* /*s*/, const float* p, bool shift)
	{
		m_hitPosSet = true;
		rcVcopy(m_hitPos, p);
		if (m_sample)
		{
			if (shift)
				m_sample->removeTile(m_hitPos);
			else
				m_sample->buildTile(m_hitPos);
		}
	}

	virtual void handleToggle()
	{
	}

	virtual void handleUpdate(const float /*dt*/)
	{
	}

	virtual void handleRender()
	{
//		if (m_hitPosSet)
//		{
//			const float s = m_sample->getAgentRadius();
//			glColor4ub(0,0,0,128);
//			glLineWidth(2.0f);
//			glBegin(GL_LINES);
//			glVertex3f(m_hitPos[0]-s,m_hitPos[1]+0.1f,m_hitPos[2]);
//			glVertex3f(m_hitPos[0]+s,m_hitPos[1]+0.1f,m_hitPos[2]);
//			glVertex3f(m_hitPos[0],m_hitPos[1]-s+0.1f,m_hitPos[2]);
//			glVertex3f(m_hitPos[0],m_hitPos[1]+s+0.1f,m_hitPos[2]);
//			glVertex3f(m_hitPos[0],m_hitPos[1]+0.1f,m_hitPos[2]-s);
//			glVertex3f(m_hitPos[0],m_hitPos[1]+0.1f,m_hitPos[2]+s);
//			glEnd();
//			glLineWidth(1.0f);
//		}
	}

	virtual void handleRenderOverlay(double* proj, double* model, int* view)
	{
//		GLdouble x, y, z;
//		if (m_hitPosSet && gluProject((GLdouble)m_hitPos[0], (GLdouble)m_hitPos[1], (GLdouble)m_hitPos[2],
//									  model, proj, view, &x, &y, &z))
//		{
//			int tx=0, ty=0;
//			m_sample->getTilePos(m_hitPos, tx, ty);
//			char text[32];
//			snprintf(text,32,"(%d,%d)", tx,ty);
//			imguiDrawText((int)x, (int)y-25, IMGUI_ALIGN_CENTER, text, imguiRGBA(0,0,0,220));
//		}
//
//		// Tool help
//		const int h = view[3];
//		imguiDrawText(280, h-40, IMGUI_ALIGN_LEFT, "LMB: Rebuild hit tile.  Shift+LMB: Clear hit tile.", imguiRGBA(255,255,255,192));
	}
};

} // namespace ely

#endif // RECASTSAMPLETILEMESH_H
