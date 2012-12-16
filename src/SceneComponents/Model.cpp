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

const ComponentFamilyType Model::familyType() const
{
	return mTmpl->familyType();
}

const ComponentType Model::componentType() const
{
	return mTmpl->componentType();
}

void Model::r_find_bundles(SMARTPTR(PandaNode)node, Anims& anims, Parts& parts)
{
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
	//lock (guard) the mutex
	HOLDMUTEX(mMutex)

	bool result = true;
	//check if from file
	mFromFile = (
			mTmpl->parameter(std::string("from_file")) == std::string("false") ?
					false : true);
	if (mFromFile)
	{
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
		std::string modelName = mTmpl->parameter(std::string("model_file"));
		mNodePath = mTmpl->windowFramework()->load_model(
				mTmpl->pandaFramework()->get_models(), Filename(modelName));
		if (mNodePath.is_empty())
		{
			result = false;
		}
		//find all the bundles into mNodePath.node
		r_find_bundles(mNodePath.node(), anims, parts);
		PT(PartBundle)firstPartBundle;
		firstPartBundle.clear();
		//check if there is at least one PartBundle
		for (partsIter = parts.begin(); partsIter != parts.end(); ++partsIter)
		{
			for (partBundlesIter = partsIter->second.begin();
					partBundlesIter != partsIter->second.end();
					++partBundlesIter)
			{
				if (not firstPartBundle)
				{
					//set the first PartBundle
					firstPartBundle = *partBundlesIter;
					PRINT(
							"First PartBundle: '" << (*partBundlesIter)->get_name() << "'");
				}
				else
				{
					PRINT(
							"Next PartBundle: '" << (*partBundlesIter)->get_name() << "'");
				}
			}
		}
		//proceeds with animations only if there is at least one PartBundle
		if (firstPartBundle)
		{
			//check if there are AnimBundles within the model file
			//and bind them to the first PartBundle
			PT(AnimBundle)firstAnimBundle;
			std::string animName;
			for (animsIter = anims.begin(); animsIter != anims.end(); ++animsIter)
			{
				int j;
				for (j=0, animBundlesIter = animsIter->second.begin();
						animBundlesIter != animsIter->second.end();
						++animBundlesIter, ++j)
				{
					if (j>0)
					{
						animName = modelName + '.' + format_string(j);
					}
					else
					{
						animName = modelName;
					}
					PRINT("Binding animation '" << (*animBundlesIter)->get_name() << "' with name '"
							<< animName << "'");
					PT(AnimControl)control = firstPartBundle->bind_anim(*animBundlesIter,
							PartGroup::HMF_ok_wrong_root_name|PartGroup::HMF_ok_part_extra|PartGroup::HMF_ok_anim_extra);
					mAnimations.store_anim(control, animName);
				}
			}

			//setup more animations (if any)
			std::list<std::string>::iterator iter;
			std::list<std::string> animFileList = mTmpl->parameterList(
					std::string("anim_files"));
			for (iter = animFileList.begin(); iter != animFileList.end(); ++iter)
			{
				//any "anim_files" string is a "compound" one, i.e. could have the form:
				// "anim_file1:anim_file2:...:anim_fileN"
				std::vector<std::string> animFiles = parseCompoundString(*iter, ':');
				std::vector<std::string>::const_iterator iterAnimFile;
				for (iterAnimFile = animFiles.begin(); iterAnimFile != animFiles.end(); ++iterAnimFile)
				{
					//an empty anim file is ignored
					if (not iterAnimFile->empty())
					{
						parts.clear();
						anims.clear();
						//get the AnimBundle node path
						std::string baseAnimName = *iterAnimFile;
						NodePath animNP = mTmpl->windowFramework()->load_model(mNodePath,
								Filename(baseAnimName));
						if (animNP.is_empty())
						{
							result = false;
						}
						//find all the bundles into animNP.node
						r_find_bundles(animNP.node(), anims, parts);
						for (animsIter = anims.begin(); animsIter != anims.end(); ++animsIter)
						{
							int j;
							for (j=0, animBundlesIter = animsIter->second.begin(); animBundlesIter != animsIter->second.end();
									++animBundlesIter, ++j)
							{
								if (j>0)
								{
									animName = baseAnimName + '.' + format_string(j);
								}
								else
								{
									animName = baseAnimName;
								}
								PRINT("Binding animation '" << (*animBundlesIter)->get_name() << " with name '"
										<< animName << "'");
								PT(AnimControl)control = firstPartBundle->bind_anim(*animBundlesIter,
										PartGroup::HMF_ok_wrong_root_name|PartGroup::HMF_ok_part_extra|PartGroup::HMF_ok_anim_extra);
								mAnimations.store_anim(control, animName);
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
	//setup event callbacks if any
	setupEvents();
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
