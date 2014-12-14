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
 * \file /Ely/src/CommonComponents/Default.cpp
 *
 * \date 27/ago/2014 (10:45:49)
 * \author consultit
 */

#include "CommonComponents/Default.h"
#include "CommonComponents/DefaultTemplate.h"

namespace ely
{

Default::Default()
{
	reset();
}

Default::Default(SMARTPTR(DefaultTemplate)tmpl)
{
	mTmpl = tmpl;
	reset();
}

Default::~Default()
{
}

ComponentFamilyType Default::familyType() const
{
	return mTmpl->familyType();
}

ComponentType Default::componentType() const
{
	return mTmpl->componentType();
}

//TypedObject semantics: hardcoded
TypeHandle Default::_type_handle;

//MemoryPool semantics: hardcoded
GCC_MEMORYPOOL_DEFINITION(Default);
GCC_MEMORYPOOL_AUTOINIT(Default, 16);

} /* namespace ely */
