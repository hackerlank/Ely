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
 * \date 2012-05-07 
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
 * \brief Object instance identifier type.
 */
typedef std::string ObjectId;

/**
 * \brief The Object is the basic entity that can exist in the game world.
 *
 * A Object should "compose", as far as possible, the different features
 * (graphics, sound, ai, physics, animation, etc...), supplied by Panda3d
 * game engine. When a feature is missing or considered of low quality,
 * other external libraries may be used.\n
 * Each Object has an embedded NodePath.\n
 * An Object is an aggregation of Components: a “has a,” or whole/part,
 * relationship where neither class owns the other.
 * An Object can have any number of Components of different types, but only
 * one can belong to any family type: two Components with different types
 * belonging to the same family type, cannot coexist into an Object.\n
 * Objects can be initialized after they are added to the game scene, through
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
	 * \brief Resets all Object data members.
	 */
	void doReset();

	/**
	 * \brief Adds a Component to this Object.
	 *
	 * A Component is added only if there is not another with
	 * its same family type.\n
	 * @param component The new Component to add.
	 * @return True if successfully removed, false otherwise.
	 */
	bool doAddComponent(SMARTPTR(Component) component);

	/**
	 * \brief Removes a Component from this Object.
	 *
	 * Component is removed only if it's there.\n
	 * @param component The Component to remove.
	 * @return True if successfully removed, false otherwise.
	 */
	bool doRemoveComponent(SMARTPTR(Component) component);

	///List of all <family,Component> pairs.
	typedef std::pair<const ComponentFamilyType, SMARTPTR(Component)> FamilyTypeComponentPair;
	typedef std::list<FamilyTypeComponentPair> FamilyTypeComponentList;
	/**
	 * \brief Gets a reference to the Component list.
	 *
	 * The list is ordered by insertion: i.e Component's
	 * position in the list reflects the Component's
	 * (relative) insertion order.\n
	 * @return A reference to the Component list.
	 */
	FamilyTypeComponentList& doGetComponents();

	/**
	 * \brief On remove Object cleanup.
	 *
	 * Gives the Object the ability to perform cleanup and any
	 * required finalization before being definitively destroyed.\n
	 * Called only by ObjectTemplateManager::destroyObject().\n
	 */
	void onRemoveObjectCleanup();

	/**
	 * \brief On addition to scene setup.
	 *
	 * Gives the Object the ability to perform any required
	 * setup just "after" its addition to the game scene.\n
	 * Called only by ObjectTemplateManager::createObject().\n
	 * \note this method is called exclusively by the Object creation thread,
	 * i.e. before it will be publicly accessible, so no other thread can access
	 * the Object during its execution, then it doesn't need to hold the mutex.
	 */
	void onAddToSceneSetup();

	/**
	 * \brief On remove from scene cleanup.
	 *
	 * Gives the Object the ability to perform any required
	 * cleanup just "before" its removal from the game scene.\n
	 * Called only by ObjectTemplateManager::destroyObject().\n
	 * \note this method is called by the Object destruction thread,
	 * i.e. when it is be publicly accessible, so other thread can access
	 * the Object during its execution, then it does need to hold the mutex.
	 */
	void onRemoveFromSceneCleanup();

	/**
	 * \brief This methods will store, in an internal storage, the
	 * Object and Components' parameters.\n
	 *
	 * Both Object and Components' parameters, are stored into the
	 * corresponding templates.
	 * \note This method is intended to be used during the Object
	 * creation if the Object is susceptible to be cloned with clones
	 * having (approximately) the same parameters values, so they are
	 * available at once.
	 * @param objTmplParams The Object parameters' table.
	 * @param compTmplParams The Components' parameters' table.
	 */
	void doStoreParameters( const ParameterTable& objTmplParams,
			const ParameterTableMap& compTmplParams);

