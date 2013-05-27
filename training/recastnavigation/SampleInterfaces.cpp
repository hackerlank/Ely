#define _USE_MATH_DEFINES
#include <cmath>
#include <cstdio>
#include <cstdarg>
#include <cstring>
#include "SampleInterfaces.h"
#include "Recast.h"
#include "RecastDebugDraw.h"
#include "DetourDebugDraw.h"
#include "PerfTimer.h"
#include "RN.h"
//#include "SDL.h"
//#include "SDL_opengl.h"

#ifdef WIN32
#	define snprintf _snprintf
#endif

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

//class GLCheckerTexture
//{
//	unsigned int m_texId;
//public:
//	GLCheckerTexture() : m_texId(0)
//	{
//	}
//
//	~GLCheckerTexture()
//	{
//		if (m_texId != 0)
//			glDeleteTextures(1, &m_texId);
//	}
//	void bind()
//	{
//		if (m_texId == 0)
//		{
//			// Create checker pattern.
//			const unsigned int col0 = duRGBA(215,215,215,255);
//			const unsigned int col1 = duRGBA(255,255,255,255);
//			static const int TSIZE = 64;
//			unsigned int data[TSIZE*TSIZE];
//
//			glGenTextures(1, &m_texId);
//			glBindTexture(GL_TEXTURE_2D, m_texId);
//
//			int level = 0;
//			int size = TSIZE;
//			while (size > 0)
//			{
//				for (int y = 0; y < size; ++y)
//					for (int x = 0; x < size; ++x)
//						data[x+y*size] = (x==0 || y==0) ? col0 : col1;
//				glTexImage2D(GL_TEXTURE_2D, level, GL_RGBA, size,size, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
//				size /= 2;
//				level++;
//			}
//
//			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
//			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//		}
//		else
//		{
//			glBindTexture(GL_TEXTURE_2D, m_texId);
//		}
//	}
//};
//GLCheckerTexture g_tex;


void DebugDrawGL::depthMask(bool state)
{
	//glDepthMask(state ? GL_TRUE : GL_FALSE);
}

void DebugDrawGL::texture(bool state)
{
	if (state)
	{
//		glEnable(GL_TEXTURE_2D);
//		g_tex.bind();
	}
	else
	{
//		glDisable(GL_TEXTURE_2D);
	}
}

void DebugDrawGL::begin(duDebugDrawPrimitives prim, float size)
{
//	switch (prim)
//	{
//		case DU_DRAW_POINTS:
//			glPointSize(size);
//			glBegin(GL_POINTS);
//			break;
//		case DU_DRAW_LINES:
//			glLineWidth(size);
//			glBegin(GL_LINES);
//			break;
//		case DU_DRAW_TRIS:
//			glBegin(GL_TRIANGLES);
//			break;
//		case DU_DRAW_QUADS:
//			glBegin(GL_QUADS);
//			break;
//	};
}

void DebugDrawGL::vertex(const float* pos, unsigned int color)
{
//	glColor4ubv((GLubyte*)&color);
//	glVertex3fv(pos);
}

void DebugDrawGL::vertex(const float x, const float y, const float z, unsigned int color)
{
//	glColor4ubv((GLubyte*)&color);
//	glVertex3f(x,y,z);
}

void DebugDrawGL::vertex(const float* pos, unsigned int color, const float* uv)
{
//	glColor4ubv((GLubyte*)&color);
//	glTexCoord2fv(uv);
//	glVertex3fv(pos);
}

void DebugDrawGL::vertex(const float x, const float y, const float z, unsigned int color, const float u, const float v)
{
//	glColor4ubv((GLubyte*)&color);
//	glTexCoord2f(u,v);
//	glVertex3f(x,y,z);
}

void DebugDrawGL::end()
{
//	glEnd();
//	glLineWidth(1.0f);
//	glPointSize(1.0f);
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
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)
}

void DebugDrawPanda3d::depthMask(bool state)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	m_depthMask = state;
}

void DebugDrawPanda3d::texture(bool state)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	m_texture = state;
}

void DebugDrawPanda3d::begin(duDebugDrawPrimitives prim, float size)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

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
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	doVertex(Recast3fToLVecBase3f(pos[0], pos[1], pos[2]),
			LVector4f(red(color), green(color), blue(color), alpha(color)));
}

void DebugDrawPanda3d::vertex(const float x, const float y, const float z, unsigned int color)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	doVertex(Recast3fToLVecBase3f(x, y, z),
			LVector4f(red(color), green(color), blue(color), alpha(color)));
}

void DebugDrawPanda3d::vertex(const float* pos, unsigned int color, const float* uv)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	doVertex(Recast3fToLVecBase3f(pos[0], pos[1], pos[2]),
			LVector4f(red(color), green(color), blue(color), alpha(color)),
			LVector2f(uv[0], uv[1]));
}

void DebugDrawPanda3d::vertex(const float x, const float y, const float z, unsigned int color, const float u, const float v)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	doVertex(Recast3fToLVecBase3f(x, y, z),
			LVector4f(red(color), green(color), blue(color), alpha(color)),
			LVector2f(u, v));
}

void DebugDrawPanda3d::end()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	m_geomPrim->close_primitive();
	m_geom = new Geom(m_vertexData);
	m_geom->add_primitive(m_geomPrim);
	ostringstream idx;
	idx << m_geomNodeNPCollection.size();
	m_geomNodeNP = NodePath(new GeomNode("DebugDrawPanda3d_GeomNode_" + idx.str()));
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
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return m_geomNodeNPCollection[i];
}

void DebugDrawPanda3d::removeGeomNodes()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	std::vector<NodePath>::iterator iter;
	for (iter = m_geomNodeNPCollection.begin();
			iter != m_geomNodeNPCollection.end(); ++iter)
	{
		(*iter).remove_node();
	}
	m_geomNodeNPCollection.clear();
}

ReMutex& DebugDrawPanda3d::getMutex()
{
	return mMutex;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FileIO::FileIO() :
	m_fp(0),
	m_mode(-1)
{
}

FileIO::~FileIO()
{
	if (m_fp) fclose(m_fp);
}

bool FileIO::openForWrite(const char* path)
{
	if (m_fp) return false;
	m_fp = fopen(path, "wb");
	if (!m_fp) return false;
	m_mode = 1;
	return true;
}

bool FileIO::openForRead(const char* path)
{
	if (m_fp) return false;
	m_fp = fopen(path, "rb");
	if (!m_fp) return false;
	m_mode = 2;
	return true;
}

bool FileIO::isWriting() const
{
	return m_mode == 1;
}

bool FileIO::isReading() const
{
	return m_mode == 2;
}

bool FileIO::write(const void* ptr, const size_t size)
{
	if (!m_fp || m_mode != 1) return false;
	fwrite(ptr, size, 1, m_fp);
	return true;
}

bool FileIO::read(void* ptr, const size_t size)
{
	if (!m_fp || m_mode != 2) return false;
	fread(ptr, size, 1, m_fp);
	return true;
}
