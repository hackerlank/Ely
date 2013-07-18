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

#ifndef SAMPLEINTERFACES_H
#define SAMPLEINTERFACES_H

#include "DebugDraw.h"
#include "Recast.h"
#include "RecastDump.h"
#include "PerfTimer.h"

#include <nodePath.h>
#include <geomVertexFormat.h>
#include <geomVertexWriter.h>
#include <geomPoints.h>
#include <geomLines.h>
#include <geomTriangles.h>
#include <geomTristrips.h>
#include <transparencyAttrib.h>
#include <meshDrawer.h>
#include "Utilities/Tools.h"

// These are example implementations of various interfaces used in Recast and Detour.

/// Recast build context.
class BuildContext : public rcContext
{
	rnTimeVal m_startTime[RC_MAX_TIMERS];
	int m_accTime[RC_MAX_TIMERS];

	static const int MAX_MESSAGES = 1000;
	const char* m_messages[MAX_MESSAGES];
	int m_messageCount;
	static const int TEXT_POOL_SIZE = 8000;
	char m_textPool[TEXT_POOL_SIZE];
	int m_textPoolSize;
	
public:
	BuildContext();
	virtual ~BuildContext();
	
	/// Dumps the log to stdout.
	void dumpLog(const char* format, ...);
	/// Returns number of log messages.
	int getLogCount() const;
	/// Returns log message text.
	const char* getLogText(const int i) const;
	
protected:	
	/// Virtual functions for custom implementations.
	///@{
	virtual void doResetLog();
	virtual void doLog(const rcLogCategory /*category*/, const char* /*msg*/, const int /*len*/);
	virtual void doResetTimers();
	virtual void doStartTimer(const rcTimerLabel /*label*/);
	virtual void doStopTimer(const rcTimerLabel /*label*/);
	virtual int doGetAccumulatedTime(const rcTimerLabel /*label*/) const;
	///@}
};

/// OpenGL debug draw implementation.
class DebugDrawGL : public duDebugDraw
{
public:
	virtual void depthMask(bool state);
	virtual void texture(bool state);
	virtual void begin(duDebugDrawPrimitives prim, float size = 1.0f);
	virtual void vertex(const float* pos, unsigned int color);
	virtual void vertex(const float x, const float y, const float z, unsigned int color);
	virtual void vertex(const float* pos, unsigned int color, const float* uv);
	virtual void vertex(const float x, const float y, const float z, unsigned int color, const float u, const float v);
	virtual void end();
};

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
	///Size (for points)
	float m_size;
	///The current Geom.
	SMARTPTR(Geom) m_geom;
	///The current GeomNode node path.
	NodePath m_geomNodeNP;
	///The GeomNodes' node paths.
	std::vector<NodePath> m_geomNodeNPCollection;
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

	NodePath getGeomNode(int i);
	int getGeomNodesNum();
	void removeGeomNodes();

};

/// MeshDrawer debug draw implementation.
class DebugDrawMeshDrawer : public duDebugDraw
{
protected:
	///The render node path.
	NodePath m_render;
	///The camera node path.
	NodePath m_camera;
	///Depth Mask.
	bool m_depthMask;
	///Texture.
	bool m_texture;
	///Inner MeshDrawers.
	std::vector<MeshDrawer*> m_generators;
	///Current MeshDrawer index.
	int m_meshDrawerIdx;
	///Current MeshDrawers number.
	int m_meshDrawersSize;
	///Budget.
	int m_budget;
	///The current GeomPrimitive and draw type.
	duDebugDrawPrimitives m_prim;
	enum {DU_NULL_PRIM=-1};
	///Size (for points and lines).
	float m_size;
	///Line previous store.
	LVecBase3f m_lineVertex;
	LVecBase4f m_lineColor;
	LVecBase2f m_lineUV;
	int m_lineIdx;
	///Triangle previous store.
	LVecBase3f m_triVertex[2];
	LVecBase4f m_triColor[2];
	LVecBase2f m_triUV[2];
	int m_triIdx;
	///Quad previous store.
	LVecBase3f m_quadVertex[2];
	LVecBase4f m_quadColor[2];
	LVecBase2f m_quadUV[2];
	int m_quadIdx;

private:
	///Helper
	void doVertex(const LVector3f& vertex, const LVector4f& color,
			const LVector2f& uv = LVecBase2f::zero());
public:
	DebugDrawMeshDrawer(NodePath render, NodePath camera, int budget=1000);
	virtual ~DebugDrawMeshDrawer();

