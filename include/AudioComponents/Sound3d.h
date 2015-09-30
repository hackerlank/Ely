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
#include <cfloat>
#include "ObjectModel/Component.h"
#include "ObjectModel/Object.h"

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
 * > **XML Param(s)**:
 * param | type | default | note
 * ------|------|---------|-----
 * | *scene_root* 			|single| *render* | -
 * | *sound_files* 			|multiple| - | each one specified as "sound_name1@sound_file1[:sound_name2@sound_file2:...:sound_nameN@sound_fileN]"
 *
 * \note parts inside [] are optional.\n
 */
class Sound3d: public Component
{
protected:
	friend class Sound3dTemplate;

	Sound3d(SMARTPTR(Sound3dTemplate)tmpl);
	virtual void reset();
	virtual bool initialize();
	virtual void onAddToObjectSetup();
	virtual void onRemoveFromObjectCleanup();
	virtual void onAddToSceneSetup();
	virtual void onRemoveFromSceneCleanup();

public:
	virtual ~Sound3d();

	struct Result: public Component::Result
	{
		Result(int value):Component::Result(value)
		{
		}
		enum
		{
		};
	};

	/**
	 * \brief Adds dynamically a new sound to this component by
	 * loading it from a file.
	 *
	 * The file is added only if this component already belongs to an object.
	 * @param soundName The sound name.
	 * @param fileName The sound file name.
	 * @return True if successful, false otherwise.
	 */
	Result addSound(const std::string& soundName, const std::string& fileName);

	/**
	 * \brief Removes dynamically a sound from this component.
	 *
	 * @param soundName The sound name.
	 * @return True if successful, false otherwise.
	 */
	Result removeSound(const std::string& soundName);

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
	ObjectId mSceneRootId;
	///The set of sounds attached to this component.
	SoundTable mSounds;
	/**
	 * \name Main parameters.
	 */
	///@{
	std::list<std::string> mSoundFileListParam;
	///@}

	///@{
	/// Sounds' characteristics.
	float mMinDist, mMaxDist;
	LPoint3f mPosition;
	///@}

	/**
	 * \brief Actually sets position/velocity for static objects.
	 */
	void doSet3dStaticAttributes();

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

///inline definitions

inline void Sound3d::reset()
{
	//
	mSceneRoot = NodePath();
	mSceneRootId = ObjectId();
	mSounds.clear();
	mSoundFileListParam.clear();
	mMinDist = FLT_MIN;
	mMaxDist = FLT_MAX;
	mPosition = LPoint3f::zero();
}

inline float Sound3d::getMinDistance()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return mMinDist;
}

inline float Sound3d::getMaxDistance()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return mMaxDist;
}

///Template

class Sound3dTemplate: public ComponentTemplate
{
protected:

	virtual SMARTPTR(Component)makeComponent(const ComponentId& compId);

public:
	Sound3dTemplate(PandaFramework* pandaFramework,
			WindowFramework* windowFramework);
	virtual ~Sound3dTemplate();

	virtual ComponentType componentType() const;
	virtual ComponentFamilyType familyType() const;

	virtual void setParametersDefaults();

private:

	///TypedObject semantics: hardcoded
public:
	static TypeHandle get_class_type()
	{
		return _type_handle;
	}

	static void init_type()
	{
		ComponentTemplate::init_type();
		register_type(_type_handle, "Sound3dTemplate",
				ComponentTemplate::get_class_type());
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
