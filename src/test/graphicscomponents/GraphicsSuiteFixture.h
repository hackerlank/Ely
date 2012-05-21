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
 * \file /Ely/src/test/graphics/GraphicsSuiteFixture.h
 *
 * \date 15/mag/2012 (15:33:43)
 * \author marco
 */

#ifndef GRAPHICSSUITEFIXTURE_H_
#define GRAPHICSSUITEFIXTURE_H_

#include <pandaFramework.h>
#include <windowFramework.h>

struct GraphicsSuiteFixture
{
	GraphicsSuiteFixture()
	{
		mPandaFrmwk = new PandaFramework();
		mWindowFrmwk = mPandaFrmwk->open_window();
	}

	~GraphicsSuiteFixture()
	{
		delete mWindowFrmwk;
		delete mPandaFrmwk;
	}
	PandaFramework* mPandaFrmwk;
	WindowFramework* mWindowFrmwk;
};

#endif /* GRAPHICSSUITEFIXTURE_H_ */
