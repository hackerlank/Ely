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
 * \file /Ely/src/test/commoncomponents/CommonSuiteFixture.h
 *
 * \date 2014-08-27 
 * \author consultit
 */

#ifndef COMMONSUITEFIXTURE_H_
#define COMMONSUITEFIXTURE_H_

#include <boost/test/unit_test.hpp>
#include "CommonComponents/Default.h"
#include "CommonComponents/GameConfig.h"

using namespace ely;

struct CommonSuiteFixture
{
	CommonSuiteFixture()
	{
	}
	~CommonSuiteFixture()
	{
	}
};

#endif /* COMMONSUITEFIXTURE_H_ */
