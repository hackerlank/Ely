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
 * \author consultit
 */

#include "SceneComponents/Model.h"
#include "SceneComponents/ModelTemplate.h"
#include "ObjectModel/Object.h"
#include "Game/GameSceneManager.h"
#include <animBundleNode.h>
#include <partBundleNode.h>
#include <cardMaker.h>

namespace ely
{

Model::Model()
{
	// TODO Auto-generated constructor stub
}

Model::Model(SMARTPTR(ModelTemplate)tmpl)
{
	CHECKEXISTENCE(GameSceneManager::GetSingletonPtr(),
			"Model::Model: invalid GameSceneManager")
	mTmpl = tmpl;
}

Model::~Model()
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	mNodePath.remove_node();
}

ComponentFamilyType Model::familyType() const
{
	return mTmpl->familyType();
}

ComponentType Model::componentType() const
{
	return mTmpl->componentType();
}

void Model::r_find_bundles(SMARTPTR(PandaNode)node, Anims& anims, Parts& parts)
{
	//on empty node return
		if (node.is_null())
		{
			return;
		}
		if (node->is_of_type(AnimBundleNode::get_class_type()))
		{
			SMARTPTR(AnimBundleNode) bn = DCAST(AnimBundleNode, node.p());
			SMARTPTR(AnimBundle) bundle = bn->get_bundle();
			anims[bundle->get_name()].insert(bundle);

		}
		else if (node->is_of_type(PartBundleNode::get_class_type()))
		{
			SMARTPTR(PartBundleNode) bn = DCAST(PartBundleNode, node.p());
			int num_bundles = bn->get_num_bundles();
			for (int i = 0; i < num_bundles; ++i)
			{
				PartBundle *bundle = bn->get_bundle(i);
				parts[bundle->get_name()].insert(bundle);
			}
		}

		PandaNode::Children cr = node->get_children();
		int num_children = cr.get_num_children();
		for (int i = 0; i < num_children; i++)
		{
			r_find_bundles(cr.get_child(i), anims, parts);
		}
	}

bool Model::initialize()
{
	bool result = true;
	//check if from file
	mFromFile = (
			mTmpl->parameter(std::string("from_file")) == std::string("false") ?
					false : true);
	//get model
	mModelName = mTmpl->parameter(std::string("model_file"));
	//get more animations
	mAnimFileList = mTmpl->parameterList(std::string("anim_files"));
	//get model if procedurally generated
	mModelType = mTmpl->parameter(std::string("model_type"));
	//get card parameters
	mCardLeft = (float) strtof(
			mTmpl->parameter(std::string("model_card_left")).c_str(), NULL);
	mCardRight = (float) strtof(
			mTmpl->parameter(std::string("model_card_right")).c_str(), NULL);
	mCardBottom = (float) strtof(
			mTmpl->parameter(std::string("model_card_bottom")).c_str(), NULL);
	mCardTop = (float) strtof(
			mTmpl->parameter(std::string("model_card_top")).c_str(), NULL);
	//get scaling (default: (1.0,1.0,1.0))
	std::vector<std::string> scaleStr = parseCompoundString(
			mTmpl->parameter(std::string("scale")), ',');
	mScale[0] = mScale[1] = mScale[2] = 1.0;
	for (unsigned int i = 0; (i < 3) and (i < scaleStr.size()); ++i)
	{
		mScale[i] = strtof(scaleStr[i].c_str(), NULL);
	}
	//
	return result;
}

