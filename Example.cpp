/*
 * Example.cpp
 *
 *  Created on: Mar 25, 2011
 *      Author: marco
 */

#include "Example.h"

Example::Example()
{
	//	FrameListener = NULL;
	//	compListener = NULL;
	compListener = NULL;
	compListener2 = NULL;
	compListener3 = NULL;

}

Example::~Example()
{
	//	if (compListener)
	//	{
	//		delete compListener;
	//	}
	//	if (FrameListener)
	//	{
	//		delete FrameListener;
	//	}
	if (compListener)
	{
		delete compListener;
	}
	if (compListener2)
	{
		delete compListener2;
	}
	if (compListener3)
	{
		delete compListener3;
	}

}

void Example::createScene()
{
	//	Ogre::SceneNode* node =
	//			mSceneMgr->getRootSceneNode()->createChildSceneNode("Node1",
	//					Ogre::Vector3(0, 0, 450));
	//	Ogre::Entity* ent = mSceneMgr->createEntity("Entity1", "Sinbad.mesh");
	//	node->attachObject(ent);

	Ogre::SceneNode* node =
			mSceneMgr->getRootSceneNode()->createChildSceneNode();
	Ogre::Entity* ent = mSceneMgr->createEntity("Sinbad.mesh");
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
	//	Ogre::CompositorManager::getSingleton().addCompositor(
	//			mCamera->getViewport(), "Compositor8");
	//	Ogre::CompositorManager::getSingleton().setCompositorEnabled(
	//			mCamera->getViewport(), "Compositor8", true);
	//	Ogre::CompositorInstance* comp =
	//			Ogre::CompositorManager::getSingleton().getCompositorChain(
	//					mCamera->getViewport())->getCompositor("Compositor8");
	//	compListener = new CompositorListener1();
	//	comp->addListener(compListener);
	Ogre::CompositorManager::getSingleton().addCompositor(vp, "Compositor9");
	Ogre::CompositorManager::getSingleton().setCompositorEnabled(vp,
			"Compositor9", true);
	Ogre::CompositorInstance
			* comp =
					Ogre::CompositorManager::getSingleton().getCompositorChain(
							vp)->getCompositor("Compositor9");
	compListener = new CompositorListener2();
	comp->addListener(compListener);
	Ogre::CompositorManager::getSingleton().addCompositor(vp2, "Compositor9");
	Ogre::CompositorManager::getSingleton().setCompositorEnabled(vp2,
			"Compositor9", true);
	Ogre::CompositorInstance
			* comp2 =
					Ogre::CompositorManager::getSingleton().getCompositorChain(
							vp2)->getCompositor("Compositor9");
	compListener2 = new CompositorListener3();
	comp2->addListener(compListener2);
	Ogre::CompositorManager::getSingleton().addCompositor(vp3, "Compositor9");
	Ogre::CompositorManager::getSingleton().setCompositorEnabled(vp3,
			"Compositor9", true);
	Ogre::CompositorInstance
			* comp3 =
					Ogre::CompositorManager::getSingleton().getCompositorChain(
							vp3)->getCompositor("Compositor9");
	compListener3 = new CompositorListener4();
	comp3->addListener(compListener3);

}

void Example::createCamera()
{
	//	mCamera = mSceneMgr->createCamera("MyCamera1");
	//	mCamera->setPosition(0, 10, 20);
	//	mCamera->lookAt(0, 0, 0);
	//	mCamera->setNearClipDistance(5);
	//
	//	mCamera2 = mSceneMgr->createCamera("MyCamera2");
	//	mCamera2->setPosition(20, 10, 0);
	//	mCamera2->lookAt(0, 0, 0);
	//	mCamera2->setNearClipDistance(5);

	mCamera = mSceneMgr->createCamera("MyCamera1");
	mCamera->setPosition(0, 10, 20);
	mCamera->lookAt(0, 0, 0);
	mCamera->setNearClipDistance(5);

}

