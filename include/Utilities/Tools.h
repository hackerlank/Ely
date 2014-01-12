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
 * \author consultit
 */

#ifndef TOOLS_H_
#define TOOLS_H_

#ifdef HAVE_CONFIG_H
# include "config.h"
# undef HAVE_PYTHON
#endif

#include <iostream>
#include <referenceCount.h>
#include <event.h>
#include <eventHandler.h>
#include <genericAsyncTask.h>
#include <pointerTo.h>
#include <threadSafePointerTo.h>
/////Dynamic linked libraries loading (Libtool)
#include <ltdl.h>

namespace ely
{
///Macros for generic debug
#if defined (ELY_DEBUG) && !defined (TESTING)
#	define PRINT_DEBUG(msg) std::cout << msg << std::endl
#	define PRINT_ERR_DEBUG(msg) std::cerr << msg << std::endl
#	define CHECK_EXISTENCE_DEBUG(entity,msg) \
	if (not entity)\
	{\
		throw GameException(msg);\
	}
#else
#	define PRINT_DEBUG(msg)
#	define PRINT_ERR_DEBUG(msg)
#	define CHECK_EXISTENCE_DEBUG(entity,msg)
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
		return dynamic_cast<std::ostringstream&>(std::ostringstream().operator <<(i)).str();
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
 * 	void myEventCallback(const Event* event);
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
 * separator.\n
 * \note all blanks are erased before parsing.
 * @param compoundString The source string.
 * @param separator The character separator.
 * @return The substrings std::vector.
 */
std::vector<std::string> parseCompoundString(const std::string& srcCompoundString,
		char separator);

/**
 * \brief Into a given string, replaces any occurrence of a character with
 * another character.
 * @param source The source string.
 * @param character To be replaced character.
 * @param replacement Replaced character.
 * @return The result string.
 */
std::string replaceCharacter(const std::string& source, int character,
		int replacement);

/**
 * \brief Into a given string, erases any occurrence of a given character.
 * @param source The source string.
 * @param character To be erased character.
 * @return The result string.
 */
std::string eraseCharacter(const std::string& source, int character);

#define RETURN_ON_COND(_flag_,_return_)\
	if (_flag_)\
	{\
		return _return_;\
	}

///ELY_THREAD
#ifdef ELY_THREAD
#	define HOLD_MUTEX(_mutex_) MutexHolder guard(_mutex_);
#	define HOLD_REMUTEX(_remutex_) ReMutexHolder reguard(_remutex_);
#	define SMARTPTR(_type_) ThreadSafePointerTo<_type_>
#	define CSMARTPTR(_type_) ThreadSafeConstPointerTo<_type_>
#	define RETURN_ON_ASYNC_COND(_flag_,_return_)\
	if (_flag_)\
	{\
		return _return_;\
	}
#else
#	define HOLD_MUTEX(_mutex_)
#	define HOLD_REMUTEX(_remutex_)
#	define SMARTPTR(_type_) PointerTo<_type_>
#	define CSMARTPTR(_type_) ConstPointerTo<_type_>
#	define RETURN_ON_ASYNC_COND(_flag_,_return_)
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

///Some useful macro
#define MATH_PI 3.141592654

///Define a standard name usable by components:
/// "ObjectId_ObjectType_ComponentId_ComponentType"
#define COMPONENT_STANDARD_NAME \
	std::string(mOwnerObject->objectId()) + "_" \
	+ std::string(mOwnerObject->objectTmpl()->objectType()) + "_" \
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

/// the default callback function name
#define DEFAULT_CALLBACK_NAME "default_callback__"

} // namespace ely

#endif /* TOOLS_H_ */
