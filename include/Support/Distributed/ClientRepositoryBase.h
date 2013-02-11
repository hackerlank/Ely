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
 * \file /Ely/include/Support/Distributed/ClientRepositoryBase.h
 *
 * \date 27/nov/2012 (18:20:38)
 * \author marco
 */

#ifndef CLIENTREPOSITORYBASE_H_
#define CLIENTREPOSITORYBASE_H_

#include "Utilities/Tools.h"

#include <cConnectionRepository.h>

/**
 * This maintains a client-side connection with a Panda server.
 * This base class exists to collect the common code between
 * ClientRepository, which is the CMU-provided, open-source version
 * of the client repository code, and OTPClientRepository, which is
 * the VR Studio's implementation of the same.
 */
class ClientRepositoryBase: public CConnectionRepository
{
public:
	ClientRepositoryBase();
	virtual ~ClientRepositoryBase();
};

#endif /* CLIENTREPOSITORYBASE_H_ */
