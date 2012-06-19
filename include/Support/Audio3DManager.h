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
 * \file /Ely/include/Support/Audio3DManager.h
 *
 * \date 19/giu/2012 (09:02:08)
 * \author marco
 */

#ifndef AUDIO3DMANAGER_H_
#define AUDIO3DMANAGER_H_

#include <pandaFramework.h>
#include <referenceCount.h>
#include <pointerTo.h>
#include <nodePath.h>
#include <audioManager.h>
#include <audioSound.h>
#include <lvector3.h>
#include <map>
#include <set>
#include "Utilities/Tools.h"

/**
 * \brief Porting of python class direct.showbase.Audio3DManager.
 */
class Audio3DManager: public ReferenceCount
{
public:
	Audio3DManager(AudioManager* audio_manager, NodePath listener_target =
			NodePath(), NodePath root = NodePath(), int taskPriority = 51);
	virtual ~Audio3DManager();

	PT(AudioSound) loadSfx(const std::string& name);
	void setDistanceFactor(float factor);
	float getDistanceFactor();
	void setDopplerFactor(float factor);
	float getDopplerFactor();
	void setDropOffFactor(float factor);
	float getDropOffFactor();
	void setSoundMinDistance(AudioSound* sound, float dist);
	float getSoundMinDistance(AudioSound* sound);
	void setSoundMaxDistance(AudioSound* sound, float dist);
	float getSoundMaxDistance(AudioSound* sound);
	void setSoundVelocity(AudioSound* sound, const LVector3& factor);
	void setSoundVelocityAuto(AudioSound* sound);
	LVector3 getSoundVelocity(AudioSound* sound);
	void setListenerVelocity(const LVector3& factor);
	void setListenerVelocityAuto();
	LVector3 getListenerVelocity();
	bool attachSoundToObject(AudioSound* sound, NodePath object);
	bool detachSound(AudioSound* sound);
	std::set<AudioSound*> getSoundsOnObject(NodePath object);
	bool attachListener(NodePath object);
	bool detachListener();
	AsyncTask::DoneStatus update(GenericAsyncTask* task = NULL);
	void disable();

protected:
	AudioManager* mAudioMgr;

};

#endif /* AUDIO3DMANAGER_H_ */
