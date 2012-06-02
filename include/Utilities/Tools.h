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

#include <referenceCount.h>
#include <event.h>
#include <utility>
#include <exception>
#include <string>
#include <sstream>
#include <cassert>

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

///TypedObject semantics: hardcoded
void initTypedObjects();

/**
 * \brief Template class to enable methods as event handler.
 *
 * This template allows class method to be registered as event handlers
 * through the define_key method of PandaFramework.
 * A class should declare the method with the same signature as
 * EventHandler::EventCallbackFunction, i.e.
 * \code
 * 	void handler(const Event *, void *);
 * \endcode
 * and then call define_key with a template specialization as in
 * this example:
 * \code
 * 	pandaFrmwk.define_key(&handler<A,*this, &A::handler>);
 * \endcode
 * whera A is the class and the handler method is declared as above.
 *
 * @param event_name The event name of the key.
 * @param description A description of the function of the key.
 * @param handlerMethod The handler method.
 * @param data User data.
 */
template<typename A, A& a, void (A::*pmf)(const Event *, void *)>
void handler(const string &event_name, const string &description,
		void (A::*handlerMethod)(const Event *, void *), void *data)
{
	(a.*pmf)(i);
}

#endif /* TOOLS_H_ */
