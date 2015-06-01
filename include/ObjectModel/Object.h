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
#include "Component.h"
#include <pandaFramework.h>
#include <nodePath.h>
#include <typedWritableReferenceCount.h>

namespace ely
{

class ObjectTemplate;

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
 * an initialization function, whose name can be specified as parameter
 * or by a default which is the string "<OBJECTID>_initialization",
 * loaded at runtime from a dynamic linked library
 * (\see GameManager::GameDataInfo::INITIALIZATIONS).
 *
 * > **XML Param(s)**:
 * param | type | default | note
 * ------|------|---------|-----
 * | *parent*  				|single| - | -
 * | *store_params*			|single| *false* | -
 * | *is_steady*  			|single| *false* | -
 * | *pos*  				|single| 0.0,0.0,0.0 | -
 * | *rot*  				|single| 0.0,0.0,0.0 | -
 * | *init_func* 			|single| - | -
 *
 * \note The Object Model is based on the article "Game Object
 * Component System" by Chris Stoy in "Game Programming Gems 6" book.
 * \note parts inside [] are optional.\n
 */
class Object: public TypedWritableReferenceCount
{
private:
	friend class ObjectTemplateManager;

	/**
	 * \brief Constructor.
	 */
	Object(const ObjectId& objectId, SMARTPTR(ObjectTemplate)tmpl);

	/**
	 * \brief Resets all object data members.
	 */
	void doReset();

	/**
	 * \brief Adds a component to this object.
	 *
	 * It will add the component to the object.\n
	 * @param component The new component to add.
	 * @param familyId The family type of the component.
	 * @return True if successfully removed, false otherwise.
	 */
	bool doAddComponent(SMARTPTR(Component) component,
			const ComponentFamilyType& familyId);

	/**
	 * \brief Removes a component from this object.
	 *
	 * It will remove the component from the object.\n
	 * @param component The component to remove.
	 * @return True if successfully removed, false otherwise.
	 */
	bool doRemoveComponent(SMARTPTR(Component) component,
			const ComponentFamilyType& familyId);

	///List of all <family,component>s pair stored by insertion order.
	typedef std::pair<const ComponentFamilyType, SMARTPTR(Component)> FamilyTypeComponentPair;
	typedef std::list<FamilyTypeComponentPair> ComponentOrderedList;
	/**
	 * \brief Gets a reference to the components list.
	 *
	 * @return A reference to the components list.
	 */
	ComponentOrderedList& doGetComponents();

	/**
	 * \brief On remove Object cleanup.
	 *
	 * Gives an object the ability to perform the cleanup and any
	 * required finalization before being definitively destroyed.\n
	 * Called only by ObjectTemplateManager::destroyObject().\n
	 */
	void onRemoveObjectCleanup();

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
	void onAddToSceneSetup();

	/**
	 * \brief On remove from scene cleanup.
	 *
	 * Gives an object the ability to perform the
	 * removing from scene cleanup and any required finalization.\n
	 * Called only by ObjectTemplateManager::destroyObject().\n
	 * \note this method is called by the object destruction thread,
	 * i.e. when it is be publicly accessible, so other thread can access
	 * the object during its execution, then it does need to hold the mutex.
	 */
	void onRemoveFromSceneCleanup();

	/**
	 * \brief This methods will store, in an internal storage, the
	 * object template and component templates' parameters.\n
	 * \note This method is intended to be used during the object
	 * creation if the object is susceptible to be cloned with clones
	 * having (more or less) the same parameters values, so they are
	 * available at once.
	 * @param objTmplParams The object template parameters table.
	 * @param compTmplParams The component templates' parameters' table.
	 */
	void doStoreParameters( const ParameterTable& objTmplParams,
			const ParameterTableMap& compTmplParams);

public:

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
	SMARTPTR(Component) getComponent(const ComponentFamilyType& familyId) const;

	/**
	 * \brief Returns the number of components.
	 * @return The number of components.
	 */
	unsigned int numComponents() const;

