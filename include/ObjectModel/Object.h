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
 * \author marco
 */

#ifndef OBJECT_H_
#define OBJECT_H_

#include "Utilities/Tools.h"

#include <map>
#include <string>
#include <pandaFramework.h>
#include <windowFramework.h>
#include <nodePath.h>
#include <typedWritableReferenceCount.h>
#include <reMutex.h>
#include <reMutexHolder.h>
#include "ObjectTemplate.h"
#include "Component.h"

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
 * - "pos_x"  				|single|"0.0"
 * - "pos_y"  				|single|"0.0"
 * - "pos_z"  				|single|"0.0"
 * - "rot_h"  				|single|"0.0"
 * - "rot_p"  				|single|"0.0"
 * - "rot_r"  				|single|"0.0"
 *
 * \note The Object Model is based on the article "Game Object
 * Component System" by Chris Stoy in "Game Programming Gems 6" book.
 */
class Object: public TypedWritableReferenceCount
{
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
	 * \brief Gets the component of that given family.
	 * @param familyID The family of the component.
	 * @return The component, or NULL if no component of that
	 * family exists.
	 */
	SMARTPTR(Component) getComponent(const ComponentFamilyType& familyID);

	/**
	 * \brief Sets a new component into this object.
	 *
	 * It will add the component in the object to the passed component,
	 * and if a component of that family already existed it'll be
	 * replaced by this new component (and its ownership released by
	 * the object).\n
	 * \note When you add a component, its template should contain its
	 * initialization parameters; this means that when you want to add
	 * a component (perhaps by replacing an old one), you should first
	 * prepare its template with initialization parameter and then actually
	 * add the component.
	 * @param newComponent The new component to add.
	 * @return SMARTPTR(NULL) if there wasn't a component of that family, otherwise
	 * the previous component.
	 */
	SMARTPTR(Component) addComponent(SMARTPTR(Component) newComponent);

	/**
	 * \brief Clears the table of all components of this object.
	 */
	void clearComponents();

	/**
	 * \brief Returns the number of components.
	 * @return The number of components.
	 */
	unsigned int numComponents();

	/**
	 * \brief On addition to scene setup.
	 *
	 * Gives an object the ability to perform the
	 * addition to scene setup and any required initialization.
	 */
	void sceneSetup();

	/**
	 * \brief Gets a reference to the id of this object.
	 * @return The id of this object.
	 */
	const ObjectId& objectId() const;

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
	operator NodePath();

	/**
	 * \brief Gets a reference to the object template.
	 * @return The a reference to the object template.
	 */
	SMARTPTR(ObjectTemplate) const objectTmpl() const;

	/**
	 * \brief Gets/sets a reference to the steady flag.
	 *
	 * This flag represents if this object doesn't move in the world.
	 * Various components can set or get this value to implement
	 * some optimization. By default false (i.e. object is dynamic).
	 */
	///@{
	bool isSteady();
	void setSteady(bool value);
	///@}

	/**
	 * \brief Initialization function type.
	 */
	typedef void (*PINITIALIZATION)(SMARTPTR(Object), const ParameterTable& paramTable,
			PandaFramework* pandaFramework, WindowFramework* windowFramework);

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
	 * \brief Returns the object template and component templates'
	 * parameters tables.
	 * @return  The object template and component templates'
	 * parameters tables.
	 */
	///@{
	ParameterTable getStoredObjTmplParams();
	ParameterTableMap getStoredCompTmplParams();
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
	///Unique identifier for this object.
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

#endif /* OBJECT_H_ */
