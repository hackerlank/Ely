/**
 * \file Prerequisites.h
 *
 * \date 14/set/2011
 * \author Marco Paone
 */

#ifndef PREREQUISITES_H_
#define PREREQUISITES_H_

#include <cassert>
#include <iostream>

#include <boost/scoped_ptr.hpp>
#include <boost/any.hpp>
#include <boost/thread.hpp>
#include <boost/thread/once.hpp>


#define FastAnyCast(x, y) boost::any_cast<x>(y)

typedef boost::any Any;
typedef std::string String;

#endif /* PREREQUISITES_H_ */
