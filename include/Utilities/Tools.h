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

#include <exception>
#include <string>
#include <sstream>
#include <cassert>
#include <utility>

#include <referenceCount.h>
#include <event.h>
#include <eventHandler.h>
#include <asyncTask.h>
#include <genericAsyncTask.h>
#include <pmutex.h>

/**
 * \brief Implements a mutex lock guard (RAII idiom).
 *
 * Directly "stolen" (and adapted) from boost::thread.
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
 * 	PT(TaskInterface<A>::TaskData) myData;
 * 	\endcode
 * 2) and a method (that will execute the real task) with signature:
 * \code
 * 	AsyncTask::DoneStatus myTask(GenericAsyncTask* task);
 * \endcode
 * 3) in code associate to myData a new TaskData referring to this
 * class instance and myTask, then create a new GenericAsyncTask
 * referring to taskFunction and with data parameter equal to
 * myData (reinterpreted as void*):
 * \code
 * 	myData = new TaskInterface<A>::TaskData(this, &A::firstTask);
 * 	AsyncTask* task = new GenericAsyncTask("my task",
 * 							&TaskInterface<A>::taskFunction,
 * 							reinterpret_cast<void*>(myData.p()));
 * 	\endcode
 * 4) finally register the async-task to your manager:
 * \code
 * 	pandaFramework.get_task_mgr().add(task);
 * 	\endcode
 * From now on myTask will execute the task, while being able
 * to refer directly to data members of the class.
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
 * \brief Table for parameters management.
 *
 * See ObjectTemplate, ComponentTemplate.
 */
typedef std::multimap<std::string, std::string> ParameterTable;

/**
 * \brief Helper macro used for getting parameter references.
 *
 * See ObjectTemplate, ComponentTemplate.
 */
#define CASE(name,resPtr,value,param) \
		if (name == std::string(value))\
		{\
			resPtr = &param;\
		}
#define CASELIST(name,resPtr,value,param) \
		if (name == std::string(value))\
		{\
			resPtr = &param;\
		}

///TypedObject semantics: hardcoded
void initTypedObjects();

#endif /* TOOLS_H_ */
