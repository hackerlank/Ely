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
}

const ComponentFamilyType Model::familyType() const
{
	return ComponentFamilyType("Graphics");
}

const ComponentType Model::componentType() const
{
	return ComponentType("Model");
}

NodePath& Model::nodePath()
{
	return mNodePath;
}

Model::operator NodePath()
{
	return mNodePath;
}

void Model::onAddSetup()
{
	//set the node path of the object to the
	//node path of this model
	mOwnerObject->nodePath() = mNodePath;
}

AnimControlCollection& Model::animations()
{
	return mAnimations;
}

void Model::update()
{
}

bool Model::initialize()
{
	bool result = true;
	//setup model and animations
	mNodePath = mTmpl->windowFramework()->load_model(
			mTmpl->pandaFramework()->get_models(), mTmpl->modelFile());
	if (mNodePath.is_empty())
	{
		result = false;
	}
	std::list<Filename>::iterator it;
	for (it = mTmpl->animFiles().begin(); it != mTmpl->animFiles().end(); ++it)
	{
		NodePath resultNP;
		resultNP = mTmpl->windowFramework()->load_model(mNodePath, *it);
		if (resultNP.is_empty())
		{
			result = false;
		}
		auto_bind(mNodePath.node(), mAnimations);
	}
	return result;
}

//TypedObject semantics: hardcoded
TypeHandle Model::_type_handle;
