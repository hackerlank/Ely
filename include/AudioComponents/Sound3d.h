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
 * \file /Ely/include/AudioComponents/Sound3d.h
 *
 * \date 20/giu/2012 (12:41:32)
 * \author marco
 */

#ifndef SOUND3D_H_
#define SOUND3D_H_

#include <audioManager.h>
#include <audioSound.h>
#include <pointerTo.h>
#include <map>
#include "ObjectModel/Component.h"
#include "Utilities/Tools.h"

class Sound3dTemplate;

/**
 * \brief Class manipulating 3d sounds attached to an object.
 *
 * This component both maintains the set of sounds attached to an
 * object and manages dynamic position/velocity update of them.
 * Sounds can be dynamically loaded/unloaded.
 */
class Sound3d: public Component
{
public:
	Sound3d();
	Sound3d(Sound3dTemplate* tmpl);
	virtual ~Sound3d();

	const virtual ComponentFamilyType familyType() const;
	const virtual ComponentType componentType() const;

	virtual bool initialize();
	virtual void onAddSetup();

	/**
	 * \brief Loads a new sound from a file and it to this component.
	 * @param fileName The sound file name.
	 * @return True if successful, false otherwise.
	 */
	bool loadSound(const std::string& fileName);
	/**
	 * \brief Removes a sound from this component.
	 * @param soundName The sound name.
	 * @return True if successful, false otherwise.
	 */
	bool removeSound(const std::string& soundName);

	/**
	 * \name Sets/gets Sound Min Distance.
	 * \brief Controls the distance (in units) that this sound begins to fall off.
	 *
	 * Also affects the rate it falls off.
	 * Default is 3.28 (in feet, this is 1 meter).
	 * Don't forget to change this when you change the DistanceFactor
	 * (through the AudioManager).
	 * @param sound The sound in question.
	 * @param dist The distance.
	 * @return The distance.
	 */
	///@{
	void setSoundMinDistance(AudioSound* sound, float dist);
	float getSoundMinDistance(AudioSound* sound);
	///@}

	/**
	 * \name Sets/gets Sound Max Distance.
	 * \brief Controls the maximum distance (in units) that this sound stops
	 * falling off.
	 *
	 * The sound does not stop at that point, it just doesn't get any quieter.
	 * You should rarely need to adjust this.
	 * Default is 1000000000.0
	 * @param sound The sound in question.
	 * @param dist The distance.
	 * @return The distance.
	 */
	///@{
	void setSoundMaxDistance(AudioSound* sound, float dist);
	float getSoundMaxDistance(AudioSound* sound);
	///@}

	/**
	 * \brief Set the velocity vector (in units/sec) of the sound, for
	 * calculating doppler shift.
	 *
	 * This is relative to the sound root (probably render).
	 * Default: VBase3(0, 0, 0)
	 * @param sound The sound in question.
	 * @param velocity The velocity.
	 */
	void setSoundVelocity(AudioSound* sound, const LVector3& velocity);
	/**
	 * \brief If velocity is set to auto, the velocity will be determined by
	 * the previous position of the object the sound is attached
	 * to and the frame dt.
	 *
	 * Make sure if you use this method that you remember to clear the previous
	 * transformation between frames.
	 * @param sound The sound in question.
	 */
	void setSoundVelocityAuto(AudioSound* sound);
	/**
	 * \brief Get the velocity of the sound.
	 * @param sound The sound in question.
	 * @return The velocity.
	 */
	LVector3 getSoundVelocity(AudioSound* sound);
	/**
	 * \brief Sound will come from the location of the object it is attached to.
	 * @param sound The sound in question.
	 */
	void attachSound(AudioSound* sound);
	/**
	 * \brief Sound will no longer have it's 3D position updated
	 * @param sound The sound in question.
	 */
	void detachSound(AudioSound* sound);
	/**
	 * \brief Gets a reference to the sound table indexed by the sound name,
	 * which is the file path.
	 */
	typedef std::map<std::string, PT(AudioSound)> SoundTable;
	SoundTable& sounds();

	/**
	 * \brief Updates position of sounds in the 3D audio system.
	 *
	 * Will be called automatically in a task.
	 * @param task The task.
	 * @return The "done" status.
	 */
	virtual AsyncTask::DoneStatus update(GenericAsyncTask* task);

private:
	///The template used to construct this component.
	Sound3dTemplate* mTmpl;
	///The set of sounds attached to this component.
	SoundTable mSounds;
	///The AudioManager used to load dynamically sounds.
	AudioManager* mAudioMgr;

	///TypedObject semantics: hardcoded
public:
	static TypeHandle get_class_type()
	{
		return _type_handle;
	}
	static void init_type()
	{
		Component::init_type();
		register_type(_type_handle, "Sound3d", Component::get_class_type());
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

#endif /* SOUND3D_H_ */
