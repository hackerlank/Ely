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
 * \file /Ely/src/test/game/GameSuiteFixture.h
 *
 * \date 07/mag/2012 (18:09:17)
 * \author consultit
 */

#ifndef GAMESUITEFIXTURE_H_
#define GAMESUITEFIXTURE_H_

#include "Game/GameManager.h"
#include "Game/GameAudioManager.h"
#include "Game/GameControlManager.h"
#include "Game/GamePhysicsManager.h"
#include <boost/test/unit_test.hpp>

struct GameSuiteFixture
{
	GameSuiteFixture()
	{
	}
	~GameSuiteFixture()
	{
	}
};

#endif /* GAMESUITEFIXTURE_H_ */