	void reset();

	virtual void depthMask(bool state);
	virtual void texture(bool state);
	virtual void begin(duDebugDrawPrimitives prim, float size = 1.0f);
	virtual void vertex(const float* pos, unsigned int color);
	virtual void vertex(const float x, const float y, const float z, unsigned int color);
	virtual void vertex(const float* pos, unsigned int color, const float* uv);
	virtual void vertex(const float x, const float y, const float z, unsigned int color, const float u, const float v);
	virtual void end();

};

/// MeshDrawer debug draw implementation.
class DebugDrawMeshDrawerTMP : public duDebugDraw
{
protected:
	///The render node path.
	NodePath m_render;
	///The camera node path.
	NodePath m_camera;
	///Depth Mask.
	bool m_depthMask;
	///Texture.
	bool m_texture;
	///Inner MeshDrawer.
	MeshDrawer* m_generator;
	///Budget.
	int m_budget;
	///The current GeomPrimitive and draw type.
	duDebugDrawPrimitives m_prim;
	enum {DU_NULL_PRIM=-1};
	///Size (for points)
	float m_size;
	///Line previous store.
	LVecBase3f m_lineVertex;
	LVecBase4f m_lineColor;
	LVecBase2f m_lineUV;
	int m_lineIdx;
	///Triangle previous store.
	LVecBase3f m_triVertex[2];
	LVecBase4f m_triColor[2];
	LVecBase2f m_triUV[2];
	int m_triIdx;
	///Quad previous store.
	LVecBase3f m_quadVertex[2];
	LVecBase4f m_quadColor[2];
	LVecBase2f m_quadUV[2];
	int m_quadIdx;

private:
	///Helper
	void doVertex(const LVector3f& vertex, const LVector4f& color,
			const LVector2f& uv = LVecBase2f::zero());
public:
	DebugDrawMeshDrawerTMP(NodePath render, NodePath camera, int budget=1000);
	virtual ~DebugDrawMeshDrawerTMP();

	void startDraw();
	void stopDraw();

	virtual void depthMask(bool state);
	virtual void texture(bool state);
	virtual void begin(duDebugDrawPrimitives prim, float size = 1.0f);
	virtual void vertex(const float* pos, unsigned int color);
	virtual void vertex(const float x, const float y, const float z, unsigned int color);
	virtual void vertex(const float* pos, unsigned int color, const float* uv);
	virtual void vertex(const float x, const float y, const float z, unsigned int color, const float u, const float v);
	virtual void end();

};

inline float red(unsigned int color)
{
	return ((float) ((color & 0x000000FF) >> 0)) / 255.0;
}
inline float green(unsigned int color)
{
	return ((float) ((color & 0x0000FF00) >> 8)) / 255.0;
}
inline float blue(unsigned int color)
{
	return ((float) ((color & 0x00FF0000) >> 16)) / 255.0;
}
inline float alpha(unsigned int color)
{
	return ((float) ((color & 0xFF000000) >> 24)) / 255.0;
}

/// stdio file implementation.
class FileIO : public duFileIO
{
	FILE* m_fp;
	int m_mode;
public:
	FileIO();
	virtual ~FileIO();
	bool openForWrite(const char* path);
	bool openForRead(const char* path);
	virtual bool isWriting() const;
	virtual bool isReading() const;
	virtual bool write(const void* ptr, const size_t size);
	virtual bool read(void* ptr, const size_t size);
};

#endif // SAMPLEINTERFACES_H

