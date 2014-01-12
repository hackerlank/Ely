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
 * \file /Ely/src/AIComponents/RecastNavigation/DebugInterfaces.cpp
 *
 * \date 02/lug/2013 09:17:03
 * \author marco
 */

#define _USE_MATH_DEFINES
#include <cmath>
#include <cstdio>
#include <cstdarg>
#include <cstring>
#include <cfloat>
#include "AIComponents/RecastNavigation/DebugInterfaces.h"
#include <RecastDebugDraw.h>
#include <DetourDebugDraw.h>
#include <geomNode.h>
#include <geomPoints.h>
#include <geomLines.h>
#include <geomTriangles.h>
#include <omniBoundingVolume.h>

#ifdef WIN32
#	define snprintf _snprintf
#endif

namespace ely
{

////////////////////////////////////////////////////////////////////////////////////////////////////

BuildContext::BuildContext() :
	m_messageCount(0),
	m_textPoolSize(0)
{
	resetTimers();
}

BuildContext::~BuildContext()
{
}

// Virtual functions for custom implementations.
void BuildContext::doResetLog()
{
	m_messageCount = 0;
	m_textPoolSize = 0;
}

void BuildContext::doLog(const rcLogCategory category, const char* msg, const int len)
{
	if (!len) return;
	if (m_messageCount >= MAX_MESSAGES)
		return;
	char* dst = &m_textPool[m_textPoolSize];
	int n = TEXT_POOL_SIZE - m_textPoolSize;
	if (n < 2)
		return;
	char* cat = dst;
	char* text = dst+1;
	const int maxtext = n-1;
	// Store category
	*cat = (char)category;
	// Store message
	const int count = rcMin(len+1, maxtext);
	memcpy(text, msg, count);
	text[count-1] = '\0';
	m_textPoolSize += 1 + count;
	m_messages[m_messageCount++] = dst;
}

void BuildContext::doResetTimers()
{
	for (int i = 0; i < RC_MAX_TIMERS; ++i)
		m_accTime[i] = -1;
}

void BuildContext::doStartTimer(const rcTimerLabel label)
{
	m_startTime[label] = getPerfTime();
}

void BuildContext::doStopTimer(const rcTimerLabel label)
{
	const rnTimeVal endTime = getPerfTime();
	const int deltaTime = (int)(endTime - m_startTime[label]);
	if (m_accTime[label] == -1)
		m_accTime[label] = deltaTime;
	else
		m_accTime[label] += deltaTime;
}

int BuildContext::doGetAccumulatedTime(const rcTimerLabel label) const
{
	return m_accTime[label];
}

void BuildContext::dumpLog(const char* format, ...)
{
	// Print header.
	va_list ap;
	va_start(ap, format);
	vprintf(format, ap);
	va_end(ap);
	printf("\n");

	// Print messages
	const int TAB_STOPS[4] = { 28, 36, 44, 52 };
	for (int i = 0; i < m_messageCount; ++i)
	{
		const char* msg = m_messages[i]+1;
		int n = 0;
		while (*msg)
		{
			if (*msg == '\t')
			{
				int count = 1;
				for (int j = 0; j < 4; ++j)
				{
					if (n < TAB_STOPS[j])
					{
						count = TAB_STOPS[j] - n;
						break;
					}
				}
				while (--count)
				{
					putchar(' ');
					n++;
				}
			}
			else
			{
				putchar(*msg);
				n++;
			}
			msg++;
		}
		putchar('\n');
	}
}

int BuildContext::getLogCount() const
{
	return m_messageCount;
}

const char* BuildContext::getLogText(const int i) const
{
	return m_messages[i]+1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

DebugDrawPanda3d::DebugDrawPanda3d(NodePath render) :
		m_render(render),
		m_depthMask(true),
		m_texture(true),
		m_vertexIdx(0),
		m_prim(DU_DRAW_TRIS)
{
}

DebugDrawPanda3d::~DebugDrawPanda3d()
{
}

void DebugDrawPanda3d::depthMask(bool state)
{
	m_depthMask = state;
}

void DebugDrawPanda3d::texture(bool state)
{
	m_texture = state;
}

void DebugDrawPanda3d::begin(duDebugDrawPrimitives prim, float size)
{
	m_vertexData = new GeomVertexData("VertexData", GeomVertexFormat::get_v3c4t2(),
					Geom::UH_static);
	m_vertex = GeomVertexWriter(m_vertexData, "vertex");
	m_color = GeomVertexWriter(m_vertexData, "color");
	m_texcoord = GeomVertexWriter(m_vertexData, "texcoord");
	switch (prim)
	{
	case DU_DRAW_POINTS:
		m_geomPrim = new GeomPoints(Geom::UH_static);
		break;
	case DU_DRAW_LINES:
		m_geomPrim = new GeomLines(Geom::UH_static);
		break;
	case DU_DRAW_TRIS:
		m_geomPrim = new GeomTriangles(Geom::UH_static);
		break;
	case DU_DRAW_QUADS:
		m_geomPrim = new GeomTriangles(Geom::UH_static);
		m_quadCurrIdx = 0;
		break;
	};
	m_prim = prim;
	m_size = size;
	m_vertexIdx = 0;
}

void DebugDrawPanda3d::doVertex(const LVector3f& vertex, const LVector4f& color,
		const LVector2f& uv)
{
	if (m_prim != DU_DRAW_QUADS)
	{
		m_vertex.add_data3f(vertex);
		m_color.add_data4f(color);
		m_texcoord.add_data2f(uv);
		//
		m_geomPrim->add_vertex(m_vertexIdx);
		++m_vertexIdx;
	}
	else
	{
		int quadCurrIdxMod = m_quadCurrIdx % 4;
		LVector3f currVertex = vertex;
		LVector4f currColor = color;
		LVector2f currUV = uv;
		switch (quadCurrIdxMod)
		{
		case 0:
			m_quadFirstVertex = currVertex;
			m_quadFirstColor = currColor;
			m_quadFirstUV = currUV;
			++m_quadCurrIdx;
			break;
		case 2:
			m_quadThirdVertex = currVertex;
			m_quadThirdColor = currColor;
			m_quadThirdUV = currUV;
			++m_quadCurrIdx;
			break;
		case 3:
			//first
			m_vertex.add_data3f(m_quadFirstVertex);
			m_color.add_data4f(m_quadFirstColor);
			m_texcoord.add_data2f(m_quadFirstUV);
			//
			m_geomPrim->add_vertex(m_vertexIdx);
			++m_vertexIdx;
			//last
			m_vertex.add_data3f(m_quadThirdVertex);
			m_color.add_data4f(m_quadThirdColor);
			m_texcoord.add_data2f(m_quadThirdUV);
			//
			m_geomPrim->add_vertex(m_vertexIdx);
			++m_vertexIdx;
			m_quadCurrIdx = 0;
			break;
		case 1:
			++m_quadCurrIdx;
			break;
		default:
			break;
		};
		//current vertex
		///
		m_vertex.add_data3f(currVertex);
		m_color.add_data4f(currColor);
		m_texcoord.add_data2f(currUV);
		//
		m_geomPrim->add_vertex(m_vertexIdx);
		++m_vertexIdx;
		///
	}
}

void DebugDrawPanda3d::vertex(const float* pos, unsigned int color)
{
	doVertex(Recast3fToLVecBase3f(pos[0], pos[1], pos[2]),
			LVector4f(red(color), green(color), blue(color), alpha(color)));
}

void DebugDrawPanda3d::vertex(const float x, const float y, const float z, unsigned int color)
{
	doVertex(Recast3fToLVecBase3f(x, y, z),
			LVector4f(red(color), green(color), blue(color), alpha(color)));
}

void DebugDrawPanda3d::vertex(const float* pos, unsigned int color, const float* uv)
{
	doVertex(Recast3fToLVecBase3f(pos[0], pos[1], pos[2]),
			LVector4f(red(color), green(color), blue(color), alpha(color)),
			LVector2f(uv[0], uv[1]));
}

void DebugDrawPanda3d::vertex(const float x, const float y, const float z, unsigned int color, const float u, const float v)
{
	doVertex(Recast3fToLVecBase3f(x, y, z),
			LVector4f(red(color), green(color), blue(color), alpha(color)),
			LVector2f(u, v));
}

void DebugDrawPanda3d::end()
{
	m_geomPrim->close_primitive();
	m_geom = new Geom(m_vertexData);
	m_geom->add_primitive(m_geomPrim);
	m_geomNodeNP = NodePath(new GeomNode("DebugDrawPanda3d_GeomNode_" +
			dynamic_cast<std::ostringstream&>(std::ostringstream().operator <<(m_geomNodeNPCollection.size())).str()));
	DCAST(GeomNode, m_geomNodeNP.node())->add_geom(m_geom);
	m_geomNodeNP.reparent_to(m_render);
	m_geomNodeNP.set_depth_write(m_depthMask);
	m_geomNodeNP.set_transparency(TransparencyAttrib::M_alpha);
	m_geomNodeNP.set_render_mode_thickness(m_size);
	//add to geom node paths.
	m_geomNodeNPCollection.push_back(m_geomNodeNP);
}


NodePath DebugDrawPanda3d::getGeomNode(int i)
{
	return m_geomNodeNPCollection[i];
}

int DebugDrawPanda3d::getGeomNodesNum()
{
	return m_geomNodeNPCollection.size();
}

void DebugDrawPanda3d::reset()
{
	std::vector<NodePath>::iterator iter;
	for (iter = m_geomNodeNPCollection.begin();
			iter != m_geomNodeNPCollection.end(); ++iter)
	{
		(*iter).remove_node();
	}
	m_geomNodeNPCollection.clear();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

DebugDrawMeshDrawer::DebugDrawMeshDrawer(NodePath render, NodePath camera,
		int budget, bool singleMesh) :
		m_render(render), m_camera(camera),
		m_meshDrawersSize(0),
		m_budget(budget), m_singleMesh(singleMesh)
{
	reset();
}

DebugDrawMeshDrawer::~DebugDrawMeshDrawer()
{
	for (unsigned int i = 0; i < m_generators.size(); ++i)
	{
		delete m_generators[i];
	}
}

void DebugDrawMeshDrawer::reset()
{
	//reset current MeshDrawer index
	m_meshDrawerIdx = 0;
	m_prim = static_cast<duDebugDrawPrimitives>(DU_NULL_PRIM);
}

void DebugDrawMeshDrawer::begin(duDebugDrawPrimitives prim, float size)
{
	//dynamically allocate MeshDrawers if necessary
	if (m_meshDrawerIdx >= m_meshDrawersSize)
	{
		//allocate a new MeshDrawer
		m_generators.push_back(new MeshDrawer());
		//common MeshDrawers setup
		m_generators.back()->set_budget(m_budget);
		m_generators.back()->get_root().set_transparency(
				TransparencyAttrib::M_alpha);
		m_generators.back()->get_root().node()->set_bounds(
				new OmniBoundingVolume());
		m_generators.back()->get_root().node()->set_final(true);
		m_generators.back()->get_root().reparent_to(m_render);
		//update number of MeshDrawers
		m_meshDrawersSize = m_generators.size();
	}
	//setup current MeshDrawer
	m_generators[m_meshDrawerIdx]->get_root().set_depth_write(m_depthMask);
//	m_generator[m_meshDrawerIdx]->get_root().set_render_mode_thickness(size);
	//begin current MeshDrawer
	m_generators[m_meshDrawerIdx]->begin(m_camera, m_render);
	m_prim = prim;
	m_size = size / 50;
	m_lineIdx = m_triIdx = m_quadIdx = 0;
}

void DebugDrawMeshDrawer::end()
{
	//end current MeshDrawer
	m_generators[m_meshDrawerIdx]->end();
	//increase MeshDrawer index only if multiple mesh
	if (not m_singleMesh)
	{
		++m_meshDrawerIdx;
	}
	m_prim = static_cast<duDebugDrawPrimitives>(DU_NULL_PRIM);
}

void DebugDrawMeshDrawer::doVertex(const LVector3f& vertex,
		const LVector4f& color, const LVector2f& uv)
{
	switch (m_prim)
	{
	case DU_DRAW_POINTS:
		m_generators[m_meshDrawerIdx]->billboard(vertex,
				LVector4f(uv.get_x(), uv.get_y(), uv.get_x(), uv.get_y()),
				m_size, color);
		break;
	case DU_DRAW_LINES:
		if ((m_lineIdx % 2) == 1)
		{
			m_generators[m_meshDrawerIdx]->segment(m_lineVertex, vertex,
					LVector4f(m_lineUV.get_x(), m_lineUV.get_y(), uv.get_x(),
							uv.get_y()), m_size, color);
			m_lineIdx = 0;
		}
		else
		{
			m_lineVertex = vertex;
			m_lineColor = color;
			m_lineUV = uv;
			++m_lineIdx;
		}
		break;
	case DU_DRAW_TRIS:
		if ((m_triIdx % 3) == 2)
		{
			m_generators[m_meshDrawerIdx]->tri(m_triVertex[0], m_triColor[0],
					m_triUV[0], m_triVertex[1], m_triColor[1], m_triUV[1],
					vertex, color, uv);
			m_triIdx = 0;
		}
		else
		{
			m_triVertex[m_triIdx] = vertex;
			m_triColor[m_triIdx] = color;
			m_triUV[m_triIdx] = uv;
			++m_triIdx;
		}
		break;
	case DU_DRAW_QUADS:
		if ((m_quadIdx % 4) == 3)
		{
			m_generators[m_meshDrawerIdx]->tri(m_quadVertex[0], m_quadColor[0],
					m_quadUV[0], m_quadVertex[1], m_quadColor[1], m_quadUV[1],
					m_quadVertex[2], m_quadColor[2], m_quadUV[2]);
			m_generators[m_meshDrawerIdx]->tri(m_quadVertex[0], m_quadColor[0],
					LVector2f::zero(), m_quadVertex[2], m_quadColor[2],
					LVector2f::zero(), vertex, color, uv);
			m_quadIdx = 0;
		}
		else
		{
			m_quadVertex[m_quadIdx] = vertex;
			m_quadColor[m_quadIdx] = color;
			m_quadUV[m_quadIdx] = uv;
			++m_quadIdx;
		}
		break;
	};
}

void DebugDrawMeshDrawer::depthMask(bool state)
{
	m_depthMask = state;
}

void DebugDrawMeshDrawer::texture(bool state)
{
	m_texture = state;
}

void DebugDrawMeshDrawer::vertex(const float* pos, unsigned int color)
{
	doVertex(Recast3fToLVecBase3f(pos[0], pos[1], pos[2]),
			LVector4f(red(color), green(color), blue(color), alpha(color)),
			LVector2f::zero());
}

void DebugDrawMeshDrawer::vertex(const float x, const float y, const float z,
		unsigned int color)
{
	doVertex(Recast3fToLVecBase3f(x, y, z),
			LVector4f(red(color), green(color), blue(color), alpha(color)),
			LVector2f::zero());
}

void DebugDrawMeshDrawer::vertex(const float* pos, unsigned int color,
		const float* uv)
{
	doVertex(Recast3fToLVecBase3f(pos[0], pos[1], pos[2]),
			LVector4f(red(color), green(color), blue(color), alpha(color)),
			LVector2f(uv[0], uv[1]));
}

void DebugDrawMeshDrawer::vertex(const float x, const float y, const float z,
		unsigned int color, const float u, const float v)
{
	doVertex(Recast3fToLVecBase3f(x, y, z),
			LVector4f(red(color), green(color), blue(color), alpha(color)),
			LVector2f(u, v));
}

}  // namespace ely
