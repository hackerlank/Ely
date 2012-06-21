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
 * \file /Ely/include/AudioComponents/Sound3dTemplate.h
 *
 * \date 20/giu/2012 (12:47:59)
 * \author marco
 */

#ifndef SOUND3DTEMPLATE_H_
#define SOUND3DTEMPLATE_H_

#include <audioManager.h>
#include <string>
#include <list>
#include "ObjectModel/Component.h"
#include "ObjectModel/ComponentTemplate.h"
#include "AudioComponents/Sound3d.h"
#include "Utilities/Tools.h"

class Sound3dTemplate: public ComponentTemplate
{
public:
	Sound3dTemplate(AudioManager* audioMgr);
	virtual ~Sound3dTemplate();

	const virtual ComponentType componentType() const;
	const virtual ComponentFamilyType familyType() const;

	virtual Component* makeComponent(ComponentId& compId);

	virtual void setParameters(ParameterTable& parameterTable);
	virtual void resetParameters();

	/**
	 * \brief Gets/sets the list of the sounds file names.
	 * @return A reference to the list of the sounds file names.
	 */
	std::list<std::string>& soundFiles();

	/**
	 * \brief Gets/sets the AudioManager.
	 * @return A reference to the AudioManager.
	 */
	AudioManager*& audioManager();

private:
	///The AudioManager used to load dynamically sounds.
	AudioManager* mAudioMgr;
	///The names of the sound files.
	std::list<std::string> mSoundFiles;

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

#endif /* SOUND3DTEMPLATE_H_ */