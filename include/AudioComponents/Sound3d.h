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
 * \author consultit
 */

#ifndef SOUND3D_H_
#define SOUND3D_H_

#include <audioSound.h>
#include <lvector3.h>
#include <lpoint3.h>
#include <nodePath.h>
#include <list>
#include "ObjectModel/Component.h"

namespace ely
{
class Sound3dTemplate;

/**
 * \brief Component manipulating 3d sounds attached to an object.
 *
 * This component both maintains the set of sounds attached to an
 * object and manages their automatic position/velocity update.\n
 * Sounds can be dynamically loaded/unloaded.\n
 * Only playing sounds will have their position/velocity updated.
 * For "static" objects sound position/velocity can be set directly
 * and is not automatically updated.\n
 * All updates is done wrt scene root (e.g. render).
 *
 * XML Param(s):
 * - "sound_files" 		|multiple|no default (each specified as "sound_name@sound_file")
 */
class Sound3d: public Component
{
public:
	Sound3d();
	Sound3d(SMARTPTR(Sound3dTemplate)tmpl);
	virtual ~Sound3d();

	const virtual ComponentFamilyType familyType() const;
	const virtual ComponentType componentType() const;

	virtual bool initialize();
	virtual void onAddToObjectSetup();
	virtual void onAddToSceneSetup();

	/**
	 * \brief Adds dynamically a new sound to this component by
	 * loading it from a file.
	 *
	 * The file is added only if this component already belongs to an object.
	 * @param soundName The sound name.
	 * @param fileName The sound file name.
	 * @return True if successful, false otherwise.
	 */
	bool addSound(const std::string& soundName, const std::string& fileName);
	/**
	 * \brief Removes dynamically a sound from this component.
	 *
	 * @param soundName The sound name.
	 * @return True if successful, false otherwise.
	 */
	bool removeSound(const std::string& soundName);

	/**
	 * \name Sets/gets Sound Min Distance.
	 * \brief Controls the distance (in units) that these sounds begin
	 * to fall off.
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
	void setMinDistance(float dist);
	float getMinDistance();
	///@}

	/**
	 * \name Sets/gets Sound Max Distance.
	 * \brief Controls the maximum distance (in units) that these sounds
	 * stop falling off.
	 *
	 * The sound does not stop at that point, it just doesn't get any quieter.
	 * You should rarely need to adjust this.
	 * Default is 1000000000.0
	 * @param sound The sound in question.
	 * @param dist The distance.
	 * @return The distance.
	 */
	///@{
	void setMaxDistance(float dist);
	float getMaxDistance();
	///@}

	/**
	 * \brief Set position/velocity for static objects.
	 *
	 * The position is the that of the owner object wrt scene root
	 * (i.e. render). Velocity is zero.
	 */
	void set3dStaticAttributes();
	/**
	 * \brief Gets a pointer to an audio sound given its name, or
	 * the index in the table (lexicographically ordered by name).
	 * @param name The sound name.
	 * @param index The sound index.
	 * @return The pointer to the audio sound (NULL otherwise).
	 */
	///@{
	typedef std::map<std::string, SMARTPTR(AudioSound)> SoundTable;
	SMARTPTR(AudioSound) getSound(const std::string& soundName);
	SMARTPTR(AudioSound) getSound(int index);
	///@}

	/**
	 * \brief Updates position and velocity of sounds of this component.
	 *
	 * Will be called automatically by an audio manager update.
	 * @param data The custom data.
	 */
	virtual void update(void* data);

private:
	///The root of the scene (e.g. render)
	NodePath mSceneRoot;
	///The set of sounds attached to this component.
	SoundTable mSounds;
	/**
	 * \name Main parameters.
	 */
	///@{
	std::list<std::string> mSoundFileList;
	///@}

	///@{
	/// Sounds' characteristics.
	float mMinDist, mMaxDist;
	LPoint3 mPosition;
	///@}

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
}  // namespace ely

#endif /* SOUND3D_H_ */
