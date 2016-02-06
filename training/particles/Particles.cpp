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
 * \file /Ely/src/Support/particles/Particles.cpp
 *
 * \date 2015-12-22
 * \author consultit
 */
#include "Particles.h"
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
#include "GameParticlesManager.h"

namespace ely
{

unsigned int Particles::id = 1;

Particles::Particles(const std::string& _name, unsigned int poolSize) :
		ParticleSystem(poolSize), factory(NULL), renderer(NULL), emitter(
		NULL), factoryType("undefined"), rendererType("undefined"), emitterType(
				"undefined"), fEnabled(false), geomReference("")
{
	CHECK_EXISTENCE_DEBUG(GameParticlesManager::GetSingletonPtr(),
			"GameParticlesManager::GameParticlesManager: invalid GameParticlesManager")
	physicsMgr = GameParticlesManager::GetSingletonPtr()->physicsMgr();
	particleMgr = GameParticlesManager::GetSingletonPtr()->particleSystemMgr();
	//
	{
		HOLD_REMUTEX(Particles::mMutexId)

		name = _name;
		if (name == "")
		{
			name =
					std::string("particles-")
							+ dynamic_cast<std::ostringstream&>(std::ostringstream().operator <<(
									id)).str();
			id++;
		}
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
}

void Particles::cleanup()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	disable();
	clear_linear_forces();
	clear_angular_forces();
	set_render_parent(node);
	node->remove_physical(this);
	nodePath.remove_node();
	node.clear();
	nodePath.clear();
	factory.clear();
	renderer.clear();
	emitter.clear();
}

void Particles::enable()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	if (fEnabled == false)
	{
		physicsMgr->attach_physical(this);
		particleMgr->attach_particlesystem(this);
		fEnabled = true;
	}
}

void Particles::disable()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	if (fEnabled == true)
	{
		physicsMgr->remove_physical(this);
		particleMgr->remove_particlesystem(this);
		fEnabled = false;
	}
}

void Particles::setFactory(const std::string& type)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	if (factoryType == type)
	{
		return;
	}
	if (factory)
	{
		factory.clear();
	}
	factoryType = type;
	if (type == "PointParticleFactory")
	{
		factory = new PointParticleFactory();
	}
	else if (type == "ZSpinParticleFactory")
	{
		factory = new ZSpinParticleFactory();
	}
//	else if (type == "OrientedParticleFactory")
//	{
//		factory = new OrientedParticleFactory();
//	}
	else
	{
		PRINT_ERR_DEBUG("unknown factory type: " << type);
		return;
	}
	factory->set_lifespan_base(0.5);
	set_factory(factory);
}

void Particles::setRenderer(const std::string& type)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	if (rendererType == type)
	{
		return;
	}
	if (renderer)
	{
		renderer.clear();
	}
	rendererType = type;
	if (type == "PointParticleRenderer")
	{
		renderer = new PointParticleRenderer();
		dynamic_cast<PointParticleRenderer*>(renderer.p())->set_point_size(1.0);
	}
	else if (type == "LineParticleRenderer")
	{
		renderer = new LineParticleRenderer();
	}
	else if (type == "GeomParticleRenderer")
	{
		renderer = new GeomParticleRenderer();
#ifdef ELY_DEBUG
		///TODO
//		# This was moved here because we do not want to download
//		# the direct tools with toontown.
//		if __dev__:
//			from direct.directtools import DirectSelection
//			npath = NodePath('default-geom')
//			bbox = DirectSelection.DirectBoundingBox(npath)
//			self.renderer.setGeomNode(bbox.lines.node())
		///
#endif
	}
	else if (type == "SparkleParticleRenderer")
	{
		renderer = new SparkleParticleRenderer();
	}
	else if (type == "SpriteParticleRenderer")
	{
		renderer = new SpriteParticleRendererExt();
		dynamic_cast<SpriteParticleRendererExt*>(renderer.p())->setTextureFromFile();
	}
	else
	{
		PRINT_ERR_DEBUG("unknown renderer type: " << type);
		return;
	}
	set_renderer(renderer);
}

