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

#include <pandaSystem.h>
#include <pandaFramework.h>
#include "Utilities/Tools.h"
#include "Support/Raycaster.h"
#include <unistd.h>
#include <cctype>
#include <cmath>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <load_prc_file.h>
#include <auto_bind.h>
#include <partBundleHandle.h>
#include <geomTriangles.h>
#include <geomTrifans.h>
#include <geomTristrips.h>
#include <character.h>
#include <animControlCollection.h>
#include <mouseWatcher.h>
#include <lvector3.h>
#include <nodePath.h>
#include <genericAsyncTask.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define _USE_MATH_DEFINES
#include <math.h>

//https://groups.google.com/forum/?fromgroups=#!searchin/recastnavigation/z$20axis/recastnavigation/fMqEAqSBOBk/zwOzHmjRsj0J
inline void LVecBase3fToRecast(const LVecBase3f& v, float* p)
{
	p[0] = v.get_x();
	p[1] = v.get_z();
	p[2] = -v.get_y();
}
inline LVecBase3f RecastToLVecBase3f(const float* p)
{
	return LVecBase3f(p[0], -p[2], p[1]);
}
inline LVecBase3f Recast3fToLVecBase3f(const float x, const float y, const float z)
{
	return LVecBase3f(x, -z, y);
}

class Panda3dMeshLoader
{
public:
	Panda3dMeshLoader();
	~Panda3dMeshLoader();

	bool load(const char* fileName, float scale = 1.0,
			float* translation = NULL);

	bool load(NodePath model, float scale = 1.0,
			float* translation = NULL);

	inline const float* getVerts() const
	{
		return m_verts;
	}
	inline const float* getNormals() const
	{
		return m_normals;
	}
	inline const int* getTris() const
	{
		return m_tris;
	}
	inline int getVertCount() const
	{
		return m_vertCount;
	}
	inline int getTriCount() const
	{
		return m_triCount;
	}
	inline const char* getFileName() const
	{
		return m_filename;
	}

private:

	void addVertex(float x, float y, float z, int& cap);
	void addTriangle(int a, int b, int c, int& cap);

	void processGeomNode(PT(GeomNode)geomNode);
	void processGeom(CPT(Geom)geom);
	void processVertexData(CPT(GeomVertexData)vertexData);
	void processPrimitive(CPT(GeomPrimitive)primitive, CPT(GeomVertexData)vertexData);

	char m_filename[260];
	float m_scale;
	float m_translation[3];
	float* m_verts;
	int* m_tris;
	float* m_normals;
	int m_vertCount;
	int m_triCount;
};

Panda3dMeshLoader::Panda3dMeshLoader() :
		m_scale(1.0f), m_verts(0), m_tris(0), m_normals(0), m_vertCount(0), m_triCount(
				0)
{
	for (int i = 0; i < 3; ++i)
	{
		m_translation[i] = 0.0;
	}
}

Panda3dMeshLoader::~Panda3dMeshLoader()
{
	delete[] m_verts;
	delete[] m_normals;
	delete[] m_tris;
}

void Panda3dMeshLoader::addVertex(float x, float y, float z, int& cap)
{
	if (m_vertCount + 1 > cap)
	{
		cap = !cap ? 8 : cap * 2;
		float* nv = new float[cap * 3];
		if (m_vertCount)
			memcpy(nv, m_verts, m_vertCount * 3 * sizeof(float));
		delete[] m_verts;
		m_verts = nv;
	}
	float* dst = &m_verts[m_vertCount * 3];
	*dst++ = x * m_scale + m_translation[0];
	*dst++ = y * m_scale + m_translation[1];
	*dst++ = z * m_scale + m_translation[2];
	m_vertCount++;
}

