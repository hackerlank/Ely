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
 * \file /Ely/src/AIComponents/NavMesh.cpp
 *
 * \date 23/giu/2013 (18:51:03)
 * \author consultit
 */

#include "AIComponents/NavMesh.h"
#include "AIComponents/NavMeshTemplate.h"
#include "ObjectModel/Object.h"
#include "Game/GameAIManager.h"

namespace ely
{

NavMesh::NavMesh()
{
	// TODO Auto-generated constructor stub
}

NavMesh::NavMesh(SMARTPTR(NavMeshTemplate)tmpl)
{
	CHECKEXISTENCE(GameAIManager::GetSingletonPtr(),
			"NavMesh::NavMesh: invalid GameAIManager")
}

NavMesh::~NavMesh()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)
}

const ComponentFamilyType NavMesh::familyType() const
{
	return mTmpl->familyType();
}

const ComponentType NavMesh::componentType() const
{
	return mTmpl->componentType();
}

bool NavMesh::initialize()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	bool result = true;
	//set NavMesh parameters
	//

	//
	return result;
}

void NavMesh::onAddToObjectSetup()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	//add only for a not empty object node path
	if (mOwnerObject->getNodePath().is_empty())
	{
		return;
	}

	//first check if owner Geom is a triangle mesh (from a model component)


	//setup event callbacks if any
	setupEvents();
	//register event callbacks if any
	registerEventCallbacks();
}

void NavMesh::onAddToSceneSetup()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	//add only for a not empty object node path
	if (mOwnerObject->getNodePath().is_empty())
	{
		return;
	}
}

void NavMesh::update(void* data)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	float dt = *(reinterpret_cast<float*>(data));

#ifdef TESTING
	dt = 0.016666667; //60 fps
#endif

}

InputGeom* NavMesh::getInputGeom()
{
	return mGeom;
}

dtNavMesh* NavMesh::getNavMesh()
{
	return mNavMesh;
}

dtNavMeshQuery* NavMesh::getNavMeshQuery()
{
	return mNavQuery;
}

dtCrowd* NavMesh::getCrowd()
{
	return mCrowd;
}

virtual float NavMesh::getAgentRadius()
{
	return mAgentRadius;
}

virtual float NavMesh::getAgentHeight()
{
	return mAgentHeight;
}

virtual float NavMesh::getAgentClimb()
{
	return mAgentMaxClimb;
}

void NavMesh::setNavMeshSettings(const NavMeshSettings& settings)
{
	mCellSize = settings.mCellSize;
	mCellHeight = settings.mCellHeight;
	mAgentHeight = settings.mAgentHeight;
	mAgentRadius = settings.mAgentRadius;
	mAgentMaxClimb = settings.mAgentMaxClimb;
	mAgentMaxSlope = settings.mAgentMaxSlope;
	mRegionMinSize = settings.mRegionMinSize;
	mRegionMergeSize = settings.mRegionMergeSize;
	mMonotonePartitioning = settings.mMonotonePartitioning;
	mEdgeMaxLen = settings.mEdgeMaxLen;
	mEdgeMaxError = settings.mEdgeMaxError;
	mVertsPerPoly = settings.mVertsPerPoly;
	mDetailSampleDist = settings.mDetailSampleDist;
	mDetailSampleMaxError = settings.mDetailSampleMaxError;
}

NavMeshSettings NavMesh::getNavMeshSettings()
{
	NavMeshSettings settings;
	settings.mCellSize = mCellSize;
	settings.mCellHeight = mCellHeight;
	settings.mAgentHeight = mAgentHeight;
	settings.mAgentRadius = mAgentRadius;
	settings.mAgentMaxClimb = mAgentMaxClimb;
	settings.mAgentMaxSlope = mAgentMaxSlope;
	settings.mRegionMinSize = mRegionMinSize;
	settings.mRegionMergeSize = mRegionMergeSize;
	settings.mMonotonePartitioning = mMonotonePartitioning;
	settings.mEdgeMaxLen = mEdgeMaxLen;
	settings.mEdgeMaxError = mEdgeMaxError;
	settings.mVertsPerPoly = mVertsPerPoly;
	settings.mDetailSampleDist = mDetailSampleDist;
	settings.mDetailSampleMaxError = mDetailSampleMaxError;
	return settings;
}

//TypedObject semantics: hardcoded
TypeHandle NavMesh::_type_handle;

}  // namespace ely
