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
#include <sheetNode.h>
#include <geomNode.h>
#include <texturePool.h>

namespace ely
{

Model::Model()
{
	// TODO Auto-generated constructor stub
}

Model::Model(SMARTPTR(ModelTemplate)tmpl)
{
	CHECK_EXISTENCE_DEBUG(GameSceneManager::GetSingletonPtr(),
			"Model::Model: invalid GameSceneManager")

	mTmpl = tmpl;
	reset();
}

Model::~Model()
{
}

ComponentFamilyType Model::familyType() const
{
	return mTmpl->familyType();
}

ComponentType Model::componentType() const
{
	return mTmpl->componentType();
}

void Model::do_r_find_bundles(SMARTPTR(PandaNode)node, Anims& anims, Parts& parts)
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
		do_r_find_bundles(cr.get_child(i), anims, parts);
	}
}

bool Model::initialize()
{
	bool result = true;
	//check if model is from file
	mFromFile = (
			mTmpl->parameter(std::string("from_file")) == std::string("false") ?
					false : true);
	//model
	mModelNameParam = mTmpl->parameter(std::string("model_file"));
	//more animations
	mAnimFileListParam = mTmpl->parameterList(std::string("anim_files"));
	//if model not from file then get which type
	mModelTypeParam = mTmpl->parameter(std::string("model_type"));
	//
	std::string param;
	unsigned int idx, valueNum;
	std::vector<std::string> paramValuesStr;
	float value;
	int valueInt;
	//scale
	param = mTmpl->parameter(std::string("scale"));
	paramValuesStr = parseCompoundString(param, ',');
	valueNum = paramValuesStr.size();
	if ((valueNum > 0) and (valueNum < 3))
	{
		paramValuesStr.resize(3, paramValuesStr[0]);
	}
	else if (valueNum < 3)
	{
		paramValuesStr.resize(3, "1.0");
	}
	for (idx = 0; idx < 3; ++idx)
	{
		value = strtof(paramValuesStr[idx].c_str(), NULL);
		mScale[idx] = (value >= 0.0 ? value : -value);
	}
	//card points
	param = mTmpl->parameter(std::string("card_points"));
	paramValuesStr = parseCompoundString(param, ',');
	valueNum = paramValuesStr.size();
	if (valueNum < 4)
	{
		paramValuesStr.resize(4, "0.0");
	}
	for (idx = 0; idx < 4; ++idx)
	{
		mCardPoints.push_back(strtof(paramValuesStr[idx].c_str(), NULL));
	}
	//rope render mode
	param = mTmpl->parameter(std::string("rope_render_mode"));
	if (param == std::string("thread"))
	{
		mRopeRenderMode = RopeNode::RM_thread;
	}
	else if (param == std::string("tape"))
	{
		mRopeRenderMode = RopeNode::RM_tape;
	}
	else if (param == std::string("billboard"))
	{
		mRopeRenderMode = RopeNode::RM_billboard;
	}
	else
	{
		mRopeRenderMode = RopeNode::RM_tube;
	}
	//rope uv mode
	param = mTmpl->parameter(std::string("rope_uv_mode"));
	if (param == std::string("parametric"))
	{
		mRopeUVMode = RopeNode::UV_parametric;
	}
	else if (param == std::string("distance"))
	{
		mRopeUVMode = RopeNode::UV_distance;
	}
	else if (param == std::string("distance2"))
	{
		mRopeUVMode = RopeNode::UV_distance2;
	}
	else
	{
		mRopeUVMode = RopeNode::UV_none;
	}
	//rope normal mode
	param = mTmpl->parameter(std::string("rope_normal_mode"));
	if (param == std::string("vertex"))
	{
		mRopeNormalMode = RopeNode::NM_vertex;
	}
	else
	{
		mRopeNormalMode = RopeNode::NM_none;
	}
	//rope num subdiv
	valueInt = strtol(mTmpl->parameter(std::string("rope_num_subdiv")).c_str(),
	NULL, 0);
	mRopeNumSubdiv = (valueInt >= 0.0 ? valueInt : -valueInt);
	//rope num slices
	valueInt = strtol(mTmpl->parameter(std::string("rope_num_slices")).c_str(),
	NULL, 0);
	mRopeNumSlices = (valueInt >= 0.0 ? valueInt : -valueInt);
	//rope thickness
	value = strtof(mTmpl->parameter(std::string("rope_thickness")).c_str(),
	NULL);
	mRopeThickness = (value >= 0.0 ? value : -value);
	//sheet num u subdiv
	valueInt = strtol(
			mTmpl->parameter(std::string("sheet_num_u_subdiv")).c_str(), NULL,
			0);
	mSheetNumUSubdiv = (valueInt >= 0.0 ? valueInt : -valueInt);
	//sheet num v subdiv
	valueInt = strtol(
			mTmpl->parameter(std::string("sheet_num_v_subdiv")).c_str(), NULL,
			0);
	mSheetNumVSubdiv = (valueInt >= 0.0 ? valueInt : -valueInt);
	//texture
	mTextureImage = TexturePool::load_texture(
			Filename(mTmpl->parameter(std::string("texture_file"))));
	//texture uscale
	value = strtof(mTmpl->parameter(std::string("texture_uscale")).c_str(),
			NULL);
	mTextureUscale = (value >= 0.0 ? value : -value);
	//texture vscale
	value = strtof(mTmpl->parameter(std::string("texture_vscale")).c_str(),
			NULL);
	mTextureVscale = (value >= 0.0 ? value : -value);
	//
	return result;
}

