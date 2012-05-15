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
 * \file /Ely/src/Graphics/ModelComponent.cpp
 *
 * \date 15/mag/2012 (15:32:23)
 * \author marco
 */

#include "Graphics/ModelComponent.h"

ModelComponent::ModelComponent()
{
	// TODO Auto-generated constructor stub

}

ModelComponent::~ModelComponent()
{
	// TODO Auto-generated destructor stub
}

const ComponentFamilyId ModelComponent::familyID()
{
	return ComponentFamilyId("Model");
}

const ComponentId ModelComponent::componentID()
{
	return ComponentId("Model");
}

const std::string& ModelComponent::getFilename() const
{
	return mFilename;
}

void ModelComponent::setFilename(const std::string& filename)
{
	mFilename = filename;
}

const NodePath& ModelComponent::getNodePath() const
{
	return mNodePath;
}

void ModelComponent::setNodePath(const NodePath& nodePath)
{
	mNodePath = nodePath;
}

const AnimControlCollection& ModelComponent::getAnimations() const
{
	return mAnimations;
}

void ModelComponent::setAnimations(const AnimControlCollection& animations)
{
	mAnimations = animations;
}


