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
#include <pointerTo.h>
#include <nodePath.h>
#include <audioManager.h>
#include <audioSound.h>
#include <lvector3.h>
#include "Utilities/Tools.h"

/**
 * \brief Porting of python class direct.showbase.Audio3DManager.
 */
class Audio3DManager
{
public:
	//    def __init__(self, audio_manager, listener_target = None, root = None, taskPriority = 51)
	Audio3DManager(AudioManager* audio_manager, NodePath listener_target =
			NodePath(), NodePath root = NodePath(), int taskPriority = 51);
	virtual ~Audio3DManager();

//    def loadSfx(self, name):
	PT(AudioSound) loadSfx(const std::string& name);
//    def setDistanceFactor(self, factor):
	void setDistanceFactor(float factor);
//    def getDistanceFactor(self):
	float getDistanceFactor();
//    def setDopplerFactor(self, factor):
	void setDopplerFactor(float factor);
//    def getDopplerFactor(self):
	float getDopplerFactor();
//    def setDropOffFactor(self, factor):
	void setDropOffFactor(float factor);
//    def getDropOffFactor(self):
	float getDropOffFactor();
//    def setSoundMinDistance(self, sound, dist):
	void setSoundMinDistance(AudioSound* sound, float dist);
//    def getSoundMinDistance(self, sound):
	float getSoundMinDistance(AudioSound* sound);
//    def setSoundMaxDistance(self, sound, dist):
	void setSoundMaxDistance(AudioSound* sound, float dist);
//    def getSoundMaxDistance(self, sound):
	float getSoundMaxDistance(AudioSound* sound);
//    def setSoundVelocity(self, sound, velocity):
	void setSoundVelocity(AudioSound* sound, const LVector3& factor);
//    def setSoundVelocityAuto(self, sound):
	void setSoundVelocityAuto(AudioSound* sound);
//    def getSoundVelocity(self, sound):
	LVector3 getSoundVelocity(AudioSound* sound);
//    def setListenerVelocity(self, velocity):
	void setListenerVelocity(const LVector3& factor);
//    def setListenerVelocityAuto(self):
	void setListenerVelocityAuto();
//    def getListenerVelocity(self):
	LVector3 getListenerVelocity();
//    def attachSoundToObject(self, sound, object):
//    def detachSound(self, sound):
//    def getSoundsOnObject(self, object):
//    def attachListener(self, object):
//    def detachListener(self):
//    def update(self, task=None):
//    def disable(self):

protected:
	AudioManager* mAudioMgr;

};

#endif /* AUDIO3DMANAGER_H_ */
