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
 * \file /Ely/training/particles/Particles.h
 *
 * \date 2015-12-22
 * \author consultit
 */
#ifndef PARTICLES_PARTICLES_H_
#define PARTICLES_PARTICLES_H_

#include <particleSystem.h>

namespace ely
{

class Particles: public ParticleSystem
{
private:
	std::string name;

public:
	Particles(const std::string& name = std::string(), unsigned int poolSize =
			1024);
	virtual ~Particles();

protected:
	static unsigned int id;
};

} /* namespace ely */

#endif /* PARTICLES_PARTICLES_H_ */
