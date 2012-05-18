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
 * \file /Ely/include/GraphicsComponents/Model.h
 *
 * \date 15/mag/2012 (15:32:23)
 * \author marco
 */

#ifndef MODEL_H_
#define MODEL_H_

#include <string>
#include <list>
#include <pandaFramework.h>
#include <windowFramework.h>
#include <nodePath.h>
#include <animControlCollection.h>
#include <auto_bind.h>
#include <filename.h>
#include "ObjectModel/Component.h"

class ModelTemplate;

/**
 * \brief Component representing the graphics structure of an object.
 *
 * It contains the references to the model and animations of the object.
 */
class Model: public Component
{
public:
	Model();
	Model(ModelTemplate* tmpl);
	virtual ~Model();

	virtual const ComponentFamilyId familyID() const;
	virtual const ComponentId componentID() const;

	NodePath& nodePath();

	AnimControlCollection& animations();

	virtual void initialize();
	virtual void postAddSetup();

private:
	///The template used to construct this component.
	ModelTemplate* mTmpl;
	///The name of the model file containing the static model.
	std::string mModelFile;
	///The name of the file containing the animations.
	std::list<std::string> mAnimFiles;
	///The NodePath associated to this model.
	NodePath mNodePath;
	///The list of animations associated with this model.
	AnimControlCollection mAnimations;
};

#endif /* MODEL_H_ */
