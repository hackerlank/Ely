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
 * \file /Ely/initializations/all_configs.h
 *
 * \date 05/set/2012 (20:07:54)
 * \author marco
 */

#ifndef ALL_CONFIGS_H_
#define ALL_CONFIGS_H_

#include <iostream>
#include <string>
#include "ObjectModel/Object.h"
#include "Utilities/Tools.h"

__attribute__((constructor)) void initializationsInit();
__attribute__((destructor)) void initializationsEnd();

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef __cplusplus
}
#endif

#endif /* ALL_CONFIGS_H_ */
