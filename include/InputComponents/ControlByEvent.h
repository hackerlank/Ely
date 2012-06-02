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
 * \file /Ely/include/InputComponents/ControlByEvent.h
 *
 * \date 31/mag/2012 (16:42:14)
 * \author marco
 */

#ifndef CONTROLBYEVENT_H_
#define CONTROLBYEVENT_H_

#include <string>
#include <nodePath.h>
#include <typedObject.h>
#include "ObjectModel/Component.h"

class ControlByEventTemplate;

/**
 * \brief Component representing the control of movement
 * of an object through keyboard and mouse events.
 */
class ControlByEvent: public Component
{
public:
	ControlByEvent();
	ControlByEvent(ControlByEventTemplate* tmpl);
	virtual ~ControlByEvent();

	const virtual ComponentFamilyType familyType() const;
	const virtual ComponentType componentType() const;

	virtual bool initialize();
	virtual void onAddSetup();

	/**
	 * \brief Get a reference to the node path controlled by this component.
	 * @return The node path controlled by this component.
	 */
	NodePath& nodePath();
	/**
	 * \brief NodePath conversion function.
	 */
	operator NodePath();

private:
	///The template used to construct this component.
	ControlByEventTemplate* mTmpl;
	///The NodePath controlled by this component.
	NodePath mNodePath;
	///@{
	///Key controls.
	bool mForward, mBackward, mLeft, mRight, mUp, mDown, mRollLeft, mRollRight;
	///@}

	///@{
	/// Sensitivity settings.
	float mSpeed, mSpeedFast;
	float mMovSens, mMovSensFast;
	float mRollSens;
	float mSensX, mSensY;
	///@}

	///TypedObject semantics: hardcoded
public:
	static TypeHandle get_class_type()
	{
		return _type_handle;
	}
	static void init_type()
	{
		Component::init_type();
		register_type(_type_handle, "ControlByEvent",
				Component::get_class_type());
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

#endif /* CONTROLBYEVENT_H_ */