void Panda3dMeshLoader::addTriangle(int a, int b, int c, int& cap)
{
	if (m_triCount + 1 > cap)
	{
		cap = !cap ? 8 : cap * 2;
		int* nv = new int[cap * 3];
		if (m_triCount)
			memcpy(nv, m_tris, m_triCount * 3 * sizeof(int));
		delete[] m_tris;
		m_tris = nv;
	}
	int* dst = &m_tris[m_triCount * 3];
	*dst++ = a;
	*dst++ = b;
	*dst++ = c;
	m_triCount++;
}

static char* parseRow(char* buf, char* bufEnd, char* row, int len)
{
	bool cont = false;
	bool start = true;
	bool done = false;
	int n = 0;
	while (!done && buf < bufEnd)
	{
		char c = *buf;
		buf++;
		// multirow
		switch (c)
		{
		case '\\':
			cont = true; // multirow
			break;
		case '\n':
			if (start)
				break;
			done = true;
			break;
		case '\r':
			break;
		case '\t':
		case ' ':
			if (start)
				break;
		default:
			start = false;
			cont = false;
			row[n++] = c;
			if (n >= len - 1)
				done = true;
			break;
		}
	}
	row[n] = '\0';
	return buf;
}

static int parseFace(char* row, int* data, int n, int vcnt)
{
	int j = 0;
	while (*row != '\0')
	{
		// Skip initial white space
		while (*row != '\0' && (*row == ' ' || *row == '\t'))
			row++;
		char* s = row;
		// Find vertex delimiter and terminated the string there for conversion.
		while (*row != '\0' && *row != ' ' && *row != '\t')
		{
			if (*row == '/')
				*row = '\0';
			row++;
		}
		if (*s == '\0')
			continue;
		int vi = atoi(s);
		data[j++] = vi < 0 ? vi + vcnt : vi - 1;
		if (j >= n)
			return j;
	}
	return j;
}

bool Panda3dMeshLoader::load(const char* filename, float scale,
		float* translation)
{
	char* buf = 0;
	FILE* fp = fopen(filename, "rb");
	if (!fp)
		return false;
	m_scale = scale;
	if (translation != NULL)
	{
		m_translation[0] = translation[0];
		m_translation[1] = translation[1];
		m_translation[2] = translation[2];
	}
	fseek(fp, 0, SEEK_END);
	int bufSize = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	buf = new char[bufSize];
	if (!buf)
	{
		fclose(fp);
		return false;
	}
	fread(buf, bufSize, 1, fp);
	fclose(fp);

	char* src = buf;
	char* srcEnd = buf + bufSize;
	char row[512];
	int face[32];
	float x, y, z;
	int nv;
	int vcap = 0;
	int tcap = 0;

	while (src < srcEnd)
	{
		// Parse one row
		row[0] = '\0';
		src = parseRow(src, srcEnd, row, sizeof(row) / sizeof(char));
		// Skip comments
		if (row[0] == '#')
			continue;
		if (row[0] == 'v' && row[1] != 'n' && row[1] != 't')
		{
			// Vertex pos
			sscanf(row + 1, "%f %f %f", &x, &y, &z);
			addVertex(x, y, z, vcap);
		}
		if (row[0] == 'f')
		{
			// Faces
			nv = parseFace(row + 1, face, 32, m_vertCount);
			for (int i = 2; i < nv; ++i)
			{
				const int a = face[0];
				const int b = face[i - 1];
				const int c = face[i];
				if (a < 0 || a >= m_vertCount || b < 0 || b >= m_vertCount
						|| c < 0 || c >= m_vertCount)
					continue;
				addTriangle(a, b, c, tcap);
			}
		}
	}

	delete[] buf;

	// Calculate normals.
	m_normals = new float[m_triCount * 3];
	for (int i = 0; i < m_triCount * 3; i += 3)
	{
		const float* v0 = &m_verts[m_tris[i] * 3];
		const float* v1 = &m_verts[m_tris[i + 1] * 3];
		const float* v2 = &m_verts[m_tris[i + 2] * 3];
		float e0[3], e1[3];
		for (int j = 0; j < 3; ++j)
		{
			e0[j] = v1[j] - v0[j];
			e1[j] = v2[j] - v0[j];
		}
		float* n = &m_normals[i];
		n[0] = e0[1] * e1[2] - e0[2] * e1[1];
		n[1] = e0[2] * e1[0] - e0[0] * e1[2];
		n[2] = e0[0] * e1[1] - e0[1] * e1[0];
		float d = sqrtf(n[0] * n[0] + n[1] * n[1] + n[2] * n[2]);
		if (d > 0)
		{
			d = 1.0f / d;
			n[0] *= d;
			n[1] *= d;
			n[2] *= d;
		}
	}

	strncpy(m_filename, filename, sizeof(m_filename));
	m_filename[sizeof(m_filename) - 1] = '\0';

	return true;
}

