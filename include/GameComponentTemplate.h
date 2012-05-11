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
 * \file /Ely/include/GameComponentTemplate.h
 *
 * \date 11/mag/2012 (13:09:41)
 * \author marco
 */

#ifndef GAMECOMPONENTTEMPLATE_H_
#define GAMECOMPONENTTEMPLATE_H_

#include <referenceCount.h>
#include "GameObjectComponent.h"

class GameComponentTemplate: public ReferenceCount
{
public:
	/**
	 * \brief Constructor.
	 */
	GameComponentTemplate();
	/**
	 * \brief Destructor.
	 */
	virtual ~GameComponentTemplate() = 0;

	/**
	 * \brief Get the id of the component created.
	 * @return The id of the component created.
	 */
	virtual const GameObjectComponentId& componentID() const = 0;
	/**
	 * \brief Get the family id of the component created.
	 * @return The family id of the component created.
	 */
	virtual const GameObjectComponentId& familyID() const = 0;

};

#endif /* GAMECOMPONENTTEMPLATE_H_ */
