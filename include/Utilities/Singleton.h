/**
 * \file Singleton.h
 *
 * \date 13/set/2011
 * \author Marco Paone
 */

#ifndef SINGLETON_H_
#define SINGLETON_H_

#include <boost/scoped_ptr.hpp>
#include <boost/thread/once.hpp>

#include "Utilities/NonCopyable.h"

namespace utils
{
/**
 * \brief Singleton base class.
 *
 * To implement a Singleton derive your class from this.
 * Access members of the class through "MyClass::GetSingleton()": the
 * first thread that the first time calls this function allocates the
 * singleton of your class.
 * Example:
 * \code
 * class MyClass: public utils::Singleton<MyClass>
 * {
 * 	public:
 * 		void myMethod(){}
 * 		...
 * };
 *
 * int main()
 * {
 * 	MyClass::GetSingleton().myMethod();
 * 	return 0;
 * }
 * \endcode
 * This class is designed to be thread-safe, so it can be used in a
 * multi-threaded environment.
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
	static void initializeOnce(void); ///< Called by boost::call_once.

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

//	typedef boost::scoped_ptr<T> SmartPtr; ///< Smart shared pointer to Singleton.

	static boost::scoped_ptr<T> instancePtr; ///< The scoped pointer to the unique instance.
	static boost::once_flag onceFlag; ///< Flag used by boost::call_once().

};

template<typename T> Singleton<T>::~Singleton()
{
}

template<typename T> T& Singleton<T>::getSingleton(void)
{
	//Thread-safe
	if (not instancePtr)
	{
		boost::call_once(onceFlag, initializeOnce);
	}
	return *instancePtr.get();
}

template<typename T> void Singleton<T>::initializeOnce(void)
{
	//Thread-safe
	instancePtr.reset(new T());
}

template<typename T> boost::scoped_ptr<T> Singleton<T>::instancePtr(NULL);

template<typename T> boost::once_flag Singleton<T>::onceFlag = BOOST_ONCE_INIT;

} /* namespace utils */

#endif /* SINGLETON_H_ */
