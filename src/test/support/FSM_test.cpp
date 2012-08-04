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

//functors
struct F
{
	//good for enter
	void operator()(fsm* _fsm, const ValueList& _data)
	{
		std::string _from, _to;
		if (not _fsm->getCurrentStateOrTransition(_from, _to))
		{
			std::cout << "Enter: to '" << _to << "('"
					<< _fsm->getCurrentOrNextState() << "')" << "' from '"
					<< _from << "' -> F::operator()" << std::endl;
		}
	}
	//exit method
	void exit1(fsm* _fsm)
	{
		std::string _from, _to;
		if (not _fsm->getCurrentStateOrTransition(_from, _to))
		{
			std::cout << "Exit: from '" << _from << "' to '" << _to << "'('"
					<< _fsm->getCurrentOrNextState() << "') -> F::exit1"
					<< std::endl;
		}
	}
	//filter method
	ValueList filter1(const std::string& _curr, const std::string& _to,
			const ValueList& _data)
	{
		ValueList valueList;
		valueList.push_front(_to);
		std::cout << "Filter: current '" << _curr << "' request to '" << _to
				<< "' -> F::filter1" << std::endl;
		return valueList;
	}
};

struct F2
{
	std::string demandStateOnEnter, demandStateOnExit;
	F2(const std::string& _demandStateOnEnter,
			const std::string& _demandStateOnExit)
	{
		demandStateOnEnter = _demandStateOnEnter;
		demandStateOnExit = _demandStateOnExit;
	}
	//good for enter
	void operator()(fsm* _fsm, const ValueList& _data)
	{
		std::string _from, _to;
		if (not _fsm->getCurrentStateOrTransition(_from, _to))
		{
			std::cout << _to;
		}
		if (not demandStateOnEnter.empty())
		{
			_fsm->demand(demandStateOnEnter);
		}
	}
	//exit method
	void exit2(fsm* _fsm)
	{
		std::string _from, _to;
		if (not _fsm->getCurrentStateOrTransition(_from, _to))
		{
			std::cout << "->";
		}
		if (not demandStateOnExit.empty())
		{
			_fsm->demand(demandStateOnExit);
		}
	}
};
//free functions
void enter1(fsm* _fsm, const ValueList& _data)
{
	std::string _from, _to;
	if (not _fsm->getCurrentStateOrTransition(_from, _to))
	{
		std::cout << "Enter: to '" << _to << "('"
				<< _fsm->getCurrentOrNextState() << "')" << "' from '" << _from
				<< "' -> enter1" << std::endl;
	}
}
void exit1(fsm* _fsm)
{
	std::string _from, _to;
	if (not _fsm->getCurrentStateOrTransition(_from, _to))
	{
		std::cout << "Exit: from '" << _from << "' to '" << _to << "'('"
				<< _fsm->getCurrentOrNextState() << "') -> exit1" << std::endl;
	}
}
void fromTo1(fsm* _fsm, const ValueList& _data)
{
	std::string _from, _to;
	if (not _fsm->getCurrentStateOrTransition(_from, _to))
	{
		std::cout << "fromTo1: from '" << _from << "' to '" << _to << "'('"
				<< _fsm->getCurrentOrNextState() << "') -> fromTo1"
				<< std::endl;
	}
}
ValueList filter1(const std::string& _curr, const std::string& _to,
		const ValueList& _data)
{
	ValueList valueList;
	valueList.push_front(_to);
	std::cout << "Filter: current '" << _curr << "' request to '" << _to
			<< "' -> filter1" << std::endl;
	return valueList;
}

/// Support suite
BOOST_FIXTURE_TEST_SUITE(Support, SupportSuiteFixture)

/// Test cases
BOOST_FIXTURE_TEST_CASE(FSMConstructorTEST, FSMTestCaseFixture)
{
	BOOST_CHECK(fsm1->getCurrentOrNextState() == fsm1->Off);
	BOOST_CHECK(fsm1->Off == "__Off");
	BOOST_CHECK(fsm1->Null == "__Null");
	BOOST_CHECK(fsm1->getCurrentOrNextState() == fsm1->Off);
	std::string sCurr, sNext;
	BOOST_CHECK(fsm1->getCurrentStateOrTransition(sCurr, sNext));
	BOOST_CHECK(not fsm1->isInTransition());
}

