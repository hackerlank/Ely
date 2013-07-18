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

#ifndef CONVEXVOLUMETOOL_H
#define CONVEXVOLUMETOOL_H

#include "NavMeshType.h"

namespace ely
{

// Tool to create convex volumess for InputGeom

class ConvexVolumeTool : public NavMeshTypeTool
{
	NavMeshType* m_sample;
	int m_areaType;
	float m_polyOffset;
	float m_boxHeight;
	float m_boxDescent;
	
	static const int MAX_PTS = 12;
	float m_pts[MAX_PTS*3];
	int m_npts;
	int m_hull[MAX_PTS];
	int m_nhull;
	
public:
	ConvexVolumeTool(NodePath renderDebug=NodePath(),
			NodePath camera=NodePath(), int budget=1000, bool singleMesh=false);
	~ConvexVolumeTool();
	
	virtual int type() { return TOOL_CONVEX_VOLUME; }
	virtual void init(NavMeshType* sample);
	virtual void reset();
	virtual void handleClick(const float* s, const float* p, bool shift);
	virtual void handleStep();
	virtual void handleToggle();
	virtual void handleUpdate(const float dt);
	virtual void handleRender();

	void setAreaType(NavMeshPolyAreasEnum type);
	NavMeshPolyAreasEnum getAreaType();
};

} // namespace ely

#endif // CONVEXVOLUMETOOL_H
