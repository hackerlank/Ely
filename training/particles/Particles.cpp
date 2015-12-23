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
 * \file /Ely/training/particles/Particles.cpp
 *
 * \date 2015-12-22
 * \author consultit
 */
#include "Particles.h"

namespace ely
{

unsigned int Particles::id = 0;

Particles::Particles(const std::string& name, unsigned int poolSize) :
		ParticleSystem(poolSize), name(name), factory(NULL), renderer(NULL), emitter(
				NULL), factoryType("undefined"), rendererType("undefined"), emitterType(
				"undefined"), fEnabled(false), geomReference("")
{
	if (not name)
	{
		name =
				std::string("particles-")
						+ dynamic_cast<std::ostringstream&>(std::ostringstream().operator <<(
								id)).str();
		id++;
	}
	else
	{
		this->name = name;
	}
	//
//	set_birth_rate(0.02);
//	set_litter_size(10);
//	set_litter_spread(0);
	// Set up a physical node
	node = new PhysicalNode(name);
	nodePath = NodePath(node);
	set_render_parent(node);
	node->add_physical(this);
	//
//	setFactory("PointParticleFactory");
//	setRenderer("PointParticleRenderer");
//	setEmitter("SphereVolumeEmitter");
	//
	// Enable particles by default
//	enable();
}

Particles::~Particles()
{
// TODO Auto-generated destructor stub
}

void Particles::enable()
{
}

void Particles::disable()
{
}

void Particles::setFactory(const std::string& type)
{
}

void Particles::setRenderer(const std::string& type)
{
}

void Particles::setEmitter(const std::string& type)
{
}

} /* namespace ely */
