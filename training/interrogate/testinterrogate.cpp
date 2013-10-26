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
 * \file /Ely/training/testinterrogate.cpp
 *
 * \date 25/ott/2013 (17:03:57)
 * \author consultit
 */

#include "testinterrogate.h"

DemoClass::DemoClass(void)
{
}
DemoClass::~DemoClass(void)
{
}

const char *DemoClass::sayHello()
{
	return "Hello World!";
}

void DemoClass::someVectorOperation(LVecBase3f *vec)
{
	printf("Vector = LVecBase3f(%f,%f,%f)\n", vec->get_x(), vec->get_y(),
			vec->get_z());
	vec->set_x(vec->get_x() + 5.0f);
}