	/**
	 * \brief On world creation setup.
	 *
	 * Gives an object the ability to perform any given
	 * initialization after the entire world creation.\n
	 */
	void worldSetup();

	/**
	 * \brief Gets a reference to the id of this object.
	 * @return The id of this object.
	 */
	ObjectId objectId() const;

	/**
	 * \brief NodePath getter/setter & conversion function.
	 */
	///@{
	NodePath getNodePath() const;
	void setNodePath(const NodePath& nodePath);
	operator NodePath() const;
	///@}

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
	 * \brief This method will free the stored object template
	 * and component templates' parameters.\n
	 * \note This method is intended to be used when done with
	 * the parameters' storage.
	 */
	void freeParameters();

	/**
	 * \brief Gets/sets this object's owner, i.e. the object responsible
	 * of its lifetime.
	 * @param owner The owner object.
	 */
	///@{
	SMARTPTR(Object) getOwner() const;
	void setOwner(SMARTPTR(Object) owner);
	///@}

#ifdef ELY_THREAD
	/**
	 * \brief Get the mutex to lock the entire structure.
	 * @return The internal mutex.
	 */
	ReMutex& getMutex();
#endif

private:
	///The template used to construct this component.
	SMARTPTR(ObjectTemplate) const mTmpl;
	///The underlying NodePath (read-only after creation & before destruction).
	NodePath mNodePath;
	///Unique identifier for this object (read only after creation).
	ObjectId mObjectId;
	///The owner of this object (responsible for its lifetime).
	SMARTPTR(Object) mOwner;
	///@{
	ComponentOrderedList mComponents;
	class IsFamily
	{
		ComponentFamilyType mFamilyType;
	public:
		IsFamily(const ComponentFamilyType& familyType):mFamilyType(familyType)
		{
		}
		~IsFamily()
		{
		}
		bool operator()(const FamilyTypeComponentPair& familyComponentPair)
		{
			return (familyComponentPair.first == mFamilyType);
		}
	};
	///@}
	///Steady flag: if this object doesn't move in the world.
	///Various components can set or get this value to implement
	///some optimization.
	bool mIsSteady;

	///Handle of the library of initialization functions.
	lt_dlhandle mInitializationLib;
	///Helper flag.
	bool mInitializationsLoaded;
	///Initialization function name (optional).
	std::string mInititializationFuncName;
	///Initialization function.
	PINITIALIZATION mInitializationFunction;

	/**
	 * \name Helper functions to load/unload initialization functions.
	 */
	///@{
	void doLoadInitializationFunctions();
	void doUnloadInitializationFunctions();
	///@}

	///Parameters tables.
	ParameterTable mObjTmplParams;
	ParameterTableMap mCompTmplParams;

#ifdef ELY_THREAD
	///The mutex associated with this object.
	ReMutex mMutex;
#endif

	///TypedObject semantics: hardcoded
public:
	static TypeHandle get_class_type()
	{
		return _type_handle;
	}
	static void init_type()
	{
		TypedWritableReferenceCount::init_type();
		register_type(_type_handle, "Object",
				TypedWritableReferenceCount::get_class_type());
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
	return mNodePath;
}

inline void Object::setNodePath(const NodePath& nodePath)
{
	//called only by ObjectTemplateManager methods which
	//already lock the object mutex

	mNodePath = nodePath;
}

inline Object::operator NodePath() const
{
	return mNodePath;
}

inline Object::ComponentOrderedList& Object::doGetComponents()
{
	return mComponents;
}

inline unsigned int Object::numComponents() const
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return static_cast<unsigned int>(mComponents.size());
}

inline SMARTPTR(ObjectTemplate)const Object::objectTmpl() const
{
	return mTmpl;
}

#ifdef ELY_THREAD
inline ReMutex& Object::getMutex()
{
	return mMutex;
}
#endif

inline ParameterTable Object::getStoredObjTmplParams() const
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return mObjTmplParams;
}

