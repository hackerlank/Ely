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
 * \file /Ely/src/Support/FSM.cpp
 *
 * \date 02/ago/2012 (09:50:25)
 * \author marco
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
