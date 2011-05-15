/*
 * Example.cpp
 *
 *  Created on: Mar 25, 2011
 *      Author: marco
 */

#include "Example.h"

void Example::createScene()
{
	Ogre::SceneNode* node =
			mSceneMgr->getRootSceneNode()->createChildSceneNode("Node1",
					Ogre::Vector3(0, 0, 450));
	Ogre::Entity* ent = mSceneMgr->createEntity("Entity1", "Sinbad.mesh");
	node->attachObject(ent);

	//	Ogre::CompositorManager::getSingleton().addCompositor(
	//			mCamera->getViewport(), "Compositor1");
	//	Ogre::CompositorManager::getSingleton().setCompositorEnabled(
	//			mCamera->getViewport(), "Compositor1", true);
	//	Ogre::CompositorManager::getSingleton().addCompositor(
	//			mCamera->getViewport(), "Compositor2");
	//	Ogre::CompositorManager::getSingleton().setCompositorEnabled(
	//			mCamera->getViewport(), "Compositor2", true);
	//	Ogre::CompositorManager::getSingleton().addCompositor(
	//			mCamera->getViewport(), "Compositor3");
	//	Ogre::CompositorManager::getSingleton().setCompositorEnabled(
	//			mCamera->getViewport(), "Compositor3", true);
	Ogre::CompositorManager::getSingleton().addCompositor(
			mCamera->getViewport(), "Compositor4");
	Ogre::CompositorManager::getSingleton().setCompositorEnabled(
			mCamera->getViewport(), "Compositor4", true);

}