inline ParameterTableMap Object::getStoredCompTmplParams() const
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return mCompTmplParams;
}

inline void Object::doStoreParameters(const ParameterTable& objTmplParams,
		const ParameterTableMap& compTmplParams)
{
	mObjTmplParams = objTmplParams;
	mCompTmplParams = compTmplParams;
}

inline void Object::freeParameters()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	mObjTmplParams.clear();
	mCompTmplParams.clear();
}

inline bool Object::isSteady() const
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return mIsSteady;
}

inline void Object::doReset()
{
	//
	mComponents.clear();
	mIsSteady = false;
}

inline void Object::setOwner(SMARTPTR(Object) owner)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	mOwner = owner;
}


inline SMARTPTR(Object) Object::getOwner() const
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return mOwner;
}

///Template

class ObjectTemplateManager;

/**
 * \brief Object type.
 *
 * This type identifies the name of template that create these type of objects.
 */
typedef std::string ObjectType;

/**
 * \brief Class storing all of objects templates used to create an object.
 */
class ObjectTemplate: public TypedWritableReferenceCount
{
public:
	/**
	 * \brief Type used for the ordered list of the component templates.
	 */
	typedef std::vector<SMARTPTR(ComponentTemplate)> ComponentTemplateList;

	/**
	 * \brief Constructor.
	 * @param name The name of this template.
	 * @param objectTmplMgr The ObjectTemplateManager.
	 * @param pandaFramework The PandaFramework.
	 * @param windowFramework The WindowFramework.
	 */
	ObjectTemplate(const ObjectType& name, ObjectTemplateManager* objectTmplMgr,
			PandaFramework* pandaFramework, WindowFramework* windowFramework);

	/**
	 * \brief Destructor.
	 */
	virtual ~ObjectTemplate();

	/**
	 * \brief For the object this template is designed to create,
	 * this function sets the (mandatory) parameters to their default values.
	 */
	void setParametersDefaults();

	/**
	 * \name Parameters management.
	 * \brief Sets the parameters of the object, this template is
	 * designed to create, to custom values.
	 *
	 * These parameters overwrite (and/or are added to) the parameters defaults
	 * set by setParametersDefaults.
	 * @param parameterTable The table of (parameter,value).
	 */
	void setParameters(const ParameterTable& parameterTable);

	/**
	 * \brief Clears the table of all component templates of this
	 * object template.
	 */
	void clearComponentTemplates();

	/**
	 * \brief Gets a reference to the name (i.e. the object type) of
	 * this object template.
	 * @return The name of this object template.
	 */
	ObjectType objectType() const;

	/**
	 * \brief Gets the component template list.
	 * @return The component template list.
	 */
	ComponentTemplateList getComponentTemplates() const;

	/**
	 * \brief Adds a component template.
	 * @param componentTmpl The component template.
	 */
	void addComponentTemplate(SMARTPTR(ComponentTemplate) componentTmpl);

	/**
	 * \brief Gets a component template given the component type it can create.
	 * @param componentType The component type.
	 * @return The component template, NULL if it doesn't exist.
	 */
	SMARTPTR(ComponentTemplate) getComponentTemplate(const ComponentType&componentType) const;

	/**
	 * \brief Gets the parameter value associated to the object.
	 * @param paramName The name of the parameter.
	 * @return The value of the parameter, empty string if none exists.
	 */
	std::string parameter(const std::string& paramName) const;
	/**
	 * \brief Gets the parameter multi-values associated to the object.
	 * @param paramName The name of the parameter.
	 * @return The value list  of the parameter, empty list if none exists.
	 */
	std::list<std::string> parameterList(const std::string& paramName);

	/**
	 * \brief Gets the entire parameter table.
	 * @return The parameter table.
	 */
	ParameterTable getParameterTable() const;

