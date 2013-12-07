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
 * \file /Ely/include/AIComponents/SteerPlugIn.h
 *
 * \date 04/dic/2013 (09:11:38)
 * \author consultit
 */
#ifndef STEERPLUGIN_H_
#define STEERPLUGIN_H_

#include "ObjectModel/Component.h"
#include <OpenSteer/PlugIn.h>

namespace ely
{

class SteerPlugInTemplate;

/**
 * \brief Component implementing OpenSteer PlugIns.
 *
 * \see http://opensteer.sourceforge.net
 *
 * This component should be used only in association to stationary
 * (i.e. is_steady=true) "Scene" components.\n
 *
 * XML Param(s):
 * - "plugin_type"					|single|"solo" (values: solo|tile|obstacle)
 *
 * \note parts inside [] are optional.\n
 */
class SteerPlugIn: public Component
{
protected:
	friend class SteerPlugInTemplate;

	virtual void reset();
	virtual bool initialize();
	virtual void onAddToObjectSetup();
	virtual void onRemoveFromObjectCleanup();
	virtual void onAddToSceneSetup();
	virtual void onRemoveFromSceneCleanup();

public:
	SteerPlugIn();
	SteerPlugIn(SMARTPTR(SteerPlugInTemplate)tmpl);
	virtual ~SteerPlugIn();

	virtual ComponentFamilyType familyType() const;
	virtual ComponentType componentType() const;

	/**
	 * \brief Updates OpenSteer underlying component.
	 *
	 * Will be called automatically by an ai manager update.
	 * @param data The custom data.
	 */
	virtual void update(void* data);

private:
	///Current underlying PlugIn.
	OpenSteer::PlugIn* mPlugIn;

	///TypedObject semantics: hardcoded
public:
	static TypeHandle get_class_type()
	{
		return _type_handle;
	}
	static void init_type()
	{
		Component::init_type();
		register_type(_type_handle, "SteerPlugIn", Component::get_class_type());
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

///inline definitions

inline void SteerPlugIn::reset()
{
	//
	mPlugIn = NULL;

}

} /* namespace ely */

#endif /* STEERPLUGIN_H_ */
