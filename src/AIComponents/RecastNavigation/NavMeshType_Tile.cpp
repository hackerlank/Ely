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

#define _USE_MATH_DEFINES
#include <math.h>
#include <stdio.h>
#include <string.h>
#include "AIComponents/RecastNavigation/NavMeshType_Tile.h"
#include <RecastDump.h>
#include <DetourNavMeshBuilder.h>
#include <DetourDebugDraw.h>

#ifdef WIN32
#	define snprintf _snprintf
#endif

namespace ely
{

NavMeshType_Tile::NavMeshType_Tile() :
		m_keepInterResults(false),
		m_buildAll(true),
		m_totalBuildTimeMs(0),
		m_triareas(0),
		m_solid(0),
		m_chf(0),
		m_cset(0),
		m_pmesh(0),
		m_dmesh(0),
		m_drawMode(DRAWMODE_NAVMESH),
		m_maxTiles(0),
		m_maxPolysPerTile(0),
		m_tileSize(32),
		m_tileCol(duRGBA(0, 0, 0, 32)),
		m_tileBuildTime(0),
		m_tileMemUsage(0),
		m_tileTriCount(0)
{
	resetNavMeshSettings();
	memset(m_tileBmin, 0, sizeof(m_tileBmin));
	memset(m_tileBmax, 0, sizeof(m_tileBmax));

//	setTool(new NavMeshTileTool);
}

NavMeshType_Tile::~NavMeshType_Tile()
{
	cleanup();
	dtFreeNavMesh(m_navMesh);
	m_navMesh = 0;
}

void NavMeshType_Tile::cleanup()
{
	delete[] m_triareas;
	m_triareas = 0;
	rcFreeHeightField(m_solid);
	m_solid = 0;
	rcFreeCompactHeightfield(m_chf);
	m_chf = 0;
	rcFreeContourSet(m_cset);
	m_cset = 0;
	rcFreePolyMesh(m_pmesh);
	m_pmesh = 0;
	rcFreePolyMeshDetail(m_dmesh);
	m_dmesh = 0;
}

} // ely

namespace
{
const int NAVMESHSET_MAGIC = 'M' << 24 | 'S' << 16 | 'E' << 8 | 'T'; //'MSET';
const int NAVMESHSET_VERSION = 1;
}

