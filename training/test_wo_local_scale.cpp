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
 * \file /Ely/training/manual_example.cpp
 *
 * \date 10/mar/2013 (15:39:11)
 * \author marco
 */

#include "pandaFramework.h"
#include "pandaSystem.h"

int test_wo_local_scale(int argc, char *argv[])
{
	//open a new window framework
	PandaFramework framework;
	framework.open_framework(argc, argv);
	//set the window title to My Panda3D Window
	framework.set_window_title("My Panda3D Window");
	//open the window
	WindowFramework *window = framework.open_window();
	if (window != (WindowFramework *) NULL)
	{
		std::cout << "Opened the window successfully!\n";
		// common setup
		window->enable_keyboard(); // Enable keyboard detection
		window->setup_trackball(); // Enable default camera movement
	}

	//here is room for your own code

	//create the actual terrain
	//terrain definition
	mTerrain = new GeoMipTerrain(name);
	//set height field
	if (not mTerrain->set_heightfield(mHeightField))
	{
		//heightField the image is set to an
		//empty (black) image with 512x512 dimensions.
		mTerrain->set_heightfield(PNMImage(512, 512));
	}
	//sizing
	float environmentWidthX = (mHeightField.get_x_size() - 1) * mWidthScale;
	float environmentWidthY = (mHeightField.get_y_size() - 1) * mWidthScale;
	float environmentWidth = (environmentWidthX + environmentWidthY) / 2.0;
	//set terrain properties effectively
	mTerrain->set_block_size(mBlockSize);
	mTerrain->set_near(mNearPercent * environmentWidth);
	mTerrain->set_far(mFarPercent * environmentWidth);
	//other properties
	float terrainLODmin = min<float>(mMinimumLevel, mTerrain->get_max_level());
	mTerrain->set_min_level(terrainLODmin);
	mTerrain->set_auto_flatten(mFlattenMode);
	mTerrain->set_bruteforce(mBruteForce);
	mTerrain->get_root().set_sx(mWidthScale);
	mTerrain->get_root().set_sy(mWidthScale);
	mTerrain->get_root().set_sz(mHeightScale);
	//terrain texturing
	mTerrain->get_root().set_tex_scale(TextureStage::get_default(),
			mTextureUscale, mTextureVscale);
	if (mTextureImage != NULL)
	{
		mTerrain->get_root().set_texture(TextureStage::get_default(),
				mTextureImage, 1);
	}

	//set the node path of the object to the
	//node path of this model
	mOwnerObject->setNodePath(mTerrain->get_root());
	//set the focal point
	SMARTPTR(Object)createdObject =
	ObjectTemplateManager::GetSingleton().getCreatedObject(
			mFocalPointObject);
	if (createdObject == NULL)
	{
		//set render as focal point
		createdObject = ObjectTemplateManager::GetSingleton().getCreatedObject(
				"render");
	}
	mFocalPointNP = createdObject->getNodePath();
	//Generate the terrain
	mTerrain->generate();
	//Add to the scene manager update if not brute force
	GameSceneManager::GetSingletonPtr()->addToSceneUpdate(this);

	//do the main loop, equal to run() in python
	framework.main_loop();
	//close the window framework
	framework.close_framework();
	return (0);
}

