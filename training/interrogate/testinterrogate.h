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
 * \file /Ely/training/testinterrogate.h
 *
 * \date 2013-10-25 
 * \author consultit
 */
#ifndef TESTINTERROGATE_H_
#define TESTINTERROGATE_H_

// Include the config of the panda3d installation.
// This will ensure we have the same settings.
#include <dtool_config.h>
#undef DO_MEMORY_USAGE

// Import panda base
#include <pandabase.h>

#include <lvector3.h>

class EXPCL_PANDASKEL DemoClass
{
PUBLISHED:

    // every exposed class, which is not static, needs a constructor and destructor
    DemoClass(void);
    ~DemoClass(void);

    static const char* sayHello();
    static void someVectorOperation(LVecBase3f *vec);
};

#endif /* TESTINTERROGATE_H_ */
