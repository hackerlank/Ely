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
 * \file /Ely/include/Support/particles/ForceGroup.h
 *
 * \date 2016-02-04
 * \author consultit
 */
#ifndef FORCEGROUP_H_
#define FORCEGROUP_H_

#include "Utilities/Tools.h"
#include <forceNode.h>

namespace ely
{

class ParticleEffect;

class ForceGroup: public TypedReferenceCount
{
private:
	std::string name;
	//
	SMARTPTR(ForceNode)node;
	NodePath nodePath;
	bool fEnabled;
	ParticleEffect* particleEffect;
#ifdef ELY_THREAD
	///The mutex associated with this Component.
	ReMutex mMutex;
#endif

public:
	ForceGroup(std::string name = std::string());
	virtual ~ForceGroup();

	void cleanup();
	/**
	 * \brief Convenience function to enable all forces in force group.
	 */
	void enable();
	/**
	 * \brief Convenience function to disable all forces in force group.
	 */
	void disable();
	bool isEnabled();
	virtual void addForce(SMARTPTR(BaseForce) force);
	virtual void removeForce(SMARTPTR(BaseForce) force);
	// Get/set
	std::string getName();
	SMARTPTR(ForceNode) getNode();
	NodePath getNodePath();
	ParticleEffect* getParticleEffect();
	void setParticleEffect(ParticleEffect* particleEffect);
	// Utility functions
	SMARTPTR(BaseForce) operator[](int index);
	int len();
	std::list<SMARTPTR(BaseForce)> asList();

#ifdef ELY_THREAD
	/**
	 * \brief Get the mutex to lock the entire structure.
	 * @return The internal mutex.
	 */
	ReMutex& getMutex();
#endif

protected:
	static unsigned int id;
	static ReMutex mMutexId;

	///TypedObject semantics: hardcoded
public:
	static TypeHandle get_class_type()
	{
		return _type_handle;
	}
	static void init_type()
	{
		TypedReferenceCount::init_type();
		register_type(_type_handle, "ForceGroup", TypedReferenceCount::get_class_type());
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

inline bool ForceGroup::isEnabled()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return fEnabled;
}

inline std::string ForceGroup::getName()
{
	return name;
}

inline SMARTPTR(ForceNode)ForceGroup::getNode()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return node;
}

inline NodePath ForceGroup::getNodePath()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return nodePath;
}

inline ParticleEffect* ForceGroup::getParticleEffect()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return particleEffect;
}

inline void ForceGroup::setParticleEffect(ParticleEffect* particleEffect)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	this->particleEffect = particleEffect;
}

inline int ForceGroup::len()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return node->get_num_forces();
}

#ifdef ELY_THREAD
inline ReMutex& ForceGroup::getMutex()
{
	return mMutex;
}
#endif

}
/* namespace ely */

#endif /* FORCEGROUP_H_ */
