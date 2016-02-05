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
 * \file /Ely/src/Support/particles/ParticleEffect.cpp
 *
 * \date 2016-02-04
 * \author consultit
 */
#include "ParticleEffect.h"

#include "SpriteParticleRendererExt.h"
#include <physicsManager.h>
#include <particleSystemManager.h>
#include <zSpinParticleFactory.h>
#include <pointParticleFactory.h>
//#include <orientedParticleFactory.h>
#include <pointParticleRenderer.h>
#include <lineParticleRenderer.h>
#include <geomParticleRenderer.h>
#include <sparkleParticleRenderer.h>
#include <arcEmitter.h>
#include <boxEmitter.h>
#include <discEmitter.h>
#include <lineEmitter.h>
#include <pointEmitter.h>
#include <rectangleEmitter.h>
#include <sphereSurfaceEmitter.h>
#include <sphereVolumeEmitter.h>
#include <tangentRingEmitter.h>

namespace ely
{

std::string ParticleEffect::set_real_name(const std::string& name)
{
	HOLD_REMUTEX(ParticleEffect::mMutexPid)

	std::string realName;
	if (name == "")
	{
		realName =
				std::string("particle-effect-")
						+ dynamic_cast<std::ostringstream&>(std::ostringstream().operator <<(
								ParticleEffect::pid)).str();
		ParticleEffect::pid++;
	}
	else
	{
		realName = name;
	}
	return realName;
}

unsigned int ParticleEffect::pid = 1;

ParticleEffect::ParticleEffect(const std::string& name, SMARTPTR(Particles)particles) :
NodePath(set_real_name(name)), fEnabled(false)
{
	// Record particle effect name
	this->name = NodePath::get_name();
	// Dictionary of particles and forceGroups
	particlesDict.clear();
	forceGroupDict.clear();
	// The effect's particle system
	if (particles != NULL)
	{
		addParticles(particles);
	}
	renderParent = NodePath();
}

ParticleEffect::~ParticleEffect()
{
}

void ParticleEffect::cleanup()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	remove_node();
	disable();
	for (std::map<std::string, SMARTPTR(ForceGroup)>::iterator it=forceGroupDict.begin(); it!=forceGroupDict.end(); ++it)
	{
		it->second->cleanup();
	}
	for (std::map<std::string, SMARTPTR(Particles)>::iterator it=particlesDict.begin(); it!=particlesDict.end(); ++it)
	{
		it->second->cleanup();
	}
	forceGroupDict.clear();
	particlesDict.clear();
	renderParent = NodePath();
}

void ParticleEffect::reset()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	removeAllForces();
	removeAllParticles();
	forceGroupDict.clear();
	particlesDict.clear();
}

void ParticleEffect::start(NodePath parent, NodePath renderParent)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	this->renderParent = renderParent;
	enable();
	if (parent != NodePath())
	{
		reparent_to(parent);
	}
}

void ParticleEffect::enable()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	// band-aid added for client crash - grw
	if (not renderParent.is_empty())
	{
		for (std::map<std::string, SMARTPTR(Particles)>::iterator it=particlesDict.begin(); it!=particlesDict.end(); ++it)
		{
			it->second->set_render_parent(renderParent.node());
		}
	}
	for (std::map<std::string, SMARTPTR(ForceGroup)>::iterator it=forceGroupDict.begin(); it!=forceGroupDict.end(); ++it)
	{
		it->second->enable();
	}
	for (std::map<std::string, SMARTPTR(Particles)>::iterator it=particlesDict.begin(); it!=particlesDict.end(); ++it)
	{
		it->second->enable();
	}
	fEnabled = true;
}

void ParticleEffect::disable()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	detach_node();
	// band-aid added for client crash - grw
	for (std::map<std::string, SMARTPTR(Particles)>::iterator it=particlesDict.begin(); it!=particlesDict.end(); ++it)
	{
		it->second->set_render_parent(it->second->getNode());
	}
	for (std::map<std::string, SMARTPTR(ForceGroup)>::iterator it=forceGroupDict.begin(); it!=forceGroupDict.end(); ++it)
	{
		it->second->disable();
	}
	for (std::map<std::string, SMARTPTR(Particles)>::iterator it=particlesDict.begin(); it!=particlesDict.end(); ++it)
	{
		it->second->disable();
	}
	fEnabled = false;
}

void ParticleEffect::addForceGroup(SMARTPTR(ForceGroup)forceGroup)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	forceGroup->getNodePath().reparent_to(*this);
	forceGroup->setParticleEffect(this);
	forceGroupDict[forceGroup->getName()] = forceGroup;

	// Associate the force group with all particles
	for (int i = 0; i < forceGroup->len(); ++i)
	{
		addForce((*forceGroup)[i]);
	}
}