BOOST_FIXTURE_TEST_CASE(FSMConstructionTEST, FSMTestCaseFixture)
{
	F f;
	//test add & cleanup & request
	fsm1->addState("s01", &enter1, &exit1, &filter1);
	fsm1->addState("s02", boost::ref(f), boost::bind(&F::exit1, boost::ref(f),_1),
			boost::bind(&F::filter1, boost::ref(f),_1,_2,_3));
	fsm1->addState("s03", NULL, NULL, NULL);
	fsm1->addState("s04", NULL, NULL, NULL);
	fsm1->addFromToFunc("s03","s04", &fromTo1);
	//expected callbacks sequence:
	//"s01"->enter1		||
	//"s01"->filter1	|| "s01"->exit1			|| "s02"->F::operator()
	//"s02"->F::filter1	|| "s02"->F::exit1		||
	//					|| "s03","s04"->fromTo1	||
	fsm1->request("s01");
	BOOST_CHECK(fsm1->getCurrentOrNextState() == "s01");
	fsm1->request("s02");
	BOOST_CHECK(fsm1->getCurrentOrNextState() == "s02");
	fsm1->request("s03");
	BOOST_CHECK(fsm1->getCurrentOrNextState() == "s03");
	fsm1->request("s04");
	BOOST_CHECK(fsm1->getCurrentOrNextState() == "s04");
	fsm1->cleanup();
	BOOST_CHECK(fsm1->getCurrentOrNextState() == fsm1->Off);
	//test add & remove
	fsm1->addState("s01", &enter1, &exit1, &filter1);
	fsm1->addState("s02", NULL, NULL, NULL);
	fsm1->addState("s03", NULL, NULL, NULL);
	fsm1->addFromToFunc("s02","s03", &fromTo1);
	fsm1->addState("s02", &enter1, &exit1, &filter1);
	fsm1->addState("s03", boost::ref(f), boost::bind(&F::exit1, boost::ref(f),_1),
			boost::bind(&F::filter1, boost::ref(f),_1,_2,_3));
	fsm1->addState("s04", &enter1, &exit1, &filter1);
	fsm1->removeFromToFunc("s02","s03");
	fsm1->removeState("s01");
	//expected callbacks sequence:
	//"s02"->enter1		||
	//"s02"->filter1	|| FSM::setState: State 's01' doesn't exist
	//"s02"->filter1	|| "s02"->exit1			|| "s03"->F::operator()
	//"s03"->F::filter1	|| "s03"->F::exit1		|| "s04"->enter1
	fsm1->request("s02");
	BOOST_CHECK(fsm1->getCurrentOrNextState() == "s02");
	fsm1->request("s01");
	BOOST_CHECK(fsm1->getCurrentOrNextState() == "s02");
	fsm1->request("s03");
	BOOST_CHECK(fsm1->getCurrentOrNextState() == "s03");
	fsm1->request("s04");
	BOOST_CHECK(fsm1->getCurrentOrNextState() == "s04");
	//test setStateSet
	fsm1->cleanup();
	State state1("s01"), state2("s02");
	state1.enter = enter1;
	state1.exit = exit1;
	state1.filter = filter1;
	state2.enter = boost::ref(f);
	state2.exit = boost::bind(&F::exit1, boost::ref(f),_1);
	state2.filter = boost::bind(&F::filter1, boost::ref(f),_1,_2,_3);
	fsm::StateSet stateSet;
	stateSet.insert(state1);
	stateSet.insert(state2);
	fsm1->setStateSet(stateSet);
	//expected callbacks sequence:
	//"s01"->enter1		||
	//"s01"->filter1	|| "s01"->exit1			|| "s02"->F::operator()
	//"s02"->F::filter1	|| "s02"->F::exit1		||
	fsm1->request("s01");
	BOOST_CHECK(fsm1->getCurrentOrNextState() == "s01");
	fsm1->request("s02");
	BOOST_CHECK(fsm1->getCurrentOrNextState() == "s02");
	fsm1->request(fsm1->Off);
	BOOST_CHECK(fsm1->getCurrentOrNextState() == fsm1->Off);
	//test default filter
	fsm1->cleanup();
	fsm1->addState("s01", &enter1, &exit1, NULL);
	fsm1->addState("s02", &enter1, &exit1, NULL);
	fsm1->addState("s03", &enter1, &exit1, NULL);
	//expected callbacks sequence:
	//"s01"->enter1		||
	//					|| "s01"->exit1			|| "s02"->enter1
	//					|| "s02"->exit1			|| "s03"->enter1
	//					|| "s03"->exit1			||
	fsm1->request("s01");
	BOOST_CHECK(fsm1->getCurrentOrNextState() == "s01");
	fsm1->request("s02");
	BOOST_CHECK(fsm1->getCurrentOrNextState() == "s02");
	fsm1->request("s03");
	BOOST_CHECK(fsm1->getCurrentOrNextState() == "s03");
	fsm1->request(fsm1->Off);
	BOOST_CHECK(fsm1->getCurrentOrNextState() == fsm1->Off);
}

