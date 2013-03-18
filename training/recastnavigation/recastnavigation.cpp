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
 * \file /Ely/training/recastnavigation.cpp
 *
 * \date 14/mar/2013 10:05:13
 * \author marco
 */

#include "Utilities/Tools.h"
#include <iostream>
#include <string>
#include <vector>
#include <load_prc_file.h>
#include <auto_bind.h>
#include <partBundleHandle.h>
#include <character.h>
#include <animControlCollection.h>
#include <pandaFramework.h>

//rn
#include <cstring>
#include <cmath>
#include <Recast.h>
#include <DetourNavMesh.h>
#include <DetourNavMeshQuery.h>
#include <DetourNavMeshBuilder.h>
#include <DetourCommon.h>
#include "InputGeom.h"
#include "SampleInterfaces.h"

//Bind the Model and the Animation
// don't use PT or CPT with AnimControlCollection
AnimControlCollection rn_anim_collection;
AsyncTask::DoneStatus rn_check_playing(GenericAsyncTask* task, void* data);

//rn
std::string baseDir("/REPOSITORY/KProjects/WORKSPACE/Ely/");
std::string rnDir(
		"/REPOSITORY/KProjects/WORKSPACE/recastnavigation/RecastDemo/Bin/Meshes/");
//obj2egg -TR 90,0,0 nav_test.obj -o nav_test_panda.egg
std::string meshNameEgg("nav_test_panda.egg");
std::string meshNameObj("nav_test.obj");
//https://groups.google.com/forum/?fromgroups=#!searchin/recastnavigation/z$20axis/recastnavigation/fMqEAqSBOBk/zwOzHmjRsj0J
inline void LVector3fToRecast(const LVector3f& v, float* p)
{
	p[0] = -v.get_x();
	p[1] = v.get_z();
	p[2] = v.get_y();
}
inline LVector3f recastToLVector3f(const float* p)
{
	return LVector3f(-p[0], p[2], p[1]);
}

/// These are just sample areas to use consistent values across the samples.
/// The use should specify these base on his needs.
enum SamplePolyAreas
{
	SAMPLE_POLYAREA_GROUND,
	SAMPLE_POLYAREA_WATER,
	SAMPLE_POLYAREA_ROAD,
	SAMPLE_POLYAREA_DOOR,
	SAMPLE_POLYAREA_GRASS,
	SAMPLE_POLYAREA_JUMP,
};
enum SamplePolyFlags
{
	SAMPLE_POLYFLAGS_WALK = 0x01,		// Ability to walk (ground, grass, road)
	SAMPLE_POLYFLAGS_SWIM = 0x02,		// Ability to swim (water).
	SAMPLE_POLYFLAGS_DOOR = 0x04,		// Ability to move through doors.
	SAMPLE_POLYFLAGS_JUMP = 0x08,		// Ability to jump.
	SAMPLE_POLYFLAGS_DISABLED = 0x10,		// Disabled polygon
	SAMPLE_POLYFLAGS_ALL = 0xffff	// All abilities.
};

//tool
static bool getSteerTarget(dtNavMeshQuery* navQuery, const float* startPos, const float* endPos,
						   const float minTargetDist,
						   const dtPolyRef* path, const int pathSize,
						   float* steerPos, unsigned char& steerPosFlag, dtPolyRef& steerPosRef,
						   float* outPoints = 0, int* outPointCount = 0)
{
	// Find steer target.
	static const int MAX_STEER_POINTS = 3;
	float steerPath[MAX_STEER_POINTS*3];
	unsigned char steerPathFlags[MAX_STEER_POINTS];
	dtPolyRef steerPathPolys[MAX_STEER_POINTS];
	int nsteerPath = 0;
	navQuery->findStraightPath(startPos, endPos, path, pathSize,
							   steerPath, steerPathFlags, steerPathPolys, &nsteerPath, MAX_STEER_POINTS);
	if (!nsteerPath)
		return false;

	if (outPoints && outPointCount)
	{
		*outPointCount = nsteerPath;
		for (int i = 0; i < nsteerPath; ++i)
			dtVcopy(&outPoints[i*3], &steerPath[i*3]);
	}


	// Find vertex far enough to steer to.
	int ns = 0;
	while (ns < nsteerPath)
	{
		// Stop at Off-Mesh link or when point is further than slop away.
		if ((steerPathFlags[ns] & DT_STRAIGHTPATH_OFFMESH_CONNECTION) ||
			!inRange(&steerPath[ns*3], startPos, minTargetDist, 1000.0f))
			break;
		ns++;
	}
	// Failed to find good point to steer to.
	if (ns >= nsteerPath)
		return false;

	dtVcopy(steerPos, &steerPath[ns*3]);
	steerPos[1] = startPos[1];
	steerPosFlag = steerPathFlags[ns];
	steerPosRef = steerPathPolys[ns];

	return true;
}
static int fixupCorridor(dtPolyRef* path, const int npath, const int maxPath,
						 const dtPolyRef* visited, const int nvisited)
{
	int furthestPath = -1;
	int furthestVisited = -1;

	// Find furthest common polygon.
	for (int i = npath-1; i >= 0; --i)
	{
		bool found = false;
		for (int j = nvisited-1; j >= 0; --j)
		{
			if (path[i] == visited[j])
			{
				furthestPath = i;
				furthestVisited = j;
				found = true;
			}
		}
		if (found)
			break;
	}

	// If no intersection found just return current path.
	if (furthestPath == -1 || furthestVisited == -1)
		return npath;

	// Concatenate paths.

	// Adjust beginning of the buffer to include the visited.
	const int req = nvisited - furthestVisited;
	const int orig = rcMin(furthestPath+1, npath);
	int size = rcMax(0, npath-orig);
	if (req+size > maxPath)
		size = maxPath-req;
	if (size)
		memmove(path+req, path+orig, size*sizeof(dtPolyRef));

	// Store visited
	for (int i = 0; i < req; ++i)
		path[i] = visited[(nvisited-1)-i];

	return req+size;
}

