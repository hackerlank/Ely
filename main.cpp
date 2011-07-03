/*
 * main.cpp
 *
 *  Created on: Mar 25, 2011
 *      Author: marco
 */

#include "Example.h"

int main(void)
{
	//#if OGRE_DEBUG_MODE
	//	Ogre::Root* root = new Ogre::Root("plugins_d.cfg");
	//#else
	//	Ogre::Root* root = new Ogre::Root("plugins.cfg");
	//#endif
	//	if (!root->showConfigDialog())
	//	{
	//		return -1;
	//	}
	//	Ogre::RenderWindow* window = root->initialise(true,
	//			"Ogre3D Beginners Guide");
	//	Ogre::SceneManager* sceneManager = root->createSceneManager(
	//			Ogre::ST_GENERIC);
	//	Ogre::Camera* camera = sceneManager->createCamera("Camera");
	//	camera->setPosition(Ogre::Vector3(0, 0, 50));
	//	camera->lookAt(Ogre::Vector3(0, 0, 0));
	//	camera->setNearClipDistance(5);
	//	Ogre::Viewport* viewport = window->addViewport(camera);
	//	viewport->setBackgroundColour(Ogre::ColourValue(0.2, 0.2, 0.2));
	//	camera->setAspectRatio(Ogre::Real(viewport->getActualWidth()) / Ogre::Real(
	//			viewport->getActualHeight()));
	//	//	Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
	//	//			"/REPOSITORY/KProjects/usr/share/OGRE/media/packs/Sinbad.zip",
	//	//			"Zip");
	//	Ogre::ConfigFile cf;
	//#if OGRE_DEBUG_MODE
	//	cf.load("resources_d.cfg");
	//#else
	//	cf.load("resources.cfg");
	//#endif
	//	Ogre::ConfigFile::SectionIterator sectionIter = cf.getSectionIterator();
	//	Ogre::String sectionName, typeName, dataname;
	//	while (sectionIter.hasMoreElements())
	//	{
	//		sectionName = sectionIter.peekNextKey();
	//		Ogre::ConfigFile::SettingsMultiMap *settings = sectionIter.getNext();
	//		Ogre::ConfigFile::SettingsMultiMap::iterator i;
	//		for (i = settings->begin(); i != settings->end(); ++i)
	//		{
	//			typeName = i->first;
	//			dataname = i->second;
	//			Ogre::ResourceGroupManager::getSingleton(). addResourceLocation(
	//					dataname, typeName, sectionName);
	//		}
	//	}
	//
	//	Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
	//
	//	Ogre::Entity* ent = sceneManager->createEntity("Sinbad.mesh");
	//	sceneManager->getRootSceneNode()->attachObject(ent);
	//
	//	root->startRendering();

	MyApplication app;
	app.startup();

	while(app.keepRunning())
	{
	  app.renderOneFrame();
	}

	return 0;
}
