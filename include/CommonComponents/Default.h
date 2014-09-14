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
 * \file /Ely/include/CommonComponents/Default.h
 *
 * \date 27/ago/2014 (10:45:48)
 * \author consultit
 */

#ifndef DEFAULT_H_
#define DEFAULT_H_

#include "ObjectModel/Component.h"

namespace ely
{
class DefaultTemplate;

/**
 * \brief Component representing a minimum default implementation.
 *
 * It could be useful for attaching events' callbacks.
 *
 * > **XML Param(s)**:
 * param | type | default | note
 * ------|------|---------|-----
 * | - | - | - | -
 *
 * \note parts inside [] are optional.\n
 */

class Default: public Component
{
protected:
	friend class DefaultTemplate;

	virtual void reset();
	virtual bool initialize();
	virtual void onAddToObjectSetup();
	virtual void onRemoveFromObjectCleanup();
	virtual void onAddToSceneSetup();
	virtual void onRemoveFromSceneCleanup();

public:
	Default();
	Default(SMARTPTR(DefaultTemplate)tmpl);
	virtual ~Default();

	virtual ComponentFamilyType familyType() const;
	virtual ComponentType componentType() const;

private:

	///TypedObject semantics: hardcoded
public:
	static TypeHandle get_class_type()
	{
		return _type_handle;
	}
	static void init_type()
	{
		Component::init_type();
		register_type(_type_handle, "Default", Component::get_class_type());
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

inline void Default::reset()
{
}

inline bool Default::initialize()
{
	bool result = true;
	//
	return result;
}

inline void Default::onAddToObjectSetup()
{
}

inline void Default::onRemoveFromObjectCleanup()
{
}

inline void Default::onAddToSceneSetup()
{
}

inline void Default::onRemoveFromSceneCleanup()
{
}

} /* namespace ely */

#endif /* DEFAULT_H_ */