void Particles::setEmitter(const std::string& type)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	if (emitterType == type)
	{
		return;
	}
	if (emitter)
	{
		emitter.clear();
	}
	emitterType = type;
	if (type == "ArcEmitter")
	{
		emitter = new ArcEmitter();
	}
	else if (type == "BoxEmitter")
	{
		emitter = new BoxEmitter();
	}
	else if (type == "DiscEmitter")
	{
		emitter = new DiscEmitter();
	}
	else if (type == "LineEmitter")
	{
		emitter = new LineEmitter();
	}
	else if (type == "PointEmitter")
	{
		emitter = new PointEmitter();
	}
	else if (type == "RectangleEmitter")
	{
		emitter = new RectangleEmitter();
	}
	else if (type == "RingEmitter")
	{
		emitter = new RingEmitter();
	}
	else if (type == "SphereSurfaceEmitter")
	{
		emitter = new SphereSurfaceEmitter();
	}
	else if (type == "SphereVolumeEmitter")
	{
		emitter = new SphereVolumeEmitter();
		dynamic_cast<SphereVolumeEmitter*>(emitter.p())->set_radius(1.0);
	}
	else if (type == "TangentRingEmitter")
	{
		emitter = new TangentRingEmitter();
	}
	else
	{
		PRINT_ERR_DEBUG("unknown emitter type: " << type);
		return;
	}
	set_emitter(emitter);
}

void Particles::addForce(SMARTPTR(BaseForce)force)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	if (force->is_linear())
	{
		add_linear_force(dynamic_cast<LinearForce*>(force.p()));
	}
	else
	{
		add_angular_force(dynamic_cast<AngularForce*>(force.p()));
	}
}

void Particles::removeForce(SMARTPTR(BaseForce)force)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	RETURN_ON_COND(force.is_null(),)
	if (force->is_linear())
	{
		remove_linear_force(dynamic_cast<LinearForce*>(force.p()));
	}
	else
	{
		remove_angular_force(dynamic_cast<AngularForce*>(force.p()));
	}
}

std::map<std::string, float> Particles::getPoolSizeRanges()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	int litterRange[3] =
	{
			max(1, get_litter_size() - get_litter_spread()),
			get_litter_size(),
			get_litter_size() + get_litter_spread()
	};
	float lifespanRange[3] =
	{
			factory->get_lifespan_base() - factory->get_lifespan_spread(),
			factory->get_lifespan_base(),
			factory->get_lifespan_base() + factory->get_lifespan_spread()
	};
	float birthRateRange[3] =
	{
			get_birth_rate(),
			get_birth_rate(),
			get_birth_rate()
	};

	PRINT_DEBUG("Litter Ranges:    " << litterRange);
	PRINT_DEBUG("LifeSpan Ranges:  " << lifespanRange);
	PRINT_DEBUG("BirthRate Ranges: " << birthRateRange);

	std::map<std::string, float> dict;
	dict["min"] = litterRange[0] * lifespanRange[0] / birthRateRange[0];
	dict["median"] = litterRange[1] * lifespanRange[1] / birthRateRange[1];
	dict["max"] = litterRange[2] * lifespanRange[2] / birthRateRange[2];
	return dict;
}

void Particles::accelerate(float time, int stepCount, float stepTime)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	if (time > 0.0)
	{
		float remainder;
		if (stepTime == 0.0)
		{
			stepTime = float(time) / stepCount;
			remainder = 0.0;
		}
		else
		{
			stepCount = int(float(time) / stepTime);
			remainder = time - stepCount * stepTime;
		}

		for (int var = 0; var < stepCount; ++var)
		{
			particleMgr->do_particles(stepTime, this, false);
			physicsMgr->do_physics(stepTime, this);
		}

		if (remainder)
		{
			particleMgr->do_particles(remainder, this, false);
			physicsMgr->do_physics(remainder, this);
		}

		render();
	}
}

//TypedObject semantics: hardcoded
TypeHandle Particles::_type_handle;

} /* namespace ely */
