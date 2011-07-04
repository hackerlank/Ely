/*
 * Example.cpp
 *
 *  Created on: Mar 25, 2011
 *      Author: marco
 */

#include "Example.h"

MyApplication::MyApplication()
{
	_listener = NULL;
	_sceneManager = NULL;
	_root = NULL;

}

MyApplication::~MyApplication()
{
	delete _listener;
	delete _root;
}

void MyApplication::loadResources()
{
	Ogre::ConfigFile cf;
#if OGRE_DEBUG_MODE
	cf.load("resources_d.cfg");
#else
	cf.load("resources.cfg");
#endif
	Ogre::ConfigFile::SectionIterator sectionIter = cf.getSectionIterator();
	Ogre::String sectionName, typeName, dataname;
	while (sectionIter.hasMoreElements())
	{
		sectionName = sectionIter.peekNextKey();
		Ogre::ConfigFile::SettingsMultiMap *settings = sectionIter.getNext();
		Ogre::ConfigFile::SettingsMultiMap::iterator i;
		for (i = settings->begin(); i != settings->end(); ++i)
		{
			typeName = i->first;
			dataname = i->second;
			Ogre::ResourceGroupManager::getSingleton(). addResourceLocation(
					dataname, typeName, sectionName);
		}
	}
	Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
}

int MyApplication::startup()
{
#if OGRE_DEBUG_MODE
	_root = new Ogre::Root("plugins_d.cfg");
#else
	_root = new Ogre::Root("plugins.cfg");
#endif
	if (!_root->showConfigDialog())
	{
		return -1;
	}
	Ogre::RenderWindow* window = _root->initialise(true,
			"Ogre3D Beginners Guide");
	_sceneManager = _root->createSceneManager(Ogre::ST_GENERIC);

	Ogre::Camera* camera = _sceneManager->createCamera("Camera");
	camera->setPosition(Ogre::Vector3(0, 0, 50));
	camera->lookAt(Ogre::Vector3(0, 0, 0));
	camera->setNearClipDistance(5);

	Ogre::Viewport* viewport = window->addViewport(camera);
	viewport->setBackgroundColour(Ogre::ColourValue(0.0, 0.0, 0.0));
	camera->setAspectRatio(Ogre::Real(viewport->getActualWidth()) / Ogre::Real(
			viewport->getActualHeight()));

	_listener = new MyFrameListener(window, camera);
	_root->addFrameListener(_listener);

	loadResources();
	createScene();
	//	_root->startRendering();

	return 0;
}

void MyApplication::renderOneFrame()
{
	Ogre::WindowEventUtilities::messagePump();
	_keepRunning = _root->renderOneFrame();
}

void MyApplication::createScene()
{
	Ogre::Entity* ent = _sceneManager->createEntity("Sinbad.mesh");
	_sceneManager->getRootSceneNode()->attachObject(ent);
}

//------------MyFrameListener----------//

MyFrameListener::MyFrameListener(Ogre::RenderWindow *win, Ogre::Camera* cam)
{
	OIS::ParamList parameters;
	unsigned long int windowHandle = 0;
	std::ostringstream windowHandleString;
	win->getCustomAttribute("WINDOW", &windowHandle);
	windowHandleString << windowHandle;
	parameters.insert(std::make_pair("WINDOW", windowHandleString.str()));
	_InputManager = OIS::InputManager::createInputSystem(parameters);
	_Keyboard = static_cast<OIS::Keyboard*> (_InputManager->createInputObject(
			OIS::OISKeyboard, false));
	_Cam = cam;
	_movementspeed = 50.0f;
	_Mouse = static_cast<OIS::Mouse*> (_InputManager->createInputObject(
			OIS::OISMouse, false));

}

MyFrameListener::~MyFrameListener()
{
	_InputManager->destroyInputObject(_Keyboard);
	OIS::InputManager::destroyInputSystem(_InputManager);
	_InputManager->destroyInputObject(_Mouse);
}

bool MyFrameListener::frameStarted(const Ogre::FrameEvent& evt)
{
	_Keyboard->capture();
	if (_Keyboard->isKeyDown(OIS::KC_ESCAPE))
	{
		return false;
	}
	Ogre::Vector3 translate(0, 0, 0);
	if (_Keyboard->isKeyDown(OIS::KC_W))
	{
		translate += Ogre::Vector3(0, 0, -1);
	}
	if (_Keyboard->isKeyDown(OIS::KC_S))
	{
		translate += Ogre::Vector3(0, 0, 1);
	}
	if (_Keyboard->isKeyDown(OIS::KC_A))
	{
		translate += Ogre::Vector3(-1, 0, 0);
	}
	if (_Keyboard->isKeyDown(OIS::KC_D))
	{
		translate += Ogre::Vector3(1, 0, 0);
	}
	_Cam->moveRelative(translate * evt.timeSinceLastFrame * _movementspeed);
	_Mouse->capture();
	float rotX = _Mouse->getMouseState().X.rel * evt. timeSinceLastFrame * -1;
	float rotY = _Mouse->getMouseState().Y.rel * evt. timeSinceLastFrame * -1;
	_Cam->yaw(Ogre::Radian(rotX));
	_Cam->pitch(Ogre::Radian(rotY));

	return true;
}

bool MyFrameListener::frameEnded(const Ogre::FrameEvent& evt)
{
	return true;
}

bool MyFrameListener::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
	return true;
}
