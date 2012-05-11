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
 * \file /Ely/include/Utilitiy.h
 *
 * \date 08/mag/2012 (16:49:10)
 * \author marco
 */

#ifndef UTILITIY_H_
#define UTILITIY_H_

#include <referenceCount.h>
#include <utility>
#include <exception>
#include <string>
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

#endif /* UTILITIY_H_ */
