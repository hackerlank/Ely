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
 * \author marco
 */

#include <pandaFramework.h>
#include <pandaSystem.h>
#include <geomVertexFormat.h>
#include <geomPoints.h>
#include <geomLines.h>
#include <geomTriangles.h>
#include "Utilities/Tools.h"

#include <DebugDraw.h>
#include <RecastDebugDraw.h>

#define DD

/// Panda3d debug draw implementation.
class DebugDrawPanda3d : public duDebugDraw
{
protected:
	///The render node path.
	NodePath m_render;
	///Creating and filling a GeomVertexData.
	SMARTPTR(GeomVertexData) m_vertexData;
	///Create a number of GeomVertexWriters.
	GeomVertexWriter m_vertex, m_color, m_texcoord;
	///The current vertex index.
	int m_vertexIdx;
	///Depth Mask.
	bool m_depthMask;
	///Texture.
	bool m_texture;
	///The current GeomPrimitive.
	GeomPrimitive* m_geomPrim;
	///The current Geom and index.
	Geom* m_geom;
	int m_geomIdx;
	///The current GeomNode node path.
	NodePath m_geomNodeNP;
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
		m_vertexData(new GeomVertexData("VertexData", GeomVertexFormat::get_v3c4t2(),
						Geom::UH_static)),
		m_vertex(GeomVertexWriter(m_vertexData, "vertex")),
		m_color(GeomVertexWriter(m_vertexData, "color")),
		m_texcoord(GeomVertexWriter(m_vertexData, "texcoord")),
		m_vertexIdx(0),
		m_depthMask(true),
		m_texture(true),
		m_geomIdx(0)
{
}

DebugDrawPanda3d::~DebugDrawPanda3d()
{
}

void DebugDrawPanda3d::depthMask(bool state)
{
	m_geomNodeNP.set_depth_write(state);
}

void DebugDrawPanda3d::texture(bool state)
{
}

void DebugDrawPanda3d::begin(duDebugDrawPrimitives prim, float size)
{
	switch (prim)
	{
	case DU_DRAW_POINTS:
		break;
	case DU_DRAW_LINES:
		break;
	case DU_DRAW_TRIS:
		m_geomPrim = new GeomTriangles(Geom::UH_static);
		m_vertex.set_row(0);
		m_color.set_row(0);
		m_texcoord.set_row(0);;
		m_vertexIdx = 0;
		break;
	case DU_DRAW_QUADS:
		break;
	};
}

void DebugDrawPanda3d::vertex(const float* pos, unsigned int color)
{
}

void DebugDrawPanda3d::vertex(const float x, const float y, const float z, unsigned int color)
{
}

void DebugDrawPanda3d::vertex(const float* pos, unsigned int color, const float* uv)
{
}

void DebugDrawPanda3d::vertex(const float x, const float y, const float z, unsigned int color, const float u, const float v)
{
	m_vertex.add_data3f(x, y, z);
	float red = ((float)((color & 0xFF000000) >> 24)) / 255.0;
	float green = ((float)((color & 0x00FF0000) >> 16)) / 255.0;
	float blue = ((float)((color & 0x0000FF00) >> 8)) / 255.0;
	float alpha = ((float)((color & 0x000000FF) >> 0)) / 255.0;
	m_color.add_data4f(red, green, blue, alpha);
	m_texcoord.add_data2f(u, v);
	//
	m_geomPrim->add_vertex(m_vertexIdx);
	++m_vertexIdx;
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

	///The render node path.
	NodePath m_render;
	///Creating and filling a GeomVertexData.
	SMARTPTR(GeomVertexData) m_vertexData;
	///Create a number of GeomVertexWriters.
	GeomVertexWriter m_vertex, m_color, m_texcoord;
	///The current vertex index.
	int m_vertexIdx;
	///Depth Mask.
	bool m_depthMask;
	///Texture.
	bool m_texture;
	///The current GeomPrimitive.
	GeomPrimitive* m_geomPrim;
	///The current Geom and index.
	Geom* m_geom;
	int m_geomIdx;
	///The current GeomNode node path.
	NodePath m_geomNodeNP;

	///<!--Constructor
//	DebugDrawPanda3d dd(window->get_render());

	m_render = window->get_render();
	m_vertexData = new GeomVertexData("VertexData", GeomVertexFormat::get_v3c4t2(),
					Geom::UH_static);
	m_vertex = GeomVertexWriter(m_vertexData, "vertex");
	m_color = GeomVertexWriter(m_vertexData, "color");
	m_texcoord = GeomVertexWriter(m_vertexData, "texcoord");
	m_vertexIdx = 0;
	m_depthMask = true;
	m_texture = true;
	m_geomIdx = 0;

	///-->

	///<!--begin
//	dd.begin(DU_DRAW_TRIS);

	duDebugDrawPrimitives prim = DU_DRAW_TRIS;
	switch (prim)
	{
	case DU_DRAW_POINTS:
		break;
	case DU_DRAW_LINES:
		break;
	case DU_DRAW_TRIS:
		m_geomPrim = new GeomTriangles(Geom::UH_static);
		m_vertex.set_row(0);
		m_color.set_row(0);
		m_texcoord.set_row(0);;
		m_vertexIdx = 0;
		break;
	case DU_DRAW_QUADS:
		break;
	};

	///-->

	///<!--vertex
	//Add data
//	unsigned int blue = 0x0000FFFF;
	//triangle 1
	//0
//	dd.vertex(0, 0, 0, blue, 0, 0);

	float x = 0;
	float y = 0;
	float z = 0;
	unsigned int color = 0x0000FFFF;
	float u = 0;
	float v = 0;
	m_vertex.add_data3f(x, y, z);
	float red = ((float) ((color & 0xFF000000) >> 24)) / 255.0;
	float green = ((float) ((color & 0x00FF0000) >> 16)) / 255.0;
	float blue = ((float) ((color & 0x0000FF00) >> 8)) / 255.0;
	float alpha = ((float) ((color & 0x000000FF) >> 0)) / 255.0;
	m_color.add_data4f(red, green, blue, alpha);
	m_texcoord.add_data2f(u, v);
	m_geomPrim->add_vertex(m_vertexIdx);
	++m_vertexIdx;

	//1
//	dd.vertex(1, 0, 0, blue, 1, 0);

	x = 1;
	y = 0;
	z = 0;
	color = 0x0000FFFF;
	u = 1;
	v = 0;
	m_vertex.add_data3f(x, y, z);
	red = ((float) ((color & 0xFF000000) >> 24)) / 255.0;
	green = ((float) ((color & 0x00FF0000) >> 16)) / 255.0;
	blue = ((float) ((color & 0x0000FF00) >> 8)) / 255.0;
	alpha = ((float) ((color & 0x000000FF) >> 0)) / 255.0;
	m_color.add_data4f(red, green, blue, alpha);
	m_texcoord.add_data2f(u, v);
	m_geomPrim->add_vertex(m_vertexIdx);
	++m_vertexIdx;

	//6
//	dd.vertex(1, 0, 1, blue, 1, 1);

	x = 1;
	y = 0;
	z = 1;
	color = 0x0000FFFF;
	u = 1;
	v = 1;
	m_vertex.add_data3f(x, y, z);
	red = ((float) ((color & 0xFF000000) >> 24)) / 255.0;
	green = ((float) ((color & 0x00FF0000) >> 16)) / 255.0;
	blue = ((float) ((color & 0x0000FF00) >> 8)) / 255.0;
	alpha = ((float) ((color & 0x000000FF) >> 0)) / 255.0;
	m_color.add_data4f(red, green, blue, alpha);
	m_texcoord.add_data2f(u, v);
	m_geomPrim->add_vertex(m_vertexIdx);
	++m_vertexIdx;

	//triangle 2
	//1
//	dd.vertex(1, 0, 0, blue, 0, 0);

	x = 1;
	y = 0;
	z = 0;
	color = 0x0000FFFF;
	u = 0;
	v = 0;
	m_vertex.add_data3f(x, y, z);
	red = ((float) ((color & 0xFF000000) >> 24)) / 255.0;
	green = ((float) ((color & 0x00FF0000) >> 16)) / 255.0;
	blue = ((float) ((color & 0x0000FF00) >> 8)) / 255.0;
	alpha = ((float) ((color & 0x000000FF) >> 0)) / 255.0;
	m_color.add_data4f(red, green, blue, alpha);
	m_texcoord.add_data2f(u, v);
	m_geomPrim->add_vertex(m_vertexIdx);
	++m_vertexIdx;

	//2
//	dd.vertex(2, 0, 0, blue, 1, 0);

	x = 2;
	y = 0;
	z = 0;
	color = 0x0000FFFF;
	u = 1;
	v = 0;
	m_vertex.add_data3f(x, y, z);
	red = ((float) ((color & 0xFF000000) >> 24)) / 255.0;
	green = ((float) ((color & 0x00FF0000) >> 16)) / 255.0;
	blue = ((float) ((color & 0x0000FF00) >> 8)) / 255.0;
	alpha = ((float) ((color & 0x000000FF) >> 0)) / 255.0;
	m_color.add_data4f(red, green, blue, alpha);
	m_texcoord.add_data2f(u, v);
	m_geomPrim->add_vertex(m_vertexIdx);
	++m_vertexIdx;

	//5
//	dd.vertex(2, 0, 1, blue, 1, 1);

	x = 2;
	y = 0;
	z = 1;
	color = 0x0000FFFF;
	u = 1;
	v = 1;
	m_vertex.add_data3f(x, y, z);
	red = ((float) ((color & 0xFF000000) >> 24)) / 255.0;
	green = ((float) ((color & 0x00FF0000) >> 16)) / 255.0;
	blue = ((float) ((color & 0x0000FF00) >> 8)) / 255.0;
	alpha = ((float) ((color & 0x000000FF) >> 0)) / 255.0;
	m_color.add_data4f(red, green, blue, alpha);
	m_texcoord.add_data2f(u, v);
	m_geomPrim->add_vertex(m_vertexIdx);
	++m_vertexIdx;

	//triangle 3
	//2
//	dd.vertex(2, 0, 0, blue, 0, 0);

	x = 2;
	y = 0;
	z = 0;
	color = 0x0000FFFF;
	u = 0;
	v = 0;
	m_vertex.add_data3f(x, y, z);
	red = ((float) ((color & 0xFF000000) >> 24)) / 255.0;
	green = ((float) ((color & 0x00FF0000) >> 16)) / 255.0;
	blue = ((float) ((color & 0x0000FF00) >> 8)) / 255.0;
	alpha = ((float) ((color & 0x000000FF) >> 0)) / 255.0;
	m_color.add_data4f(red, green, blue, alpha);
	m_texcoord.add_data2f(u, v);
	m_geomPrim->add_vertex(m_vertexIdx);
	++m_vertexIdx;

	//3
//	dd.vertex(3, 0, 0, blue, 1, 0);

	x = 3;
	y = 0;
	z = 0;
	color = 0x0000FFFF;
	u = 1;
	v = 0;
	m_vertex.add_data3f(x, y, z);
	red = ((float) ((color & 0xFF000000) >> 24)) / 255.0;
	green = ((float) ((color & 0x00FF0000) >> 16)) / 255.0;
	blue = ((float) ((color & 0x0000FF00) >> 8)) / 255.0;
	alpha = ((float) ((color & 0x000000FF) >> 0)) / 255.0;
	m_color.add_data4f(red, green, blue, alpha);
	m_texcoord.add_data2f(u, v);
	m_geomPrim->add_vertex(m_vertexIdx);
	++m_vertexIdx;

	//4
//	dd.vertex(3, 0, 1, blue, 1, 1);

	x = 3;
	y = 0;
	z = 1;
	color = 0x0000FFFF;
	u = 1;
	v = 0;
	m_vertex.add_data3f(x, y, z);
	red = ((float) ((color & 0xFF000000) >> 24)) / 255.0;
	green = ((float) ((color & 0x00FF0000) >> 16)) / 255.0;
	blue = ((float) ((color & 0x0000FF00) >> 8)) / 255.0;
	alpha = ((float) ((color & 0x000000FF) >> 0)) / 255.0;
	m_color.add_data4f(red, green, blue, alpha);
	m_texcoord.add_data2f(u, v);
	m_geomPrim->add_vertex(m_vertexIdx);
	++m_vertexIdx;

	///-->

	///<!--end
//	dd.end();

	m_geomPrim->close_primitive();
	m_geom = new Geom(m_vertexData);
	m_geom->add_primitive(m_geomPrim);
	ostringstream idx;
	idx << m_geomIdx;
	m_geomNodeNP = NodePath(new GeomNode("geomNode" + idx.str()));
	DCAST(GeomNode, m_geomNodeNP.node())->add_geom(m_geom);
	m_geomNodeNP.reparent_to(m_render);
	++m_geomIdx;

	///-->
#else
	///<!--Constructor
	//Creating and filling a GeomVertexData
	SMARTPTR(GeomVertexData) vertexData = new GeomVertexData("VertexData",
			GeomVertexFormat::get_v3c4t2(), Geom::UH_static);
	//Create a number of GeomVertexWriters
	GeomVertexWriter vertex, color, texcoord;
	vertex = GeomVertexWriter(vertexData, "vertex");
	color = GeomVertexWriter(vertexData, "color");
	texcoord = GeomVertexWriter(vertexData, "texcoord");
	///-->

	///<!--begin
	SMARTPTR(GeomTriangles) prim = new GeomTriangles(Geom::UH_static);
	vertex.set_row(0);
	color.set_row(0);
	texcoord.set_row(0);;
	int vertexIdx = 0;
	///-->

	///<!--vertex
	//Add data
	unsigned int blue = 0x0000FFFF;
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
	SMARTPTR(Geom) geom = new Geom(vertexData);
	geom->add_primitive(prim);
	//Putting your new geometry in the scene graph
	ostringstream idx;
	idx << "gnode" << 1;
	std::cout << idx.str() << std::endl;
	SMARTPTR(GeomNode) node = new GeomNode(idx.str());
	node->add_geom(geom);
	NodePath nodePath = window->get_render().attach_new_node(node);
	///-->
#endif

/*
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
*/

	//do the main loop, equal to run() in python
	framework.main_loop();
	//close the window framework
	framework.close_framework();
	return (0);
}




