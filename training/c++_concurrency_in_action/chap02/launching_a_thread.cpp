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
 * \file /Ely/training/c++cia/chap02/launching_a_thread.cpp
 *
 * \date Nov 23, 2011
 * \author marco
 */

#include <boost/thread.hpp>
#include <iostream>
using namespace std;
using namespace boost;

void do_some_work()
{
	for (int i = 0; i < 1000000; ++i)
	{
		cout << "thread " << i << endl;
	}
}

int main(int argc, char **argv)
{
	thread my_thread(do_some_work);
	my_thread.join();
	return 0;
}

