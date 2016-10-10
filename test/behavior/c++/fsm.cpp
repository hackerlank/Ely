/**
 * \file fsm.cpp
 *
 * \date 2016-10-10
 * \author consultit
 */

#include "FSM.h"

using namespace boost;

void enterF(FSM<char>* fsm, const AnyValueList& values)
{
	AnyValueList::const_iterator iter;
	for (iter = values.begin(); iter != values.end(); ++iter)
	{
		any_cast<string>(&(*iter)) ?
				cout << any_cast<string>(*iter) << endl :
				cout << "not a string" << endl;
	}
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
	FSM<char> fsmc('1');

	return 0;
}

