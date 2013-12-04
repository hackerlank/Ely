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
 * \file /Ely/include/AIComponents/OpenSteerVehicleTemplate.h
 *
 * \date 04/dic/2013 (09:23:38)
 * \author consultit
 */
#ifndef OPENSTEERVEHICLETEMPLATE_H_
#define OPENSTEERVEHICLETEMPLATE_H_

#include "ObjectModel/ComponentTemplate.h"

namespace ely
{

class OpenSteerVehicleTemplate: public ely::ComponentTemplate
{
public:
	OpenSteerVehicleTemplate(PandaFramework* pandaFramework,
			WindowFramework* windowFramework);
	virtual ~OpenSteerVehicleTemplate();
};

} /* namespace ely */

#endif /* OPENSTEERVEHICLETEMPLATE_H_ */