void Model::onAddToObjectSetup()
{
	//build model
	if (mFromFile)
	{
		PRINT("'" <<getOwnerObject()->objectId()
				<< "'::'" << mComponentId << "'::onAddToObjectSetup");
		// some declarations
		Parts parts;
		Anims anims;
		Parts::const_iterator partsIter;
		Anims::const_iterator animsIter;
		PartBundles::const_iterator partBundlesIter;
		AnimBundles::const_iterator animBundlesIter;
		parts.clear();
		anims.clear();
		//setup model (with possible animations)
		//mModelName can have this form: [anim_name1@anim_name2@
		// ...@anim_nameN@]model_filename ([] means optional)
		std::vector<std::string> animsFileNames = parseCompoundString(
				mModelName, '@');
		if (animsFileNames.empty())
		{
			animsFileNames.push_back("");
		}
		//use the last element (model file name)
		std::string modelFileName = animsFileNames.back();
		//remove last element
		animsFileNames.pop_back();
		mNodePath = mTmpl->windowFramework()->load_model(
				mTmpl->pandaFramework()->get_models(), Filename(modelFileName));
		if (mNodePath.is_empty())
		{
			//On error loads our favorite blue triangle.
			mTmpl->windowFramework()->load_default_model(
					mTmpl->pandaFramework()->get_models());
		}
		//find all the bundles into mNodePath.node
		r_find_bundles(mNodePath.node(), anims, parts);
		mFirstPartBundle.clear();
		//check if there is at least one PartBundle
		for (partsIter = parts.begin(); partsIter != parts.end(); ++partsIter)
		{
			for (partBundlesIter = partsIter->second.begin();
					partBundlesIter != partsIter->second.end();
					++partBundlesIter)
			{
				if (not mFirstPartBundle)
				{
					//set the first PartBundle
					mFirstPartBundle = *partBundlesIter;
					PRINT(
							"\tFirst PartBundle: '" << (*partBundlesIter)->get_name() << "'");
				}
				else
				{
					PRINT(
							"\tNext PartBundle: '" << (*partBundlesIter)->get_name() << "'");
				}
			}
		}
		//proceeds with animations only if there is at least one PartBundle
		if (mFirstPartBundle)
		{
			//check if there are some AnimBundles within the model file
			//and bind them to the first PartBundle
			std::string animName;
			int j = 1;
			for (animsIter = anims.begin(); animsIter != anims.end();
					++animsIter)
			{
				for (animBundlesIter = animsIter->second.begin();
						animBundlesIter != animsIter->second.end();
						++animBundlesIter)
				{
					if (not animsFileNames.empty())
					{
						//anim file names specified not finished:
						//use the first name
						animName = animsFileNames.front();
						//remove first name
						animsFileNames.erase(animsFileNames.begin());
					}
					else
					{
						//anim names finished
						animName = modelFileName + '.' + format_string(j);
						++j;
					}
					PRINT(
							"\tBinding animation '" << (*animBundlesIter)->get_name() <<
							"' (from '" << modelFileName << "') with name '" << animName << "'");
					SMARTPTR(AnimControl)control = (mFirstPartBundle->bind_anim(*animBundlesIter,
							PartGroup::HMF_ok_wrong_root_name|PartGroup::HMF_ok_part_extra|PartGroup::HMF_ok_anim_extra)).p();
					mAnimations.store_anim(control, animName);
				}
			}

			//setup more animations (if any)
			std::list<std::string>::iterator iter;
			for (iter = mAnimFileList.begin(); iter != mAnimFileList.end();
					++iter)
			{
				//any "anim_files" string is a "compound" one, i.e. could have the form:
				// "anim_name1@anim_file1:anim_name2@anim_file2:...:anim_nameN@anim_fileN"
				std::vector<std::string> nameFilePairs = parseCompoundString(*iter,
						':');
				std::vector<std::string>::const_iterator iterPair;
				for (iterPair = nameFilePairs.begin();
						iterPair != nameFilePairs.end(); ++iterPair)
				{
					//an empty anim_name@anim_file is ignored
					if (not iterPair->empty())
					{
						parts.clear();
						anims.clear();
						//get anim name and anim file name
						std::vector<std::string> nameFilePair =
								parseCompoundString(*iterPair, '@');
						//check only if there is a pair
						if (nameFilePair.size() == 2)
						{
							//anim name == nameFilePair[0]
							//anim file name == nameFilePair[1]
							//get the AnimBundle node path
							NodePath animNP = mTmpl->windowFramework()->load_model(
									mNodePath, Filename(nameFilePair[1]));
							if (animNP.is_empty())
							{
								animNP = NodePath();
							}
							//find all the bundles into animNP.node
							r_find_bundles(animNP.node(), anims, parts);
							for (animsIter = anims.begin();
									animsIter != anims.end(); ++animsIter)
							{
								int j;
								for (j = 0, animBundlesIter =
										animsIter->second.begin();
										animBundlesIter != animsIter->second.end();
										++animBundlesIter, ++j)
								{
									if (j > 0)
									{
										animName = nameFilePair[0] + '.'
												+ format_string(j);
									}
									else
									{
										animName = nameFilePair[0];
									}
									PRINT(
											"\tBinding animation '" << (*animBundlesIter)->get_name() <<
											"' (from '" << nameFilePair[1] << "') with name '" << animName << "'");
									SMARTPTR(AnimControl)control = (mFirstPartBundle->bind_anim(*animBundlesIter,
											PartGroup::HMF_ok_wrong_root_name|PartGroup::HMF_ok_part_extra|PartGroup::HMF_ok_anim_extra)).p();
									mAnimations.store_anim(control, animName);
								}
							}
						}
					}
				}
			}
		}
	}
	else
	{
		//model is programmatically generated
		//card (e.g. finite plane)
		if (mModelType == std::string("card"))
		{
			if ((mCardRight - mCardLeft) * (mCardTop - mCardBottom) == 0.0)
			{
				mCardLeft = -1.0;
				mCardRight = 1.0;
				mCardBottom = -1.0;
				mCardTop = 1.0;
			}
			//Component standard name: ObjectId_ObjectType_ComponentId_ComponentType
			std::string name = COMPONENT_STANDARD_NAME;
			CardMaker card("card" + name);
			card.set_frame(mCardLeft, mCardRight, mCardBottom, mCardTop);
			mNodePath = NodePath(card.generate());
		}
		else
		{
			//On error loads our favorite blue triangle.
			mTmpl->windowFramework()->load_default_model(
					mTmpl->pandaFramework()->get_models());
		}
	}

	//set scaling (default: (1.0,1.0,1.0))
	mNodePath.set_scale(mScale[0], mScale[1], mScale[2]);

	//Rename the node
	//Component standard name: ObjectId_ObjectType_ComponentId_ComponentType
	std::string name = COMPONENT_STANDARD_NAME;
	mNodePath.set_name(name);

	//set the node path of the object to the
	//node path of this model
	mOwnerObject->setNodePath(mNodePath);
	//setup event callbacks if any
	setupEvents();
	//register event callbacks if any
	registerEventCallbacks();
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

SMARTPTR(PartBundle)Model::getPartBundle() const
{
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	return mFirstPartBundle;
}

//TypedObject semantics: hardcoded
TypeHandle Model::_type_handle;

} // namespace ely