namespace ely
{

struct NavMeshSetHeader
{
	int magic;
	int version;
	int numTiles;
	dtNavMeshParams params;
};

struct NavMeshTileHeader
{
	dtTileRef tileRef;
	int dataSize;
};

void NavMeshType_Tile::saveAll(const char* path, const dtNavMesh* mesh)
{
	if (!mesh)
		return;

	FILE* fp = fopen(path, "wb");
	if (!fp)
		return;

	// Store header.
	NavMeshSetHeader header;
	header.magic = NAVMESHSET_MAGIC;
	header.version = NAVMESHSET_VERSION;
	header.numTiles = 0;
	for (int i = 0; i < mesh->getMaxTiles(); ++i)
	{
		const dtMeshTile* tile = mesh->getTile(i);
		if (!tile || !tile->header || !tile->dataSize)
			continue;
		header.numTiles++;
	}
	memcpy(&header.params, mesh->getParams(), sizeof(dtNavMeshParams));
	fwrite(&header, sizeof(NavMeshSetHeader), 1, fp);

	// Store tiles.
	for (int i = 0; i < mesh->getMaxTiles(); ++i)
	{
		const dtMeshTile* tile = mesh->getTile(i);
		if (!tile || !tile->header || !tile->dataSize)
			continue;

		NavMeshTileHeader tileHeader;
		tileHeader.tileRef = mesh->getTileRef(tile);
		tileHeader.dataSize = tile->dataSize;
		fwrite(&tileHeader, sizeof(tileHeader), 1, fp);

		fwrite(tile->data, tile->dataSize, 1, fp);
	}

	fclose(fp);
}

dtNavMesh* NavMeshType_Tile::loadAll(const char* path)
{
	FILE* fp = fopen(path, "rb");
	if (!fp)
		return 0;

	// Read header.
	NavMeshSetHeader header;
	fread(&header, sizeof(NavMeshSetHeader), 1, fp);
	if (header.magic != NAVMESHSET_MAGIC)
	{
		fclose(fp);
		return 0;
	}
	if (header.version != NAVMESHSET_VERSION)
	{
		fclose(fp);
		return 0;
	}

	dtNavMesh* mesh = dtAllocNavMesh();
	if (!mesh)
	{
		fclose(fp);
		return 0;
	}
	dtStatus status = mesh->init(&header.params);
	if (dtStatusFailed(status))
	{
		fclose(fp);
		return 0;
	}

	// Read tiles.
	for (int i = 0; i < header.numTiles; ++i)
	{
		NavMeshTileHeader tileHeader;
		fread(&tileHeader, sizeof(tileHeader), 1, fp);
		if (!tileHeader.tileRef || !tileHeader.dataSize)
			break;

		unsigned char* data = (unsigned char*) dtAlloc(tileHeader.dataSize,
				DT_ALLOC_PERM);
		if (!data)
			break;
		memset(data, 0, tileHeader.dataSize);
		fread(data, tileHeader.dataSize, 1, fp);

		mesh->addTile(data, tileHeader.dataSize, DT_TILE_FREE_DATA,
				tileHeader.tileRef, 0);
	}

	fclose(fp);

	return mesh;
}

void NavMeshType_Tile::handleRender(duDebugDraw& dd)
{
	if (!m_geom || !m_geom->getMesh())
		return;

//	DebugDrawGL dd;

//	const float texScale = 1.0f / (m_cellSize * 10.0f);

	// Draw mesh
//	if (m_drawMode != DRAWMODE_NAVMESH_TRANS)
//	{
//		// Draw mesh
//		duDebugDrawTriMeshSlope(&dd, m_geom->getMesh()->getVerts(), m_geom->getMesh()->getVertCount(),
//								m_geom->getMesh()->getTris(), m_geom->getMesh()->getNormals(), m_geom->getMesh()->getTriCount(),
//								m_agentMaxSlope, texScale);
//		m_geom->drawOffMeshConnections(&dd);
//	}

//	glDepthMask(GL_FALSE);
	dd.depthMask(false);

	// Draw bounds
	const float* bmin = m_geom->getMeshBoundsMin();
	const float* bmax = m_geom->getMeshBoundsMax();
	duDebugDrawBoxWire(&dd, bmin[0],bmin[1],bmin[2], bmax[0],bmax[1],bmax[2], duRGBA(255,255,255,128), 1.0f);

	// Tiling grid.
	int gw = 0, gh = 0;
	rcCalcGridSize(bmin, bmax, m_cellSize, &gw, &gh);
	const int tw = (gw + (int)m_tileSize-1) / (int)m_tileSize;
	const int th = (gh + (int)m_tileSize-1) / (int)m_tileSize;
	const float s = m_tileSize*m_cellSize;
	duDebugDrawGridXZ(&dd, bmin[0],bmin[1],bmin[2], tw,th, s, duRGBA(0,0,0,64), 1.0f);

	// Draw active tile
	duDebugDrawBoxWire(&dd, m_tileBmin[0],m_tileBmin[1],m_tileBmin[2],
					   m_tileBmax[0],m_tileBmax[1],m_tileBmax[2], m_tileCol, 1.0f);

//	if (m_navMesh && m_navQuery &&
//		(m_drawMode == DRAWMODE_NAVMESH ||
//		 m_drawMode == DRAWMODE_NAVMESH_TRANS ||
//		 m_drawMode == DRAWMODE_NAVMESH_BVTREE ||
//		 m_drawMode == DRAWMODE_NAVMESH_NODES ||
//		 m_drawMode == DRAWMODE_NAVMESH_PORTALS ||
//		 m_drawMode == DRAWMODE_NAVMESH_INVIS))
//	{
//		if (m_drawMode != DRAWMODE_NAVMESH_INVIS)
			duDebugDrawNavMeshWithClosedList(&dd, *m_navMesh, *m_navQuery, m_navMeshDrawFlags);
//		if (m_drawMode == DRAWMODE_NAVMESH_BVTREE)
//			duDebugDrawNavMeshBVTree(&dd, *m_navMesh);
//		if (m_drawMode == DRAWMODE_NAVMESH_PORTALS)
//			duDebugDrawNavMeshPortals(&dd, *m_navMesh);
//		if (m_drawMode == DRAWMODE_NAVMESH_NODES)
//			duDebugDrawNavMeshNodes(&dd, *m_navQuery);
		duDebugDrawNavMeshPolysWithFlags(&dd, *m_navMesh, NAVMESH_POLYFLAGS_DISABLED, duRGBA(0,0,0,128));
//	}

//	glDepthMask(GL_TRUE);
	dd.depthMask(true);

//	if (m_chf && m_drawMode == DRAWMODE_COMPACT)
//		duDebugDrawCompactHeightfieldSolid(&dd, *m_chf);
//
//	if (m_chf && m_drawMode == DRAWMODE_COMPACT_DISTANCE)
//		duDebugDrawCompactHeightfieldDistance(&dd, *m_chf);
//	if (m_chf && m_drawMode == DRAWMODE_COMPACT_REGIONS)
//		duDebugDrawCompactHeightfieldRegions(&dd, *m_chf);
//	if (m_solid && m_drawMode == DRAWMODE_VOXELS)
//	{
//		glEnable(GL_FOG);
//		duDebugDrawHeightfieldSolid(&dd, *m_solid);
//		glDisable(GL_FOG);
//	}
//	if (m_solid && m_drawMode == DRAWMODE_VOXELS_WALKABLE)
//	{
//		glEnable(GL_FOG);
//		duDebugDrawHeightfieldWalkable(&dd, *m_solid);
//		glDisable(GL_FOG);
//	}
//
//	if (m_cset && m_drawMode == DRAWMODE_RAW_CONTOURS)
//	{
//		glDepthMask(GL_FALSE);
//		duDebugDrawRawContours(&dd, *m_cset);
//		glDepthMask(GL_TRUE);
//	}
//
//	if (m_cset && m_drawMode == DRAWMODE_BOTH_CONTOURS)
//	{
//		glDepthMask(GL_FALSE);
//		duDebugDrawRawContours(&dd, *m_cset, 0.5f);
//		duDebugDrawContours(&dd, *m_cset);
//		glDepthMask(GL_TRUE);
//	}
//	if (m_cset && m_drawMode == DRAWMODE_CONTOURS)
//	{
//		glDepthMask(GL_FALSE);
//		duDebugDrawContours(&dd, *m_cset);
//		glDepthMask(GL_TRUE);
//	}
//	if (m_chf && m_cset && m_drawMode == DRAWMODE_REGION_CONNECTIONS)
//	{
//		duDebugDrawCompactHeightfieldRegions(&dd, *m_chf);
//
//		glDepthMask(GL_FALSE);
//		duDebugDrawRegionConnections(&dd, *m_cset);
//		glDepthMask(GL_TRUE);
//	}
//	if (m_pmesh && m_drawMode == DRAWMODE_POLYMESH)
//	{
//		glDepthMask(GL_FALSE);
//		duDebugDrawPolyMesh(&dd, *m_pmesh);
//		glDepthMask(GL_TRUE);
//	}
//	if (m_dmesh && m_drawMode == DRAWMODE_POLYMESH_DETAIL)
//	{
//		glDepthMask(GL_FALSE);
//		duDebugDrawPolyMeshDetail(&dd, *m_dmesh);
//		glDepthMask(GL_TRUE);
//	}
//
//	m_geom->drawConvexVolumes(&dd);
//
//	if (m_tool)
//		m_tool->handleRender();
//	renderToolStates();
//
//	glDepthMask(GL_TRUE);
//	dd.depthMask(true);
}

void NavMeshType_Tile::handleMeshChanged(class InputGeom* geom)
{
	NavMeshType::handleMeshChanged(geom);

	cleanup();

	dtFreeNavMesh(m_navMesh);
	m_navMesh = 0;

	if (m_tool)
	{
		m_tool->reset();
		m_tool->init(this);
	}
}

bool NavMeshType_Tile::handleBuild()
{
	if (!m_geom || !m_geom->getMesh())
	{
		CTXLOG(m_ctx, RC_LOG_ERROR,
				"buildTiledNavigation: No vertices and triangles.");
		return false;
	}

	dtFreeNavMesh(m_navMesh);

	m_navMesh = dtAllocNavMesh();
	if (!m_navMesh)
	{
		CTXLOG(m_ctx, RC_LOG_ERROR,
				"buildTiledNavigation: Could not allocate navmesh.");
		return false;
	}

	dtNavMeshParams params;
	rcVcopy(params.orig, m_geom->getMeshBoundsMin());
	params.tileWidth = m_tileSize * m_cellSize;
	params.tileHeight = m_tileSize * m_cellSize;
	params.maxTiles = m_maxTiles;
	params.maxPolys = m_maxPolysPerTile;

	dtStatus status;

	status = m_navMesh->init(&params);
	if (dtStatusFailed(status))
	{
		CTXLOG(m_ctx, RC_LOG_ERROR,
				"buildTiledNavigation: Could not init navmesh.");
		return false;
	}

	status = m_navQuery->init(m_navMesh, 2048);
	if (dtStatusFailed(status))
	{
		CTXLOG(m_ctx, RC_LOG_ERROR,
				"buildTiledNavigation: Could not init Detour navmesh query");
		return false;
	}

	if (m_buildAll)
		buildAllTiles();

	if (m_tool)
		m_tool->init(this);

	return true;
}

void NavMeshType_Tile::buildTile(const float* pos)
{
	if (!m_geom)
		return;
	if (!m_navMesh)
		return;

	const float* bmin = m_geom->getMeshBoundsMin();
	const float* bmax = m_geom->getMeshBoundsMax();

	const float ts = m_tileSize * m_cellSize;
	const int tx = (int) ((pos[0] - bmin[0]) / ts);
	const int ty = (int) ((pos[2] - bmin[2]) / ts);

	m_tileBmin[0] = bmin[0] + tx * ts;
	m_tileBmin[1] = bmin[1];
	m_tileBmin[2] = bmin[2] + ty * ts;

	m_tileBmax[0] = bmin[0] + (tx + 1) * ts;
	m_tileBmax[1] = bmax[1];
	m_tileBmax[2] = bmin[2] + (ty + 1) * ts;

	m_tileCol = duRGBA(255, 255, 255, 64);

#ifdef ELY_DEBUG
	m_ctx->resetLog();
#endif

	int dataSize = 0;
	unsigned char* data = buildTileMesh(tx, ty, m_tileBmin, m_tileBmax,
			dataSize);

	// Remove any previous data (navmesh owns and deletes the data).
	m_navMesh->removeTile(m_navMesh->getTileRefAt(tx, ty, 0), 0, 0);

	// Add tile, or leave the location empty.
	if (data)
	{
		// Let the navmesh own the data.
		dtStatus status = m_navMesh->addTile(data, dataSize, DT_TILE_FREE_DATA,
				0, 0);
		if (dtStatusFailed(status))
			dtFree(data);
	}

#ifdef ELY_DEBUG
	m_ctx->dumpLog("Build Tile (%d,%d):", tx, ty);
#endif
}

void NavMeshType_Tile::getTilePos(const float* pos, int& tx, int& ty)
{
	if (!m_geom)
		return;

	const float* bmin = m_geom->getMeshBoundsMin();

	const float ts = m_tileSize * m_cellSize;
	tx = (int) ((pos[0] - bmin[0]) / ts);
	ty = (int) ((pos[2] - bmin[2]) / ts);
}

void NavMeshType_Tile::removeTile(const float* pos)
{
	if (!m_geom)
		return;
	if (!m_navMesh)
		return;

	const float* bmin = m_geom->getMeshBoundsMin();
	const float* bmax = m_geom->getMeshBoundsMax();

	const float ts = m_tileSize * m_cellSize;
	const int tx = (int) ((pos[0] - bmin[0]) / ts);
	const int ty = (int) ((pos[2] - bmin[2]) / ts);

	m_tileBmin[0] = bmin[0] + tx * ts;
	m_tileBmin[1] = bmin[1];
	m_tileBmin[2] = bmin[2] + ty * ts;

	m_tileBmax[0] = bmin[0] + (tx + 1) * ts;
	m_tileBmax[1] = bmax[1];
	m_tileBmax[2] = bmin[2] + (ty + 1) * ts;

	m_tileCol = duRGBA(128, 32, 16, 64);

	m_navMesh->removeTile(m_navMesh->getTileRefAt(tx, ty, 0), 0, 0);
}

void NavMeshType_Tile::buildAllTiles()
{
	if (!m_geom)
		return;
	if (!m_navMesh)
		return;

	const float* bmin = m_geom->getMeshBoundsMin();
	const float* bmax = m_geom->getMeshBoundsMax();
	int gw = 0, gh = 0;
	rcCalcGridSize(bmin, bmax, m_cellSize, &gw, &gh);
	const int ts = (int) m_tileSize;
	const int tw = (gw + ts - 1) / ts;
	const int th = (gh + ts - 1) / ts;
	const float tcs = m_tileSize * m_cellSize;

#ifdef ELY_DEBUG
	// Start the build process.
	m_ctx->startTimer(RC_TIMER_TEMP);
#endif

	for (int y = 0; y < th; ++y)
	{
		for (int x = 0; x < tw; ++x)
		{
			m_tileBmin[0] = bmin[0] + x * tcs;
			m_tileBmin[1] = bmin[1];
			m_tileBmin[2] = bmin[2] + y * tcs;

			m_tileBmax[0] = bmin[0] + (x + 1) * tcs;
			m_tileBmax[1] = bmax[1];
			m_tileBmax[2] = bmin[2] + (y + 1) * tcs;

			int dataSize = 0;
			unsigned char* data = buildTileMesh(x, y, m_tileBmin, m_tileBmax,
					dataSize);
			if (data)
			{
				// Remove any previous data (navmesh owns and deletes the data).
				m_navMesh->removeTile(m_navMesh->getTileRefAt(x, y, 0), 0, 0);
				// Let the navmesh own the data.
				dtStatus status = m_navMesh->addTile(data, dataSize,
						DT_TILE_FREE_DATA, 0, 0);
				if (dtStatusFailed(status))
					dtFree(data);
			}
		}
	}

#ifdef ELY_DEBUG
	// Start the build process.	
	m_ctx->stopTimer(RC_TIMER_TEMP);
	m_totalBuildTimeMs = m_ctx->getAccumulatedTime(RC_TIMER_TEMP) / 1000.0f;
#endif

}

void NavMeshType_Tile::removeAllTiles()
{
	const float* bmin = m_geom->getMeshBoundsMin();
	const float* bmax = m_geom->getMeshBoundsMax();
	int gw = 0, gh = 0;
	rcCalcGridSize(bmin, bmax, m_cellSize, &gw, &gh);
	const int ts = (int) m_tileSize;
	const int tw = (gw + ts - 1) / ts;
	const int th = (gh + ts - 1) / ts;

	for (int y = 0; y < th; ++y)
		for (int x = 0; x < tw; ++x)
			m_navMesh->removeTile(m_navMesh->getTileRefAt(x, y, 0), 0, 0);
}

unsigned char* NavMeshType_Tile::buildTileMesh(const int tx, const int ty,
		const float* bmin, const float* bmax, int& dataSize)
{
	if (!m_geom || !m_geom->getMesh() || !m_geom->getChunkyMesh())
	{
		CTXLOG(m_ctx, RC_LOG_ERROR,
				"buildNavigation: Input mesh is not specified.");
		return 0;
	}

	m_tileMemUsage = 0;
	m_tileBuildTime = 0;

	cleanup();

	const float* verts = m_geom->getMesh()->getVerts();
	const int nverts = m_geom->getMesh()->getVertCount();
	const int ntris = m_geom->getMesh()->getTriCount();
	const rcChunkyTriMesh* chunkyMesh = m_geom->getChunkyMesh();

	// Init build configuration from GUI
	memset(&m_cfg, 0, sizeof(m_cfg));
	m_cfg.cs = m_cellSize;
	m_cfg.ch = m_cellHeight;
	m_cfg.walkableSlopeAngle = m_agentMaxSlope;
	m_cfg.walkableHeight = (int) ceilf(m_agentHeight / m_cfg.ch);
	m_cfg.walkableClimb = (int) floorf(m_agentMaxClimb / m_cfg.ch);
	m_cfg.walkableRadius = (int) ceilf(m_agentRadius / m_cfg.cs);
	m_cfg.maxEdgeLen = (int) (m_edgeMaxLen / m_cellSize);
	m_cfg.maxSimplificationError = m_edgeMaxError;
	m_cfg.minRegionArea = (int) rcSqr(m_regionMinSize);	// Note: area = size*size
	m_cfg.mergeRegionArea = (int) rcSqr(m_regionMergeSize);	// Note: area = size*size
	m_cfg.maxVertsPerPoly = (int) m_vertsPerPoly;
	m_cfg.tileSize = (int) m_tileSize;
	m_cfg.borderSize = m_cfg.walkableRadius + 3; // Reserve enough padding.
	m_cfg.width = m_cfg.tileSize + m_cfg.borderSize * 2;
	m_cfg.height = m_cfg.tileSize + m_cfg.borderSize * 2;
	m_cfg.detailSampleDist =
			m_detailSampleDist < 0.9f ? 0 : m_cellSize * m_detailSampleDist;
	m_cfg.detailSampleMaxError = m_cellHeight * m_detailSampleMaxError;

	rcVcopy(m_cfg.bmin, bmin);
	rcVcopy(m_cfg.bmax, bmax);
	m_cfg.bmin[0] -= m_cfg.borderSize * m_cfg.cs;
	m_cfg.bmin[2] -= m_cfg.borderSize * m_cfg.cs;
	m_cfg.bmax[0] += m_cfg.borderSize * m_cfg.cs;
	m_cfg.bmax[2] += m_cfg.borderSize * m_cfg.cs;

#ifdef ELY_DEBUG
	// Reset build times gathering.
	m_ctx->resetTimers();

	// Start the build process.
	m_ctx->startTimer(RC_TIMER_TOTAL);

	m_ctx->log(RC_LOG_PROGRESS, "Building navigation:");
	m_ctx->log(RC_LOG_PROGRESS, " - %d x %d cells", m_cfg.width, m_cfg.height);
	m_ctx->log(RC_LOG_PROGRESS, " - %.1fK verts, %.1fK tris", nverts / 1000.0f,
			ntris / 1000.0f);
#endif
	// Allocate voxel heightfield where we rasterize our input data to.
	m_solid = rcAllocHeightfield();
	if (!m_solid)
	{
		CTXLOG(m_ctx, RC_LOG_ERROR, "buildNavigation: Out of memory 'solid'.");
		return 0;
	}
	if (!rcCreateHeightfield(m_ctx, *m_solid, m_cfg.width, m_cfg.height,
			m_cfg.bmin, m_cfg.bmax, m_cfg.cs, m_cfg.ch))
	{
		CTXLOG(m_ctx, RC_LOG_ERROR,
				"buildNavigation: Could not create solid heightfield.");
		return 0;
	}

	// Allocate array that can hold triangle flags.
	// If you have multiple meshes you need to process, allocate
	// and array which can hold the max number of triangles you need to process.
	m_triareas = new unsigned char[chunkyMesh->maxTrisPerChunk];
	if (!m_triareas)
	{
		CTXLOG1(m_ctx, RC_LOG_ERROR,
				"buildNavigation: Out of memory 'm_triareas' (%d).",
				chunkyMesh->maxTrisPerChunk);
		return 0;
	}

	float tbmin[2], tbmax[2];
	tbmin[0] = m_cfg.bmin[0];
	tbmin[1] = m_cfg.bmin[2];
	tbmax[0] = m_cfg.bmax[0];
	tbmax[1] = m_cfg.bmax[2];
	int cid[512];	// TODO: Make grow when returning too many items.
	const int ncid = rcGetChunksOverlappingRect(chunkyMesh, tbmin, tbmax, cid,
			512);
	if (!ncid)
		return 0;

	m_tileTriCount = 0;

	for (int i = 0; i < ncid; ++i)
	{
		const rcChunkyTriMeshNode& node = chunkyMesh->nodes[cid[i]];
		const int* ctris = &chunkyMesh->tris[node.i * 3];
		const int nctris = node.n;

		m_tileTriCount += nctris;

		memset(m_triareas, 0, nctris * sizeof(unsigned char));
		rcMarkWalkableTriangles(m_ctx, m_cfg.walkableSlopeAngle, verts, nverts,
				ctris, nctris, m_triareas);

		rcRasterizeTriangles(m_ctx, verts, nverts, ctris, m_triareas, nctris,
				*m_solid, m_cfg.walkableClimb);
	}

	if (!m_keepInterResults)
	{
		delete[] m_triareas;
		m_triareas = 0;
	}

	// Once all geometry is rasterized, we do initial pass of filtering to
	// remove unwanted overhangs caused by the conservative rasterization
	// as well as filter spans where the character cannot possibly stand.
	rcFilterLowHangingWalkableObstacles(m_ctx, m_cfg.walkableClimb, *m_solid);
	rcFilterLedgeSpans(m_ctx, m_cfg.walkableHeight, m_cfg.walkableClimb,
			*m_solid);
	rcFilterWalkableLowHeightSpans(m_ctx, m_cfg.walkableHeight, *m_solid);

	// Compact the heightfield so that it is faster to handle from now on.
	// This will result more cache coherent data as well as the neighbours
	// between walkable cells will be calculated.
	m_chf = rcAllocCompactHeightfield();
	if (!m_chf)
	{
		CTXLOG(m_ctx, RC_LOG_ERROR, "buildNavigation: Out of memory 'chf'.");
		return 0;
	}
	if (!rcBuildCompactHeightfield(m_ctx, m_cfg.walkableHeight,
			m_cfg.walkableClimb, *m_solid, *m_chf))
	{
		CTXLOG(m_ctx, RC_LOG_ERROR,
				"buildNavigation: Could not build compact data.");
		return 0;
	}

	if (!m_keepInterResults)
	{
		rcFreeHeightField(m_solid);
		m_solid = 0;
	}

	// Erode the walkable area by agent radius.
	if (!rcErodeWalkableArea(m_ctx, m_cfg.walkableRadius, *m_chf))
	{
		CTXLOG(m_ctx, RC_LOG_ERROR, "buildNavigation: Could not erode.");
		return 0;
	}

	// (Optional) Mark areas.
	const ConvexVolume* vols = m_geom->getConvexVolumes();
	for (int i = 0; i < m_geom->getConvexVolumeCount(); ++i)
		rcMarkConvexPolyArea(m_ctx, vols[i].verts, vols[i].nverts, vols[i].hmin,
				vols[i].hmax, (unsigned char) vols[i].area, *m_chf);

	if (m_monotonePartitioning)
	{
		// Partition the walkable surface into simple regions without holes.
		if (!rcBuildRegionsMonotone(m_ctx, *m_chf, m_cfg.borderSize,
				m_cfg.minRegionArea, m_cfg.mergeRegionArea))
		{
			CTXLOG(m_ctx, RC_LOG_ERROR,
					"buildNavigation: Could not build regions.");
			return 0;
		}
	}
	else
	{
		// Prepare for region partitioning, by calculating distance field along the walkable surface.
		if (!rcBuildDistanceField(m_ctx, *m_chf))
		{
			CTXLOG(m_ctx, RC_LOG_ERROR,
					"buildNavigation: Could not build distance field.");
			return 0;
		}

		// Partition the walkable surface into simple regions without holes.
		if (!rcBuildRegions(m_ctx, *m_chf, m_cfg.borderSize,
				m_cfg.minRegionArea, m_cfg.mergeRegionArea))
		{
			CTXLOG(m_ctx, RC_LOG_ERROR,
					"buildNavigation: Could not build regions.");
			return 0;
		}
	}

	// Create contours.
	m_cset = rcAllocContourSet();
	if (!m_cset)
	{
		CTXLOG(m_ctx, RC_LOG_ERROR, "buildNavigation: Out of memory 'cset'.");
		return 0;
	}
	if (!rcBuildContours(m_ctx, *m_chf, m_cfg.maxSimplificationError,
			m_cfg.maxEdgeLen, *m_cset))
	{
		CTXLOG(m_ctx, RC_LOG_ERROR, "buildNavigation: Could not create contours.");
		return 0;
	}

	if (m_cset->nconts == 0)
	{
		return 0;
	}

	// Build polygon navmesh from the contours.
	m_pmesh = rcAllocPolyMesh();
	if (!m_pmesh)
	{
		CTXLOG(m_ctx, RC_LOG_ERROR, "buildNavigation: Out of memory 'pmesh'.");
		return 0;
	}
	if (!rcBuildPolyMesh(m_ctx, *m_cset, m_cfg.maxVertsPerPoly, *m_pmesh))
	{
		CTXLOG(m_ctx, RC_LOG_ERROR,
				"buildNavigation: Could not triangulate contours.");
		return 0;
	}

	// Build detail mesh.
	m_dmesh = rcAllocPolyMeshDetail();
	if (!m_dmesh)
	{
		CTXLOG(m_ctx, RC_LOG_ERROR, "buildNavigation: Out of memory 'dmesh'.");
		return 0;
	}

	if (!rcBuildPolyMeshDetail(m_ctx, *m_pmesh, *m_chf, m_cfg.detailSampleDist,
			m_cfg.detailSampleMaxError, *m_dmesh))
	{
		CTXLOG(m_ctx, RC_LOG_ERROR,
				"buildNavigation: Could build polymesh detail.");
		return 0;
	}

	if (!m_keepInterResults)
	{
		rcFreeCompactHeightfield(m_chf);
		m_chf = 0;
		rcFreeContourSet(m_cset);
		m_cset = 0;
	}

	unsigned char* navData = 0;
	int navDataSize = 0;
	if (m_cfg.maxVertsPerPoly <= DT_VERTS_PER_POLYGON)
	{
		if (m_pmesh->nverts >= 0xffff)
		{
			// The vertex indices are ushorts, and cannot point to more than 0xffff vertices.
			CTXLOG2(m_ctx, RC_LOG_ERROR, "Too many vertices per tile %d (max: %d).",
					m_pmesh->nverts, 0xffff);
			return 0;
		}

		// Update poly flags from areas.
		for (int i = 0; i < m_pmesh->npolys; ++i)
		{
			if (m_pmesh->areas[i] == RC_WALKABLE_AREA)
				m_pmesh->areas[i] = NAVMESH_POLYAREA_GROUND;

			//set polyFlags for polyAreas only if m_flagsAreaTable not empty
			if (not m_flagsAreaTable.empty())
			{
				// get flags from a table indexed by areas
				m_pmesh->flags[i] = m_flagsAreaTable[m_pmesh->areas[i]];
			}
			else
			{
				if (m_pmesh->areas[i] == NAVMESH_POLYAREA_GROUND
						|| m_pmesh->areas[i] == NAVMESH_POLYAREA_GRASS
						|| m_pmesh->areas[i] == NAVMESH_POLYAREA_ROAD)
				{
					m_pmesh->flags[i] = NAVMESH_POLYFLAGS_WALK;
				}
				else if (m_pmesh->areas[i] == NAVMESH_POLYAREA_WATER)
				{
					m_pmesh->flags[i] = NAVMESH_POLYFLAGS_SWIM;
				}
				else if (m_pmesh->areas[i] == NAVMESH_POLYAREA_DOOR)
				{
					m_pmesh->flags[i] = NAVMESH_POLYFLAGS_WALK
							| NAVMESH_POLYFLAGS_DOOR;
				}
			}
		}

		dtNavMeshCreateParams params;
		memset(&params, 0, sizeof(params));
		params.verts = m_pmesh->verts;
		params.vertCount = m_pmesh->nverts;
		params.polys = m_pmesh->polys;
		params.polyAreas = m_pmesh->areas;
		params.polyFlags = m_pmesh->flags;
		params.polyCount = m_pmesh->npolys;
		params.nvp = m_pmesh->nvp;
		params.detailMeshes = m_dmesh->meshes;
		params.detailVerts = m_dmesh->verts;
		params.detailVertsCount = m_dmesh->nverts;
		params.detailTris = m_dmesh->tris;
		params.detailTriCount = m_dmesh->ntris;
		params.offMeshConVerts = m_geom->getOffMeshConnectionVerts();
		params.offMeshConRad = m_geom->getOffMeshConnectionRads();
		params.offMeshConDir = m_geom->getOffMeshConnectionDirs();
		params.offMeshConAreas = m_geom->getOffMeshConnectionAreas();
		params.offMeshConFlags = m_geom->getOffMeshConnectionFlags();
		params.offMeshConUserID = m_geom->getOffMeshConnectionId();
		params.offMeshConCount = m_geom->getOffMeshConnectionCount();
		params.walkableHeight = m_agentHeight;
		params.walkableRadius = m_agentRadius;
		params.walkableClimb = m_agentMaxClimb;
		params.tileX = tx;
		params.tileY = ty;
		params.tileLayer = 0;
		rcVcopy(params.bmin, m_pmesh->bmin);
		rcVcopy(params.bmax, m_pmesh->bmax);
		params.cs = m_cfg.cs;
		params.ch = m_cfg.ch;
		params.buildBvTree = true;

		if (!dtCreateNavMeshData(&params, &navData, &navDataSize))
		{
			CTXLOG(m_ctx, RC_LOG_ERROR, "Could not build Detour navmesh.");
			return 0;
		}
	}
	m_tileMemUsage = navDataSize / 1024.0f;

#ifdef ELY_DEBUG
	m_ctx->stopTimer(RC_TIMER_TOTAL);

	// Show performance stats.
	duLogBuildTimes(*m_ctx, m_ctx->getAccumulatedTime(RC_TIMER_TOTAL));
	m_ctx->log(RC_LOG_PROGRESS, ">> Polymesh: %d vertices  %d polygons",
			m_pmesh->nverts, m_pmesh->npolys);

	m_tileBuildTime = m_ctx->getAccumulatedTime(RC_TIMER_TOTAL) / 1000.0f;
#endif

	dataSize = navDataSize;
	return navData;
}

void NavMeshType_Tile::setTileSettings(const NavMeshTileSettings& settings)
{
	m_buildAll = settings.m_buildAllTiles;
	m_maxTiles = settings.m_maxTiles;
	m_maxPolysPerTile = settings.m_maxPolysPerTile;
	m_tileSize = settings.m_tileSize;
}

NavMeshTileSettings NavMeshType_Tile::getTileSettings()
{
	NavMeshTileSettings settings;
	settings.m_buildAllTiles = m_buildAll;
	settings.m_maxTiles = m_maxTiles;
	settings.m_maxPolysPerTile = m_maxPolysPerTile;
	settings.m_tileSize = m_tileSize;
	return settings;
}

} // namespace ely
