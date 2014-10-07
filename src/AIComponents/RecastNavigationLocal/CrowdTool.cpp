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
#include <float.h>
#include "AIComponents/RecastNavigationLocal/CrowdTool.h"
#include "AIComponents/RecastNavigationLocal/DebugInterfaces.h"
#include <DetourDebugDraw.h>
#include <DetourCommon.h>

#ifdef WIN32
#	define snprintf _snprintf
#endif

namespace
{

bool isectSegAABB(const float* sp, const float* sq, const float* amin,
		const float* amax, float& tmin, float& tmax)
{
	static const float EPS = 1e-6f;

	float d[3];
	dtVsub(d, sq, sp);
	tmin = 0;  // set to -FLT_MAX to get first hit on line
	tmax = FLT_MAX;		// set to max distance ray can travel (for segment)

	// For all three slabs
	for (int i = 0; i < 3; i++)
	{
		if (fabsf(d[i]) < EPS)
		{
			// Ray is parallel to slab. No hit if origin not within slab
			if (sp[i] < amin[i] || sp[i] > amax[i])
				return false;
		}
		else
		{
			// Compute intersection t value of ray with near and far plane of slab
			const float ood = 1.0f / d[i];
			float t1 = (amin[i] - sp[i]) * ood;
			float t2 = (amax[i] - sp[i]) * ood;
			// Make t1 be intersection with near plane, t2 with far plane
			if (t1 > t2)
				dtSwap(t1, t2);
			// Compute the intersection of slab intersections intervals
			if (t1 > tmin)
				tmin = t1;
			if (t2 < tmax)
				tmax = t2;
			// Exit with no collision as soon as slab intersection becomes empty
			if (tmin > tmax)
				return false;
		}
	}

	return true;
}

void getAgentBounds(const dtCrowdAgent* ag, float* bmin, float* bmax)
{
	const float* p = ag->npos;
	const float r = ag->params.radius;
	const float h = ag->params.height;
	bmin[0] = p[0] - r;
	bmin[1] = p[1];
	bmin[2] = p[2] - r;
	bmax[0] = p[0] + r;
	bmax[1] = p[1] + h;
	bmax[2] = p[2] + r;
}

}

