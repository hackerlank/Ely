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
	mNodePath.remove_node();
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

Model::operator NodePath()
{
	return mNodePath;
}

AnimControlCollection& Model::animations()
{
	return mAnimations;
}

void Model::update()
{
}

void Model::initialize()
{
	//setup model and animations
	mNodePath = mTmpl->windowFramework()->load_model(
			mTmpl->pandaFramework()->get_models(), mTmpl->modelFile());
	std::list<Filename>::iterator it;
	for (it = mTmpl->animFiles().begin(); it != mTmpl->animFiles().end(); ++it)
	{
		mTmpl->windowFramework()->load_model(mNodePath, *it);
		auto_bind(mNodePath.node(), mAnimations);
	}
	//setup initial state
	if (not mTmpl->parent().is_empty())
	{
		mNodePath.reparent_to(mTmpl->parent());
	}
	mNodePath.set_pos(mTmpl->initPosition());
	mNodePath.set_hpr(mTmpl->initOrientation());
}

