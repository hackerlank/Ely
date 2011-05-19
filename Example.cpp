/*
 * Example.cpp
 *
 *  Created on: Mar 25, 2011
 *      Author: marco
 */

#include "Example.h"

Example::Example()
{
	FrameListener = NULL;
	compListener = NULL;
}

Example::~Example()
{
	if (compListener)
	{
		delete compListener;
	}
	if (FrameListener)
	{
		delete FrameListener;
	}

}

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
	//	Ogre::CompositorManager::getSingleton().addCompositor(
	//			mCamera->getViewport(), "Compositor4");
	//	Ogre::CompositorManager::getSingleton().setCompositorEnabled(
	//			mCamera->getViewport(), "Compositor4", true);
	//	Ogre::CompositorManager::getSingleton().addCompositor(
	//			mCamera->getViewport(), "Compositor5");
	//	Ogre::CompositorManager::getSingleton().setCompositorEnabled(
	//			mCamera->getViewport(), "Compositor5", true);

	//	Ogre::CompositorManager::getSingleton().addCompositor(
	//			mCamera->getViewport(), "Compositor3");
	//	Ogre::CompositorManager::getSingleton(). setCompositorEnabled(
	//			mCamera->getViewport(), "Compositor3", true);
	//	Ogre::CompositorManager::getSingleton().addCompositor(
	//			mCamera->getViewport(), "Compositor2");
	//	Ogre::CompositorManager::getSingleton().setCompositorEnabled(
	//			mCamera->getViewport(), "Compositor2", true);

	//	Ogre::CompositorManager::getSingleton().addCompositor(
	//			mCamera->getViewport(), "Compositor7");
	//	Ogre::CompositorManager::getSingleton().setCompositorEnabled(
	//			mCamera->getViewport(), "Compositor7", true);
	Ogre::CompositorManager::getSingleton().addCompositor(
			mCamera->getViewport(), "Compositor8");
	Ogre::CompositorManager::getSingleton().setCompositorEnabled(
			mCamera->getViewport(), "Compositor8", true);
	Ogre::CompositorInstance* comp =
			Ogre::CompositorManager::getSingleton().getCompositorChain(
					mCamera->getViewport())->getCompositor("Compositor8");
	compListener = new CompositorListener1();
	comp->addListener(compListener);

}

void Example::createFrameListener()
{
	FrameListener = new Example8FrameListener(mWindow, compListener);
	mRoot->addFrameListener(FrameListener);
}

//-----------------------------CompositorListener1-----------------------------------//

CompositorListener1::CompositorListener1()
{
	number = 125.0f;
}

void CompositorListener1::notifyMaterialRender(uint32 pass_id, MaterialPtr &mat)
{
	mat->getBestTechnique()->getPass(pass_id)->getFragmentProgramParameters()->setNamedConstant(
			"numpixels", number);

}

//-----------------------------ExampleFrameListener-----------------------------------//

Example8FrameListener::Example8FrameListener(RenderWindow* win,
		CompositorListener1* listener)
{
	_listener = listener;
	size_t windowHnd = 0;
	std::stringstream windowHndStr;
	win->getCustomAttribute("WINDOW", &windowHnd);
	windowHndStr << windowHnd;
	OIS::ParamList pl;
	pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));
	_man = OIS::InputManager::createInputSystem(pl);
	_key = static_cast<OIS::Keyboard*> (_man->createInputObject(
			OIS::OISKeyboard, false));
}

Example8FrameListener::~Example8FrameListener()
{
	_man->destroyInputObject(_key);
	OIS::InputManager::destroyInputSystem(_man);
}

bool Example8FrameListener::frameStarted(const Ogre::FrameEvent &evt)
{
	_key->capture();
	if (_key->isKeyDown(OIS::KC_ESCAPE))
	{
		return false;
	}
	if (_key->isKeyDown(OIS::KC_UP))
	{
		float num = _listener->getNumber();
		num++;
		_listener->setNumber(num);
		std::cout << num << std::endl;
	}
	if (_key->isKeyDown(OIS::KC_DOWN))
	{
		float num = _listener->getNumber();
		num--;
		_listener->setNumber(num);
		std::cout << num << std::endl;
	}
	return true;
}
