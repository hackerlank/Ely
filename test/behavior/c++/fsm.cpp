/**
 * \file fsm.cpp
 *
 * \date 2016-10-10
 * \author consultit
 */

#include "FSM.h"

using namespace boost;

void enterF(FSM<unsigned char>* fsm, const AnyValueList& values)
{
	unsigned char currState, nextState;
	fsm->get_current_state_or_transition(currState, nextState);
	cout << "entering: " << nextState << endl;
	AnyValueList::const_iterator iter;
	for (iter = values.begin(); iter != values.end(); ++iter)
	{
		any_cast<string>(&(*iter)) ?
				cout << any_cast<string>(*iter) << endl :
				cout << "not a string" << endl;
	}
}
void exitF(FSM<unsigned char>* fsm)
{
	unsigned char currState, nextState;
	fsm->get_current_state_or_transition(currState, nextState);
	cout << "exiting: " << currState << endl;
}

template<> FSM<unsigned char>::FSM(const unsigned char& name) :
		InTransition('\n'), Null('\0'), Off('\xFF')
{
	//any specialization must call this, otherwise no
	//initialization would take place
	initialize(name);
}

int main(int argc, char **argv)
{
	/// construction/assignment
	// direct constructors from variables
	any v(10);
	any s(string("ten"));
	// direct assignment to variables
	any v1 = 10;
	any s1 = string("ten");

	/// check types
	//1: through typeid()
	cout << (v.type() == typeid(string)) << endl;
	cout << (s1.type() == typeid(string)) << endl;
	//2: any_cast<type> throws exception
	try
	{
		any_cast<string>(v1);
		cout << 1 << endl;
	} catch (const boost::bad_any_cast &)
	{
		cout << 0 << endl;
	}
	try
	{
		any_cast<string>(s);
		cout << 1 << endl;
	} catch (const boost::bad_any_cast &)
	{
		cout << 0 << endl;
	}
	//3: any_cast<type*> returns NULL or pointer
	any_cast<string>(&v) ? cout << 1 << endl : cout << 0 << endl;
	any_cast<string>(&s) ? cout << 1 << endl : cout << 0 << endl;

	/// Finite State Machine tests
	// creation
	FSM<unsigned char> fsmc('f');
	fsmc.add_state('0', &enterF, &exitF, NULL);
	fsmc.add_state('1', &enterF, &exitF, NULL);
	fsmc.add_state('2', &enterF, &exitF, NULL);
	fsmc.add_state('3', &enterF, &exitF, NULL);
	fsmc.add_state('4', &enterF, &exitF, NULL);
	// go to start state
	fsmc.request('0');
	// transitions
	AnyValueList values;
	values.push_back(10);
	values.push_back(string("ten"));
	fsmc.request('1', values);

	return 0;
}

