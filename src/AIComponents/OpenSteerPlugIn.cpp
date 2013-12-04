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
 * \file /Ely/src/AIComponents/OpenSteerPlugIn.cpp
 *
 * \date 04/dic/2013 (09:11:38)
 * \author consultit
 */

#include "AIComponents/OpenSteerPlugIn.h"
#include "AIComponents/OpenSteerPlugInTemplate.h"
#include "Game/GameAIManager.h"

namespace ely
{

OpenSteerPlugIn::OpenSteerPlugIn()
{
	// TODO Auto-generated constructor stub

}

OpenSteerPlugIn::OpenSteerPlugIn(SMARTPTR(OpenSteerPlugInTemplate)tmpl)
{
	CHECK_EXISTENCE_DEBUG(GameAIManager::GetSingletonPtr(),
	"OpenSteerPlugIn::OpenSteerPlugIn: invalid GameAIManager")

	mTmpl = tmpl;
	reset();
}

OpenSteerPlugIn::~OpenSteerPlugIn()
{
	// TODO Auto-generated destructor stub
}

ComponentFamilyType OpenSteerPlugIn::familyType() const
{
	return mTmpl->familyType();
}

ComponentType OpenSteerPlugIn::componentType() const
{
	return mTmpl->componentType();
}

bool OpenSteerPlugIn::initialize()
{
	bool result = true;
	//set OpenSteerPlugIn parameters (store internally for future use)
	//PlugIn type
	std::string plugInStr = mTmpl->parameter(std::string("plugin_type"));
	//
	return result;
}

void OpenSteerPlugIn::onAddToObjectSetup()
{
}

void OpenSteerPlugIn::onRemoveFromObjectCleanup()
{
	//
	reset();
}

void OpenSteerPlugIn::onAddToSceneSetup()
{
}

void OpenSteerPlugIn::onRemoveFromSceneCleanup()
{
}

void OpenSteerPlugIn::update(void* data)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	float dt = *(reinterpret_cast<float*>(data));

#ifdef TESTING
	dt = 0.016666667; //60 fps
#endif

}

//TypedObject semantics: hardcoded
TypeHandle OpenSteerPlugIn::_type_handle;

} /* namespace ely */
