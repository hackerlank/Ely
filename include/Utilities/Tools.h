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
 * \file /Ely/include/Utilities/Tools.h
 *
 * \date 08/mag/2012 (16:49:10)
 * \author marco
 */

#ifndef TOOLS_H_
#define TOOLS_H_

#if HAVE_CONFIG_H
# include <config.h>
# undef HAVE_PYTHON
#endif

#include <exception>
#include <iostream>
#include <map>
#include <string>
#include <sstream>
#include <cassert>
#include <utility>
#include <vector>
#include <referenceCount.h>
#include <event.h>
#include <eventHandler.h>
#include <asyncTask.h>
#include <genericAsyncTask.h>
#include <pmutex.h>
#include <pointerTo.h>
#include <threadSafePointerTo.h>

///Macros for generic debug
#ifdef DEBUG
#	define PRINT(msg) std::cout << msg << std::endl
#	define PRINTERR(msg) std::cerr << msg << std::endl
#	define CHECKEXISTENCE(entity,msg) \
	if (not entity)\
	{\
		throw GameException(msg);\
	}
#else
#	define PRINT(msg)
#	define PRINTERR(msg)
#	define CHECKEXISTENCE(entity,msg)
#endif

/**
 * \brief An exception during the game.
 */
class GameException: public std::exception
{
public:
	GameException(const std::string& what) :
			exception()
	{
		mWhat = what;
	}
	virtual ~GameException() throw ()
	{
	}
	virtual const char* what()
	{
		return mWhat.c_str();
	}
protected:
	std::string mWhat;

};

/**
 * \brief A pair that can be used with PT
 */
template<typename T1, typename T2> struct Pair: public ReferenceCount
{
public:
	Pair() :
			mPair()
	{
	}
	Pair(const T1& first, const T2& second) :
			mPair(first, second)
	{
	}
	T1& first()
	{
		return mPair.first;
	}
	T2& second()
	{
		return mPair.second;
	}
private:
	std::pair<T1, T2> mPair;
};

/**
 * \brief An automatic Singleton Utility.
 *
 * \note This Singleton class is based on the article "An automatic
 * Singleton Utility" by Scott Bilas in "Game Programming Gems 1" book.
 * Non multi-threaded.
 */
template<typename T> class Singleton
{
	static T* ms_Singleton;

public:
	Singleton(void)
	{
		assert(!ms_Singleton);
		unsigned long int offset = (unsigned long int) (T*) 1
				- (unsigned long int) (Singleton<T>*) (T*) 1;
		ms_Singleton = (T*) ((unsigned long int) this + offset);
	}
	~Singleton(void)
	{
		assert(ms_Singleton);
		ms_Singleton = 0;
	}
	static T& GetSingleton(void)
	{
		assert(ms_Singleton);
		return (*ms_Singleton);
	}
	static T* GetSingletonPtr(void)
	{
		return (ms_Singleton);
	}
};

template<typename T> T* Singleton<T>::ms_Singleton = 0;

/**
 * \brief Type of the generated object counter.
 */
struct IdType
{
	unsigned long int i;
	IdType() :
			i(0)
	{
	}
	IdType& operator ++()
	{
		++i;
		return *this;
	}
	operator std::string()
	{
		std::ostringstream oStringI;
		oStringI << i;
		return oStringI.str();
	}
};

/**
 * \brief Template struct for generic Task Function interface
 *
 * The effective Tasks are composed by a Pair of an object and
 * a method (member function) doing the effective task.
 * To register a task:
 * 1) in class A define a (pointer to) TaskData member:
 * \code
 * 	SMARTPTR(TaskInterface<A>::TaskData) myData;
 * \endcode
 * 2) and a method (that will execute the real task) with signature:
 * \code
 * 	AsyncTask::DoneStatus myTask(GenericAsyncTask* task);
 * \endcode
 * 3) in code associate to myData a new TaskData referring to this
 * class instance and myTask, then create a new GenericAsyncTask
 * referring to taskFunction and with data parameter equal to
 * myData (reinterpreted as void*):
 * \code
 * 	myData = new TaskInterface<A>::TaskData(this, &A::myTask);
 * 	AsyncTask* task = new GenericAsyncTask("my task",
 * 							&TaskInterface<A>::taskFunction,
 * 							reinterpret_cast<void*>(myData.p()));
 * \endcode
 * 4) finally register the async-task to your manager:
 * \code
 * 	pandaFramework.get_task_mgr().add(task);
 * 	\endcode
 * From now on myTask will execute the task, while being able
 * to refer directly to data members of the A class instance.
 */
template<typename A> struct TaskInterface
{
	typedef AsyncTask::DoneStatus (A::*TaskPtr)(GenericAsyncTask* taskFunction);
	typedef Pair<A*, TaskPtr> TaskData;
	static AsyncTask::DoneStatus taskFunction(GenericAsyncTask* task,
			void * data)
	{
		TaskData* appData = reinterpret_cast<TaskData*>(data);
		return ((appData->first())->*(appData->second()))(task);
	}
};

