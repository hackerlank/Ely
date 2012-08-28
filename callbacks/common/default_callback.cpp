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
 * \file /Ely/callbacks/common/default_callback.cpp
 *
 * \date 20/ago/2012 (10:14:38)
 * \author marco
 */

#include "../all_configs.h"

void default_callback__(const Event* event, void* data)
{
#ifdef DEBUG
	//get data
	SMARTPTR(Component) component = (Component*) data;
	std::string evtStr = event->get_name();
	std::string compTypeStr = std::string(component->componentType());
	std::string objIdStr = std::string(component->getOwnerObject()->objectId());
	//show message
	std::cout << "Default Callback called: [EVENT: '" << evtStr
			<< "'][COMPONENT: '" << compTypeStr << "'][OBJECT: '" << objIdStr
			<< "']" << std::endl;
#endif
}

