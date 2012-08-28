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
 * \file /Ely/src/SceneComponents/Model.cpp
 *
 * \date 15/mag/2012 (15:32:23)
 * \author marco
 */

#include "SceneComponents/Model.h"
#include "SceneComponents/ModelTemplate.h"

Model::Model()
{
	// TODO Auto-generated constructor stub
}

Model::Model(SMARTPTR(ModelTemplate) tmpl)
{
	mTmpl = tmpl;
}

Model::~Model()
{
	mNodePath.remove_node();
}

const ComponentFamilyType Model::familyType() const
{
	return mTmpl->familyType();
}

const ComponentType Model::componentType() const
{
	return mTmpl.p()->componentType();
}

bool Model::initialize()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	bool result = true;
	//check if from file
	mFromFile =
			(mTmpl->parameter(std::string("from_file"))
					== std::string("false") ? false : true);
	if (mFromFile)
	{
		//setup model
		mNodePath = mTmpl->windowFramework()->load_model(
				mTmpl->pandaFramework()->get_models(),
				Filename(mTmpl->parameter(std::string("model_file"))));
		if (mNodePath.is_empty())
		{
			result = false;
		}
		//setup animations
		std::list<std::string>::iterator it;
		std::list<std::string> animFileList = mTmpl->parameterList(
				std::string("anim_files"));
		if (not animFileList.empty())
		{
			for (it = animFileList.begin(); it != animFileList.end(); ++it)
			{
				NodePath resultNP;
				resultNP = mTmpl->windowFramework()->load_model(mNodePath,
						Filename(*it));
				if (resultNP.is_empty())
				{
					result = false;
				}
			}
			//bind all loaded animations
			auto_bind(mNodePath.node(), mAnimations);
		}
	}
	else
	{
		//model is programmatically generated
		std::string modelType = mTmpl->parameter(std::string("model_type"));
		//card (e.g. finite plane)
		if (modelType == std::string("card"))
		{
			float left = (float) atof(
					mTmpl->parameter(std::string("model_card_left")).c_str());
			float right = (float) atof(
					mTmpl->parameter(std::string("model_card_right")).c_str());
			float bottom = (float) atof(
					mTmpl->parameter(std::string("model_card_bottom")).c_str());
			float top = (float) atof(
					mTmpl->parameter(std::string("model_card_top")).c_str());
			if ((right - left) * (top - bottom) == 0.0)
			{
				left = -1.0;
				right = 1.0;
				bottom = -1.0;
				top = 1.0;
			}
			CardMaker card("card" + std::string(mComponentId));
			card.set_frame(left, right, bottom, top);
			mNodePath = NodePath(card.generate());
		}
		else
		{
			result = false;
		}
	}
	//Scaling (default: (1.0,1.0,1.0))
	float scaleX = atof(mTmpl->parameter(std::string("scale_x")).c_str());
	float scaleY = atof(mTmpl->parameter(std::string("scale_y")).c_str());
	float scaleZ = atof(mTmpl->parameter(std::string("scale_z")).c_str());
	mNodePath.set_sx((scaleX != 0.0 ? scaleX : 1.0));
	mNodePath.set_sy((scaleY != 0.0 ? scaleY : 1.0));
	mNodePath.set_sz((scaleZ != 0.0 ? scaleZ : 1.0));
	//setup event callbacks if any
	setupEvents();
	//
	return result;
}

void Model::onAddToObjectSetup()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	//set the node path of the object to the
	//node path of this model
	mOwnerObject->setNodePath(mNodePath);
	//register event callbacks if any
	registerEventCallbacks();
}

AnimControlCollection Model::animations() const
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mAnimations;
}

NodePath Model::getNodePath() const
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mNodePath;
}

void Model::setNodePath(const NodePath& nodePath)
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	mNodePath = nodePath;
}

//TypedObject semantics: hardcoded
TypeHandle Model::_type_handle;
