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
 * \file /Ely/include/Graphics/ModelComponent.h
 *
 * \date 15/mag/2012 (15:32:23)
 * \author marco
 */

#ifndef MODELCOMPONENT_H_
#define MODELCOMPONENT_H_

#include <string>
#include <map>
#include <nodePath.h>
#include <animControlCollection.h>
#include "ObjectModel/Component.h"

/**
 * \brief Component representing the graphics structure of an object.
 *
 * It contains the references to the model and animations of the object.
 */
class ModelComponent: public Component
{
public:
	ModelComponent();
	virtual ~ModelComponent();
	const virtual ComponentFamilyId familyID();
	const virtual ComponentId componentID();
	const std::string& getFilename() const;
	void setFilename(const std::string& filename);
	const NodePath& getNodePath() const;
	void setNodePath(const NodePath& nodePath);
	const AnimControlCollection& getAnimations() const;
	void setAnimations(const AnimControlCollection& animations);

private:
	///The name of the file containing the static model.
	std::string mFilename;
	///The NodePath associated to this model.
	NodePath mNodePath;
	///The list of animations associated with this model.
	AnimControlCollection mAnimations;
};

#endif /* MODELCOMPONENT_H_ */