public:

	/**
	 * \brief Destructor.
	 */
	virtual ~Object();

	/**
	 * \name Get a Component.
	 */
	///@{
	/**
	 * \brief Gets the Component of a given family type.
	 * @param compFamilyType The family type of the Component.
	 * @return The Component if it exists, NULL otherwise.
	 */
	SMARTPTR(Component) getComponent(const ComponentFamilyType& compFamilyType) const;
	/**
	 * \brief Gets the Component of a given type.
	 * @param compType The type of the Component.
	 * @return The Component if it exists, NULL otherwise.
	 */
	SMARTPTR(Component) getComponent(const ComponentType& compType) const;
	///@}

	/**
	 * \brief Returns the number of Components.
	 * @return The number of Components.
	 */
	unsigned int numComponents() const;

	/**
	 * \brief On game world creation setup.
	 *
	 * Gives an Object the ability to perform any given
	 * initialization after the whole world creation.\n
	 */
	void worldSetup();

	/**
	 * \brief Gets a the identifier of this Object.
	 * @return The id of this Object.
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
	 * \brief Gets a reference to the ObjectTemplate.
	 * @return The a reference to the ObjectTemplate.
	 */
	SMARTPTR(ObjectTemplate) const objectTmpl() const;

	/**
	 * \brief Gets the Object steady-ness.
	 *
	 * This flag represents if this Object doesn't move in the world.
	 * Various components can set or get this value to implement
	 * some optimization. By default false (i.e. Object is dynamic).\n
	 */
	bool isSteady() const;

	/**
	 * \brief Initialization function type.
	 */
	typedef void (*PINITIALIZATION)(SMARTPTR(Object), const ParameterTable& paramTable,
			PandaFramework* pandaFramework, WindowFramework* windowFramework);

	/** \name Get the parameters' tables.
	 *
	 * These tables are stored into their corresponding ObjectTemplate and
	 * ComponentTemplates.\n
	 */
	///@{
	/**
	 * \brief Returns the Object parameter table.
	 * @return The Object parameter table.
	 */
	ParameterTable getStoredObjTmplParams() const;
	/**
	 * \brief Returns the Components' parameter tables.
	 * @return The Components' parameter tables.
	 */
	ParameterTableMap getStoredCompTmplParams() const;
	///@}

	/**
	 * \brief This method will free the stored Object and Components' parameters.\n
	 *
	 * These parameters are stored in tables into their corresponding ObjectTemplate
	 * and ComponentTemplates.\n
	 * \note This method is intended to be used when done with the parameters' storage.
	 */
	void freeParameters();

	/**
	 * \brief Gets/sets this Object's owner, i.e. the Object
	 * responsible of its lifetime (if any).
	 * @param owner The owner Object. NULL if none.
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
	///The ObjectTemplate used to construct this Object.
	SMARTPTR(ObjectTemplate) const mTmpl;
	///The underlying NodePath (read-only after creation & before destruction).
	NodePath mNodePath;
	///Unique identifier for this Object (read only after creation).
	ObjectId mObjectId;
	///The owner of this Object (responsible for its lifetime).
	SMARTPTR(Object) mOwner;
	///@{
	FamilyTypeComponentList mComponents;
	struct componentHasType
	{
		ComponentType mCompType;
	public:
		componentHasType(const ComponentType& compType):mCompType(compType)
		{
		}
		bool operator()(const FamilyTypeComponentPair& familyComponentPair)
		{
			return (familyComponentPair.second->componentType() == mCompType);
		}
	};
	struct componentHasFamilyType
	{
		ComponentFamilyType mFamilyType;
	public:
		componentHasFamilyType(const ComponentFamilyType& compFamilyType):mFamilyType(compFamilyType)
		{
		}
		bool operator()(const FamilyTypeComponentPair& familyComponentPair)
		{
			return (familyComponentPair.first == mFamilyType);
		}
	};
	///@}
	///Steady flag: if this Object doesn't move in the game world.
	///Various Components can set or get this value to implement
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

	/**
	 * \name Object and Components' parameters tables.
	 */
	///@{
	ParameterTable mObjTmplParams;
	ParameterTableMap mCompTmplParams;
	///@}

#ifdef ELY_THREAD
	///The mutex associated with this Object.
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
	//already lock the Object mutex

	mNodePath = nodePath;
}

inline Object::operator NodePath() const
{
	return mNodePath;
}

inline Object::FamilyTypeComponentList& Object::doGetComponents()
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
 * This type identifies the name of ObjectTemplate that create these type of Objects.
 */
typedef std::string ObjectType;

/**
 * \brief Class modeling templates used to create Objects.
 */
class ObjectTemplate: public TypedWritableReferenceCount
{
public:
	/**
	 * \brief Type used for the ordered list of the ComponentTemplates.
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
	 * \brief Gets the name (i.e. the Object type) of ObjectTemplate.
	 * @return The name of this ObjectTemplate.
	 */
	ObjectType objectType() const;

	/**
	 * \brief Gets the ComponentTemplate list.
	 *
	 * This ObjectTemplate has an internal (ordered by insertion)
	 * list of references to ComponentTamplates, corresponding
	 * to Components owned by Objects it defines.
	 * @return The ComponentTemplate list.
	 */
	ComponentTemplateList getComponentTemplates() const;

	/**
	 * \brief Adds a ComponentTemplate.
	 *
	 * This ObjectTemplate has an internal (ordered by insertion)
	 * list of references to ComponentTemplates, corresponding
	 * to Components owned by Objects it defines.
	 * @param componentTmpl The ComponentTemplate.
	 */
	void addComponentTemplate(SMARTPTR(ComponentTemplate) componentTmpl);

