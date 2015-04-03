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
 * \file /Ely/elygame/initializations/defs/Game_init.h
 *
 * \date 22/mar/2014 (18:19:01)
 * \author consultit
 */

#ifndef GAME_INIT_H_
#define GAME_INIT_H_

#include "elygame_ini.h"

//(Rocket) externs
extern std::string rocketBaseDir;
//utilities
void writeText(NodePath& textNode, const std::string& text, float scale, const LVecBase4& color,
		const LVector3f& location);

#endif /* GAME_INIT_H_ */
