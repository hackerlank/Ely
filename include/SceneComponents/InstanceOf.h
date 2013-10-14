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
 * \file /Ely/include/SceneComponents/InstanceOf.h
 *
 * \date 20/mag/2012 (09:42:38)
 * \author consultit
 */

#ifndef INSTANCEOF_H_
#define INSTANCEOF_H_

#include <nodePath.h>
#include "ObjectModel/Component.h"
#include "ObjectModel/Object.h"

namespace ely
{
class InstanceOfTemplate;

/**
 * \brief Component representing an instance of another object.
 *
 * XML Param(s):
 * - "instance_of"  		|single|no default
 * - "scale"  				|single|"1.0,1.0,1.0" (specified
 * as "scalex[,scaley,scalez]")
 *
 * \note parts inside [] are optional.\n
 */
class InstanceOf: public Component
{
protected:
	friend class InstanceOfTemplate;

	virtual void reset();
	virtual bool initialize();
	virtual void onAddToObjectSetup();
	virtual void onRemoveFromObjectCleanup();
	virtual void onAddToSceneSetup();
	virtual void onRemoveFromSceneCleanup();

public:
	InstanceOf();
	InstanceOf(SMARTPTR(InstanceOfTemplate)tmpl);
	virtual ~InstanceOf();

	virtual ComponentFamilyType familyType() const;
	virtual ComponentType componentType() const;

	/**
	 * \brief Gets/sets the node path associated to this instance of.
	 */
	///@{
	NodePath getNodePath() const;
	void setNodePath(const NodePath& nodePath);
	///@}

	/**
	 * \brief Gets a reference to the instanced object.
	 * @return The reference to the instanced object.
	 */
	///@{
	SMARTPTR(Object) getInstancedObject() const;
	///@}

private:
	///The NodePath associated to this instance of.
	NodePath mNodePath;

	/**
	 * \name Main parameters.
	 */
	///@{
	///Instance of object id.
	ObjectId mInstanceOfId;
	///The instanced object.
	SMARTPTR(Object) mInstancedObject;
	///Old owner object node path.
	NodePath mOldObjectNodePath;
	///@}

	///Scaling  (default: (1.0,1.0,1.0)).
	float mScale[3];

	///TypedObject semantics: hardcoded
public:
	static TypeHandle get_class_type()
	{
		return _type_handle;
	}
	static void init_type()
	{
		Component::init_type();
		register_type(_type_handle, "InstanceOf", Component::get_class_type());
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

inline SMARTPTR(Object)InstanceOf::getInstancedObject() const
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return mInstancedObject;
}

inline void InstanceOf::reset()
{
	//
	mNodePath = NodePath();
	mInstanceOfId = ObjectId();
	mInstancedObject.clear();
	mOldObjectNodePath = NodePath();
	mScale[0] = mScale[1] = mScale[2] = 1.0;
}

inline void InstanceOf::onAddToSceneSetup()
{
}

inline void InstanceOf::onRemoveFromSceneCleanup()
{
}

inline NodePath InstanceOf::getNodePath() const
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return mNodePath;
}

inline void InstanceOf::setNodePath(const NodePath& nodePath)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	mNodePath = nodePath;
}

}  // namespace ely

#endif /* INSTANCEOF_H_ */
