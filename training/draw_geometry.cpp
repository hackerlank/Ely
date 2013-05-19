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
 * \file /Ely/training/draw_geometry.cpp
 *
 * \date 21/apr/2013 (16:29:04)
 * \author consultit
 */

/*
#include <pandaFramework.h>
#include <pandaSystem.h>
#include <geomVertexFormat.h>
#include <geomPoints.h>
#include <geomLines.h>
#include <geomTriangles.h>
#include <geomTristrips.h>
#include <geomTrifans.h>
#include "Utilities/Tools.h"

#include <DebugDraw.h>
#include <RecastDebugDraw.h>

#define DD

inline LVecBase3f RecastToLVecBase3f(const float* p)
{
	return LVecBase3f(p[0], -p[2], p[1]);
}
inline LVecBase3f Recast3fToLVecBase3f(const float x, const float y, const float z)
{
	return LVecBase3f(x, -z, y);
}

/// Panda3d debug draw implementation.
class DebugDrawPanda3d : public duDebugDraw
{
protected:
	///The render node path.
	NodePath m_render;
	///Depth Mask.
	bool m_depthMask;
	///Texture.
	bool m_texture;
	///The current GeomVertexData.
	SMARTPTR(GeomVertexData) m_vertexData;
	///The current vertex index.
	int m_vertexIdx;
	///The current GeomVertexWriters.
	GeomVertexWriter m_vertex, m_color, m_texcoord;
	///The current GeomPrimitive and draw type.
	SMARTPTR(GeomPrimitive) m_geomPrim;
	duDebugDrawPrimitives m_prim;
	///The current Geom and index.
	SMARTPTR(Geom) m_geom;
	int m_geomIdx;
	///The current GeomNode node path.
	NodePath m_geomNodeNP;
	///QUADS stuff.
	int m_quadCurrIdx;
	LVector3f m_quadFirstVertex, m_quadThirdVertex;
	LVector4f m_quadFirstColor, m_quadThirdColor;
	LVector2f m_quadFirstUV, m_quadThirdUV;

private:
	///Helper
	void doVertex(const LVector3f& vertex, const LVector4f& color,
			const LVector2f& uv = LVecBase2f::zero());
public:
	DebugDrawPanda3d(NodePath render);
	virtual ~DebugDrawPanda3d();

	virtual void depthMask(bool state);
	virtual void texture(bool state);
	virtual void begin(duDebugDrawPrimitives prim, float size = 1.0f);
	virtual void vertex(const float* pos, unsigned int color);
	virtual void vertex(const float x, const float y, const float z, unsigned int color);
	virtual void vertex(const float* pos, unsigned int color, const float* uv);
	virtual void vertex(const float x, const float y, const float z, unsigned int color, const float u, const float v);
	virtual void end();
};

DebugDrawPanda3d::DebugDrawPanda3d(NodePath render) :
		m_render(render),
		m_depthMask(true),
		m_texture(true),
		m_vertexIdx(0),
		m_prim(DU_DRAW_TRIS),
		m_geomIdx(0)
{
}

DebugDrawPanda3d::~DebugDrawPanda3d()
{
}

static inline float red(unsigned int color)
{
	return ((float) ((color & 0xFF000000) >> 24)) / 255.0;
}
static inline float green(unsigned int color)
{
	return ((float) ((color & 0x00FF0000) >> 16)) / 255.0;
}
static inline float blue(unsigned int color)
{
	return ((float) ((color & 0x0000FF00) >> 8)) / 255.0;
}
static inline float alpha(unsigned int color)
{
	return ((float) ((color & 0x000000FF) >> 0)) / 255.0;
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
	ostringstream idx;
	idx << m_geomIdx;
	m_geomNodeNP = NodePath(new GeomNode("geomNode" + idx.str()));
	DCAST(GeomNode, m_geomNodeNP.node())->add_geom(m_geom);
	m_geomNodeNP.reparent_to(m_render);
//	m_geomNodeNP.set_depth_write(m_depthMask);
	++m_geomIdx;
}

int draw_geometry_main(int argc, char *argv[])
{
	//open a new window framework
	PandaFramework framework;
	framework.open_framework(argc, argv);
	//set the window title to My Panda3D Window
	framework.set_window_title("My Panda3D Window");
	//open the window
	WindowFramework *window = framework.open_window();
	if (window != (WindowFramework *) NULL)
	{
		std::cout << "Opened the window successfully!\n";
		// common setup
		window->enable_keyboard(); // Enable keyboard detection
		window->setup_trackball(); // Enable default camera movement
	}

	//here is room for your own code

#ifdef DD
	///<!--Constructor
	DebugDrawPanda3d dd(window->get_render());
	///-->

	duDebugDrawPrimitives primType;
//	primType = DU_DRAW_POINTS;
//	primType = DU_DRAW_LINES;
	primType = DU_DRAW_TRIS;
//	primType = DU_DRAW_QUADS;

	unsigned int color;

	///prim 1
	///<!--begin
	dd.begin(primType);
	///-->
	///<!--vertex
	//Add data
	color = 0xFFFF00FF;
	if (primType != DU_DRAW_QUADS)
	{
		//triangle 1
		//0
		dd.vertex(0, 0, 0, color, 0, 0);
		//1
		dd.vertex(1, 0, 0, color, 1, 0);
		//6
		dd.vertex(1, 1, 0, color, 1, 1);
		//triangle 2
		//1
		dd.vertex(1, 0, 0, color, 0, 0);
		//2
		dd.vertex(2, 0, 0, color, 1, 0);
		//5
		dd.vertex(2, 1, 0, color, 1, 1);
		//triangle 3
		//2
		dd.vertex(2, 0, 0, color, 0, 0);
		//3
		dd.vertex(3, 0, 0, color, 1, 0);
		//4
		dd.vertex(3, 1, 0, color, 1, 1);
	}
	else
	{
		//quad 1
		//0
		dd.vertex(0, 0, 0, color, 0, 0);
		//1
		dd.vertex(1, 0, 0, color, 1, 0);
		//6
		dd.vertex(1, 1, 0, color, 1, 1);
		//7
		dd.vertex(0, 1, 0, color, 0, 1);
		//quad 2
		//2
		dd.vertex(2, 0, 0, color, 0, 0);
		//3
		dd.vertex(3, 0, 0, color, 1, 0);
		//4
		dd.vertex(3, 1, 0, color, 1, 1);
		//5
		dd.vertex(2, 1, 0, color, 0, 1);
	}
	///-->
	///<!--end
	dd.end();
	///-->

	///prim 2
	///<!--begin
	dd.begin(primType);
	///-->
	///<!--vertex
	//Add data
	color = 0x00FF00FF;
	if (primType != DU_DRAW_QUADS)
	{
		//triangle 1
		//0
		dd.vertex(0, 0, 0, color, 0, 0);
		//6
		dd.vertex(1, 1, 0, color, 1, 1);
		//7
		dd.vertex(0, 1, 0, color, 0, 1);
		//triangle 2
		//2
		dd.vertex(2, 0, 0, color, 0, 0);
		//4
		dd.vertex(3, 1, 0, color, 1, 1);
		//5
		dd.vertex(2, 1, 0, color, 0, 1);
	}
	else
	{
		//quad 1
		//1
		dd.vertex(1, 0, 0, color, 0, 0);
		//2
		dd.vertex(2, 0, 0, color, 1, 0);
		//5
		dd.vertex(2, 1, 0, color, 1, 1);
		//6
		dd.vertex(1, 1, 0, color, 0, 1);
	}
	///-->
	///<!--end
	dd.end();
	///-->

#else
	SMARTPTR(GeomVertexData) vertexData;
	SMARTPTR(GeomTriangles) prim;
	GeomVertexWriter vertex, color, texcoord;
	SMARTPTR(Geom) geom;
	SMARTPTR(GeomNode) node;
	NodePath nodePath;

	///<!--Constructor
	//Creating and filling a GeomVertexData
	vertexData = new GeomVertexData("VertexData",
			GeomVertexFormat::get_v3c4t2(), Geom::UH_static);
	//Create a number of GeomVertexWriters
	vertex = GeomVertexWriter(vertexData, "vertex");
	color = GeomVertexWriter(vertexData, "color");
	texcoord = GeomVertexWriter(vertexData, "texcoord");
	///-->

	///<!--begin
	prim = new GeomTriangles(Geom::UH_static);
	vertex.set_row(0);
	color.set_row(0);
	texcoord.set_row(0);;
	int vertexIdx = 0;
	///-->

	///<!--vertex
	//Add data
	//triangle 1
	//0
	vertex.add_data3f(0, 0, 0);
	color.add_data4f(0, 0, 1, 1);
	texcoord.add_data2f(0, 0);
	prim->add_vertex(vertexIdx);
	++vertexIdx;
	//1
	vertex.add_data3f(1, 0, 0);
	color.add_data4f(0, 0, 1, 1);
	texcoord.add_data2f(1, 0);
	prim->add_vertex(vertexIdx);
	++vertexIdx;
	//6
	vertex.add_data3f(1, 0, 1);
	color.add_data4f(0, 0, 1, 1);
	texcoord.add_data2f(1, 1);
	prim->add_vertex(vertexIdx);
	++vertexIdx;
	//triangle 2
	//1
	vertex.add_data3f(1, 0, 0);
	color.add_data4f(0, 0, 1, 1);
	texcoord.add_data2f(0, 0);
	prim->add_vertex(vertexIdx);
	++vertexIdx;
	//2
	vertex.add_data3f(2, 0, 0);
	color.add_data4f(0, 0, 1, 1);
	texcoord.add_data2f(1, 0);
	prim->add_vertex(vertexIdx);
	++vertexIdx;
	//5
	vertex.add_data3f(2, 0, 1);
	color.add_data4f(0, 0, 1, 1);
	texcoord.add_data2f(1, 1);
	prim->add_vertex(vertexIdx);
	++vertexIdx;
	//triangle 3
	//2
	vertex.add_data3f(2, 0, 0);
	color.add_data4f(0, 0, 1, 1);
	texcoord.add_data2f(0, 0);
	prim->add_vertex(vertexIdx);
	++vertexIdx;
	//3
	vertex.add_data3f(3, 0, 0);
	color.add_data4f(0, 0, 1, 1);
	texcoord.add_data2f(1, 0);
	prim->add_vertex(vertexIdx);
	++vertexIdx;
	//4
	vertex.add_data3f(3, 0, 1);
	color.add_data4f(0, 0, 1, 1);
	texcoord.add_data2f(1, 1);
	prim->add_vertex(vertexIdx);
	++vertexIdx;
	///-->

	///<!--end
	prim->close_primitive();
	geom = new Geom(vertexData);
	geom->add_primitive(prim);
	//Putting your new geometry in the scene graph
	ostringstream idx;
	idx << "gnode" << 1;
	std::cout << idx.str() << std::endl;
	node = new GeomNode(idx.str());
	node->add_geom(geom);
	nodePath = window->get_render().attach_new_node(node);
	///-->
#endif


	//7
//	vertex.add_data3f(0, 0, 1);
//	color.add_data4f(0, 0, 1, 1);
//	texcoord.add_data2f(0, 1);
//	prim->add_vertex(vertexIdx);
//	++vertexIdx;
	//create one or more GeomPrimitive objects
	//the first triangle
//	prim->add_vertex(0);
//	prim->add_vertex(1);
//	prim->add_vertex(6);
//	prim->add_vertex(1);
//	prim->add_vertex(2);
//	prim->add_vertex(5);
//	prim->add_vertex(2);
//	prim->add_vertex(3);
//	prim->add_vertex(4);
	// you can also add a few at once
//	prim->add_vertices(0, 1, 6);
//	prim->add_vertices(1, 2, 5);
//	prim->add_vertices(2, 3, 4);


	//do the main loop, equal to run() in python
	framework.main_loop();
	//close the window framework
	framework.close_framework();
	return (0);
}
*/




