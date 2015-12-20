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
 * \file /Ely/elygame/elygame_memory_pools.cpp
 *
 * \date 2014-12-14 
 * \author consultit
 */

#include "Support/MemoryPool/MemoryPool.h"
#include "Support/MemoryPool/MemoryMacros.h"

//Set the static objects used to initialize/finalize memory pools
//that some objects and/or components would use
#include "CommonComponents/Default.h"

namespace ely
{
GCC_MEMORYPOOL_AUTOINIT(Default, 16);

}  // namespace ely

