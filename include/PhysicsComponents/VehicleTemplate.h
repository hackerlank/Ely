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
 * \file /Ely/include/PhysicsComponents/VehicleTemplate.h
 *
 * \date 15/set/2013 (10:45:28)
 * \author consultit
 */

#ifndef VEHICLETEMPLATE_H_
#define VEHICLETEMPLATE_H_

#include "ComponentTemplate.h"

namespace ely
{

class VehicleTemplate: public ely::ComponentTemplate
{
public:
	VehicleTemplate();
	virtual ~VehicleTemplate();
};

} /* namespace ely */
#endif /* VEHICLETEMPLATE_H_ */