namespace ely
{

CrowdToolState::CrowdToolState() :
		m_sample(0),
		m_nav(0),
		m_crowd(0),
		m_targetRef(0),
		m_run(true)
{
	m_toolParams.m_expandSelectedDebugDraw = true;
	m_toolParams.m_showCorners = false;
	m_toolParams.m_showCollisionSegments = false;
	m_toolParams.m_showPath = false;
	m_toolParams.m_showVO = false;
	m_toolParams.m_showOpt = false;
	m_toolParams.m_showNeis = false;
	m_toolParams.m_expandDebugDraw = false;
	m_toolParams.m_showLabels = false;
	m_toolParams.m_showGrid = false;
	m_toolParams.m_showNodes = false;
	m_toolParams.m_showPerfGraph = false;
	m_toolParams.m_showDetailAll = false;
	m_toolParams.m_expandOptions = true;
	m_toolParams.m_anticipateTurns = true;
	m_toolParams.m_optimizeVis = true;
	m_toolParams.m_optimizeTopo = true;
	m_toolParams.m_obstacleAvoidance = true;
	m_toolParams.m_obstacleAvoidanceType = 3;
	m_toolParams.m_separation = false;
	m_toolParams.m_separationWeight = 2.0f;

	memset(m_trails, 0, sizeof(m_trails));

	m_vod = dtAllocObstacleAvoidanceDebugData();
	m_vod->init(2048);

	memset(&m_agentDebug, 0, sizeof(m_agentDebug));
	m_agentDebug.idx = -1;
	m_agentDebug.vod = m_vod;
}

CrowdToolState::~CrowdToolState()
{
	dtFreeObstacleAvoidanceDebugData(m_vod);
}

void CrowdToolState::init(class NavMeshType* sample)
{
	if (m_sample != sample)
	{
		m_sample = sample;
//		m_oldFlags = m_sample->getNavMeshDrawFlags();
//		m_sample->setNavMeshDrawFlags(m_oldFlags & ~DU_DRAWNAVMESH_CLOSEDLIST);
	}

	dtNavMesh* nav = m_sample->getNavMesh();
	dtCrowd* crowd = m_sample->getCrowd();

	if (nav && crowd && (m_nav != nav || m_crowd != crowd))
	{
		m_nav = nav;
		m_crowd = crowd;

		crowd->init(MAX_AGENTS, m_sample->getAgentRadius(), nav);

		// Make polygons with 'disabled' flag invalid.
		crowd->getEditableFilter(0)->setExcludeFlags(NAVMESH_POLYFLAGS_DISABLED);

		// Setup local avoidance params to different qualities.
		dtObstacleAvoidanceParams params;
		// Use mostly default settings, copy from dtCrowd.
		memcpy(&params, crowd->getObstacleAvoidanceParams(0),
				sizeof(dtObstacleAvoidanceParams));

		// Low (11)
		params.velBias = 0.5f;
		params.adaptiveDivs = 5;
		params.adaptiveRings = 2;
		params.adaptiveDepth = 1;
		crowd->setObstacleAvoidanceParams(0, &params);

		// Medium (22)
		params.velBias = 0.5f;
		params.adaptiveDivs = 5;
		params.adaptiveRings = 2;
		params.adaptiveDepth = 2;
		crowd->setObstacleAvoidanceParams(1, &params);

		// Good (45)
		params.velBias = 0.5f;
		params.adaptiveDivs = 7;
		params.adaptiveRings = 2;
		params.adaptiveDepth = 3;
		crowd->setObstacleAvoidanceParams(2, &params);

		// High (66)
		params.velBias = 0.5f;
		params.adaptiveDivs = 7;
		params.adaptiveRings = 3;
		params.adaptiveDepth = 3;

		crowd->setObstacleAvoidanceParams(3, &params);
	}
}

void CrowdToolState::reset()
{
}

void CrowdToolState::handleRender(duDebugDraw& dd)
{
	//	DebugDrawGL dd;

	const float rad = m_sample->getAgentRadius();

	dtNavMesh* nav = m_sample->getNavMesh();
	dtCrowd* crowd = m_sample->getCrowd();
	if (!nav || !crowd)
		return;

	if (m_toolParams.m_showNodes && crowd->getPathQueue())
	{
		const dtNavMeshQuery* navquery = crowd->getPathQueue()->getNavQuery();
		if (navquery)
			duDebugDrawNavMeshNodes(&dd, *navquery);
	}

	dd.depthMask(false);

	// Draw paths
	if (m_toolParams.m_showPath)
	{
		for (int i = 0; i < crowd->getAgentCount(); i++)
		{
			if (m_toolParams.m_showDetailAll == false && i != m_agentDebug.idx)
				continue;
			const dtCrowdAgent* ag = crowd->getAgent(i);
			if (!ag->active)
				continue;
			const dtPolyRef* path = ag->corridor.getPath();
			const int npath = ag->corridor.getPathCount();
			for (int j = 0; j < npath; ++j)
				duDebugDrawNavMeshPoly(&dd, *nav, path[j],
						duRGBA(255, 255, 255, 24));
		}
	}

	if (m_targetRef)
		duDebugDrawCross(&dd, m_targetPos[0], m_targetPos[1] + 0.1f,
				m_targetPos[2], rad, duRGBA(255, 255, 255, 192), 2.0f);

	// Occupancy grid.
	if (m_toolParams.m_showGrid)
	{
		float gridy = -FLT_MAX;
		for (int i = 0; i < crowd->getAgentCount(); ++i)
		{
			const dtCrowdAgent* ag = crowd->getAgent(i);
			if (!ag->active)
				continue;
			const float* pos = ag->corridor.getPos();
			gridy = dtMax(gridy, pos[1]);
		}
		gridy += 1.0f;

		dd.begin(DU_DRAW_QUADS);
		const dtProximityGrid* grid = crowd->getGrid();
		const int* bounds = grid->getBounds();
		const float cs = grid->getCellSize();
		for (int y = bounds[1]; y <= bounds[3]; ++y)
		{
			for (int x = bounds[0]; x <= bounds[2]; ++x)
			{
				const int count = grid->getItemCountAt(x, y);
				if (!count)
					continue;
				unsigned int col = duRGBA(128, 0, 0, dtMin(count * 40, 255));
				dd.vertex(x * cs, gridy, y * cs, col);
				dd.vertex(x * cs, gridy, y * cs + cs, col);
				dd.vertex(x * cs + cs, gridy, y * cs + cs, col);
				dd.vertex(x * cs + cs, gridy, y * cs, col);
			}
		}
		dd.end();
	}

	// Trail
	for (int i = 0; i < crowd->getAgentCount(); ++i)
	{
		const dtCrowdAgent* ag = crowd->getAgent(i);
		if (!ag->active)
			continue;

		const AgentTrail* trail = &m_trails[i];
		const float* pos = ag->npos;

		dd.begin(DU_DRAW_LINES, 3.0f);
		float prev[3], preva = 1;
		dtVcopy(prev, pos);
		for (int j = 0; j < AGENT_MAX_TRAIL - 1; ++j)
		{
			const int idx = (trail->htrail + AGENT_MAX_TRAIL - j)
					% AGENT_MAX_TRAIL;
			const float* v = &trail->trail[idx * 3];
			float a = 1 - j / (float) AGENT_MAX_TRAIL;
			dd.vertex(prev[0], prev[1] + 0.1f, prev[2],
					duRGBA(0, 0, 0, (int) (128 * preva)));
			dd.vertex(v[0], v[1] + 0.1f, v[2],
					duRGBA(0, 0, 0, (int) (128 * a)));
			preva = a;
			dtVcopy(prev, v);
		}
		dd.end();

	}

	// Corners & co
	for (int i = 0; i < crowd->getAgentCount(); i++)
	{
		if (m_toolParams.m_showDetailAll == false && i != m_agentDebug.idx)
			continue;
		const dtCrowdAgent* ag = crowd->getAgent(i);
		if (!ag->active)
			continue;

		const float radius = ag->params.radius;
		const float* pos = ag->npos;

		if (m_toolParams.m_showCorners)
		{
			if (ag->ncorners)
			{
				dd.begin(DU_DRAW_LINES, 2.0f);
				for (int j = 0; j < ag->ncorners; ++j)
				{
					const float* va =
							j == 0 ? pos : &ag->cornerVerts[(j - 1) * 3];
					const float* vb = &ag->cornerVerts[j * 3];
					dd.vertex(va[0], va[1] + radius, va[2],
							duRGBA(128, 0, 0, 192));
					dd.vertex(vb[0], vb[1] + radius, vb[2],
							duRGBA(128, 0, 0, 192));
				}
				if (ag->ncorners
						&& ag->cornerFlags[ag->ncorners - 1]
								& DT_STRAIGHTPATH_OFFMESH_CONNECTION)
				{
					const float* v = &ag->cornerVerts[(ag->ncorners - 1) * 3];
					dd.vertex(v[0], v[1], v[2], duRGBA(192, 0, 0, 192));
					dd.vertex(v[0], v[1] + radius * 2, v[2],
							duRGBA(192, 0, 0, 192));
				}

				dd.end();

				if (m_toolParams.m_anticipateTurns)
				{
					/*					float dvel[3], pos[3];
					 calcSmoothSteerDirection(ag->pos, ag->cornerVerts, ag->ncorners, dvel);
					 pos[0] = ag->pos[0] + dvel[0];
					 pos[1] = ag->pos[1] + dvel[1];
					 pos[2] = ag->pos[2] + dvel[2];
					 
					 const float off = ag->radius+0.1f;
					 const float* tgt = &ag->cornerVerts[0];
					 const float y = ag->pos[1]+off;
					 
					 dd.begin(DU_DRAW_LINES, 2.0f);
					 
					 dd.vertex(ag->pos[0],y,ag->pos[2], duRGBA(255,0,0,192));
					 dd.vertex(pos[0],y,pos[2], duRGBA(255,0,0,192));
					 
					 dd.vertex(pos[0],y,pos[2], duRGBA(255,0,0,192));
					 dd.vertex(tgt[0],y,tgt[2], duRGBA(255,0,0,192));
					 
					 dd.end();*/
				}
			}
		}

		if (m_toolParams.m_showCollisionSegments)
		{
			const float* center = ag->boundary.getCenter();
			duDebugDrawCross(&dd, center[0], center[1] + radius, center[2],
					0.2f, duRGBA(192, 0, 128, 255), 2.0f);
			duDebugDrawCircle(&dd, center[0], center[1] + radius, center[2],
					ag->params.collisionQueryRange, duRGBA(192, 0, 128, 128),
					2.0f);

			dd.begin(DU_DRAW_LINES, 3.0f);
			for (int j = 0; j < ag->boundary.getSegmentCount(); ++j)
			{
				const float* s = ag->boundary.getSegment(j);
				unsigned int col = duRGBA(192, 0, 128, 192);
				if (dtTriArea2D(pos, s, s + 3) < 0.0f)
					col = duDarkenCol(col);

				duAppendArrow(&dd, s[0], s[1] + 0.2f, s[2], s[3], s[4] + 0.2f,
						s[5], 0.0f, 0.3f, col);
			}
			dd.end();
		}

		if (m_toolParams.m_showNeis)
		{
			duDebugDrawCircle(&dd, pos[0], pos[1] + radius, pos[2],
					ag->params.collisionQueryRange, duRGBA(0, 192, 128, 128),
					2.0f);

			dd.begin(DU_DRAW_LINES, 2.0f);
			for (int j = 0; j < ag->nneis; ++j)
			{
				// Get 'n'th active agent.
				// TODO: fix this properly.
				const dtCrowdAgent* nei = crowd->getAgent(ag->neis[j].idx);
				if (nei)
				{
					dd.vertex(pos[0], pos[1] + radius, pos[2],
							duRGBA(0, 192, 128, 128));
					dd.vertex(nei->npos[0], nei->npos[1] + radius, nei->npos[2],
							duRGBA(0, 192, 128, 128));
				}
			}
			dd.end();
		}

		if (m_toolParams.m_showOpt)
		{
			dd.begin(DU_DRAW_LINES, 2.0f);
			dd.vertex(m_agentDebug.optStart[0], m_agentDebug.optStart[1] + 0.3f,
					m_agentDebug.optStart[2], duRGBA(0, 128, 0, 192));
			dd.vertex(m_agentDebug.optEnd[0], m_agentDebug.optEnd[1] + 0.3f,
					m_agentDebug.optEnd[2], duRGBA(0, 128, 0, 192));
			dd.end();
		}
	}

	// Agent cylinders.
	for (int i = 0; i < crowd->getAgentCount(); ++i)
	{
		const dtCrowdAgent* ag = crowd->getAgent(i);
		if (!ag->active)
			continue;

		const float radius = ag->params.radius;
		const float* pos = ag->npos;

		unsigned int col = duRGBA(0, 0, 0, 32);
		if (m_agentDebug.idx == i)
			col = duRGBA(255, 0, 0, 128);

		duDebugDrawCircle(&dd, pos[0], pos[1], pos[2], radius, col, 2.0f);
	}

	for (int i = 0; i < crowd->getAgentCount(); ++i)
	{
		const dtCrowdAgent* ag = crowd->getAgent(i);
		if (!ag->active)
			continue;

		const float height = ag->params.height;
		const float radius = ag->params.radius;
		const float* pos = ag->npos;

		unsigned int col = duRGBA(220, 220, 220, 128);
		if (ag->targetState == DT_CROWDAGENT_TARGET_REQUESTING
				|| ag->targetState == DT_CROWDAGENT_TARGET_WAITING_FOR_QUEUE)
			col = duLerpCol(col, duRGBA(128, 0, 255, 128), 32);
		else if (ag->targetState == DT_CROWDAGENT_TARGET_WAITING_FOR_PATH)
			col = duLerpCol(col, duRGBA(128, 0, 255, 128), 128);
		else if (ag->targetState == DT_CROWDAGENT_TARGET_FAILED)
			col = duRGBA(255, 32, 16, 128);
		else if (ag->targetState == DT_CROWDAGENT_TARGET_VELOCITY)
			col = duLerpCol(col, duRGBA(64, 255, 0, 128), 128);

		duDebugDrawCylinder(&dd, pos[0] - radius, pos[1] + radius * 0.1f,
				pos[2] - radius, pos[0] + radius, pos[1] + height,
				pos[2] + radius, col);
	}

	if (m_toolParams.m_showVO)
	{
		for (int i = 0; i < crowd->getAgentCount(); i++)
		{
			if (m_toolParams.m_showDetailAll == false && i != m_agentDebug.idx)
				continue;
			const dtCrowdAgent* ag = crowd->getAgent(i);
			if (!ag->active)
				continue;

			// Draw detail about agent sela
			const dtObstacleAvoidanceDebugData* vod = m_agentDebug.vod;

			const float dx = ag->npos[0];
			const float dy = ag->npos[1] + ag->params.height;
			const float dz = ag->npos[2];

			duDebugDrawCircle(&dd, dx, dy, dz, ag->params.maxSpeed,
					duRGBA(255, 255, 255, 64), 2.0f);

			dd.begin(DU_DRAW_QUADS);
			for (int j = 0; j < vod->getSampleCount(); ++j)
			{
				const float* p = vod->getSampleVelocity(j);
				const float sr = vod->getSampleSize(j);
				const float pen = vod->getSamplePenalty(j);
				const float pen2 = vod->getSamplePreferredSidePenalty(j);
				unsigned int col = duLerpCol(duRGBA(255, 255, 255, 220),
						duRGBA(128, 96, 0, 220), (int) (pen * 255));
				col = duLerpCol(col, duRGBA(128, 0, 0, 220),
						(int) (pen2 * 128));
				dd.vertex(dx + p[0] - sr, dy, dz + p[2] - sr, col);
				dd.vertex(dx + p[0] - sr, dy, dz + p[2] + sr, col);
				dd.vertex(dx + p[0] + sr, dy, dz + p[2] + sr, col);
				dd.vertex(dx + p[0] + sr, dy, dz + p[2] - sr, col);
			}
			dd.end();
		}
	}

	// Velocity stuff.
	for (int i = 0; i < crowd->getAgentCount(); ++i)
	{
		const dtCrowdAgent* ag = crowd->getAgent(i);
		if (!ag->active)
			continue;

		const float radius = ag->params.radius;
		const float height = ag->params.height;
		const float* pos = ag->npos;
		const float* vel = ag->vel;
		const float* dvel = ag->dvel;

		unsigned int col = duRGBA(220, 220, 220, 192);
		if (ag->targetState == DT_CROWDAGENT_TARGET_REQUESTING
				|| ag->targetState == DT_CROWDAGENT_TARGET_WAITING_FOR_QUEUE)
			col = duLerpCol(col, duRGBA(128, 0, 255, 192), 32);
		else if (ag->targetState == DT_CROWDAGENT_TARGET_WAITING_FOR_PATH)
			col = duLerpCol(col, duRGBA(128, 0, 255, 192), 128);
		else if (ag->targetState == DT_CROWDAGENT_TARGET_FAILED)
			col = duRGBA(255, 32, 16, 192);
		else if (ag->targetState == DT_CROWDAGENT_TARGET_VELOCITY)
			col = duLerpCol(col, duRGBA(64, 255, 0, 192), 128);

		duDebugDrawCircle(&dd, pos[0], pos[1] + height, pos[2], radius, col,
				2.0f);

		duDebugDrawArrow(&dd, pos[0], pos[1] + height, pos[2], pos[0] + dvel[0],
				pos[1] + height + dvel[1], pos[2] + dvel[2], 0.0f, 0.4f,
				duRGBA(0, 192, 255, 192),
				(m_agentDebug.idx == i) ? 2.0f : 1.0f);

		duDebugDrawArrow(&dd, pos[0], pos[1] + height, pos[2], pos[0] + vel[0],
				pos[1] + height + vel[1], pos[2] + vel[2], 0.0f, 0.4f,
				duRGBA(0, 0, 0, 160), 2.0f);
	}

	dd.depthMask(true);
}

void CrowdToolState::handleUpdate(const float dt)
{
	if (m_run)
		updateTick(dt);
}

int CrowdToolState::addAgent(const float* p, const dtCrowdAgentParams* params)
{
	if (!m_sample)
		return -1;
	dtCrowd* crowd = m_sample->getCrowd();

	int idx = crowd->addAgent(p, params);
	if (idx != -1)
	{
		if (m_targetRef)
			crowd->requestMoveTarget(idx, m_targetRef, m_targetPos);

#ifdef ELY_DEBUG
		// Init trail
		AgentTrail* trail = &m_trails[idx];
		for (int i = 0; i < AGENT_MAX_TRAIL; ++i)
			dtVcopy(&trail->trail[i * 3], p);
		trail->htrail = 0;
#endif
	}
	return idx;
}

int CrowdToolState::addAgent(const float* p)
{
	if (!m_sample)
		return -1;
	dtCrowd* crowd = m_sample->getCrowd();

	dtCrowdAgentParams ap;
	memset(&ap, 0, sizeof(ap));
	ap.radius = m_sample->getAgentRadius();
	ap.height = m_sample->getAgentHeight();
	ap.maxAcceleration = 8.0f;
	ap.maxSpeed = 3.5f;
	ap.collisionQueryRange = ap.radius * 12.0f;
	ap.pathOptimizationRange = ap.radius * 30.0f;
	ap.updateFlags = 0;
	if (m_toolParams.m_anticipateTurns)
		ap.updateFlags |= DT_CROWD_ANTICIPATE_TURNS;
	if (m_toolParams.m_optimizeVis)
		ap.updateFlags |= DT_CROWD_OPTIMIZE_VIS;
	if (m_toolParams.m_optimizeTopo)
		ap.updateFlags |= DT_CROWD_OPTIMIZE_TOPO;
	if (m_toolParams.m_obstacleAvoidance)
		ap.updateFlags |= DT_CROWD_OBSTACLE_AVOIDANCE;
	if (m_toolParams.m_separation)
		ap.updateFlags |= DT_CROWD_SEPARATION;
	ap.obstacleAvoidanceType =
			(unsigned char) m_toolParams.m_obstacleAvoidanceType;
	ap.separationWeight = m_toolParams.m_separationWeight;

	int idx = crowd->addAgent(p, &ap);
	if (idx != -1)
	{
		if (m_targetRef)
			crowd->requestMoveTarget(idx, m_targetRef, m_targetPos);
#ifdef ELY_DEBUG
		// Init trail
		AgentTrail* trail = &m_trails[idx];
		for (int i = 0; i < AGENT_MAX_TRAIL; ++i)
			dtVcopy(&trail->trail[i * 3], p);
		trail->htrail = 0;
#endif
	}
	return idx;
}

void CrowdToolState::removeAgent(const int idx)
{
	if (!m_sample)
		return;
	dtCrowd* crowd = m_sample->getCrowd();

	crowd->removeAgent(idx);

	if (idx == m_agentDebug.idx)
		m_agentDebug.idx = -1;
}

void CrowdToolState::hilightAgent(const int idx)
{
	m_agentDebug.idx = idx;
}

} // ely