bool Panda3dMeshLoader::load(NodePath model, float scale,
		float* translation)
{
	m_scale = scale;
	if (translation != NULL)
	{
		m_translation[0] = translation[0];
		m_translation[1] = translation[1];
		m_translation[2] = translation[2];
	}
	//Walk through all the model's GeomNodes
	NodePathCollection geomNodeCollection = model.find_all_matches(
			"**/+GeomNode");
	int numPaths = geomNodeCollection.get_num_paths();
	for (int i = 0; i < numPaths; i++)
	{
		PT(GeomNode)geomNode = DCAST(GeomNode,geomNodeCollection.get_path(i).node());
		processGeomNode(geomNode);
	}

	// Calculate normals.
	m_normals = new float[m_triCount * 3];
	for (int i = 0; i < m_triCount * 3; i += 3)
	{
		const float* v0 = &m_verts[m_tris[i] * 3];
		const float* v1 = &m_verts[m_tris[i + 1] * 3];
		const float* v2 = &m_verts[m_tris[i + 2] * 3];
		float e0[3], e1[3];
		for (int j = 0; j < 3; ++j)
		{
			e0[j] = v1[j] - v0[j];
			e1[j] = v2[j] - v0[j];
		}
		float* n = &m_normals[i];
		n[0] = e0[1] * e1[2] - e0[2] * e1[1];
		n[1] = e0[2] * e1[0] - e0[0] * e1[2];
		n[2] = e0[0] * e1[1] - e0[1] * e1[0];
		float d = sqrtf(n[0] * n[0] + n[1] * n[1] + n[2] * n[2]);
		if (d > 0)
		{
			d = 1.0f / d;
			n[0] *= d;
			n[1] *= d;
			n[2] *= d;
		}
	}

	return true;
}

void Panda3dMeshLoader::processGeomNode(PT(GeomNode)geomNode)
{
	//Walk through the list of GeomNode's Geoms
	int numGeoms = geomNode->get_num_geoms();
	for (int j = 0; j < numGeoms; j++)
	{
		const CPT(Geom)geom = geomNode->get_geom(j);
		processGeom(geom);
	}
}

void Panda3dMeshLoader::processGeom(CPT(Geom)geom)
{
	const CPT(GeomVertexData)vertexData = geom->get_vertex_data();
	//Process vertices
	processVertexData(vertexData);
	int numPrimitives = geom->get_num_primitives();
	//Walk through the list of Geom's GeomPrimitives
	for(int i=0;i<numPrimitives;i++)
	{
		const CPT(GeomPrimitive) primitive = geom->get_primitive(i);
		processPrimitive(primitive, vertexData);
	}
}

void Panda3dMeshLoader::processVertexData(CPT(GeomVertexData)vertexData)
{
	GeomVertexReader vertexReader = GeomVertexReader(vertexData, "vertex");
	int vcap = 0;
	while (!vertexReader.is_at_end())
	{
		LVector3f vertex = vertexReader.get_data3f();
		//add vertex
		float v[3];
		LVecBase3fToRecast(vertex, v);
		addVertex(v[0], v[1], v[2], vcap);
	}
}