	/**
	 * \brief Gets/sets the PandaFramework.
	 * @return A reference to the PandaFramework.
	 */
	PandaFramework* const pandaFramework() const;

	/**
	 * \brief Gets/sets the WindowFramework.
	 * @return A reference to the WindowFramework.
	 */
	WindowFramework* const windowFramework() const;

	/**
	 * \brief Adds a common "multi-valued" parameter to a component type
	 * of this object.
	 *
	 * Any parameter value is interpreted as a "compound" one, i.e. as having
	 * the form: "value1:value2:...:valueN" and each valueX is stored in a
	 * list as returned by componentParameterValues().\n
	 *
	 * @param parameterName The parameter name.
	 * @param parameterValue The parameter value.
	 * @param componentType The component type the parameter is related to.
	 */
	void addComponentParameter(const std::string& parameterName,
			const std::string& parameterValue, ComponentType compType);

	/**
	 * \brief Checks if a name/value pair is an allowed parameter/value
	 * for a given component type of this object.
	 * @param name The name to check.
	 * @param value The value to check.
	 * @param componentType The component type.
	 * @return True if the name/value pair match an allowed parameter/value
	 * for a given component, false otherwise.
	 */
	bool isComponentParameterValue(const std::string& name, const std::string& value,
			ComponentType compType);

	/**
	 * \brief Gets all values of the common parameter associated to
	 * the component type of the object.
	 *
	 * @param paramName The name of the parameter.
	 * @param componentType The component type.
	 * @return The value list  of the parameter, empty list if none exists.
	 */
	std::list<std::string> componentParameterValues(const std::string& paramName,
			ComponentType compType);

#ifdef ELY_THREAD
	/**
	 * \brief Get the mutex to lock the entire structure.
	 * @return The internal mutex.
	 */
	ReMutex& getMutex();
#endif

private:
	///Name identifying this object template.
	ObjectType mName;
	///Ordered list of all component templates.
	ComponentTemplateList mComponentTemplates;
	///The ObjectTemplateManager.
	ObjectTemplateManager* const mObjectTmplMgr;
	///Parameter table.
	ParameterTable mParameterTable;
	///The PandaFramework.
	PandaFramework* mPandaFramework;
	///The WindowFramework.
	WindowFramework* mWindowFramework;

	///The table of ParameterTables indexed by component type.\n
	///These represent the allowed common attributes shared by each
	///component of a given type, belonging to any object of a given type.
	std::map<ComponentType, ParameterTable> mComponentParameterTables;

#ifdef ELY_THREAD
	///The mutex associated with this template.
	ReMutex mMutex;
#endif

	///TypedObject semantics: hardcoded
public:
	static TypeHandle get_class_type()
	{
		return _type_handle;
	}
	static void init_type()
	{
		TypedWritableReferenceCount::init_type();
		register_type(_type_handle, "ObjectTemplate",
				TypedWritableReferenceCount::get_class_type());
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

struct idIsEqualTo
{
	idIsEqualTo(const ComponentType& compType) :
			mComponentType(compType)
	{
	}
	ComponentType mComponentType;
	bool operator()(const SMARTPTR(ComponentTemplate)componentTmpl)
	{
		return componentTmpl->componentType() == mComponentType;
	}
};

///inline definitions

inline ObjectTemplate::ComponentTemplateList ObjectTemplate::getComponentTemplates() const
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return mComponentTemplates;
}

inline ParameterTable ObjectTemplate::getParameterTable() const
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return mParameterTable;
}

inline ObjectType ObjectTemplate::objectType() const
{
	return mName;
}

inline PandaFramework* const ObjectTemplate::pandaFramework() const
{
	return mPandaFramework;
}

inline WindowFramework* const ObjectTemplate::windowFramework() const
{
	return mWindowFramework;
}

#ifdef ELY_THREAD
inline ReMutex& ObjectTemplate::getMutex()
{
	return mMutex;
}
#endif

}  // namespace ely

#endif /* OBJECT_H_ */
