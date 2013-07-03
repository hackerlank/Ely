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
 * \file /Ely/include/AIComponents/RecastNavigation/DebugInterfaces.h
 *
 * \date 02/lug/2013 09:16:25
 * \author marco
 */

#ifndef DEBUGINTERFACES_H_
#define DEBUGINTERFACES_H_

#include "common.h"
#include <DebugDraw.h>
#include <Recast.h>
#include "PerfTimer.h"
#include <nodePath.h>
#include <geomVertexWriter.h>

namespace ely
{
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
	PT(GeomVertexData) m_vertexData;
	///The current vertex index.
	int m_vertexIdx;
	///The current GeomVertexWriters.
	GeomVertexWriter m_vertex, m_color, m_texcoord;
	///The current GeomPrimitive and draw type.
	PT(GeomPrimitive) m_geomPrim;
	duDebugDrawPrimitives m_prim;
	///Size (for points)
	float m_size;
	///The current Geom.
	PT(Geom) m_geom;
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

} // namespace ely

#endif /* DEBUGINTERFACES_H_ */
