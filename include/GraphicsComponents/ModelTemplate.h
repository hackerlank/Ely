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
 * \file /Ely/include/GraphicsComponents/ModelTemplate.h
 *
 * \date 16/mag/2012 (16:26:41)
 * \author marco
 */

#ifndef MODELTEMPLATE_H_
#define MODELTEMPLATE_H_

#include <string>
#include <list>
#include "ObjectModel/ComponentTemplate.h"
#include "ObjectModel/Component.h"

class ModelTemplate: public ComponentTemplate
{
public:
	ModelTemplate();
	virtual ~ModelTemplate();

	const virtual ComponentId componentID() const;
	const virtual ComponentFamilyId familyID() const;

	virtual Component* makeComponent();

	std::string& modelFile();

	std::list<std::string>& animFiles();

private:
	///The name of the model file containing the static model.
	std::string mModelFile;
	///The name of the file containing the animations.
	std::list<std::string> mAnimFiles;

};

#endif /* MODELTEMPLATE_H_ */