//
class RN
{
	static const int MAX_POLYS = 256;
	static const int MAX_SMOOTH = 2048;
	//sample
	InputGeom* m_geom;
	BuildContext* m_ctx;
	unsigned char* m_triareas;
	rcHeightfield* m_solid;
	rcCompactHeightfield* m_chf;
	rcContourSet* m_cset;
	rcPolyMesh* m_pmesh;
	rcPolyMeshDetail* m_dmesh;
	//
	bool m_keepInterResults;
	float m_totalBuildTimeMs;
	rcConfig m_cfg;
	float m_cellSize, m_cellHeight;
	float m_agentHeight, m_agentRadius, m_agentMaxClimb, m_agentMaxSlope;
	float m_regionMinSize, m_regionMergeSize;
	bool m_monotonePartitioning;
	float m_edgeMaxLen, m_edgeMaxError;
	float m_vertsPerPoly;
	float m_detailSampleDist, m_detailSampleMaxError;
	//tool
	dtNavMesh* m_navMesh;
	dtNavMeshQuery* m_navQuery;
	dtStatus m_pathFindStatus;
	dtQueryFilter m_filter;
	float m_neighbourhoodRadius, m_randomRadius;
	float m_spos[3], m_epos[3];
	float m_polyPickExt[3];
	float m_smoothPath[MAX_SMOOTH*3];
	dtPolyRef m_startRef, m_endRef;
	int m_pathIterNum;
	dtPolyRef m_polys[MAX_POLYS];
	int m_npolys;
	int m_nsmoothPath;

public:
	RN();
	~RN();
	bool loadMesh(const std::string& path, const std::string& meshName);
	bool handleBuild();
	void cleanup();
	void NavMeshTesterTool_init();
	void NavMeshTesterTool_recalc();
};

RN::RN() :
		m_geom(NULL), m_ctx(NULL), m_triareas(NULL), m_solid(NULL), m_chf(NULL), m_cset(
				NULL), m_pmesh(NULL), m_dmesh(NULL), m_navMesh(NULL), m_pathFindStatus(
				DT_FAILURE), m_startRef(0), m_endRef(0), m_npolys(0), m_nsmoothPath(
				0)
{
	m_ctx = new BuildContext;
	//tool
	m_filter.setIncludeFlags(SAMPLE_POLYFLAGS_ALL ^ SAMPLE_POLYFLAGS_DISABLED);
	m_filter.setExcludeFlags(0);
	m_polyPickExt[0] = 2;
	m_polyPickExt[1] = 4;
	m_polyPickExt[2] = 2;
	m_neighbourhoodRadius = 2.5f;
	m_randomRadius = 5.0f;
}

RN::~RN()
{
	if (m_ctx)
	{
		delete m_ctx;
	}
	if (m_geom)
	{
		delete m_geom;
	}
}

bool RN::loadMesh(const std::string& path, const std::string& meshName)
{
	bool result = true;
	m_geom = new InputGeom;
	if (not m_geom->loadMesh(m_ctx, path.c_str()))
	{
		delete m_geom;
		m_geom = NULL;
		m_ctx->dumpLog("Geom load log %s:", meshName.c_str());
		result = false;
	}
	return result;
}

void RN::cleanup()
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
	dtFreeNavMesh(m_navMesh);
	m_navMesh = 0;
}

