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
 * \file /Ely/include/PhysicsComponents/GhostTemplate.h
 *
 * \date 13/ago/2014 (11:39:39)
 * \author consultit
 */

#ifndef GHOSTTEMPLATE_H_
#define GHOSTTEMPLATE_H_

#include <ObjectModel/ComponentTemplate.h>

namespace ely
{

class GhostTemplate: public ComponentTemplate
{
public:
	GhostTemplate();
	virtual ~GhostTemplate();
};

} /* namespace ely */

#endif /* GHOSTTEMPLATE_H_ */
