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
 * \file /Ely/callbacks/callback_defs.h
 *
 * \date 21/ago/2012 (09:18:56)
 * \author marco
 */

#ifndef CALLBACK_DEFS_H_
#define CALLBACK_DEFS_H_

#include "all_configs.h"

#ifdef __cplusplus
extern "C"
{
#endif

///Camera + Driver related
CALLBACK drive;

///Actor1 + Activity related
CALLBACK state;

#ifdef __cplusplus
}
#endif

#endif /* CALLBACK_DEFS_H_ */