void Panda3dMeshLoader::processPrimitive(CPT(GeomPrimitive)primitive, CPT(GeomVertexData)vertexData)
{
	int tcap = 0;
	if (primitive->is_of_type(GeomTriangles::get_class_type()) or
			primitive->is_of_type(GeomTrifans::get_class_type()) or
			primitive->is_of_type(GeomTristrips::get_class_type()))
	{
		GeomVertexReader vertexReader = GeomVertexReader(vertexData, "vertex");
		CPT(GeomPrimitive)primitiveDec = primitive->decompose();
		int numPrimitives = primitiveDec->get_num_primitives();
		for (int k = 0; k < numPrimitives; k++)
		{
			int s = primitiveDec->get_primitive_start(k);
			int e = primitiveDec->get_primitive_end(k);
			//add vertex indices
			int vi[3];
			for (int j = s; j < e; ++j)
			{
				vi[j-s] = primitiveDec->get_vertex(j);
			}
			addTriangle(vi[0], vi[1], vi[2], tcap);
		}
	}
}

std::string baseDir("/REPOSITORY/KProjects/WORKSPACE/Ely/");
std::string rnDir("/REPOSITORY/KProjects/usr/share/DCC/recastnavigation/");
std::string meshNameEgg("nav_test_panda.egg");
void processGeomNode(PT(GeomNode)geomNode);
void processGeom(CPT(Geom)geom);
void processVertexData(CPT(GeomVertexData)vertexData);
void processPrimitive(CPT(GeomPrimitive)primitive, CPT(GeomVertexData)vertexData);

int draw_geometry_main(int argc, char *argv[])
{
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
	//setup camera trackball (local coordinate)
	NodePath tballnp = window->get_mouse().find("**/+Trackball");
	PT(Trackball)trackball = DCAST(Trackball, tballnp.node());
	trackball->set_pos(0, 0, 0);
	trackball->set_hpr(0, 0, 0);

	//here is room for your own code
	//Load world mesh
	NodePath worldMesh = window->load_model(window->get_render(),
			rnDir + meshNameEgg);
	worldMesh.flatten_strong();

	//When you load a model, you have a handle to the root node
	//of the model, which is usually a ModelRoot node.
	//The geometry itself will be stored in a series of GeomNodes,
	//which will be children of the root node. In order to examine
	//the vertex data, you must visit the GeomNodes in the model.
	//One way to do this is to walk through all the GeomNodes like this:
	NodePathCollection geomNodeCollection = worldMesh.find_all_matches(
			"**/+GeomNode");
	int numPaths = geomNodeCollection.get_num_paths();
	for (int i = 0; i < numPaths; i++)
	{
		PT(GeomNode)geomNode = DCAST(GeomNode,geomNodeCollection.get_path(i).node());
		processGeomNode(geomNode);
	}

	Panda3dMeshLoader p3dl;
	p3dl.load(worldMesh);

	//attach to scene
	worldMesh.set_pos(0.0, 0.0, 0.0);
	worldMesh.reparent_to(window->get_render());

	//do the main loop, equal to run() in python
	framework.main_loop();
	//close the window framework
	framework.close_framework();
	return (0);
}

//Once you have a particular GeomNode, you must walk through
//the list of Geoms stored on that node. Each Geom also has
//an associated RenderState, which controls the visible
//appearance of that Geom (e.g. texture, backfacing, etc.).
///\note: that geomNode->getGeom() is only appropriate if you
///will be reading, but not modifying, the data. If you intend
///to modify the geom data in any way (including any nested
///data like vertices or primitives), you should use
///geomNode.modifyGeom() instead.
void processGeomNode(PT(GeomNode)geomNode)
{
	int numGeoms = geomNode->get_num_geoms();
	for (int j = 0; j < numGeoms; j++)
	{
		const CPT(Geom)geom = geomNode->get_geom(j);
//		geom->write(nout);	//outputs basic info on the geom
//		geomNode->get_geom_state(j)->write(nout, 0);//basic renderstate info
		processGeom(geom);
	}
}

