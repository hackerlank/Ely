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
 * \file /Ely/training/opensteer/DrawMeshDrawer.cpp
 *
 * \date 21/nov/2013 (15:29:07)
 * \author consultit
 */

#include "DrawMeshDrawer.h"
#include "common.h"

namespace ely
{

DrawMeshDrawer::DrawMeshDrawer(NodePath render, NodePath camera, int budget,
		bool singleMesh) :
		m_render(render), m_camera(camera), m_meshDrawersSize(0), m_budget(
				budget), m_singleMesh(singleMesh)
{
}

DrawMeshDrawer::~DrawMeshDrawer()
{
	for (unsigned int i = 0; i < m_generators.size(); ++i)
	{
		delete m_generators[i];
	}
}

void DrawMeshDrawer::reset()
{
	//reset current MeshDrawer index
	m_meshDrawerIdx = 0;
	m_prim = NULL_PRIM;
}

void DrawMeshDrawer::begin(DrawPrimitive prim, float size)
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
		m_generators.back()->get_root().get_child(0).node()->set_bounds(
				new OmniBoundingVolume());
		m_generators.back()->get_root().get_child(0).node()->set_final(true);
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

void DrawMeshDrawer::end()
{
	//end current MeshDrawer
	m_generators[m_meshDrawerIdx]->end();
	//increase MeshDrawer index only if multiple mesh
	if (not m_singleMesh)
	{
		++m_meshDrawerIdx;
	}
	m_prim = NULL_PRIM;
}

void DrawMeshDrawer::vertex(const LVector3f& vertex, const LVector2f& uv)
{
	switch (m_prim)
	{
	case DRAW_POINTS:
		m_generators[m_meshDrawerIdx]->billboard(vertex,
				LVector4f(uv.get_x(), uv.get_y(), uv.get_x(), uv.get_y()),
				m_size, m_color);
		break;
	case DRAW_LINES:
		if ((m_lineIdx % 2) == 1)
		{
			m_generators[m_meshDrawerIdx]->segment(m_lineVertex, vertex,
					LVector4f(m_lineUV.get_x(), m_lineUV.get_y(), uv.get_x(),
							uv.get_y()), m_size, m_color);
			m_lineIdx = 0;
		}
		else
		{
			m_lineVertex = vertex;
			m_lineColor = m_color;
			m_lineUV = uv;
			++m_lineIdx;
		}
		break;
	case DRAW_TRIS:
		if ((m_triIdx % 3) == 2)
		{
			m_generators[m_meshDrawerIdx]->tri(m_triVertex[0], m_triColor[0],
					m_triUV[0], m_triVertex[1], m_triColor[1], m_triUV[1],
					vertex, m_color, uv);
			m_triIdx = 0;
		}
		else
		{
			m_triVertex[m_triIdx] = vertex;
			m_triColor[m_triIdx] = m_color;
			m_triUV[m_triIdx] = uv;
			++m_triIdx;
		}
		break;
	case DRAW_QUADS:
		if ((m_quadIdx % 4) == 3)
		{
			m_generators[m_meshDrawerIdx]->tri(m_quadVertex[0], m_quadColor[0],
					m_quadUV[0], m_quadVertex[1], m_quadColor[1], m_quadUV[1],
					m_quadVertex[2], m_quadColor[2], m_quadUV[2]);
			m_generators[m_meshDrawerIdx]->tri(m_quadVertex[0], m_quadColor[0],
					LVector2f::zero(), m_quadVertex[2], m_quadColor[2],
					LVector2f::zero(), vertex, m_color, uv);
			m_quadIdx = 0;
		}
		else
		{
			m_quadVertex[m_quadIdx] = vertex;
			m_quadColor[m_quadIdx] = m_color;
			m_quadUV[m_quadIdx] = uv;
			++m_quadIdx;
		}
		break;
	};
}

}  // namespace ely

