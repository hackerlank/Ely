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

Particles::Particles(const std::string& name, unsigned int poolSize) :
		ParticleSystem(poolSize), name(name)
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
	//# self.setBirthRate(0.02)
	//# self.setLitterSize(10)
	//# self.setLitterSpread(0)
//
//# Set up a physical node
//self.node = PhysicalNode(self.name)
//self.nodePath = NodePath(self.node)
//self.setRenderParent(self.node)
//self.node.addPhysical(self)
//
//self.factory = None
//self.factoryType = "undefined"
//# self.setFactory("PointParticleFactory")
//self.renderer = None
//self.rendererType = "undefined"
//# self.setRenderer("PointParticleRenderer")
//self.emitter = None
//self.emitterType = "undefined"
//# self.setEmitter("SphereVolumeEmitter")
//
//# Enable particles by default
//self.fEnabled = 0
//#self.enable()
//self.geomReference = ""

}

Particles::~Particles()
{
// TODO Auto-generated destructor stub
}

unsigned int Particles::id = 0;

} /* namespace ely */