//Each Geom has an associated GeomVertexData, and one or more
//GeomPrimitives. Some GeomVertexData objects may be shared
//by more than one Geom, especially if you have used
//flattenStrong() to optimize a model.
///\note: As above, get_vertex_data() is only appropriate if
///you will only be reading, but not modifying, the vertex data.
///Similarly, getPrimitive() is appropriate only if you will
///not be modifying the primitive index array. If you intend
///to modify either one, use modifyVertexData() or modifyPrimitive(),
///respectively.
void processGeom(CPT(Geom)geom)
{
	const CPT(GeomVertexData)vertexData = geom->get_vertex_data();
//	vertexData->write(nout);
	processVertexData(vertexData);
	int numPrimitives = geom->get_num_primitives();
	for(int i=0;i<numPrimitives;i++)
	{
		const CPT(GeomPrimitive) primitive = geom->get_primitive(i);
//		primitive->write(nout,0);
		processPrimitive(primitive, vertexData);
	}
}

//You can use the GeomVertexReader class to examine the vertex data.
//You should create a GeomVertexReader for each column of the data
//you intend to read. It is up to you to ensure that a given column
//exists in the vertex data before you attempt to read it (you can
//use vdata.hasColumn() to test this).
void processVertexData(CPT(GeomVertexData)vertexData)
{
	GeomVertexReader vertexReader = GeomVertexReader(vertexData, "vertex");
	int vertexNum = vertexData->get_num_rows();
	while (!vertexReader.is_at_end())
	{
		LVector3f vertex = vertexReader.get_data3f();
//		nout<< "Vertex = " << vertex  << endl;
	}
}

//Each GeomPrimitive may be any of a handful of different classes,
//according to the primitive type it is; but all GeomPrimitive
//classes have the same common interface to walk through the
//list of vertices referenced by the primitives stored within the class.
//You can use the setRow() method of GeomVertexReader to set the
//reader to a particular vertex. This affects the next call to getData().
//In this way, you can extract the vertex data for the vertices in the
//order that the primitive references them (instead of in order from
//the beginning to the end of the vertex table, as above).
///\note: You may find the call to primitive->decompose() useful
///(as shown in the below example). This call automatically
///decomposes higher-order primitive types, like GeomTristrips and
///GeomTrifans, into the equivalent component primitive types,
///like GeomTriangles; but when called on a GeomTriangles, it returns
///the GeomTriangles object unchanged. Similarly, GeomLinestrips will
///be decomposed into GeomLines. This way you can write code that
///doesn't have to know anything about GeomTristrips and GeomTrifans,
///which are fairly complex; it can assume it will only get the much
///simpler GeomTriangles (or, in the case of lines or points, GeomLines
///and GeomPoints, respectively).
void processPrimitive(CPT(GeomPrimitive)primitive, CPT(GeomVertexData)vertexData)
{
	nout << "Primitive type: " <<
			primitive->get_type().get_name() <<endl;
	if (primitive->is_of_type(GeomTriangles::get_class_type()) or
			primitive->is_of_type(GeomTrifans::get_class_type()) or
			primitive->is_of_type(GeomTristrips::get_class_type()))
	{
		GeomVertexReader vertexReader = GeomVertexReader(vertexData, "vertex");
		//Note: There should be primitive = primitive->decompose();
		//here, it wouldn't work for me but i use the cvs panda and
		//that could have been broken at this time.
		CPT(GeomPrimitive)primitiveDec = primitive->decompose();
		int numPrimitives = primitiveDec->get_num_primitives();
		nout << "PrimitiveDec type: " <<
				primitiveDec->get_type().get_name() <<endl;
		for (int k = 0; k < numPrimitives; k++)
		{
			int s = primitiveDec->get_primitive_start(k);
			int e = primitiveDec->get_primitive_end(k);
			for (int i = s; i < e; i++)
			{
				int vi = primitiveDec->get_vertex(i);
				vertexReader.set_row(vi);
				LVector3f v = vertexReader.get_data3f();
				nout << "prim " << k << " has vertex " << vi << ": " << v
				<< endl;
			}
		}
	}
}

