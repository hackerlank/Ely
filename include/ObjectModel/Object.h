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
 * \file /Ely/include/ObjectModel/Object.h
 *
 * \date 07/mag/2012 (18:10:37)
 * \author consultit
 */

#ifndef OBJECT_H_
#define OBJECT_H_

#include "Utilities/Tools.h"
#include <pandaFramework.h>
#include <nodePath.h>
#include <typedWritableReferenceCount.h>
#include "ObjectTemplate.h"

namespace ely
{
/**
 * \brief Object instance identifier type (by default the name
 * of the NodePath component).
 */
typedef std::string ObjectId;

/**
 * \brief The object is the basic entity that can exist in the game world.
 *
 * Because Panda3d is used as game engine, a object should
 * "compose" the different aspects (graphics, sound, ai,
 * physics, animation, etc...), already existing in Panda3d.
 * An object can have only one component of a given family.
 * Each object has a NodePath embedded.\n
 * An object is an aggregation of components: a “has a,” or whole/part,
 * relationship where neither class owns the other.\n
 * Object can be initialized after it is added to the scene, by
 * an initialization function, whose name is <OBJECTID>_initialization
 * loaded at runtime from a dynamic linked library (referenced by the
 * macro INITIALIZATIONS_LA).
 *
 * XML Param(s):
 * - "parent"  				|single|no default
 * - "store_params"			|single|"false"
 * - "is_steady"  			|single|"false"
 * - "pos"  				|single|"0.0,0.0,0.0"
 * - "rot"  				|single|"0.0,0.0,0.0"
 *
 * \note The Object Model is based on the article "Game Object
 * Component System" by Chris Stoy in "Game Programming Gems 6" book.
 */
class Object: public TypedWritableReferenceCount
{
private:
	friend class ObjectTemplateManager;

	/**
	 * \brief Adds a component to this object.
	 *
	 * It will add the component to the object, and if a component of
	 * that family already existed it'll be replaced by this new component
	 * (and its ownership released by the object).\n
	 * \note When you add a component, its template should contain its
	 * initialization parameters; this means that when you want to add
	 * a component you should first prepare its template with
	 * initialization parameter and then actually add the component.
	 * @param component The new component to add.
	 * @param existingObject False if this is an object under construction, true
	 * if it is an already existing object.
	 * @return SMARTPTR(NULL) if there wasn't a component of that family, otherwise
	 * the previous component.
	 */
	SMARTPTR(Component) addComponent(SMARTPTR(Component) component,
			bool existingObject=false);

	/**
	 * \brief Removes a component from this object.
	 *
	 * It will remove the component from the object, and its ownership
	 * released by the object.\n
	 * @param component The component to remove.
	 * @return True if successfully removed, false otherwise.
	 */
	bool removeComponent(SMARTPTR(Component) component);

	/**
	 * \brief On addition to scene setup.
	 *
	 * Gives an object the ability to perform the
	 * addition to scene setup and any required initialization.\n
	 * Called only by ObjectTemplateManager::createObject().\n
	 * \note this method is called exclusively by the object creation thread,
	 * i.e. before it will be publicly accessible, so no other thread can access
	 * the object during its execution, then it doesn't need to hold the mutex.
	 */
	void sceneSetup();

public:
	/**
	 * \brief Constructor.
	 */
	Object(const ObjectId& objectId, SMARTPTR(ObjectTemplate)tmpl);

	/**
	 * \brief Destructor.
	 */
	virtual ~Object();

	/**
	 * \brief Clears the table of all components of this object.
	 */
	void clearComponents();

	/**
	 * \brief These methods will store/free, in an internal storage, the
	 * object template and component templates' parameters.\n
	 * \note These methods are intended to be used during the object
	 * creation if the object is susceptible to be cloned with clones
	 * having (more or less) the same parameters values, so they are
	 * available at once. When done storage can be freed.
	 * @param objTmplParams The object template parameters table.
	 * @param compTmplParams The component templates' parameters' table.
	 */
	///@{
	void storeParameters( const ParameterTable& objTmplParams,
			const ParameterTableMap& compTmplParams);
	void freeParameters();
	///@}

	/**
	 * \brief Gets the component of that given family.
	 * @param familyID The family of the component.
	 * @return The component, or NULL if no component of that
	 * family exists.
	 */
	SMARTPTR(Component) getComponent(const ComponentFamilyType& familyID) const;

