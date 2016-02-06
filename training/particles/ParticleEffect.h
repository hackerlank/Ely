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
 * \file /Ely/include/Support/particles/ParticleEffect.h
 *
 * \date 2016-02-04
 * \author consultit
 */
#ifndef PARTICLEEFFECT_H_
#define PARTICLEEFFECT_H_

#include "Utilities/Tools.h"
#include "Particles.h"
#include "ForceGroup.h"

namespace ely
{

class ParticleEffect: public NodePath
{
private:
	std::string name;
	//
	bool fEnabled;
	std::map<std::string, SMARTPTR(Particles)> particlesDict;
	std::map<std::string, SMARTPTR(ForceGroup)> forceGroupDict;
	NodePath renderParent;
#ifdef ELY_THREAD
	///The mutex associated with this Component.
	ReMutex mMutex;
#endif

public:
	ParticleEffect(const std::string& name = std::string(),
			SMARTPTR(Particles) particles = NULL);
	virtual ~ParticleEffect();

	void cleanup();
	std::string get_name();
	void reset();
	void start(NodePath parent = NodePath(), NodePath renderParent = NodePath());
	void enable();
	void disable();
	bool isEnabled();
	void addForceGroup(SMARTPTR(ForceGroup) forceGroup);
	void addForce(SMARTPTR(BaseForce) force);
	void removeForceGroup(SMARTPTR(ForceGroup) forceGroup);
	void removeForce(SMARTPTR(BaseForce) force);
	void removeAllForces();
	void addParticles(SMARTPTR(Particles) particles);
	void removeParticles(SMARTPTR(Particles) particles);
	void removeAllParticles();
	std::list<SMARTPTR(Particles)> getParticlesList();
	SMARTPTR(Particles) getParticlesNamed(const std::string& name);
	std::map<std::string, SMARTPTR(Particles)> getParticlesDict();
	std::list<SMARTPTR(ForceGroup)> getForceGroupList();
	SMARTPTR(ForceGroup) getForceGroupNamed(const std::string& name);
	std::map<std::string, SMARTPTR(ForceGroup)> getForceGroupDict();
	void accelerate(float time, int stepCount = 1, float stepTime=0.0);
	void clearToInitial();
	void softStop();
	void softStart();

#ifdef ELY_THREAD
	/**
	 * \brief Get the mutex to lock the entire structure.
	 * @return The internal mutex.
	 */
	ReMutex& getMutex();
#endif

protected:
	static unsigned int pid;
	static ReMutex mMutexPid;
};

///inline definitions

inline std::string ParticleEffect::get_name()
{
	// override NodePath.getName()
	return name;
}

inline bool ParticleEffect::isEnabled()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	//Note: this may be misleading if enable(), disable() not used
	return fEnabled;
}

inline SMARTPTR(Particles)ParticleEffect::getParticlesNamed(const std::string& name)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return particlesDict[name];
}

inline std::map<std::string, SMARTPTR(Particles)>ParticleEffect::getParticlesDict()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return particlesDict;
}

inline SMARTPTR(ForceGroup)ParticleEffect::getForceGroupNamed(const std::string& name)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return forceGroupDict[name];
}

inline std::map<std::string, SMARTPTR(ForceGroup)> ParticleEffect::getForceGroupDict()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return forceGroupDict;
}

#ifdef ELY_THREAD
inline ReMutex& ParticleEffect::getMutex()
{
	return mMutex;
}
#endif

}
/* namespace ely */

#endif /* PARTICLEEFFECT_H_ */
