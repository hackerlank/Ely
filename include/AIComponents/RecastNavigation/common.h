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
 * \file /Ely/include/AIComponents/RecastNavigation/common.h
 *
 * \date 25/giu/2013 10:38:16
 * \author marco
 */

#ifndef COMMON_H_
#define COMMON_H_

#include <lvector3.h>

namespace ely
{
//https://groups.google.com/forum/?fromgroups=#!searchin/recastnavigation/z$20axis/recastnavigation/fMqEAqSBOBk/zwOzHmjRsj0J
inline void LVecBase3fToRecast(const LVecBase3f& v, float* p)
{
	p[0] = v.get_x();
	p[1] = v.get_z();
	p[2] = -v.get_y();
}
inline LVecBase3f RecastToLVecBase3f(const float* p)
{
	return LVecBase3f(p[0], -p[2], p[1]);
}
inline LVecBase3f Recast3fToLVecBase3f(const float x, const float y, const float z)
{
	return LVecBase3f(x, -z, y);
}

} // namespace ely

#endif /* COMMON_H_ */