void Model::onAddToObjectSetup()
{
	//build model
	if (mFromFile)
	{
		PRINT_DEBUG(
				"'" <<getOwnerObject()->objectId() << "'::'" << mComponentId << "'::onAddToObjectSetup");
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
		//mModelNameParam can have this form: [anim_name1@anim_name2@
		// ...@anim_nameN@]model_filename ([] means optional)
		std::vector<std::string> animsFileNames = parseCompoundString(
				mModelNameParam, '@');
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
		do_r_find_bundles(mNodePath.node(), anims, parts);
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
					PRINT_DEBUG(
							"\tFirst PartBundle: '" << (*partBundlesIter)->get_name() << "'");
				}
				else
				{
					PRINT_DEBUG(
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
					PRINT_DEBUG(
							"\tBinding animation '" << (*animBundlesIter)->get_name() << "' (from '" << modelFileName << "') with name '" << animName << "'");
					SMARTPTR(AnimControl)control = (mFirstPartBundle->bind_anim(*animBundlesIter,
									PartGroup::HMF_ok_wrong_root_name|PartGroup::HMF_ok_part_extra|PartGroup::HMF_ok_anim_extra)).p();
					mAnimations.store_anim(control, animName);
				}
			}

			//setup more animations (if any)
			std::list<std::string>::iterator iter;
			for (iter = mAnimFileListParam.begin();
					iter != mAnimFileListParam.end(); ++iter)
			{
				//any "anim_files" string is a "compound" one, i.e. could have the form:
				// "anim_name1@anim_file1:anim_name2@anim_file2:...:anim_nameN@anim_fileN"
				std::vector<std::string> nameFilePairs = parseCompoundString(
						*iter, ':');
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
							NodePath animNP =
									mTmpl->windowFramework()->load_model(
											mNodePath,
											Filename(nameFilePair[1]));
							if (animNP.is_empty())
							{
								animNP = NodePath();
							}
							//find all the bundles into animNP.node
							do_r_find_bundles(animNP.node(), anims, parts);
							for (animsIter = anims.begin();
									animsIter != anims.end(); ++animsIter)
							{
								int j;
								for (j = 0, animBundlesIter =
										animsIter->second.begin();
										animBundlesIter
												!= animsIter->second.end();
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
									PRINT_DEBUG(
											"\tBinding animation '" << (*animBundlesIter)->get_name() << "' (from '" << nameFilePair[1] << "') with name '" << animName << "'");
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
		if (mModelTypeParam == std::string("card"))
		{
			//card (e.g. finite plane)
			if ((mCardPoints[1] - mCardPoints[0])
					* (mCardPoints[3] - mCardPoints[2]) == 0.0)
			{
				mCardPoints[0] = -1.0;
				mCardPoints[1] = 1.0;
				mCardPoints[2] = -1.0;
				mCardPoints[3] = 1.0;
			}
			//Component standard name: ObjectId_ObjectType_ComponentId_ComponentType
			CardMaker card(COMPONENT_STANDARD_NAME+ "_card");
			card.set_frame(mCardPoints[0], mCardPoints[1], mCardPoints[2],
					mCardPoints[3]);
			mNodePath = NodePath(card.generate());
		}
		else if (mModelTypeParam == std::string("rope_node"))
		{
			///rope_node: this is a generic RopeNode to which
			///a NurbsCurveEvaluator could be associated.
			//Component standard name: ObjectId_ObjectType_ComponentId_ComponentType
			SMARTPTR(RopeNode)ropeNode = new RopeNode(COMPONENT_STANDARD_NAME + "_rope_node");
			ropeNode->set_render_mode(mRopeRenderMode);
			ropeNode->set_uv_mode(mRopeUVMode);
			ropeNode->set_normal_mode(mRopeNormalMode);
			ropeNode->set_num_subdiv(mRopeNumSubdiv);
			ropeNode->set_num_slices(mRopeNumSlices);
			ropeNode->set_thickness(mRopeThickness);
			mNodePath = NodePath(ropeNode);
		}
		else if (mModelTypeParam == std::string("sheet_node"))
		{
			///sheet_node: this is a generic SheetNode to which
			///a NurbsSurfaceEvaluator could be associated.
			//Component standard name: ObjectId_ObjectType_ComponentId_ComponentType
			SMARTPTR(SheetNode)sheetNode = new SheetNode(COMPONENT_STANDARD_NAME + "_sheet_node");
			sheetNode->set_num_u_subdiv(mSheetNumUSubdiv);
			sheetNode->set_num_v_subdiv(mSheetNumVSubdiv);
			mNodePath = NodePath(sheetNode);
		}
		else if (mModelTypeParam == std::string("geom_node"))
		{
			///geom_node: this is a generic GeomNode to which
			///one or more Geoms could be added.
			//Component standard name: ObjectId_ObjectType_ComponentId_ComponentType
			SMARTPTR(GeomNode) geomNode = new GeomNode(COMPONENT_STANDARD_NAME + "_geom_node");
			mNodePath = NodePath(geomNode);
		}
		else
		{
			///On error loads our favorite blue triangle.
			mNodePath = mTmpl->windowFramework()->load_default_model(
					mTmpl->pandaFramework()->get_models());
		}
		//texturing
		SMARTPTR(TextureStage) textureStage0 =
		new TextureStage(COMPONENT_STANDARD_NAME + "_TextureStage0");
		mNodePath.set_tex_scale(textureStage0, mTextureUscale, mTextureVscale);
		if (mTextureImage != NULL)
		{
			mNodePath.set_texture(textureStage0, mTextureImage, 1);
		}
	}

	//set scaling
	mNodePath.set_scale(mScale);

	//Rename the node
	//Component standard name: ObjectId_ObjectType_ComponentId_ComponentType
	std::string name = COMPONENT_STANDARD_NAME;
	mNodePath.set_name(name);

	//set the object node path to this model of node path
	mOldObjectNodePath = mOwnerObject->getNodePath();
	mOwnerObject->setNodePath(mNodePath);
}

void Model::onRemoveFromObjectCleanup()
{
	//set the object node path to the old one
	mOwnerObject->setNodePath(mOldObjectNodePath);

	//Remove node path
	mNodePath.remove_node();
	//
	reset();
}

SMARTPTR(PartBundle)Model::getPartBundle() const
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return mFirstPartBundle;
}

//TypedObject semantics: hardcoded
TypeHandle Model::_type_handle;

} // namespace ely
