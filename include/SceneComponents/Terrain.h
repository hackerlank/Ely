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
 * \file /Ely/include/SceneComponents/Terrain.h
 *
 * \date 15/ago/2012 (10:00:07)
 * \author marco
 */

#ifndef TERRAIN_H_
#define TERRAIN_H_

#include <string>
#include <list>
#include <cstdlib>
#include <nodePath.h>
#include <filename.h>
#include <typedObject.h>
#include "ObjectModel/Component.h"
#include "ObjectModel/Object.h"

class TerrainTemplate;

/**
 * \brief Component representing the terrain model of an object.
 *
 * XML Param(s):
 * \li \c "heightfield_file"  		|single|no default
 * \li \c "height_scale"			|single|"1.0"
 * \li \c "width_scale"				|single|"1.0"
 * \li \c "update_interval"			|single|"0.1"
 * \li \c
 * \li \c
 * \li \c
 * \li \c
 * \li \c
 * #        self.terrain = 0.1
#        self.lastTesselateTime = -1
        self.terrainHeightScale = 200
        self.terrainWidthScale = 2
        terrainNearPercent = 0.1
        terrainFarPercent = 1.0
        terrainBlockSize = 64
 */
class Terrain: public Component
{
public:
	Terrain();
	Terrain(TerrainTemplate* tmpl);
	virtual ~Terrain();

	const virtual ComponentFamilyType familyType() const;
	const virtual ComponentType componentType() const;

	virtual bool initialize();
	virtual void onAddToObjectSetup();

	/**
	 * \brief Gets/sets the node path associated to this model.
	 * @return The node path associated to this model.
	 */
	///@{
	NodePath getNodePath() const;
	void setNodePath(const NodePath& nodePath);
	///@}

private:
	///The template used to construct this component.
	TerrainTemplate* mTmpl;
	///The NodePath associated to this model.
	NodePath mNodePath;

	///TypedObject semantics: hardcoded
public:
	static TypeHandle get_class_type()
	{
		return _type_handle;
	}
	static void init_type()
	{
		Component::init_type();
		register_type(_type_handle, "Terrain", Component::get_class_type());
	}
	virtual TypeHandle get_type() const
	{
		return get_class_type();
	}
	virtual TypeHandle force_init_type()
	{
		init_type();
		return get_class_type();
	}

private:
	static TypeHandle _type_handle;

};

#endif /* TERRAIN_H_ */
