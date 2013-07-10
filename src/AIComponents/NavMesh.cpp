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

NavMesh::NavMesh(SMARTPTR(NavMeshTemplate)tmpl):
	mGeom(0),
	mCtx(new BuildContext),
	mMeshName("")
{
	CHECKEXISTENCE(GameAIManager::GetSingletonPtr(),
			"NavMesh::NavMesh: invalid GameAIManager")

}

NavMesh::~NavMesh()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	delete mCtx;
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

NavMeshType* NavMesh::getNavMeshType()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mNavMeshType;
}

NAVMESHTYPE NavMesh::getNavMeshTypeEnum()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mNavMeshTypeEnum;
}

InputGeom* NavMesh::getInputGeom()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mGeom;
}

dtNavMesh* NavMesh::getNavMesh()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mNavMeshType->getNavMesh();
}

dtNavMeshQuery* NavMesh::getNavMeshQuery()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mNavMeshType->getNavMeshQuery();
}

dtCrowd* NavMesh::getCrowd()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mNavMeshType->getCrowd();
}

float NavMesh::getAgentRadius()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mNavMeshType->getAgentRadius();
}

float NavMesh::getAgentHeight()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mNavMeshType->getAgentHeight();
}

float NavMesh::getAgentClimb()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mNavMeshType->getAgentClimb();
}

LVecBase3f NavMesh::getBoundsMin()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	CHECKEXISTENCE(mGeom,
			"NavMesh::getBoundsMin: invalid InputGeom")
	return RecastToLVecBase3f(mGeom->getMeshBoundsMin());
}

LVecBase3f NavMesh::getBoundsMax()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	CHECKEXISTENCE(mGeom,
			"NavMesh::getBoundsMax: invalid InputGeom")
	return RecastToLVecBase3f(mGeom->getMeshBoundsMax());
}

void NavMesh::setNavMeshSettings(const NavMeshSettings& settings)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	mNavMeshType->setNavMeshSettings(settings);
}

NavMeshSettings NavMesh::getNavMeshSettings()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mNavMeshType->getNavMeshSettings();
}

void NavMesh::resetNavMeshSettings()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mNavMeshType->resetNavMeshSettings();
}

bool NavMesh::loadMesh(NodePath model)
{
	bool result = true;
	mGeom = new InputGeom;
	mMeshName = model.get_name();
	//
	if (not mGeom->loadMesh(mCtx, NULL, model))
	{
		delete mGeom;
		mGeom = NULL;
#ifdef ELY_DEBUG
		mCtx->dumpLog("Geom load log %s:", mMeshName.c_str());
#endif
		result = false;
	}
	return result;
}

#ifdef ELY_DEBUG
	NodePath NavMesh::getDebugNodePath() const
	{
		//lock (guard) the mutex
		HOLDMUTEX(mMutex)

		return mDebugNodePath;
	}

	void NavMesh::initDebug(NodePath debugNodePath)
	{

	}

	void NavMesh::debug(bool enable)
	{
		//lock (guard) the mutex
		HOLDMUTEX(mMutex)

		if (mDebugNodePath.is_empty())
		{
			return;
		}
		if (enable)
		{
			if (mDebugNodePath.is_hidden())
			{
				mDebugNodePath.show();
			}
		}
		else
		{
			if (not mDebugNodePath.is_hidden())
			{
				mDebugNodePath.hide();
			}
		}
	}
#endif

//TypedObject semantics: hardcoded
TypeHandle NavMesh::_type_handle;

}  // namespace ely