BOOST_FIXTURE_TEST_CASE(FSMTransitionTEST, FSMTestCaseFixture)
{
	//construction
	F2 f1("s03","");
	fsm1->addState("s01", boost::ref(f1), boost::bind(&F2::exit2, boost::ref(f1),_1), NULL);
	F2 f2("","s04");
	fsm1->addState("s02", boost::ref(f2), boost::bind(&F2::exit2, boost::ref(f2),_1), NULL);
	F2 f3("","");
	fsm1->addState("s03", boost::ref(f3), boost::bind(&F2::exit2, boost::ref(f3),_1), NULL);
	F2 f4("s03","s01");
	fsm1->addState("s04", boost::ref(f4), boost::bind(&F2::exit2, boost::ref(f4),_1), NULL);
	//expected state transitions sequence: ?
	fsm1->request("s01");
	std::cout.flush();
	BOOST_CHECK(fsm1->getCurrentOrNextState() == "s03");
	fsm1->request("s02");
	std::cout.flush();
	BOOST_CHECK(fsm1->getCurrentOrNextState() == "s02");
	fsm1->request("s02");
	std::cout.flush();
	BOOST_CHECK(fsm1->getCurrentOrNextState() == "s03");
//	fsm1->request("s04");
//	BOOST_CHECK(fsm1->getCurrentOrNextState() == "s04");
//	fsm1->cleanup();
//	BOOST_CHECK(fsm1->getCurrentOrNextState() == fsm1->Off);
	//test add & remove
//	fsm1->addState("s01", &enter1, &exit1, &filter1);
//	fsm1->addState("s02", NULL, NULL, NULL);
//	fsm1->addState("s03", NULL, NULL, NULL);
//	fsm1->addFromToFunc("s02","s03", &fromTo1);
//	fsm1->addState("s02", &enter1, &exit1, &filter1);
//	fsm1->addState("s03", boost::ref(f), boost::bind(&F::exit1, boost::ref(f),_1),
//			boost::bind(&F::filter1, boost::ref(f),_1,_2,_3));
//	fsm1->addState("s04", &enter1, &exit1, &filter1);
//	fsm1->removeFromToFunc("s02","s03");
//	fsm1->removeState("s01");
//	//expected callbacks sequence:
//	//"s02"->enter1		||
//	//"s02"->filter1	|| FSM::setState: State 's01' doesn't exist
//	//"s02"->filter1	|| "s02"->exit1			|| "s03"->F::operator()
//	//"s03"->F::filter1	|| "s03"->F::exit1		|| "s04"->enter1
//	fsm1->request("s02");
//	BOOST_CHECK(fsm1->getCurrentOrNextState() == "s02");
//	fsm1->request("s01");
//	BOOST_CHECK(fsm1->getCurrentOrNextState() == "s02");
//	fsm1->request("s03");
//	BOOST_CHECK(fsm1->getCurrentOrNextState() == "s03");
//	fsm1->request("s04");
//	BOOST_CHECK(fsm1->getCurrentOrNextState() == "s04");
//	//test setStateSet
//	fsm1->cleanup();
//	State state1("s01"), state2("s02");
//	state1.enter = enter1;
//	state1.exit = exit1;
//	state1.filter = filter1;
//	state2.enter = boost::ref(f);
//	state2.exit = boost::bind(&F::exit1, boost::ref(f),_1);
//	state2.filter = boost::bind(&F::filter1, boost::ref(f),_1,_2,_3);
//	fsm::StateSet stateSet;
//	stateSet.insert(state1);
//	stateSet.insert(state2);
//	fsm1->setStateSet(stateSet);
//	//expected callbacks sequence:
//	//"s01"->enter1		||
//	//"s01"->filter1	|| "s01"->exit1			|| "s02"->F::operator()
//	//"s02"->F::filter1	|| "s02"->F::exit1		||
//	fsm1->request("s01");
//	BOOST_CHECK(fsm1->getCurrentOrNextState() == "s01");
//	fsm1->request("s02");
//	BOOST_CHECK(fsm1->getCurrentOrNextState() == "s02");
//	fsm1->request(fsm1->Off);
//	BOOST_CHECK(fsm1->getCurrentOrNextState() == fsm1->Off);
}

BOOST_AUTO_TEST_SUITE_END() // Support suite

