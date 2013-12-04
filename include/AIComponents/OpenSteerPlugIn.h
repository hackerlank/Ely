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
 * \file /Ely/include/AIComponents/OpenSteerPlugIn.h
 *
 * \date 04/dic/2013 (09:11:38)
 * \author consultit
 */
#ifndef OPENSTEERPLUGIN_H_
#define OPENSTEERPLUGIN_H_

#include "ObjectModel/Component.h"

namespace ely
{

class OpenSteerPlugInTemplate;

class OpenSteerPlugIn: public ely::Component
{
public:
	OpenSteerPlugIn();
	virtual ~OpenSteerPlugIn();
};

} /* namespace ely */

#endif /* OPENSTEERPLUGIN_H_ */
