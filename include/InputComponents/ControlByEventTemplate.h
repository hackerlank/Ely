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
 * \file /Ely/include/InputComponents/ControlByEventTemplate.h
 *
 * \date 31/mag/2012 (16:44:06)
 * \author marco
 */

#ifndef CONTROLBYEVENTTEMPLATE_H_
#define CONTROLBYEVENTTEMPLATE_H_

#include <pandaFramework.h>
#include <typedObject.h>
#include "ObjectModel/Component.h"
#include "ObjectModel/ComponentTemplate.h"

class ControlByEventTemplate: public ComponentTemplate
{
public:
	ControlByEventTemplate(GameManager* gameManager);
	virtual ~ControlByEventTemplate();

	const virtual ComponentType componentType() const;
	const virtual ComponentFamilyType familyType() const;

	virtual Component* makeComponent(ComponentId& compId);

	virtual void reset();

	/**
	 * \brief Gets/sets the GameManager (i.e. PandaFramework).
	 * @return A reference to the GameManager.
	 */
	GameManager*& gameManager();

	/**
	 * \name Event names getters/setters.
	 * \brief Get/set the event names associated to this component.
	 *
	 * Each of these events will be associated to an handler that will
	 * control one specific movement of the object associated to this component.
	 */
	///@{
	std::string& backwardEvent();
	std::string& boostKey();
	std::string& downEvent();
	std::string& forwardEvent();
	std::string& strafeLeftEvent();
	std::string& strafeRightEvent();
	std::string& rollLeftEvent();
	std::string& rollRightEvent();
	std::string& upEvent();
	///@}

private:
	///The GameManager (i.e. PandaFramework).
	GameManager* mGameManager;
	///@{
	/// Event names.
	std::string mForward;
	std::string mBackward;
	std::string mStrafeLeft;
	std::string mStrafeRight;
	std::string mUp;
	std::string mDown;
	std::string mRollLeft;
	std::string mRollRight;
	std::string mBoostKey;
	///@}

	///TypedObject semantics: hardcoded
public:
	static TypeHandle get_class_type()
	{
		return _type_handle;
	}

	static void init_type()
	{
		ComponentTemplate::init_type();
		register_type(_type_handle, "ControlByEventTemplate",
				ComponentTemplate::get_class_type());
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

#endif /* CONTROLBYEVENTTEMPLATE_H_ */
