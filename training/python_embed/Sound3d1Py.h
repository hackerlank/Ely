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
 * \file /Ely/include/AudioComponents/Sound3d.h
 *
 * \date 20/giu/2012 (12:41:32)
 * \author consultit
 */

#ifndef SOUND3D1PY_H_
#define SOUND3D1PY_H_

#include <Python.h>
#include "structmember.h"

//forward declarations
class Sound3d1;

///Python object
typedef struct
{
	PyObject_HEAD
	Sound3d1 *mSound3d1;
} Sound3d1Py;

//create the Sound3d1Py class instance inside Sound3d1 class instance
void createSound3d1PyInstance(PyObject *instancePy, Sound3d1 *instance);
// delete the Sound3d1Py class instance inside Sound3d1 class instance
void deleteSound3d1PyInstance(PyObject *instancePy);

#endif /* SOUND3D1PY_H_ */