bool RN::handleBuild()
{
	if (!m_geom || !m_geom->getMesh())
	{
		m_ctx->log(RC_LOG_ERROR,
				"buildNavigation: Input mesh is not specified.");
		return false;
	}

	cleanup();

	const float* bmin = m_geom->getMeshBoundsMin();
	const float* bmax = m_geom->getMeshBoundsMax();
	const float* verts = m_geom->getMesh()->getVerts();
	const int nverts = m_geom->getMesh()->getVertCount();
	const int* tris = m_geom->getMesh()->getTris();
	const int ntris = m_geom->getMesh()->getTriCount();

	//
	// Step 1. Initialize build config.
	//

	// Init build configuration from GUI
	memset(&m_cfg, 0, sizeof(m_cfg));
	m_cfg.cs = m_cellSize;
	m_cfg.ch = m_cellHeight;
	m_cfg.walkableSlopeAngle = m_agentMaxSlope;
	m_cfg.walkableHeight = (int) ceil(m_agentHeight / m_cfg.ch);
	m_cfg.walkableClimb = (int) floor(m_agentMaxClimb / m_cfg.ch);
	m_cfg.walkableRadius = (int) ceil(m_agentRadius / m_cfg.cs);
	m_cfg.maxEdgeLen = (int) (m_edgeMaxLen / m_cellSize);
	m_cfg.maxSimplificationError = m_edgeMaxError;
	m_cfg.minRegionArea = (int) rcSqr(m_regionMinSize);	// Note: area = size*size
	m_cfg.mergeRegionArea = (int) rcSqr(m_regionMergeSize);	// Note: area = size*size
	m_cfg.maxVertsPerPoly = (int) m_vertsPerPoly;
	m_cfg.detailSampleDist =
			m_detailSampleDist < 0.9f ? 0 : m_cellSize * m_detailSampleDist;
	m_cfg.detailSampleMaxError = m_cellHeight * m_detailSampleMaxError;

	// Set the area where the navigation will be build.
	// Here the bounds of the input mesh are used, but the
	// area could be specified by an user defined box, etc.
	rcVcopy(m_cfg.bmin, bmin);
	rcVcopy(m_cfg.bmax, bmax);
	rcCalcGridSize(m_cfg.bmin, m_cfg.bmax, m_cfg.cs, &m_cfg.width,
			&m_cfg.height);

	// Reset build times gathering.
	m_ctx->resetTimers();

	// Start the build process.
	m_ctx->startTimer(RC_TIMER_TOTAL);

	m_ctx->log(RC_LOG_PROGRESS, "Building navigation:");
	m_ctx->log(RC_LOG_PROGRESS, " - %d x %d cells", m_cfg.width, m_cfg.height);
	m_ctx->log(RC_LOG_PROGRESS, " - %.1fK verts, %.1fK tris", nverts / 1000.0f,
			ntris / 1000.0f);

	//
	// Step 2. Rasterize input polygon soup.
	//

	// Allocate voxel heightfield where we rasterize our input data to.
	m_solid = rcAllocHeightfield();
	if (!m_solid)
	{
		m_ctx->log(RC_LOG_ERROR, "buildNavigation: Out of memory 'solid'.");
		return false;
	}
	if (!rcCreateHeightfield(m_ctx, *m_solid, m_cfg.width, m_cfg.height,
			m_cfg.bmin, m_cfg.bmax, m_cfg.cs, m_cfg.ch))
	{
		m_ctx->log(RC_LOG_ERROR,
				"buildNavigation: Could not create solid heightfield.");
		return false;
	}

	// Allocate array that can hold triangle area types.
	// If you have multiple meshes you need to process, allocate
	// and array which can hold the max number of triangles you need to process.
	m_triareas = new unsigned char[ntris];
	if (!m_triareas)
	{
		m_ctx->log(RC_LOG_ERROR,
				"buildNavigation: Out of memory 'm_triareas' (%d).", ntris);
		return false;
	}

	// Find triangles which are walkable based on their slope and rasterize them.
	// If your input data is multiple meshes, you can transform them here, calculate
	// the are type for each of the meshes and rasterize them.
	memset(m_triareas, 0, ntris * sizeof(unsigned char));
	rcMarkWalkableTriangles(m_ctx, m_cfg.walkableSlopeAngle, verts, nverts,
			tris, ntris, m_triareas);
	rcRasterizeTriangles(m_ctx, verts, nverts, tris, m_triareas, ntris,
			*m_solid, m_cfg.walkableClimb);

	if (!m_keepInterResults)
	{
		delete[] m_triareas;
		m_triareas = 0;
	}

	//
	// Step 3. Filter walkables surfaces.
	//

	// Once all geoemtry is rasterized, we do initial pass of filtering to
	// remove unwanted overhangs caused by the conservative rasterization
	// as well as filter spans where the character cannot possibly stand.
	rcFilterLowHangingWalkableObstacles(m_ctx, m_cfg.walkableClimb, *m_solid);
	rcFilterLedgeSpans(m_ctx, m_cfg.walkableHeight, m_cfg.walkableClimb,
			*m_solid);
	rcFilterWalkableLowHeightSpans(m_ctx, m_cfg.walkableHeight, *m_solid);

	//
	// Step 4. Partition walkable surface to simple regions.
	//

	// Compact the heightfield so that it is faster to handle from now on.
	// This will result more cache coherent data as well as the neighbours
	// between walkable cells will be calculated.
	m_chf = rcAllocCompactHeightfield();
	if (!m_chf)
	{
		m_ctx->log(RC_LOG_ERROR, "buildNavigation: Out of memory 'chf'.");
		return false;
	}
	if (!rcBuildCompactHeightfield(m_ctx, m_cfg.walkableHeight,
			m_cfg.walkableClimb, *m_solid, *m_chf))
	{
		m_ctx->log(RC_LOG_ERROR,
				"buildNavigation: Could not build compact data.");
		return false;
	}

	if (!m_keepInterResults)
	{
		rcFreeHeightField(m_solid);
		m_solid = 0;
	}

	// Erode the walkable area by agent radius.
	if (!rcErodeWalkableArea(m_ctx, m_cfg.walkableRadius, *m_chf))
	{
		m_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not erode.");
		return false;
	}

	// (Optional) Mark areas.
	const ConvexVolume* vols = m_geom->getConvexVolumes();
	for (int i = 0; i < m_geom->getConvexVolumeCount(); ++i)
		rcMarkConvexPolyArea(m_ctx, vols[i].verts, vols[i].nverts, vols[i].hmin,
				vols[i].hmax, (unsigned char) vols[i].area, *m_chf);

	if (m_monotonePartitioning)
	{
		// Partition the walkable surface into simple regions without holes.
		// Monotone partitioning does not need distancefield.
		if (!rcBuildRegionsMonotone(m_ctx, *m_chf, 0, m_cfg.minRegionArea,
				m_cfg.mergeRegionArea))
		{
			m_ctx->log(RC_LOG_ERROR,
					"buildNavigation: Could not build regions.");
			return false;
		}
	}
	else
	{
		// Prepare for region partitioning, by calculating distance field along the walkable surface.
		if (!rcBuildDistanceField(m_ctx, *m_chf))
		{
			m_ctx->log(RC_LOG_ERROR,
					"buildNavigation: Could not build distance field.");
			return false;
		}

		// Partition the walkable surface into simple regions without holes.
		if (!rcBuildRegions(m_ctx, *m_chf, 0, m_cfg.minRegionArea,
				m_cfg.mergeRegionArea))
		{
			m_ctx->log(RC_LOG_ERROR,
					"buildNavigation: Could not build regions.");
			return false;
		}
	}

	//
	// Step 5. Trace and simplify region contours.
	//

	// Create contours.
	m_cset = rcAllocContourSet();
	if (!m_cset)
	{
		m_ctx->log(RC_LOG_ERROR, "buildNavigation: Out of memory 'cset'.");
		return false;
	}
	if (!rcBuildContours(m_ctx, *m_chf, m_cfg.maxSimplificationError,
			m_cfg.maxEdgeLen, *m_cset))
	{
		m_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not create contours.");
		return false;
	}

	//
	// Step 6. Build polygons mesh from contours.
	//

	// Build polygon navmesh from the contours.
	m_pmesh = rcAllocPolyMesh();
	if (!m_pmesh)
	{
		m_ctx->log(RC_LOG_ERROR, "buildNavigation: Out of memory 'pmesh'.");
		return false;
	}
	if (!rcBuildPolyMesh(m_ctx, *m_cset, m_cfg.maxVertsPerPoly, *m_pmesh))
	{
		m_ctx->log(RC_LOG_ERROR,
				"buildNavigation: Could not triangulate contours.");
		return false;
	}

	//
	// Step 7. Create detail mesh which allows to access approximate height on each polygon.
	//

	m_dmesh = rcAllocPolyMeshDetail();
	if (!m_dmesh)
	{
		m_ctx->log(RC_LOG_ERROR, "buildNavigation: Out of memory 'pmdtl'.");
		return false;
	}

	if (!rcBuildPolyMeshDetail(m_ctx, *m_pmesh, *m_chf, m_cfg.detailSampleDist,
			m_cfg.detailSampleMaxError, *m_dmesh))
	{
		m_ctx->log(RC_LOG_ERROR,
				"buildNavigation: Could not build detail mesh.");
		return false;
	}

	if (!m_keepInterResults)
	{
		rcFreeCompactHeightfield(m_chf);
		m_chf = 0;
		rcFreeContourSet(m_cset);
		m_cset = 0;
	}

	// At this point the navigation mesh data is ready, you can access it from m_pmesh.
	// See duDebugDrawPolyMesh or dtCreateNavMeshData as examples how to access the data.

	//
	// (Optional) Step 8. Create Detour data from Recast poly mesh.
	//

	// The GUI may allow more max points per polygon than Detour can handle.
	// Only build the detour navmesh if we do not exceed the limit.
	if (m_cfg.maxVertsPerPoly <= DT_VERTS_PER_POLYGON)
	{
		unsigned char* navData = 0;
		int navDataSize = 0;

		// Update poly flags from areas.
		for (int i = 0; i < m_pmesh->npolys; ++i)
		{
			if (m_pmesh->areas[i] == RC_WALKABLE_AREA)
				m_pmesh->areas[i] = SAMPLE_POLYAREA_GROUND;

			if (m_pmesh->areas[i] == SAMPLE_POLYAREA_GROUND
					|| m_pmesh->areas[i] == SAMPLE_POLYAREA_GRASS
					|| m_pmesh->areas[i] == SAMPLE_POLYAREA_ROAD)
			{
				m_pmesh->flags[i] = SAMPLE_POLYFLAGS_WALK;
			}
			else if (m_pmesh->areas[i] == SAMPLE_POLYAREA_WATER)
			{
				m_pmesh->flags[i] = SAMPLE_POLYFLAGS_SWIM;
			}
			else if (m_pmesh->areas[i] == SAMPLE_POLYAREA_DOOR)
			{
				m_pmesh->flags[i] = SAMPLE_POLYFLAGS_WALK
						| SAMPLE_POLYFLAGS_DOOR;
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
		rcVcopy(params.bmin, m_pmesh->bmin);
		rcVcopy(params.bmax, m_pmesh->bmax);
		params.cs = m_cfg.cs;
		params.ch = m_cfg.ch;
		params.buildBvTree = true;

		if (!dtCreateNavMeshData(&params, &navData, &navDataSize))
		{
			m_ctx->log(RC_LOG_ERROR, "Could not build Detour navmesh.");
			return false;
		}

		m_navMesh = dtAllocNavMesh();
		if (!m_navMesh)
		{
			dtFree(navData);
			m_ctx->log(RC_LOG_ERROR, "Could not create Detour navmesh");
			return false;
		}

		dtStatus status;

		status = m_navMesh->init(navData, navDataSize, DT_TILE_FREE_DATA);
		if (dtStatusFailed(status))
		{
			dtFree(navData);
			m_ctx->log(RC_LOG_ERROR, "Could not init Detour navmesh");
			return false;
		}

		status = m_navQuery->init(m_navMesh, 2048);
		if (dtStatusFailed(status))
		{
			m_ctx->log(RC_LOG_ERROR, "Could not init Detour navmesh query");
			return false;
		}
	}

	m_ctx->stopTimer(RC_TIMER_TOTAL);

	// Show performance stats.
	duLogBuildTimes(*m_ctx, m_ctx->getAccumulatedTime(RC_TIMER_TOTAL));
	m_ctx->log(RC_LOG_PROGRESS, ">> Polymesh: %d vertices  %d polygons",
			m_pmesh->nverts, m_pmesh->npolys);

	m_totalBuildTimeMs = m_ctx->getAccumulatedTime(RC_TIMER_TOTAL) / 1000.0f;

//	NavMeshTesterTool_init();

//	if (m_tool)
//		m_tool->init(this);//void CrowdTool::init(Sample* sample)
//	initToolStates(this);//void CrowdToolState::init(class Sample* sample)

	return true;
}

void RN::NavMeshTesterTool_init()
{
	NavMeshTesterTool_recalc();

	if (m_navQuery)
	{
		// Change costs.
		m_filter.setAreaCost(SAMPLE_POLYAREA_GROUND, 1.0f);
		m_filter.setAreaCost(SAMPLE_POLYAREA_WATER, 10.0f);
		m_filter.setAreaCost(SAMPLE_POLYAREA_ROAD, 1.0f);
		m_filter.setAreaCost(SAMPLE_POLYAREA_DOOR, 1.0f);
		m_filter.setAreaCost(SAMPLE_POLYAREA_GRASS, 2.0f);
		m_filter.setAreaCost(SAMPLE_POLYAREA_JUMP, 1.5f);
	}

	m_neighbourhoodRadius = m_agentRadius * 20.0f;
	m_randomRadius = m_agentRadius * 30.0f;
}

void RN::NavMeshTesterTool_recalc()
{
	if (!m_navMesh)
		return;

	//start pos
	m_navQuery->findNearestPoly(m_spos, m_polyPickExt, &m_filter, &m_startRef,
			0);
	//end pos
	m_navQuery->findNearestPoly(m_epos, m_polyPickExt, &m_filter, &m_endRef, 0);

	m_pathFindStatus = DT_FAILURE;

	//TOOLMODE_PATHFIND_FOLLOW
	{
		m_pathIterNum = 0;
		if (m_startRef && m_endRef)
		{
#ifdef DUMP_REQS
			printf("pi  %f %f %f  %f %f %f  0x%x 0x%x\n",
					m_spos[0],m_spos[1],m_spos[2], m_epos[0],m_epos[1],m_epos[2],
					m_filter.getIncludeFlags(), m_filter.getExcludeFlags());
#endif

			m_navQuery->findPath(m_startRef, m_endRef, m_spos, m_epos,
					&m_filter, m_polys, &m_npolys, MAX_POLYS);

			m_nsmoothPath = 0;

			if (m_npolys)
			{
				// Iterate over the path to find smooth path on the detail mesh surface.
				dtPolyRef polys[MAX_POLYS];
				memcpy(polys, m_polys, sizeof(dtPolyRef) * m_npolys);
				int npolys = m_npolys;

				float iterPos[3], targetPos[3];
				m_navQuery->closestPointOnPoly(m_startRef, m_spos, iterPos);
				m_navQuery->closestPointOnPoly(polys[npolys - 1], m_epos,
						targetPos);

				static const float STEP_SIZE = 0.5f;
				static const float SLOP = 0.01f;

				m_nsmoothPath = 0;

				dtVcopy(&m_smoothPath[m_nsmoothPath * 3], iterPos);
				m_nsmoothPath++;

				// Move towards target a small advancement at a time until target reached or
				// when ran out of memory to store the path.
				while (npolys && m_nsmoothPath < MAX_SMOOTH)
				{
					// Find location to steer towards.
					float steerPos[3];
					unsigned char steerPosFlag;
					dtPolyRef steerPosRef;

					if (!getSteerTarget(m_navQuery, iterPos, targetPos, SLOP,
							polys, npolys, steerPos, steerPosFlag, steerPosRef))
						break;

					bool endOfPath =
							(steerPosFlag & DT_STRAIGHTPATH_END) ? true : false;
					bool offMeshConnection =
							(steerPosFlag & DT_STRAIGHTPATH_OFFMESH_CONNECTION) ?
									true : false;

					// Find movement delta.
					float delta[3], len;
					dtVsub(delta, steerPos, iterPos);
					len = dtSqrt(dtVdot(delta, delta));
					// If the steer target is end of path or off-mesh link, do not move past the location.
					if ((endOfPath || offMeshConnection) && len < STEP_SIZE)
						len = 1;
					else
						len = STEP_SIZE / len;
					float moveTgt[3];
					dtVmad(moveTgt, iterPos, delta, len);

					// Move
					float result[3];
					dtPolyRef visited[16];
					int nvisited = 0;
					m_navQuery->moveAlongSurface(polys[0], iterPos, moveTgt,
							&m_filter, result, visited, &nvisited, 16);

					npolys = fixupCorridor(polys, npolys, MAX_POLYS, visited,
							nvisited);
					float h = 0;
					m_navQuery->getPolyHeight(polys[0], result, &h);
					result[1] = h;
					dtVcopy(iterPos, result);

					// Handle end of path and off-mesh links when close enough.
					if (endOfPath && inRange(iterPos, steerPos, SLOP, 1.0f))
					{
						// Reached end of path.
						dtVcopy(iterPos, targetPos);
						if (m_nsmoothPath < MAX_SMOOTH)
						{
							dtVcopy(&m_smoothPath[m_nsmoothPath * 3], iterPos);
							m_nsmoothPath++;
						}
						break;
					}
					else if (offMeshConnection
							&& inRange(iterPos, steerPos, SLOP, 1.0f))
					{
						// Reached off-mesh connection.
						float startPos[3], endPos[3];

						// Advance the path up to and over the off-mesh connection.
						dtPolyRef prevRef = 0, polyRef = polys[0];
						int npos = 0;
						while (npos < npolys && polyRef != steerPosRef)
						{
							prevRef = polyRef;
							polyRef = polys[npos];
							npos++;
						}
						for (int i = npos; i < npolys; ++i)
							polys[i - npos] = polys[i];
						npolys -= npos;

						// Handle the connection.
						dtStatus status =
								m_navMesh->getOffMeshConnectionPolyEndPoints(
										prevRef, polyRef, startPos, endPos);
						if (dtStatusSucceed(status))
						{
							if (m_nsmoothPath < MAX_SMOOTH)
							{
								dtVcopy(&m_smoothPath[m_nsmoothPath * 3],
										startPos);
								m_nsmoothPath++;
								// Hack to make the dotted path not visible during off-mesh connection.
								if (m_nsmoothPath & 1)
								{
									dtVcopy(&m_smoothPath[m_nsmoothPath * 3],
											startPos);
									m_nsmoothPath++;
								}
							}
							// Move position at the other side of the off-mesh link.
							dtVcopy(iterPos, endPos);
							float eh = 0.0f;
							m_navQuery->getPolyHeight(polys[0], iterPos, &eh);
							iterPos[1] = eh;
						}
					}

					// Store results.
					if (m_nsmoothPath < MAX_SMOOTH)
					{
						dtVcopy(&m_smoothPath[m_nsmoothPath * 3], iterPos);
						m_nsmoothPath++;
					}
				}
			}

		}
		else
		{
			m_npolys = 0;
			m_nsmoothPath = 0;
		}
	}
	//TOOLMODE_PATHFIND_STRAIGHT
//	{
//		if (m_sposSet && m_eposSet && m_startRef && m_endRef)
//		{
//#ifdef DUMP_REQS
//			printf("ps  %f %f %f  %f %f %f  0x%x 0x%x\n",
//					m_spos[0],m_spos[1],m_spos[2], m_epos[0],m_epos[1],m_epos[2],
//					m_filter.getIncludeFlags(), m_filter.getExcludeFlags());
//#endif
//			m_navQuery->findPath(m_startRef, m_endRef, m_spos, m_epos,
//					&m_filter, m_polys, &m_npolys, MAX_POLYS);
//			m_nstraightPath = 0;
//			if (m_npolys)
//			{
//				// In case of partial path, make sure the end point is clamped to the last polygon.
//				float epos[3];
//				dtVcopy(epos, m_epos);
//				if (m_polys[m_npolys - 1] != m_endRef)
//					m_navQuery->closestPointOnPoly(m_polys[m_npolys - 1],
//							m_epos, epos);
//
//				m_navQuery->findStraightPath(m_spos, epos, m_polys, m_npolys,
//						m_straightPath, m_straightPathFlags,
//						m_straightPathPolys, &m_nstraightPath, MAX_POLYS,
//						m_straightPathOptions);
//			}
//		}
//		else
//		{
//			m_npolys = 0;
//			m_nstraightPath = 0;
//		}
//	}
	//TOOLMODE_PATHFIND_SLICED
//	{
//		if (m_sposSet && m_eposSet && m_startRef && m_endRef)
//		{
//#ifdef DUMP_REQS
//			printf("ps  %f %f %f  %f %f %f  0x%x 0x%x\n",
//					m_spos[0],m_spos[1],m_spos[2], m_epos[0],m_epos[1],m_epos[2],
//					m_filter.getIncludeFlags(), m_filter.getExcludeFlags());
//#endif
//			m_npolys = 0;
//			m_nstraightPath = 0;
//
//			m_pathFindStatus = m_navQuery->initSlicedFindPath(m_startRef,
//					m_endRef, m_spos, m_epos, &m_filter);
//		}
//		else
//		{
//			m_npolys = 0;
//			m_nstraightPath = 0;
//		}
//	}
	//TOOLMODE_RAYCAST
//	{
//		m_nstraightPath = 0;
//		if (m_sposSet && m_eposSet && m_startRef)
//		{
//#ifdef DUMP_REQS
//			printf("rc  %f %f %f  %f %f %f  0x%x 0x%x\n",
//					m_spos[0],m_spos[1],m_spos[2], m_epos[0],m_epos[1],m_epos[2],
//					m_filter.getIncludeFlags(), m_filter.getExcludeFlags());
//#endif
//			float t = 0;
//			m_npolys = 0;
//			m_nstraightPath = 2;
//			m_straightPath[0] = m_spos[0];
//			m_straightPath[1] = m_spos[1];
//			m_straightPath[2] = m_spos[2];
//			m_navQuery->raycast(m_startRef, m_spos, m_epos, &m_filter, &t,
//					m_hitNormal, m_polys, &m_npolys, MAX_POLYS);
//			if (t > 1)
//			{
//				// No hit
//				dtVcopy(m_hitPos, m_epos);
//				m_hitResult = false;
//			}
//			else
//			{
//				// Hit
//				m_hitPos[0] = m_spos[0] + (m_epos[0] - m_spos[0]) * t;
//				m_hitPos[1] = m_spos[1] + (m_epos[1] - m_spos[1]) * t;
//				m_hitPos[2] = m_spos[2] + (m_epos[2] - m_spos[2]) * t;
//				if (m_npolys)
//				{
//					float h = 0;
//					m_navQuery->getPolyHeight(m_polys[m_npolys - 1], m_hitPos,
//							&h);
//					m_hitPos[1] = h;
//				}
//				m_hitResult = true;
//			}
//			dtVcopy(&m_straightPath[3], m_hitPos);
//		}
//	}
	//TOOLMODE_DISTANCE_TO_WALL
//	{
//		m_distanceToWall = 0;
//		if (m_sposSet && m_startRef)
//		{
//#ifdef DUMP_REQS
//			printf("dw  %f %f %f  %f  0x%x 0x%x\n",
//					m_spos[0],m_spos[1],m_spos[2], 100.0f,
//					m_filter.getIncludeFlags(), m_filter.getExcludeFlags());
//#endif
//			m_distanceToWall = 0.0f;
//			m_navQuery->findDistanceToWall(m_startRef, m_spos, 100.0f,
//					&m_filter, &m_distanceToWall, m_hitPos, m_hitNormal);
//		}
//	}
	//TOOLMODE_FIND_POLYS_IN_CIRCLE
//	{
//		if (m_sposSet && m_startRef && m_eposSet)
//		{
//			const float dx = m_epos[0] - m_spos[0];
//			const float dz = m_epos[2] - m_spos[2];
//			float dist = sqrtf(dx * dx + dz * dz);
//#ifdef DUMP_REQS
//			printf("fpc  %f %f %f  %f  0x%x 0x%x\n",
//					m_spos[0],m_spos[1],m_spos[2], dist,
//					m_filter.getIncludeFlags(), m_filter.getExcludeFlags());
//#endif
//			m_navQuery->findPolysAroundCircle(m_startRef, m_spos, dist,
//					&m_filter, m_polys, m_parent, 0, &m_npolys, MAX_POLYS);
//
//		}
//	}
	//TOOLMODE_FIND_POLYS_IN_SHAPE
//	{
//		if (m_sposSet && m_startRef && m_eposSet)
//		{
//			const float nx = (m_epos[2] - m_spos[2]) * 0.25f;
//			const float nz = -(m_epos[0] - m_spos[0]) * 0.25f;
//			const float agentHeight = m_sample ? m_sample->getAgentHeight() : 0;
//
//			m_queryPoly[0] = m_spos[0] + nx * 1.2f;
//			m_queryPoly[1] = m_spos[1] + agentHeight / 2;
//			m_queryPoly[2] = m_spos[2] + nz * 1.2f;
//
//			m_queryPoly[3] = m_spos[0] - nx * 1.3f;
//			m_queryPoly[4] = m_spos[1] + agentHeight / 2;
//			m_queryPoly[5] = m_spos[2] - nz * 1.3f;
//
//			m_queryPoly[6] = m_epos[0] - nx * 0.8f;
//			m_queryPoly[7] = m_epos[1] + agentHeight / 2;
//			m_queryPoly[8] = m_epos[2] - nz * 0.8f;
//
//			m_queryPoly[9] = m_epos[0] + nx;
//			m_queryPoly[10] = m_epos[1] + agentHeight / 2;
//			m_queryPoly[11] = m_epos[2] + nz;
//
//#ifdef DUMP_REQS
//			printf("fpp  %f %f %f  %f %f %f  %f %f %f  %f %f %f  0x%x 0x%x\n",
//					m_queryPoly[0],m_queryPoly[1],m_queryPoly[2],
//					m_queryPoly[3],m_queryPoly[4],m_queryPoly[5],
//					m_queryPoly[6],m_queryPoly[7],m_queryPoly[8],
//					m_queryPoly[9],m_queryPoly[10],m_queryPoly[11],
//					m_filter.getIncludeFlags(), m_filter.getExcludeFlags());
//#endif
//			m_navQuery->findPolysAroundShape(m_startRef, m_queryPoly, 4,
//					&m_filter, m_polys, m_parent, 0, &m_npolys, MAX_POLYS);
//		}
//	}
	//TOOLMODE_FIND_LOCAL_NEIGHBOURHOOD
//	{
//		if (m_sposSet && m_startRef)
//		{
//#ifdef DUMP_REQS
//			printf("fln  %f %f %f  %f  0x%x 0x%x\n",
//					m_spos[0],m_spos[1],m_spos[2], m_neighbourhoodRadius,
//					m_filter.getIncludeFlags(), m_filter.getExcludeFlags());
//#endif
//			m_navQuery->findLocalNeighbourhood(m_startRef, m_spos,
//					m_neighbourhoodRadius, &m_filter, m_polys, m_parent,
//					&m_npolys, MAX_POLYS);
//		}
//	}
}

int main(int argc, char **argv)
{
	///setup
	// Load your configuration

	load_prc_file_data("", "model-path" + baseDir + "data/models");
	load_prc_file_data("", "model-path" + baseDir + "data/shaders");
	load_prc_file_data("", "model-path" + baseDir + "data/sounds");
	load_prc_file_data("", "model-path" + baseDir + "data/textures");
	load_prc_file_data("", "show-frame-rate-meter #t");
	load_prc_file_data("", "lock-to-one-cpu 0");
	load_prc_file_data("", "support-threads 1");
	load_prc_file_data("", "audio-buffering-seconds 5");
	load_prc_file_data("", "audio-preload-threshold 2000000");
	load_prc_file_data("", "sync-video #t");

	PandaFramework panda = PandaFramework();
	panda.open_framework(argc, argv);
	panda.set_window_title("recastnavigation training");
	WindowFramework* window = panda.open_window();
	if (window != (WindowFramework *) NULL)
	{
		std::cout << "Opened the window successfully!\n";
		// common setup
		window->enable_keyboard(); // Enable keyboard detection
		window->setup_trackball(); // Enable default camera movement
	}

	//Load world mesh
	NodePath worldMesh = window->load_model(window->get_render(),
			rnDir + meshNameEgg);
	//attach to scene
	worldMesh.reparent_to(window->get_render());
	worldMesh.set_pos(0.0, 0.0, 0.0);

	//Load the Actor Model
	NodePath Actor = window->load_model(window->get_render(),
			baseDir + "data/models/eve.bam");
	SMARTPTR(Character)character =
	DCAST(Character, Actor.find("**/+Character").node());
	SMARTPTR(PartBundle)pbundle = character->get_bundle(0);
	//Load Animations
	std::vector<std::string> animations;
	animations.push_back(std::string(baseDir + "data/models/eve-run.bam"));
	animations.push_back(std::string(baseDir + "data/models/eve-walk.bam"));
	for (unsigned int i = 0; i < animations.size(); ++i)
	{
		window->load_model(Actor, animations[i]);
	}
	auto_bind(Actor.node(), rn_anim_collection);
	pbundle->set_anim_blend_flag(true);
	pbundle->set_control_effect(rn_anim_collection.get_anim(0), 0.5);
	pbundle->set_control_effect(rn_anim_collection.get_anim(1), 0.5);
	int actualAnim = 0;
	//switch among animations
	AsyncTask* task = new GenericAsyncTask("recastnavigation playing",
			&rn_check_playing, reinterpret_cast<void*>(&actualAnim));
	task->set_delay(3);
	panda.get_task_mgr().add(task);
	//attach to scene
	Actor.reparent_to(window->get_render());
	Actor.set_scale(0.3);
	Actor.set_pos(10.0, 0.0, 0.0);

	//RN
	std::string path = rnDir + meshNameObj;
	//rn_handleBuild

	// Do the main loop
	panda.main_loop();
	// close the framework
	panda.close_framework();
	return 0;
}

AsyncTask::DoneStatus rn_check_playing(GenericAsyncTask* task, void* data)
{
	//Control the Animations
	double time = ClockObject::get_global_clock()->get_real_time();
	int *actualAnim = reinterpret_cast<int*>(data);
	int num = *actualAnim % 3;
	if (num == 0)
	{
		std::cout << time << " - Blending" << std::endl;
		if (not rn_anim_collection.get_anim(0)->is_playing())
		{
			rn_anim_collection.get_anim(0)->play();
		}
		if (not rn_anim_collection.get_anim(1)->is_playing())
		{
			rn_anim_collection.get_anim(1)->play();
		}
	}
	else if (num == 1)
	{
		std::cout << time << " - Playing: "
				<< rn_anim_collection.get_anim_name(0) << std::endl;
		if (not rn_anim_collection.get_anim(0)->is_playing())
		{
			rn_anim_collection.get_anim(0)->play();
		}
		if (rn_anim_collection.get_anim(1)->is_playing())
		{
			rn_anim_collection.get_anim(1)->stop();
		}
	}
	else
	{
		std::cout << time << " - Playing: "
				<< rn_anim_collection.get_anim_name(1) << std::endl;
		rn_anim_collection.get_anim(1)->play();
		if (rn_anim_collection.get_anim(0)->is_playing())
		{
			rn_anim_collection.get_anim(0)->stop();
		}
		if (not rn_anim_collection.get_anim(1)->is_playing())
		{
			rn_anim_collection.get_anim(1)->play();
		}
	}
	*actualAnim += 1;
	return AsyncTask::DS_again;
}
