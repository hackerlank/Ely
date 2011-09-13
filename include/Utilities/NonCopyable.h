/**
 * \file NonCopyable.h
 *
 * \date 13/set/2011
 * \author Marco Paone
 */

#ifndef NONCOPYABLE_H_
#define NONCOPYABLE_H_

namespace utils
{

class NonCopyable
{
public:
	/**
	 * \brief Default destructor.
	 *
	 * This destructor is public, because it has to be called on program exit.
	 */
	virtual ~NonCopyable();

protected:

	/**
	 * \brief Default constructor.
	 *
	 * We don’t want clients to be able to create new instances.
	 * This can be done by declaring the default constructor to be private/protected,
	 * thus preventing the compiler from automatically creating it as public.
	 */
	NonCopyable();

private:

	/**
	 * \brief Copy constructor.
	 *
	 * We want the singleton to be non-copyable, to enforce that a
	 * second instance cannot be created: this can be done by declaring a
	 * private copy constructor.
	 * It is only declared.
	 */
	NonCopyable(const NonCopyable&);

	/**
	 * \brief Assignment operator.
	 *
	 * We want the singleton to be non-copyable, to enforce that a
	 * second instance cannot be created: this can be done by declaring
	 * a private assignment operator.
	 * It is only declared.
	 *
	 * @param openalResourceManager Object to be copied on construction of *this.
	 */
	const NonCopyable& operator=(const NonCopyable&);

};

} /* namespace utils */

#endif /* NONCOPYABLE_H_ */