void Example::createViewports()
{
	//	Ogre::Viewport* vp = mWindow->addViewport(mCamera, 0, 0.0, 0.0, 0.5, 1.0);
	//	vp->setBackgroundColour(ColourValue(0.0f, 0.0f, 0.0f));
	//	Ogre::Viewport* vp2 = mWindow->addViewport(mCamera2, 1, 0.5, 0.0, 0.5, 1.0);
	//	vp2->setBackgroundColour(ColourValue(0.0f, 0.0f, 0.0f));
	//	mCamera->setAspectRatio(Real(vp->getActualWidth()) / Real(
	//			vp->getActualHeight()));
	//	mCamera2->setAspectRatio(Real(vp2->getActualWidth()) / Real(
	//			vp2->getActualHeight()));

	vp = mWindow->addViewport(mCamera, 0, 0.0, 0.0, 0.5, 0.5);
	vp->setBackgroundColour(ColourValue(0.0f, 0.0f, 0.0f));
	vp2 = mWindow->addViewport(mCamera, 1, 0.5, 0.0, 0.5, 0.5);
	vp2->setBackgroundColour(ColourValue(0.0f, 0.0f, 0.0f));
	vp3 = mWindow->addViewport(mCamera, 2, 0.0, 0.5, 0.5, 0.5);
	vp3->setBackgroundColour(ColourValue(0.0f, 0.0f, 0.0f));
	vp4 = mWindow->addViewport(mCamera, 3, 0.5, 0.5, 0.5, 0.5);
	vp4->setBackgroundColour(ColourValue(0.0f, 0.0f, 0.0f));
	mCamera->setAspectRatio(Real(vp->getActualWidth()) / Real(
			vp->getActualHeight()));

}

//void Example::createFrameListener()
//{
//	FrameListener = new Example8FrameListener(mWindow, compListener);
//	mRoot->addFrameListener(FrameListener);
//}

//-----------------------------CompositorListener1-----------------------------------//

//CompositorListener1::CompositorListener1()
//{
//	number = 125.0f;
//}
//
////void CompositorListener1::notifyMaterialSetup(uint32 pass_id, MaterialPtr &mat)
//void CompositorListener1::notifyMaterialRender(uint32 pass_id, MaterialPtr &mat)
//{
//	mat->getBestTechnique()->getPass(pass_id)->getFragmentProgramParameters()->setNamedConstant(
//			"numpixels", number);
//
//}

//-----------------------------ExampleFrameListener-----------------------------------//

//Example8FrameListener::Example8FrameListener(RenderWindow* win,
//		CompositorListener1* listener)
//{
//	_listener = listener;
//	size_t windowHnd = 0;
//	std::stringstream windowHndStr;
//	win->getCustomAttribute("WINDOW", &windowHnd);
//	windowHndStr << windowHnd;
//	OIS::ParamList pl;
//	pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));
//	_man = OIS::InputManager::createInputSystem(pl);
//	_key = static_cast<OIS::Keyboard*> (_man->createInputObject(
//			OIS::OISKeyboard, false));
//}
//
//Example8FrameListener::~Example8FrameListener()
//{
//	_man->destroyInputObject(_key);
//	OIS::InputManager::destroyInputSystem(_man);
//}
//
//bool Example8FrameListener::frameStarted(const Ogre::FrameEvent &evt)
//{
//	_key->capture();
//	if (_key->isKeyDown(OIS::KC_ESCAPE))
//	{
//		return false;
//	}
//	if (_key->isKeyDown(OIS::KC_UP))
//	{
//		float num = _listener->getNumber();
//		num++;
//		_listener->setNumber(num);
//		std::cout << num << std::endl;
//	}
//	if (_key->isKeyDown(OIS::KC_DOWN))
//	{
//		float num = _listener->getNumber();
//		num--;
//		_listener->setNumber(num);
//		std::cout << num << std::endl;
//	}
//	return true;
//}

//-----------------------------CompositorListener2/3/4-----------------------------------//

void CompositorListener2::notifyMaterialSetup(uint32 pass_id, MaterialPtr &mat)
{
	mat->getBestTechnique()->getPass(pass_id)->getFragmentProgramParameters()->setNamedConstant(
			"factors", Ogre::Vector3(1, 0, 0));

}

void CompositorListener3::notifyMaterialSetup(uint32 pass_id, MaterialPtr &mat)
{
	mat->getBestTechnique()->getPass(pass_id)->getFragmentProgramParameters()->setNamedConstant(
			"factors", Ogre::Vector3(0, 1, 0));
}

void CompositorListener4::notifyMaterialSetup(uint32 pass_id, MaterialPtr &mat)
{
	mat->getBestTechnique()->getPass(pass_id)->getFragmentProgramParameters()->setNamedConstant(
			"factors", Ogre::Vector3(0, 0, 1));
}