void ParticleEffect::addForce(SMARTPTR(BaseForce)force)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	for (std::map<std::string, SMARTPTR(Particles)>::iterator it=particlesDict.begin(); it!=particlesDict.end(); ++it)
	{
		it->second->addForce(force);
	}
}

void ParticleEffect::removeForceGroup(SMARTPTR(ForceGroup)forceGroup)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	// Remove forces from all particles
	for (int i = 0; i < forceGroup->len(); ++i)
	{
		removeForce((*forceGroup)[i]);
	}

	forceGroup->getNodePath().remove_node();
	forceGroup->setParticleEffect(NULL);
	forceGroupDict.erase(forceGroup->getName());
}

void ParticleEffect::removeForce(SMARTPTR(BaseForce)force)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	for (std::map<std::string, SMARTPTR(Particles)>::iterator it=particlesDict.begin(); it!=particlesDict.end(); ++it)
	{
		it->second->removeForce(force);
	}
}

void ParticleEffect::removeAllForces()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	for (std::map<std::string, SMARTPTR(ForceGroup)>::iterator it=forceGroupDict.begin(); it!=forceGroupDict.end(); ++it)
	{
		removeForceGroup(it->second);
	}
}

void ParticleEffect::addParticles(SMARTPTR(Particles)particles)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	particles->getNodePath().reparent_to(*this);
	particlesDict[particles->getName()] = particles;

	// Associate all forces in all force groups with the particles
	for (std::map<std::string, SMARTPTR(ForceGroup)>::iterator it=forceGroupDict.begin(); it!=forceGroupDict.end(); ++it)
	{
		for (int i = 0; i < it->second->len(); ++i)
		{
			particles->addForce((*it->second)[i]);
		}
	}
}

void ParticleEffect::removeParticles(SMARTPTR(Particles)particles)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	RETURN_ON_COND(particles == NULL,)
	particles->getNodePath().detach_node();
	particlesDict.erase(particles->getName());

	// Remove all forces from the particles
	for (std::map<std::string, SMARTPTR(ForceGroup)>::iterator it=forceGroupDict.begin(); it!=forceGroupDict.end(); ++it)
	{
		for (int i = 0; i < it->second->len(); ++i)
		{
			SMARTPTR(BaseForce) f = (*it->second)[i];
			particles->removeForce(f);
		}
	}
}

void ParticleEffect::removeAllParticles()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	for (std::map<std::string, SMARTPTR(Particles)>::iterator it=particlesDict.begin(); it!=particlesDict.end(); ++it)
	{
		removeParticles(it->second);
	}
}

std::list<SMARTPTR(Particles)> ParticleEffect::getParticlesList()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	std::list<SMARTPTR(Particles)> l;
	for (std::map<std::string, SMARTPTR(Particles)>::iterator it=particlesDict.begin(); it!=particlesDict.end(); ++it)
	{
		l.push_back(it->second);
	}
	return l;
}

std::list<SMARTPTR(ForceGroup)> ParticleEffect::getForceGroupList()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	std::list<SMARTPTR(ForceGroup)> l;
	for (std::map<std::string, SMARTPTR(ForceGroup)>::iterator it=forceGroupDict.begin(); it!=forceGroupDict.end(); ++it)
	{
		l.push_back(it->second);
	}
	return l;
}

void ParticleEffect::accelerate(float time, int stepCount, float stepTime)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	std::list<SMARTPTR(Particles)>l = getParticlesList();
	for (std::list<SMARTPTR(Particles)>::iterator it=l.begin(); it!=l.end();++it)
	{
		(*it)->accelerate(time, stepCount, stepTime);
	}
}

void ParticleEffect::clearToInitial()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	std::list<SMARTPTR(Particles)>l = getParticlesList();
	for (std::list<SMARTPTR(Particles)>::iterator it=l.begin(); it!=l.end();++it)
	{
		(*it)->clear_to_initial();
	}
}

void ParticleEffect::softStop()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	std::list<SMARTPTR(Particles)>l = getParticlesList();
	for (std::list<SMARTPTR(Particles)>::iterator it=l.begin(); it!=l.end();++it)
	{
		(*it)->soft_stop();
	}
}

void ParticleEffect::softStart()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	std::list<SMARTPTR(Particles)>l = getParticlesList();
	for (std::list<SMARTPTR(Particles)>::iterator it=l.begin(); it!=l.end();++it)
	{
		(*it)->soft_start();
	}
}

} /* namespace ely */
