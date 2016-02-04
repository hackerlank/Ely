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
 * \file /Ely/include/Support/particles/Particles.h
 *
 * \date 2015-12-22
 * \author consultit
 */
#ifndef PARTICLES_H_
#define PARTICLES_H_

#include "Utilities/Tools.h"
#include <particleSystem.h>
#include <physicalNode.h>
#include <baseParticleFactory.h>
#include <baseParticleRenderer.h>
#include <baseParticleEmitter.h>
#include <baseForce.h>

namespace ely
{

class Particles: public ParticleSystem
{
private:
	PhysicsManager* physicsMgr;
	ParticleSystemManager* particleMgr;
	//
	std::string name;
	NodePath nodePath;
	//
	SMARTPTR(PhysicalNode)node;
	SMARTPTR(BaseParticleFactory) factory;
	SMARTPTR(BaseParticleRenderer) renderer;
	SMARTPTR(BaseParticleEmitter) emitter;
	std::string factoryType, rendererType, emitterType;
	bool fEnabled;
	std::string geomReference;
#ifdef ELY_THREAD
	///The mutex associated with this Component.
	ReMutex mMutex;
#endif

public:
	Particles(std::string name = std::string(), unsigned int poolSize =
			1024);
	virtual ~Particles();

	void cleanup();
	void enable();
	void disable();
	bool isEnabled();
	SMARTPTR(PhysicalNode) getNode();
	void setFactory(const std::string& type);
	void setRenderer(const std::string& type);
	void setEmitter(const std::string& type);
	void addForce(SMARTPTR(BaseForce) force);
	void removeForce(SMARTPTR(BaseForce) force);
	void setRenderNodePath(NodePath nodePath);
	// Getters
	std::string getName();
	SMARTPTR(BaseParticleFactory) getFactory();
	SMARTPTR(BaseParticleEmitter) getEmitter();
	SMARTPTR(BaseParticleRenderer) getRenderer();
	std::map<std::string,float> getPoolSizeRanges();
	void accelerate(float time, int stepCount = 1, float stepTime=0.0);

protected:
	static unsigned int id;

	///TypedObject semantics: hardcoded
public:
	static TypeHandle get_class_type()
	{
		return _type_handle;
	}
	static void init_type()
	{
		ParticleSystem::init_type();
		register_type(_type_handle, "Particles", ParticleSystem::get_class_type());
	}
	virtual TypeHandle get_type() const
	{
		return get_class_type();
	}
	virtual TypeHandle force_init_type()
	{
		init_type();
		return get_class_type();
	}

private:
	static TypeHandle _type_handle;

};

///inline definitions

}
/* namespace ely */

#endif /* PARTICLES_H_ */
