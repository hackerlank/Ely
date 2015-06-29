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
 * \file /Ely/src/AudioComponents/Sound3d.cpp
 *
 * \date 20/giu/2012 (12:41:32)
 * \author consultit
 */

#include "Sound3d1Py.h"

namespace
{

///Python Object basic stuff
void Sound3d1Py_dealloc(Sound3d1Py* self)
{
	self->ob_type->tp_free((PyObject*) self);
}

PyObject * Sound3d1Py_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
	Sound3d1Py *self;
	self = (Sound3d1Py *) type->tp_alloc(type, 0);
	return (PyObject *) self;
}

int Sound3d1Py_init(Sound3d1Py *self, PyObject *args, PyObject *kwds)
{
	return 0;
}

PyMemberDef Sound3d1Py_members[] =
{
{ NULL } /* Sentinel */
};

///Python Object methods
PyObject *Sound3d1Py_name(Sound3d1Py* self)
{
	static PyObject *format = NULL;
	PyObject *args, *result;

	return result;
}

PyMethodDef Sound3d1Py_methods[] =
{
{ "name", (PyCFunction) Sound3d1Py_name, METH_NOARGS,
		"Return the name, combining the first and last name" },
{ NULL } /* Sentinel */
};

///Python Type Object
PyTypeObject Sound3d1PyType =
{
	PyObject_HEAD_INIT(NULL) 0, /*ob_size*/
	"sound3d1Py.Sound3d1Py", /*tp_name*/
	sizeof(Sound3d1Py), /*tp_basicsize*/
	0, /*tp_itemsize*/
	(destructor) Sound3d1Py_dealloc, /*tp_dealloc*/
	0, /*tp_print*/
	0, /*tp_getattr*/
	0, /*tp_setattr*/
	0, /*tp_compare*/
	0, /*tp_repr*/
	0, /*tp_as_number*/
	0, /*tp_as_sequence*/
	0, /*tp_as_mapping*/
	0, /*tp_hash */
	0, /*tp_call*/
	0, /*tp_str*/
	0, /*tp_getattro*/
	0, /*tp_setattro*/
	0, /*tp_as_buffer*/
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
	"Sound3d1Py objects", /* tp_doc */
	0, /* tp_traverse */
	0, /* tp_clear */
	0, /* tp_richcompare */
	0, /* tp_weaklistoffset */
	0, /* tp_iter */
	0, /* tp_iternext */
	Sound3d1Py_methods, /* tp_methods */
	Sound3d1Py_members, /* tp_members */
	0, /* tp_getset */
	0, /* tp_base */
	0, /* tp_dict */
	0, /* tp_descr_get */
	0, /* tp_descr_set */
	0, /* tp_dictoffset */
	(initproc) Sound3d1Py_init, /* tp_init */
	0, /* tp_alloc */
	Sound3d1Py_new, /* tp_new */
};

} //anonymous

//create the Sound3d1Py class instance inside Sound3d1 class instance
void createSound3d1PyInstance(PyObject *instancePy, Sound3d1 *instance)
{
	// create the Sound3d1Py class instance
	instancePy = PyObject_CallObject((PyObject *) &Sound3d1PyType, NULL);
	if (not instancePy)
	{
		//set Python exception
		PyErr_SetString(PyExc_StandardError,
				"Cannot create Sound3d1Py class instance.");
	}
	else
	{
		((Sound3d1Py*) instancePy)->mSound3d1 = instance;
	}
}
// delete the Sound3d1Py class instance inside Sound3d1 class instance
void deleteSound3d1PyInstance(PyObject *instancePy)
{
	// delete the Sound3d1Py class instance
	Py_XDECREF(instancePy);
}
