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
#include <zSpinParticleFactory.h>
#include <orientedParticleFactory.h>
#include <lineParticleRenderer.h>
#include <geomParticleRenderer.h>
#include <sparkleParticleRenderer.h>
#include "SpriteParticleRendererExt.h"

namespace ely
{

unsigned int Particles::id = 0;

Particles::Particles(const std::string& name, unsigned int poolSize) :
		ParticleSystem(poolSize), name(name), factory(NULL), renderer(NULL), emitter(
		NULL), factoryType("undefined"), rendererType("undefined"), emitterType(
				"undefined"), fEnabled(false), geomReference("")
{
	///TODO
	physicsMgr = NULL; //TOBE inizialized
	particleMgr = NULL; //TOBE inizialized
	///
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

bool Particles::isEnabled()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return fEnabled;
}

SMARTPTR(PhysicalNode)Particles::getNode()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return node;
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
	else if (type == "OrientedParticleFactory")
	{
		factory = OrientedParticleFactory();
	}
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
		// This was moved here because we do not want to download
		// the direct tools with toontown.
		///TODO
//		if __dev__:
//			from direct.directtools import DirectSelection
//			npath = NodePath('default-geom')
//			bbox = DirectSelection.DirectBoundingBox(npath)
//			self.renderer.setGeomNode(bbox.lines.node());
		///
	}
	else if (type == "SparkleParticleRenderer")
	{
		renderer = new SparkleParticleRenderer();
	}
	else if (type == "SpriteParticleRenderer")
	{
		renderer = new SpriteParticleRendererExt();
		// dynamic_cast<SpriteParticleRendererExt*>(renderer.p())->setTextureFromFile();
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

}

void Particles::addForce(SMARTPTR(BaseForce)force)
{
}

void Particles::removeForce(SMARTPTR(BaseForce)force)
{
}

void Particles::setRenderNodePath(NodePath nodePath)
{
}

std::string Particles::getName()
{
}

SMARTPTR(BaseParticleFactory)Particles::getFactory()
{
}

SMARTPTR(BaseParticleEmitter)Particles::getEmitter()
{
}

SMARTPTR(BaseParticleEmitter)Particles::getRenderer()
{
}

void Particles::accelerate(float time, int stepCount, float stepTime)
{
}

//TypedObject semantics: hardcoded
TypeHandle Particles::_type_handle;

} /* namespace ely */
