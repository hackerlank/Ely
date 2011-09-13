/*
 * Singleton.h
 *
 *  Created on: 13/set/2011
 *      Author: marco
 */

#ifndef SINGLETON_H_
#define SINGLETON_H_

#include <boost/scoped_ptr.hpp>

#include "Utilities/NonCopyable.h"

namespace util
{
/**
 * \brief Singleton base class.
 *
 * To implement a Singleton derive your class
 * This class is designed to be thread-safe, so it can be used in a
 * multi-threading environment.
 */
template<typename T> class Singleton: public NonCopyable
{
public:

	/**
	 * \brief Default destructor.
	 *
	 * This destructor is public, because it has to be called on program exit.
	 */
	virtual ~Singleton();

	/**
	 * \brief Returns a reference to the singleton.
	 *
	 * The getSingleton() method return a reference to the singleton class.
	 * This is how you should ALWAYS get a reference to the Singleton.
	 * The Singleton design pattern (Gamma et al., 1994) is used to
	 * ensure that a class only ever has one instance.
	 *
	 * @return A reference to the singleton class.
	 */
	static T& getSingleton(void);

private:
	/**
	 *  \brief One time initialization.
	 *
	 *  This method is called by boost::call_once() to perform creation
	 *  and initialization of the ResourceManager unique instance.
	 *  This is necessary for achieving the thread-safeness of the ResourceManager class.
	 *  It has to be static, because it needs to allocate the unique instance.
	 */
	template<typename S> static void initializeOnce(void); ///< Called by boost::call_once.

	/**
	 * \brief Pointer operator.
	 *
	 * We want the singleton cannot be deleted by a call similar to
	 * "delete &OpenAL::Singleton::getSingleton();": this can be
	 * done by declaring a private pointer operator.
	 * It is only declared.
	 *
	 * @return A pointer to the unique instance.
	 */
	T* operator &();

	typedef boost::scoped_ptr<class T> SmartPtr; ///< Smart shared pointer to Singleton.

	static SmartPtr instancePtr; ///< The scoped pointer to the unique instance.
	static boost::once_flag onceFlag; ///< Flag used by boost::call_once().

};

template<typename T> template<typename S> void Singleton<T>::initializeOnce<S>(void)
{
	//Thread-safe
	instancePtr.reset(new T());
}

} /* namespace util */

#endif /* SINGLETON_H_ */
