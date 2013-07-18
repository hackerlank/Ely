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
#include "AIComponents/RecastNavigation/NavMeshType.h"
#include "AIComponents/RecastNavigation/InputGeom.h"
#include <RecastDebugDraw.h>

#ifdef WIN32
#	define snprintf _snprintf
#endif

namespace ely
{

NavMeshType::NavMeshType(NodePath renderDebug, NodePath camera) :
		m_geom(0), m_navMesh(0), m_navQuery(0), m_crowd(0),
		m_navMeshDrawFlags(
				0
//				| DU_DRAWNAVMESH_OFFMESHCONS
//				| DU_DRAWNAVMESH_CLOSEDLIST
//				| DU_DRAWNAVMESH_COLOR_TILES
				),
		m_tool(0), m_ctx(0), dd(renderDebug, camera)
{
	resetNavMeshSettings();
	m_navQuery = dtAllocNavMeshQuery();
	m_crowd = dtAllocCrowd();

	for (int i = 0; i < MAX_TOOLS; i++)
		m_toolStates[i] = 0;
}

NavMeshType::~NavMeshType()
{
	dtFreeNavMeshQuery(m_navQuery);
	dtFreeNavMesh(m_navMesh);
	dtFreeCrowd(m_crowd);
	delete m_tool;
	for (int i = 0; i < MAX_TOOLS; i++)
		delete m_toolStates[i];
}

void NavMeshType::setTool(NavMeshTypeTool* tool)
{
	delete m_tool;
	m_tool = tool;
	if (tool)
		m_tool->init(this);
}

void NavMeshType::handleRender()
{
	if (!m_geom)
		return;

	// Draw mesh
	duDebugDrawTriMesh(&dd, m_geom->getMesh()->getVerts(),
			m_geom->getMesh()->getVertCount(), m_geom->getMesh()->getTris(),
			m_geom->getMesh()->getNormals(), m_geom->getMesh()->getTriCount(),
			0, 1.0f);
	// Draw bounds
	const float* bmin = m_geom->getMeshBoundsMin();
	const float* bmax = m_geom->getMeshBoundsMax();
	duDebugDrawBoxWire(&dd, bmin[0], bmin[1], bmin[2], bmax[0], bmax[1],
			bmax[2], duRGBA(255, 255, 255, 128), 1.0f);
}

void NavMeshType::handleMeshChanged(InputGeom* geom)
{
	m_geom = geom;
}

const float* NavMeshType::getBoundsMin()
{
	if (!m_geom)
		return 0;
	return m_geom->getMeshBoundsMin();
}

const float* NavMeshType::getBoundsMax()
{
	if (!m_geom)
		return 0;
	return m_geom->getMeshBoundsMax();
}

void NavMeshType::resetNavMeshSettings()
{
	m_cellSize = 0.3f;
	m_cellHeight = 0.2f;
	m_agentHeight = 2.0f;
	m_agentRadius = 0.6f;
	m_agentMaxClimb = 0.9f;
	m_agentMaxSlope = 45.0f;
	m_regionMinSize = 8;
	m_regionMergeSize = 20;
	m_monotonePartitioning = false;
	m_edgeMaxLen = 12.0f;
	m_edgeMaxError = 1.3f;
	m_vertsPerPoly = 6.0f;
	m_detailSampleDist = 6.0f;
	m_detailSampleMaxError = 1.0f;
}

void NavMeshType::handleClick(const float* s, const float* p, bool shift)
{
	if (m_tool)
		m_tool->handleClick(s, p, shift);
}

void NavMeshType::handleToggle()
{
	if (m_tool)
		m_tool->handleToggle();
}

bool NavMeshType::handleBuild()
{
	return true;
}

void NavMeshType::handleUpdate(const float dt)
{
	if (m_tool)
		m_tool->handleUpdate(dt);
	updateToolStates(dt);
}

void NavMeshType::updateToolStates(const float dt)
{
	for (int i = 0; i < MAX_TOOLS; i++)
	{
		if (m_toolStates[i])
			m_toolStates[i]->handleUpdate(dt);
	}
}

void NavMeshType::setNavMeshSettings(const NavMeshSettings& settings)
{
	m_cellSize = settings.m_cellSize;
	m_cellHeight = settings.m_cellHeight;
	m_agentHeight = settings.m_agentHeight;
	m_agentRadius = settings.m_agentRadius;
	m_agentMaxClimb = settings.m_agentMaxClimb;
	m_agentMaxSlope = settings.m_agentMaxSlope;
	m_regionMinSize = settings.m_regionMinSize;
	m_regionMergeSize = settings.m_regionMergeSize;
	m_monotonePartitioning = settings.m_monotonePartitioning;
	m_edgeMaxLen = settings.m_edgeMaxLen;
	m_edgeMaxError = settings.m_edgeMaxError;
	m_vertsPerPoly = settings.m_vertsPerPoly;
	m_detailSampleDist = settings.m_detailSampleDist;
	m_detailSampleMaxError = settings.m_detailSampleMaxError;
}

NavMeshSettings NavMeshType::getNavMeshSettings()
{
	NavMeshSettings settings;
	settings.m_cellSize = m_cellSize;
	settings.m_cellHeight = m_cellHeight;
	settings.m_agentHeight = m_agentHeight;
	settings.m_agentRadius = m_agentRadius;
	settings.m_agentMaxClimb = m_agentMaxClimb;
	settings.m_agentMaxSlope = m_agentMaxSlope;
	settings.m_regionMinSize = m_regionMinSize;
	settings.m_regionMergeSize = m_regionMergeSize;
	settings.m_monotonePartitioning = m_monotonePartitioning;
	settings.m_edgeMaxLen = m_edgeMaxLen;
	settings.m_edgeMaxError = m_edgeMaxError;
	settings.m_vertsPerPoly = m_vertsPerPoly;
	settings.m_detailSampleDist = m_detailSampleDist;
	settings.m_detailSampleMaxError = m_detailSampleMaxError;
	return settings;
}

} // namespace ely