	/** \name Get a ComponentTemplate contained into this ObjectTemplate
	 *
	 * This ObjectTemplate has an internal (ordered by insertion)
	 * list of references to ComponentTamplates, corresponding
	 * to Components owned by Objects it defines.
	 */
	///@{
	/**
	 * \brief Gets a ComponentTemplate corresponding to a Component type.
	 * @param compType The Component type.
	 * @return The Component template, NULL if it doesn't exist.
	 */
	SMARTPTR(ComponentTemplate) getComponentTemplate(const ComponentType& compType) const;
	/**
	 * \brief Gets a ComponentTemplate corresponding to a Component family type.
	 * @param componentFamilyType The Component family type.
	 * @return The Component template, NULL if it doesn't exist.
	 */
	SMARTPTR(ComponentTemplate) getComponentTemplate(const ComponentFamilyType& componentFamilyType) const;
	///@}

	/**
	 * \brief Clears the table of all ComponentTemplates of this ObjectTemplate.
	 */
	void clearComponentTemplates();

	/**
	 * \brief Sets the Object parameters to their default values.
	 *
	 * For the Object this template is designed to create,
	 * this function sets the parameters to their default values.
	 */
	void setParametersDefaults();

	/**
	 * \brief Sets the Object parameters to custom values.
	 *
	 * For the Object this template designs, this function
	 * sets the parameters to custom values.
	 * These parameters overwrite (and/or are added to) the parameters
	 * defaults set by setParametersDefaults.
	 * @param parameterTable The table of (parameter,value).
	 */
	void setParameters(const ParameterTable& parameterTable);

	/**
	 * \brief Gets the parameter single value associated to the Object.
	 * @param paramName The name of the parameter.
	 * @return The value of the parameter, empty string if none exists.
	 */
	std::string parameter(const std::string& paramName) const;

	/**
	 * \brief Gets the parameter multiple values associated to the Object.
	 * @param paramName The name of the parameter.
	 * @return The value list  of the parameter, empty list if none exists.
	 */
	std::list<std::string> parameterList(const std::string& paramName);

	/**
	 * \brief Gets (a copy of) the entire parameter table.
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
	 * \brief Adds a common multiple-valued parameter to a Component type
	 * of this Object.
	 *
	 * Any parameter value is interpreted as a "compound" one, i.e. as having
	 * the form: "value1:value2:...:valueN" and each valueX is stored in a
	 * list as returned by componentTypeParameterValues().\n
	 * @param parameterName The parameter name.
	 * @param parameterValue The parameter value.
	 * @param compType The Component type the parameter is related to.
	 */
	void addComponentTypeParameter(const std::string& parameterName,
			const std::string& parameterValue, ComponentType compType);

	/**
	 * \brief Checks whether a (name, value) pair is a (parameter, value) allowed
	 * for a certain Component type of this Object.
	 * @param name The name to check.
	 * @param value The value to check.
	 * @param compType The Component type.
	 * @return True if the name/value pair match an allowed parameter/value
	 * for a given Component, false otherwise.
	 */
	bool isComponentTypeParameterValue(const std::string& name, const std::string& value,
			ComponentType compType);

	/**
	 * \brief Gets all values of the common parameter associated to
	 * a certain Component type of the Object.
	 * @param paramName The name of the parameter.
	 * @param compType The Component type.
	 * @return The value list  of the parameter, empty list if none exists.
	 */
	std::list<std::string> componentTypeParameterValues(const std::string& paramName,
			ComponentType compType);

#ifdef ELY_THREAD
	/**
	 * \brief Get the mutex to lock the entire structure.
	 * @return The internal mutex.
	 */
	ReMutex& getMutex();
#endif

private:
	///Name identifying this Object template.
	ObjectType mName;
	///Ordered list of ComponentTemplates for all the owned Components.
	ComponentTemplateList mComponentTemplates;
	///The ObjectTemplateManager.
	ObjectTemplateManager* const mObjectTmplMgr;
	///Parameter table.
	ParameterTable mParameterTable;
	///The PandaFramework.
	PandaFramework* mPandaFramework;
	///The WindowFramework.
	WindowFramework* mWindowFramework;

	///The table of ParameterTables indexed by Component type.\n
	///These represent the allowed common attributes shared by each
	///Component of a given type, belonging to any Object of a given type.
	std::map<ComponentType, ParameterTable> mComponentParameterTables;

	///@{
	struct componentHasType
	{
		componentHasType(const ComponentType& compType) :
				mComponentType(compType)
		{
		}
		ComponentType mComponentType;
		bool operator()(const SMARTPTR(ComponentTemplate)componentTmpl)
		{
			return componentTmpl->componentType() == mComponentType;
		}
	};
	struct componentHasFamilyType
	{
		componentHasFamilyType(const ComponentFamilyType& compFamilyType) :
				mComponentFamilyType(compFamilyType)
		{
		}
		ComponentFamilyType mComponentFamilyType;
		bool operator()(const SMARTPTR(ComponentTemplate)componentTmpl)
		{
			return componentTmpl->componentFamilyType() == mComponentFamilyType;
		}
	};
	///@}

#ifdef ELY_THREAD
	///The mutex associated with this ObjectTemplate.
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
