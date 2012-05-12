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
 * \file /Ely/include/ComponentTemplate.h
 *
 * \date 11/mag/2012 (13:09:41)
 * \author marco
 */

#ifndef COMPONENTTEMPLATE_H_
#define COMPONENTTEMPLATE_H_

#include <referenceCount.h>
#include "Object.h"
#include "Component.h"

class ComponentTemplate: public ReferenceCount
{
public:
	/**
	 * \brief Constructor.
	 */
	ComponentTemplate();
	/**
	 * \brief Destructor.
	 */
	virtual ~ComponentTemplate() = 0;

	/**
	 * \brief Get the id of the component created.
	 * @return The id of the component created.
	 */
	virtual const ComponentId& componentID() const = 0;
	/**
	 * \brief Get the family id of the component created.
	 * @return The family id of the component created.
	 */
	virtual const ComponentFamilyId& familyID() const = 0;

	/**
	 * \brief Create the actual component of that family.
	 * @return The component just created.
	 */
	virtual Component* makeComponent() = 0;
};

#endif /* COMPONENTTEMPLATE_H_ */
