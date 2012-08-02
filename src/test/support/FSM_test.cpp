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
 * \file /Ely/src/test/support/FSM_test.cpp
 *
 * \date 02/ago/2012 (09:50:25)
 * \author marco
 */

#include "SupportSuiteFixture.h"

struct FSMTestCaseFixture
{
	FSMTestCaseFixture()
	{
		fsm1 = new fsm("fsm1");
	}
	~FSMTestCaseFixture()
	{
		delete fsm1;
	}
	fsm* fsm1;
};

//functor
struct F
{
	//good for enter/fromTo
	void operator()(fsm* _fsm, const ValueList& _data)
	{
	}
	//exit method
	void exitMethod1(fsm* _fsm)
	{
	}
	//filter method
	ValueList filter1(const std::string& _from, const std::string& _to,
			const ValueList& _data)
	{
		ValueList valueList;
		return valueList;
	}
};
//free functions
void enter1(fsm* _fsm, const ValueList& _data)
{
}
void exit1(fsm* _fsm)
{
}
void fromTo1(fsm* _fsm, const ValueList& _data)
{
}
ValueList filter1(const std::string& _from, const std::string& _to,
		const ValueList& _data)
{
	ValueList valueList;
	return valueList;
}

/// Support suite
BOOST_FIXTURE_TEST_SUITE(Support, SupportSuiteFixture)

/// Test cases
BOOST_FIXTURE_TEST_CASE(FSMConstructorTEST, FSMTestCaseFixture)
{
	BOOST_CHECK(fsm1->Off == "__Off");
	BOOST_CHECK(fsm1->Null == "__Null");
	BOOST_CHECK(fsm1->getCurrentOrNextState() == fsm1->Off);
	std::string sCurr, sNext;
	BOOST_CHECK(fsm1->getCurrentStateOrTransition(sCurr, sNext));
	BOOST_CHECK(not fsm1->isInTransition());
}

BOOST_FIXTURE_TEST_CASE(FSMConstructionTEST, FSMTestCaseFixture)
{
	fsm1->addState("s01",&enter1,exit1)
	BOOST_CHECK(fsm1->Off == "__Off");
	BOOST_CHECK(fsm1->Null == "__Null");
	BOOST_CHECK(fsm1->getCurrentOrNextState() == fsm1->Off);
	std::string sCurr, sNext;
	BOOST_CHECK(fsm1->getCurrentStateOrTransition(sCurr, sNext));
	BOOST_CHECK(not fsm1->isInTransition());
}


BOOST_AUTO_TEST_SUITE_END() // Support suite

