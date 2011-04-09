/*
 * Example.cpp
 *
 *  Created on: Mar 25, 2011
 *      Author: marco
 */

#include "Example.h"

Example::Example()
{
	// TODO Auto-generated constructor stub

}

Example::~Example()
{
	// TODO Auto-generated destructor stub
}

void Example::createScene()
{
	Ogre::Plane plane(Vector3::UNIT_Y, -10);
	Ogre::MeshManager::getSingleton().createPlane("plane",
			ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 1500,
			1500, 20, 20, true, 1, 5, 5, Vector3::UNIT_Z);
	Ogre::Entity* ent = mSceneMgr->createEntity("LightPlaneEntity", "plane");
	mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(ent);
	ent->setMaterialName("Examples/BeachStones");

	Ogre::SceneNode* node = mSceneMgr->createSceneNode("Node1");
	mSceneMgr->getRootSceneNode()->addChild(node);

	Ogre::Entity* Sinbad = mSceneMgr->createEntity("Sinbad", "Sinbad.mesh");
	Ogre::SceneNode* SinbadNode = node->createChildSceneNode("SinbadNode");
	SinbadNode->setScale(3.0f, 3.0f, 3.0f);
	SinbadNode->setPosition(Ogre::Vector3(0.0f, 4.8f, 0.0f));
	SinbadNode->attachObject(Sinbad);

	mSceneMgr->setAmbientLight(Ogre::ColourValue(0.6, 0.6, 0.6));
	mSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);

	//<Part 1 - Adding a point light
	//	Ogre::Light* light1 = mSceneMgr->createLight("Light1");
	//	light1->setType(Ogre::Light::LT_POINT);
	//	light1->setPosition(0, 100, 0);
	//	light1->setDiffuseColour(1.0f, 1.0f, 1.0f);
	// Part 1>

	//<Part 2 - Adding a spotlight
	//	Ogre::SceneNode* node2 = node->createChildSceneNode("node2");
	//	node2->setPosition(0, 100, 0);
	//	Ogre::Light* light = mSceneMgr->createLight("Light1");
	//	light->setType(Ogre::Light::LT_SPOTLIGHT);
	//	light->setDirection(Ogre::Vector3(1, -1, 0));
	//	light->setSpotlightInnerAngle(Ogre::Degree(5.0f));
	//	light->setSpotlightOuterAngle(Ogre::Degree(45.0f));
	//	light->setSpotlightFalloff(0.0f);
	//	light->setDiffuseColour(Ogre::ColourValue(0.0f, 1.0f, 0.0f));
	//	node2->attachObject(light);
	// Part 2>

	//< Part 1 & Part 2
	//	Ogre::Entity* LightEnt = mSceneMgr->createEntity("MyEntity", "sphere.mesh");
	//	Ogre::SceneNode* node3 = node->createChildSceneNode("node3");
	//	node3->setScale(0.02f, 0.02f, 0.02f);
	//	node3->setPosition(0, 100, 0);
	//	node3->attachObject(LightEnt);
	// Part 1 & Part2>

	//<Part 3 - Directional lights
	Ogre::Light* light = mSceneMgr->createLight("Light1");
	light->setType(Ogre::Light::LT_DIRECTIONAL);
	light->setDiffuseColour(Ogre::ColourValue(1.0f, 1.0f, 1.0f));
	light->setDirection(Ogre::Vector3(1, -1, 0));
	// Part 3>

}

void Example::createCamera()
{
	mCamera = mSceneMgr->createCamera("MyCamera1");
	mCamera->setPosition(0, 100, 200);
	mCamera->lookAt(0, 0, 0);
	mCamera->setNearClipDistance(5);
	//	mCamera->setPolygonMode(Ogre::PM_WIREFRAME);
}

void Example::createViewports(void)
{
	Ogre::Viewport* vp = mWindow->addViewport(mCamera);
	vp->setBackgroundColour(ColourValue(0.0f, 0.0f, 1.0f));
	mCamera->setAspectRatio(Real(vp->getActualWidth()) / Real(
			vp->getActualHeight()));
}