	/**
	 * \brief Returns the number of components.
	 * @return The number of components.
	 */
	unsigned int numComponents() const;

	/**
	 * \brief On world creation setup.
	 *
	 * Gives an object the ability to perform any given
	 * initialization after the entire world creation.
	 */
	void worldSetup();

	/**
	 * \brief Gets a reference to the id of this object.
	 * @return The id of this object.
	 */
	ObjectId objectId() const;

	/**
	 * \brief Gets/sets the node path of this object.
	 * @return The node path of this object.
	 */
	///@{
	NodePath getNodePath() const;
	void setNodePath(const NodePath& nodePath);
	///@}

	/**
	 * \brief NodePath conversion function.
	 */
	operator NodePath() const;

	/**
	 * \brief Gets a reference to the object template.
	 * @return The a reference to the object template.
	 */
	SMARTPTR(ObjectTemplate) const objectTmpl() const;

	/**
	 * \brief Gets the object steady-ness.
	 *
	 * This flag represents if this object doesn't move in the world.
	 * Various components can set or get this value to implement
	 * some optimization. By default false (i.e. object is dynamic).
	 */
	bool isSteady() const;

	/**
	 * \brief Initialization function type.
	 */
	typedef void (*PINITIALIZATION)(SMARTPTR(Object), const ParameterTable& paramTable,
			PandaFramework* pandaFramework, WindowFramework* windowFramework);

	/**
	 * \brief Returns the object template and component templates'
	 * parameters tables.
	 * @return  The object template and component templates'
	 * parameters tables.
	 */
	///@{
	ParameterTable getStoredObjTmplParams() const;
	ParameterTableMap getStoredCompTmplParams() const;
	///@}

	/**
	 * \brief Get the mutex to lock the entire structure.
	 * @return The internal mutex.
	 */
	ReMutex& getMutex();

private:
	///The template used to construct this component.
	SMARTPTR(ObjectTemplate) const mTmpl;
	///The NodePath associated to this object.
	NodePath mNodePath;
	///Unique identifier for this object (read only after creation).
	ObjectId mObjectId;
	///@{
	///Table of all components indexed by component family type.
	typedef std::map<const ComponentFamilyType, SMARTPTR(Component)> ComponentTable;
	ComponentTable mComponents;
	///@}
	///Staedy flag: if this object doesn't move in the world.
	///Various components can set or get this value to implement
	///some optimization.
	bool mIsSteady;

	///Handle of the library of initialization functions.
	lt_dlhandle mInitializationLib;
	///Helper flag.
	bool mInitializationsLoaded;

	/**
	 * \name Helper functions to load/unload initialization functions.
	 */
	///@{
	void loadInitializationFunctions();
	void unloadInitializationFunctions();
	///@}

	///Parameters tables.
	ParameterTable mObjTmplParams;
	ParameterTableMap mCompTmplParams;

	///The (reentrant) mutex associated with this object.
	ReMutex mMutex;

	///TypedObject semantics: hardcoded
public:
	static TypeHandle get_class_type()
	{
		return _type_handle;
	}
	static void init_type()
	{
		TypedObject::init_type();
		register_type(_type_handle, "Object", TypedObject::get_class_type());
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

inline ObjectId Object::objectId() const
{
	return mObjectId;
}

inline NodePath Object::getNodePath() const
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mNodePath;
}

inline void Object::setNodePath(const NodePath& nodePath)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	mNodePath = nodePath;
}

inline Object::operator NodePath() const
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mNodePath;
}

inline unsigned int Object::numComponents() const
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return static_cast<unsigned int>(mComponents.size());
}

inline SMARTPTR(ObjectTemplate)const Object::objectTmpl() const
{
	return mTmpl;
}

inline ReMutex& Object::getMutex()
{
	return mMutex;
}

inline ParameterTable Object::getStoredObjTmplParams() const
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mObjTmplParams;
}

inline ParameterTableMap Object::getStoredCompTmplParams() const
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mCompTmplParams;
}

inline void Object::storeParameters(const ParameterTable& objTmplParams,
		const ParameterTableMap& compTmplParams)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	mObjTmplParams = objTmplParams;
	mCompTmplParams = compTmplParams;
}

inline void Object::freeParameters()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	mObjTmplParams.clear();
	mCompTmplParams.clear();
}

inline bool Object::isSteady() const
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mIsSteady;
}

}  // namespace ely

#endif /* OBJECT_H_ */
