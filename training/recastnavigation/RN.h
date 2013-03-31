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
 * \file /Ely/training/recastnavigation/RN.h
 *
 * \date 29/mar/2013 (18:59:07)
 * \author marco
 */

#ifndef RN_H_
#define RN_H_

#include <lvector3.h>
#include <nodePath.h>
#include <animControlCollection.h>
#include <genericAsyncTask.h>

//rn
#include <cstring>
#include <cmath>
#include <Recast.h>
#include <DetourNavMesh.h>
#include <DetourNavMeshQuery.h>
#include <DetourNavMeshBuilder.h>
#include <DetourCommon.h>
#include "InputGeom.h"
#include "Sample_SoloMesh.h"
#include "CrowdTool.h"

//RN
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

class Agent
{
	int m_agentIdx;
	NodePath m_pandaNP;
	AnimControlCollection* m_anims;
public:
	Agent(int agentIdx, NodePath pandaNP, AnimControlCollection* anims) :
			m_agentIdx(agentIdx), m_pandaNP(pandaNP), m_anims(anims)
	{
	}
	int getIdx()
	{
		return m_agentIdx;
	}
	void updatePosDir(const float* p, const float* v);
};

class RN
{
	std::string m_meshName;
	InputGeom* m_geom;
	BuildContext* m_ctx;

	Sample_SoloMesh* m_sampleSolo;

	CrowdTool* m_crowdTool;

	std::list<Agent*> m_agents;

public:
	RN();
	~RN();
	CrowdTool* getNavMeshTool()
	{
		return m_crowdTool;
	}
	//common
	bool loadMesh(const std::string& path, const std::string& meshName);
	bool buildNavMesh();
	void createSoloMeshSample();
	static AsyncTask::DoneStatus ai_update(GenericAsyncTask* task, void* data);

	//crowd tool
	void setCrowdTool();
	void addCrowdAgent(NodePath pandaNP, LPoint3f pos, float agentSpeed,
			AnimControlCollection* anims = NULL);
	void setCrowdTarget(LPoint3f pos);

};

#endif /* RN_H_ */
