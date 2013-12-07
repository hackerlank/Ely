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
 * \file /Ely/src/AIComponents/SteerPlugIn.cpp
 *
 * \date 04/dic/2013 (09:11:38)
 * \author consultit
 */

#include "AIComponents/SteerPlugIn.h"
#include "AIComponents/SteerPlugInTemplate.h"
#include "Game/GameAIManager.h"

namespace ely
{

SteerPlugIn::SteerPlugIn()
{
	// TODO Auto-generated constructor stub

}

SteerPlugIn::SteerPlugIn(SMARTPTR(SteerPlugInTemplate)tmpl)
{
	CHECK_EXISTENCE_DEBUG(GameAIManager::GetSingletonPtr(),
	"OpenSteerPlugIn::OpenSteerPlugIn: invalid GameAIManager")

	mTmpl = tmpl;
	reset();
}

SteerPlugIn::~SteerPlugIn()
{
	// TODO Auto-generated destructor stub
}

ComponentFamilyType SteerPlugIn::familyType() const
{
	return mTmpl->familyType();
}

ComponentType SteerPlugIn::componentType() const
{
	return mTmpl->componentType();
}

bool SteerPlugIn::initialize()
{
	bool result = true;
	//set SteerPlugIn parameters (store internally for future use)
	//PlugIn type
	std::string plugInStr = mTmpl->parameter(std::string("plugin_type"));
	//
	return result;
}

void SteerPlugIn::onAddToObjectSetup()
{
}

void SteerPlugIn::onRemoveFromObjectCleanup()
{
	//
	reset();
}

void SteerPlugIn::onAddToSceneSetup()
{
}

void SteerPlugIn::onRemoveFromSceneCleanup()
{
}

void SteerPlugIn::update(void* data)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	float dt = *(reinterpret_cast<float*>(data));

#ifdef TESTING
	dt = 0.016666667; //60 fps
#endif

}

//TypedObject semantics: hardcoded
TypeHandle SteerPlugIn::_type_handle;

} /* namespace ely */