namespace
{

void calcVel(float* vel, const float* pos, const float* tgt,
		const float speed)
{
	dtVsub(vel, tgt, pos);
	vel[1] = 0.0;
	dtVnormalize(vel);
	dtVscale(vel, vel, speed);
}

}

namespace ely
{

void CrowdToolState::setMoveTarget(int idx, const float* p)
{
	if (!m_sample)
		return;

	// Find nearest point on navmesh and set move request to that location.
	dtNavMeshQuery* navquery = m_sample->getNavMeshQuery();
	dtCrowd* crowd = m_sample->getCrowd();
	const dtQueryFilter* filter = crowd->getFilter(0);
	const float* ext = crowd->getQueryExtents();

	navquery->findNearestPoly(p, ext, filter, &m_targetRef, m_targetPos);

	const dtCrowdAgent* ag = crowd->getAgent(idx);
	if (ag && ag->active)
		crowd->requestMoveTarget(idx, m_targetRef, m_targetPos);
}

void CrowdToolState::setMoveVelocity(int idx, const float* v)
{
	if (!m_sample)
		return;

	dtCrowd* crowd = m_sample->getCrowd();

	// Request velocity
	const dtCrowdAgent* ag = crowd->getAgent(idx);
	if (ag && ag->active)
	{
		crowd->requestMoveVelocity(idx, v);
	}
}

void CrowdToolState::setMoveTarget(const float* p, bool adjust)
{
	if (!m_sample)
		return;

	// Find nearest point on navmesh and set move request to that location.
	dtNavMeshQuery* navquery = m_sample->getNavMeshQuery();
	dtCrowd* crowd = m_sample->getCrowd();
	const dtQueryFilter* filter = crowd->getFilter(0);
	const float* ext = crowd->getQueryExtents();

	if (adjust)
	{
		float vel[3];
		// Request velocity

#ifdef ELY_DEBUG
		if (m_agentDebug.idx != -1)
		{
			const dtCrowdAgent* ag = crowd->getAgent(m_agentDebug.idx);
			if (ag && ag->active)
			{
				calcVel(vel, ag->npos, p, ag->params.maxSpeed);
				crowd->requestMoveVelocity(m_agentDebug.idx, vel);
			}
		}
		else
		{
#endif
			for (int i = 0; i < crowd->getAgentCount(); ++i)
			{
				const dtCrowdAgent* ag = crowd->getAgent(i);
				if (!ag->active)
					continue;
				calcVel(vel, ag->npos, p, ag->params.maxSpeed);
				crowd->requestMoveVelocity(i, vel);
			}
#ifdef ELY_DEBUG
		}
#endif
	}
	else
	{
		navquery->findNearestPoly(p, ext, filter, &m_targetRef, m_targetPos);

#ifdef ELY_DEBUG
		if (m_agentDebug.idx != -1)
		{
			const dtCrowdAgent* ag = crowd->getAgent(m_agentDebug.idx);
			if (ag && ag->active)
				crowd->requestMoveTarget(m_agentDebug.idx, m_targetRef,
						m_targetPos);
		}
		else
		{
#endif
			for (int i = 0; i < crowd->getAgentCount(); ++i)
			{
				const dtCrowdAgent* ag = crowd->getAgent(i);
				if (!ag->active)
					continue;
				crowd->requestMoveTarget(i, m_targetRef, m_targetPos);
			}
#ifdef ELY_DEBUG
		}
#endif
	}
}

int CrowdToolState::hitTestAgents(const float* s, const float* p)
{
	if (!m_sample)
		return -1;
	dtCrowd* crowd = m_sample->getCrowd();

	int isel = -1;
	float tsel = FLT_MAX;

	for (int i = 0; i < crowd->getAgentCount(); ++i)
	{
		const dtCrowdAgent* ag = crowd->getAgent(i);
		if (!ag->active)
			continue;
		float bmin[3], bmax[3];
		getAgentBounds(ag, bmin, bmax);
		float tmin, tmax;
		if (isectSegAABB(s, p, bmin, bmax, tmin, tmax))
		{
			if (tmin > 0 && tmin < tsel)
			{
				isel = i;
				tsel = tmin;
			}
		}
	}

	return isel;
}

void CrowdToolState::updateAgentParams()
{
	if (!m_sample)
		return;
	dtCrowd* crowd = m_sample->getCrowd();
	if (!crowd)
		return;

	unsigned char updateFlags = 0;
	unsigned char obstacleAvoidanceType = 0;

	if (m_toolParams.m_anticipateTurns)
		updateFlags |= DT_CROWD_ANTICIPATE_TURNS;
	if (m_toolParams.m_optimizeVis)
		updateFlags |= DT_CROWD_OPTIMIZE_VIS;
	if (m_toolParams.m_optimizeTopo)
		updateFlags |= DT_CROWD_OPTIMIZE_TOPO;
	if (m_toolParams.m_obstacleAvoidance)
		updateFlags |= DT_CROWD_OBSTACLE_AVOIDANCE;
	if (m_toolParams.m_obstacleAvoidance)
		updateFlags |= DT_CROWD_OBSTACLE_AVOIDANCE;
	if (m_toolParams.m_separation)
		updateFlags |= DT_CROWD_SEPARATION;

	obstacleAvoidanceType =
			(unsigned char) m_toolParams.m_obstacleAvoidanceType;

	dtCrowdAgentParams params;

	for (int i = 0; i < crowd->getAgentCount(); ++i)
	{
		const dtCrowdAgent* ag = crowd->getAgent(i);
		if (!ag->active)
			continue;
		memcpy(&params, &ag->params, sizeof(dtCrowdAgentParams));
		params.updateFlags = updateFlags;
		params.obstacleAvoidanceType = obstacleAvoidanceType;
		params.separationWeight = m_toolParams.m_separationWeight;
		crowd->updateAgentParameters(i, &params);
	}
}

void CrowdToolState::updateTick(const float dt)
{
	if (!m_sample)
		return;
	dtNavMesh* nav = m_sample->getNavMesh();
	dtCrowd* crowd = m_sample->getCrowd();
	if (!nav || !crowd)
		return;

#ifdef ELY_DEBUG

//	rnTimeVal startTime = getPerfTime();

	crowd->update(dt, &m_agentDebug);

//	rnTimeVal endTime = getPerfTime();

	// Update agent trails
	for (int i = 0; i < crowd->getAgentCount(); ++i)
	{
		const dtCrowdAgent* ag = crowd->getAgent(i);
		AgentTrail* trail = &m_trails[i];
		if (!ag->active)
			continue;
		// Update agent movement trail.
		trail->htrail = (trail->htrail + 1) % AGENT_MAX_TRAIL;
		dtVcopy(&trail->trail[trail->htrail * 3], ag->npos);
	}

	m_agentDebug.vod->normalizeSamples();

//	m_crowdSampleCount.addSample((float) crowd->getVelocitySampleCount());
//	m_crowdTotalTime.addSample(
//			getPerfDeltaTimeUsec(startTime, endTime) / 1000.0f);
#else
	crowd->update(dt, NULL);
#endif
}

CrowdTool::CrowdTool() :
	m_sample(0), m_state(0), m_mode(TOOLMODE_CREATE)
{
}

CrowdTool::~CrowdTool()
{
}

void CrowdTool::init(NavMeshType* sample)
{
	if (m_sample != sample)
	{
		m_sample = sample;
	}

	if (!sample)
		return;

	m_state = (CrowdToolState*) sample->getToolState(type());
	if (!m_state)
	{
		m_state = new CrowdToolState();
		sample->setToolState(type(), m_state);
	}
	m_state->init(sample);
}

void CrowdTool::reset()
{
}

void CrowdTool::handleClick(const float* s, const float* p, bool shift)
{
	if (!m_sample)
		return;
	if (!m_state)
		return;
	InputGeom* geom = m_sample->getInputGeom();
	if (!geom)
		return;
	dtCrowd* crowd = m_sample->getCrowd();
	if (!crowd)
		return;

	if (m_mode == TOOLMODE_CREATE)
	{
		if (shift)
		{
			// Delete
			int ahit = m_state->hitTestAgents(s, p);
			if (ahit != -1)
				m_state->removeAgent(ahit);
		}
		else
		{
			// Add
			m_state->addAgent(p);
		}
	}
	else if (m_mode == TOOLMODE_MOVE_TARGET)
	{
		m_state->setMoveTarget(p, shift);
	}
	else if (m_mode == TOOLMODE_SELECT)
	{
		// Highlight
		int ahit = m_state->hitTestAgents(s, p);
		m_state->hilightAgent(ahit);
	}
	else if (m_mode == TOOLMODE_TOGGLE_POLYS)
	{
		dtNavMesh* nav = m_sample->getNavMesh();
		dtNavMeshQuery* navquery = m_sample->getNavMeshQuery();
		if (nav && navquery)
		{
			dtQueryFilter filter;
			const float* ext = crowd->getQueryExtents();
			float tgt[3];
			dtPolyRef ref;
			navquery->findNearestPoly(p, ext, &filter, &ref, tgt);
			if (ref)
			{
				unsigned short flags = 0;
				if (dtStatusSucceed(nav->getPolyFlags(ref, &flags)))
				{
					flags ^= NAVMESH_POLYFLAGS_DISABLED;
					nav->setPolyFlags(ref, flags);
				}
			}
		}
	}

}

void CrowdTool::handleStep()
{
	if (!m_state)
		return;

	const float dt = 1.0f / 20.0f;
	m_state->updateTick(dt);

	m_state->setRunning(false);
}

void CrowdTool::handleToggle()
{
	if (!m_state)
		return;
	m_state->setRunning(!m_state->isRunning());
}

void CrowdTool::handleUpdate(const float dt)
{
	rcIgnoreUnused(dt);
}

void CrowdTool::handleRender(duDebugDraw& dd)
{
}

} // namespace ely