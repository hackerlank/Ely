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
	float red = ((float)((color && 0xF000) >> 24)) / 255.0;
	float green = ((float)((color && 0x0F00) >> 16)) / 255.0;
	float blue = ((float)((color && 0x00F0) >> 8)) / 255.0;
	float alpha = ((float)((color && 0x000F) >> 0)) / 255.0;
	m_color.add_data4f(red, green, blue, alpha);
	m_texcoord.add_data2f(u, v);
	//
	m_geomPrim->add_vertex(m_vertexIdx);
	++m_vertexIdx;
}

void DebugDrawPanda3d::end()
{
	m_geom = new Geom(m_vertexData);
	m_geom->add_primitive(m_geomPrim);
	m_geomNodeNP = NodePath(new GeomNode("geomNode" + std::string(m_geomIdx)));
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
	//Creating and filling a GeomVertexData
	SMARTPTR(GeomVertexData) vertexData = new GeomVertexData("VertexData",
			GeomVertexFormat::get_v3c4t2(), Geom::UH_static);
	//Create a number of GeomVertexWriters
	GeomVertexWriter vertex, color, texcoord;

	SMARTPTR(GeomTriangles) prim = new GeomTriangles(Geom::UH_static);

	SMARTPTR(Geom) geom = new Geom(vertexData);
	SMARTPTR(GeomNode) node = new GeomNode("gnode");

	vertex = GeomVertexWriter(vertexData, "vertex");
	color = GeomVertexWriter(vertexData, "color");
	texcoord = GeomVertexWriter(vertexData, "texcoord");
	//Add data
	//0
	vertex.add_data3f(0, 0, 0);
	color.add_data4f(0, 0, 1, 1);
	texcoord.add_data2f(1, 0);
	//1
	vertex.add_data3f(1, 0, 0);
	color.add_data4f(0, 0, 1, 1);
	texcoord.add_data2f(1, 1);
	//2
	vertex.add_data3f(2, 0, 0);
	color.add_data4f(0, 0, 1, 1);
	texcoord.add_data2f(0, 1);
	//3
	vertex.add_data3f(3, 0, 0);
	color.add_data4f(0, 0, 1, 1);
	texcoord.add_data2f(0, 0);
	//4
	vertex.add_data3f(3, 0, 1);
	color.add_data4f(0, 0, 1, 1);
	texcoord.add_data2f(1, 0);
	//5
	vertex.add_data3f(2, 0, 1);
	color.add_data4f(0, 0, 1, 1);
	texcoord.add_data2f(1, 1);
	//6
	vertex.add_data3f(1, 0, 1);
	color.add_data4f(0, 0, 1, 1);
	texcoord.add_data2f(0, 1);
	//7
	vertex.add_data3f(0, 0, 1);
	color.add_data4f(0, 0, 1, 1);
	texcoord.add_data2f(0, 0);
	//create one or more GeomPrimitive objects
	//the first triangle
	prim->add_vertex(0);
	prim->add_vertex(1);
	prim->add_vertex(6);
	prim->add_vertex(1);
	prim->add_vertex(2);
	prim->add_vertex(5);
	prim->add_vertex(2);
	prim->add_vertex(3);
	prim->add_vertex(4);
	// you can also add a few at once
//	prim->add_vertices(0, 1, 6);
//	prim->add_vertices(1, 2, 5);
//	prim->add_vertices(2, 3, 4);
	//Putting your new geometry in the scene graph
	geom->add_primitive(prim);
	node->add_geom(geom);
	NodePath nodePath = window->get_render().attach_new_node(node);

	//do the main loop, equal to run() in python
	framework.main_loop();
	//close the window framework
	framework.close_framework();
	return (0);
}




