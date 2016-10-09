/**
 * \file FSM.cxx
 *
 * \date 2016-10-09
 * \author consultit
 */

#include "Support/FSM.h"

//specializations' definitions
//StateKey == std::string
template<> FSM<std::string>::FSM(const std::string& name) :
		InTransition("__InTransition"), Null("__Null"), Off("__Off")
{
	//any specialization must call this, otherwise no
	//initialization would take place
	initialize(name);
}
//StateKey == int
template<> FSM<int>::FSM(const int& name) :
		InTransition(-1), Null(0), Off(1)
{
	//any specialization must call this, otherwise no
	//initialization would take place
	initialize(name);
}
