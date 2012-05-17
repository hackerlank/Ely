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
 * \file /Ely/src/GraphicsComponents/Model.cpp
 *
 * \date 15/mag/2012 (15:32:23)
 * \author marco
 */

#include "GraphicsComponents/Model.h"
#include "GraphicsComponents/ModelTemplate.h"

Model::Model()
{
	// TODO Auto-generated constructor stub
}

Model::Model(ModelTemplate* tmpl) :
		mTmpl(tmpl)
{
}

Model::~Model()
{
	// TODO Auto-generated destructor stub
}

const ComponentFamilyId Model::familyID() const
{
	return ComponentFamilyId("Graphics");
}

const ComponentId Model::componentID() const
{
	return ComponentId("Model");
}

NodePath& Model::nodePath()
{
	return mNodePath;
}

AnimControlCollection& Model::animations()
{
	return mAnimations;
}

void Model::preSetup()
{
	mModelFile = mTmpl->modelFile();
	mAnimFiles = mTmpl->animFiles();
}

void Model::postSetup()
{
	//setup model and animations
	mNodePath = mOwnerObject->windowFramework()->load_model(
			mOwnerObject->pandaFramework()->get_models(), mModelFile);
	std::list<std::string>::iterator it;
	for (it = mAnimFiles.begin(); it != mAnimFiles.end(); ++it)
	{
		mOwnerObject->windowFramework()->load_model(mNodePath, Filename(*it));
		auto_bind(mNodePath.node(), mAnimations);
	}
	//set the NodePath of this component to be the object's one
	mOwnerObject->nodePath() = mNodePath;
}

