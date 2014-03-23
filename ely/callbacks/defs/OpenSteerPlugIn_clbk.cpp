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
 * \file /Ely/ely/callbacks/defs/OpenSteerPlugIn_clbk.cpp
 *
 * \date 11/gen/2014 (10:19:51)
 * \author consultit
 */

#include "../common_configs.h"
#include "AIComponents/SteerPlugIn.h"

using namespace ely;

///Avoid name mangling
#ifdef __cplusplus
extern "C"
{
#endif

///OpenSteerPlugIn related
#ifdef ELY_DEBUG
CALLBACK handleHits;
CALLBACK steerPluginsToggleDebug;
#endif

#ifdef __cplusplus
}
#endif

#ifdef ELY_DEBUG
void handleHits(const Event* event, void* data)
{
	//get first parameter
	SMARTPTR(SteerVehicle)steerVehicle =
			DCAST(SteerVehicle, event->get_parameter(0).get_ptr());
	//
	PRINT_DEBUG("Got: " << event->get_name() << " - Object: " <<
	std::string(steerVehicle->getOwnerObject()->objectId()));
}

void steerPluginsToggleDebug(const Event* event, void* data)
{
	//get data
	SMARTPTR(SteerPlugIn)steerPlugIn = reinterpret_cast<SteerPlugIn*>(data);

	if (steerPlugIn->getDrawer3dDebugNodePath().is_hidden() and
			steerPlugIn->getDrawer2dDebugNodePath().is_hidden())
	{
		steerPlugIn->debug(true);
	}
	else
	{
		steerPlugIn->debug(false);
	}
}
#endif

///Init/end functions: see common_configs.cpp
void OpenSteerPlugIn_clbkInit()
{
}
void OpenSteerPlugIn_clbkEnd()
{
}

