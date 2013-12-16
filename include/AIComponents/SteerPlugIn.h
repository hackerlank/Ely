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
#include "SteerVehicle.h"
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
 * - "plugin_type"					|single|"one_turning" (values: one_turning|)
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

	struct Result: public Component::Result
	{
		Result(int value):Component::Result(value)
		{
		}
		enum
		{
		};
	};

	/**
	 * \brief Adds a SteerVehicle component to the OpenSteer handling
	 * mechanism.
	 *
	 * If SteerVehicle belongs to any SteerPlugIn it is not added.\n
	 * @param steerVehicle The SteerVehicle to add.
	 * @return Result::OK on successful addition, various error conditions otherwise.
	 */
	Result addSteerVehicle(SMARTPTR(SteerVehicle)steerVehicle);

	/**
	 * \brief Removes a SteerVehicle component from the OpenSteer handling
	 * mechanism.
	 *
	 * If SteerVehicle doesn't belong to any SteerPlugIn it is not removed.\n
	 * @param steerVehicle The SteerVehicle to remove.
	 * @return Result::OK on successful removal, various error conditions otherwise.
	 */
	Result removeSteerVehicle(SMARTPTR(SteerVehicle)steerVehicle);

	/**
	 * \brief Updates OpenSteer underlying component.
	 *
	 * Will be called automatically by an ai manager update.
	 * @param data The custom data.
	 */
	virtual void update(void* data);

	/**
	 * \name AbstractPlugIn reference getter & conversion function.
	 */
	///@{
	OpenSteer::AbstractPlugIn& getAbstractPlugIn();
	operator OpenSteer::AbstractPlugIn&();
	///@}

private:
	///Current underlying AbstractPlugIn.
	OpenSteer::AbstractPlugIn* mPlugIn;

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

inline void SteerPlugIn::onAddToSceneSetup()
{
}

inline void SteerPlugIn::onRemoveFromSceneCleanup()
{
}

inline OpenSteer::AbstractPlugIn& SteerPlugIn::getAbstractPlugIn()
{
	return *mPlugIn;
}

inline SteerPlugIn::operator OpenSteer::AbstractPlugIn&()
{
	return *mPlugIn;
}

} /* namespace ely */

#endif /* STEERPLUGIN_H_ */
