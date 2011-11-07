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
 * \file /Ely/include/utilities/UtilitiesPrerequisites.h
 *
 * \date Nov 2, 2011
 * \author marco
 */

#ifndef UTILITIESPREREQUISITES_H_
#define UTILITIESPREREQUISITES_H_

#include "Ogre.h"

namespace ely
{

#ifdef ELY_DOUBLE
typedef double Real;
#else
typedef float Real;
#endif

typedef Ogre::Radian Radian;
typedef Ogre::Degree Degree;
class Math;

} // namespace ely

#endif /* UTILITIESPREREQUISITES_H_ */
