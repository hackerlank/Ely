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

	_listener = new MyFrameListener();
	_root->addFrameListener(_listener);

	Ogre::Camera* camera = _sceneManager->createCamera("Camera");
	camera->setPosition(Ogre::Vector3(0, 0, 50));
	camera->lookAt(Ogre::Vector3(0, 0, 0));
	camera->setNearClipDistance(5);

	Ogre::Viewport* viewport = window->addViewport(camera);
	viewport->setBackgroundColour(Ogre::ColourValue(0.0, 0.0, 0.0));
	camera->setAspectRatio(Ogre::Real(viewport->getActualWidth()) / Ogre::Real(
			viewport->getActualHeight()));

	loadResources();
	createScene();
	_root->startRendering();

	return 0;
}

void MyApplication::createScene()
{
	Ogre::Entity* ent = _sceneManager->createEntity("Sinbad.mesh");
	_sceneManager->getRootSceneNode()->attachObject(ent);
}

//------------MyFrameListener----------//

bool MyFrameListener::frameStarted(const Ogre::FrameEvent& evt)
{
	return false;
}

bool MyFrameListener::frameEnded(const Ogre::FrameEvent& evt)
{
	return false;
}

bool MyFrameListener::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
	return false;
}