/**
 * \brief Template struct for generic Event Callback interface
 *
 * The effective Event Callbacks are composed by a Pair of an object and
 * a method (member function) doing the effective task.
 * To define a event callback:
 * 1) in class A define a (pointer to) TaskData member:
 * \code
 * 	SMARTPTR(EventCallbackInterface<A>::EventCallbackData) myData;
 * \endcode
 * 2) and a method (that will execute the real event callback) with signature:
 * \code
 * 	void myEventCallback(const Event* event, void* data);
 * \endcode
 * 3) in code associate to myData a new EventCallbackData referring to this
 * class instance and myEventCallback:
 * \code
 * 	myData = new EventCallbackInterface<A>::
 * 			EventCallbackData(this, &A::myEventCallback);
 * \endcode
 * 4) finally register this event callback to your global event handler
 * with data parameter equal to myData (reinterpreted as void*):
 * \code
 * 	pandaFramework.define_key("myKey", "myEventCallback",
 * 		&EventCallbackInterface<A>::eventCallbackFunction,
 * 			reinterpret_cast<void*>(myData.p()));
 * \endcode
 * or
 * \code
 * 	EventHandler::get_global_event_handler()->add_hook("myKey",
 * 		&EventCallbackInterface<A>::eventCallbackFunction,
 * 			reinterpret_cast<void*>(myData.p()));
 * \endcode
 * From now on myEventCallback will execute the event callback, while being able
 * to refer directly to data members of the A class instance.
 */
template<typename A> struct EventCallbackInterface
{
	typedef void (A::*EventCallbackPtr)(const Event* event);
	typedef Pair<A*, EventCallbackPtr> EventCallbackData;
	static void eventCallbackFunction(const Event* event,
			void* data)
	{
		EventCallbackData* appData = reinterpret_cast<EventCallbackData*>(data);
		((appData->first())->*(appData->second()))(event);
	}
};

/**
 * \brief Table for parameters management.
 *
 * See ObjectTemplate, ComponentTemplate.
 */
typedef std::multimap<std::string, std::string> ParameterTable;
typedef std::multimap<std::string, std::string>::iterator ParameterTableIter;
typedef std::multimap<std::string, std::string>::const_iterator ParameterTableConstIter;
typedef std::map<std::string, ParameterTable> ParameterTableMap;
typedef std::pair<std::string, std::string> ParameterNameValue;

/**
 * \brief Parses a string composed by substrings separated by a character
 * separator
 * @param compoundString The compound string.
 * @param separator The character separator.
 * @return The substrings std::vector.
 */
std::vector<std::string> parseCompoundString(const std::string& compoundString,
		char separator);

/**
 * \brief Into a given string, replaces any occurrence of a character with
 * another character.
 * @param source To be replaced string.
 * @param character To be replaced character.
 * @param replacement Replaced character.
 * @return Replaced string.
 */
std::string replaceCharacter(const std::string& source, int character,
		int replacement);

///Macros representing the path to dynamic linked libraries loaded at runtime
///Event callbacks library (See Component)
#define CALLBACKS_SO "CallbacksLib/libElyCallbacks.so"
#define DEFAULT_CALLBACK "default_callback__"

///Transition functions library (see Activity component).
#define TRANSITIONS_SO "TransitionsLib/libElyTransitions.so"

///Initialization functions library (see Object).
#define INITIALIZATIONS_SO "InitializationsLib/libElyInitializations.so"

///Some macro dynamic linked libraries loading
#ifdef WIN32
#	include <direct.h>
#	include <windows.h>
typedef HINSTANCE LIB_HANDLE;
#else
#	include <sys/types.h>
#	include <dlfcn.h>
typedef void* LIB_HANDLE;
#endif

///ELY_THREAD
#ifdef ELY_THREAD
#	define HOLDMUTEX(mutex) ReMutexHolder guard(mutex);
#	define SMARTPTR(type) ThreadSafePointerTo< type >
#	define CSMARTPTR(type) ThreadSafeConstPointerTo< type >
#else
#	define HOLDMUTEX(mutex)
#	define SMARTPTR(type) PointerTo< type >
#	define CSMARTPTR(type) ConstPointerTo< type >
#endif

/**
 * \brief Implements a mutex lock guard (RAII idiom).
 *
 * Directly "stolen" (and adapted) from boost::thread.
 * \note not used: using ReMutexHolder.
 */
struct adopt_lock_t
{
};

template<typename Mutex>
class lock_guard
{
private:
	Mutex& m;

	explicit lock_guard(lock_guard&);
	lock_guard& operator=(lock_guard&);
public:
	explicit lock_guard(Mutex& m_) :
			m(m_)
	{
		m.acquire();
	}
	lock_guard(Mutex& m_, adopt_lock_t) :
			m(m_)
	{
	}
	~lock_guard()
	{
		m.release();
	}
};

///Some useful macro (for bullet,...)
#define PHYSICS_MAX_DISTANCE 1000000000.0
#define PHYSICS_PI 3.141592654

///Define a standard name usable by components:
/// "ObjectId_ObjectType_ComponentId_ComponentType"
#define COMPONENT_STANDARD_NAME \
	std::string(mOwnerObject->objectId()) + "_" \
	+ std::string(mOwnerObject->objectTmpl()->name()) + "_" \
	+ std::string(mComponentId) + "_" \
	+ get_type().get_name(this)

/**
 * \brief Get an item from a COMPONENT_STANDARD_NAME.
 *
 * @param name The COMPONENT_STANDARD_NAME.
 * @param item One of: "ObjectId", "ObjectType",
 * "ComponentId", "ComponentType".
 * @return The item requested.
 */
enum COMPONENT_STANDARD_NAME_ITEM
{
	ObjectId_item = 0,
	ObjectType_item = 1,
	ComponentId_item = 2,
	ComponentType_item = 3
};
std::string getComponentStandardNameItem(const std::string& name,
		COMPONENT_STANDARD_NAME_ITEM item);

///TypedObject semantics: hardcoded
void initTypedObjects();

#endif /* TOOLS_H_ */
