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
	//Reference Model
	Ogre::Entity* ent = mSceneMgr->createEntity("MyEntity", "Sinbad.mesh");
	Ogre::SceneNode* node = mSceneMgr->createSceneNode("Node1");
	mSceneMgr->getRootSceneNode()->addChild(node);
	node->attachObject(ent);

	//<Part 1 - Setting the position of a scene node
	//	node->setPosition(10, 0, 0);
	//
	//	Ogre::Entity* ent2 = mSceneMgr->createEntity("MyEntity2", "Sinbad.mesh");
	//	Ogre::SceneNode* node2 = mSceneMgr->createSceneNode("Node2");
	//	node->addChild(node2);
	//	node2->setPosition(0, 10, 20);
	//	node2->attachObject(ent2);
	// Part 1>

	//<Part 2 - Rotating a scene node
	//	node->setPosition(10, 10, 0);
	//	mSceneMgr->getRootSceneNode()->addChild(node);
	//
	//	Ogre::Entity* ent2 = mSceneMgr->createEntity("MyEntity2", "Sinbad.mesh");
	//	Ogre::SceneNode* node2 = mSceneMgr->createSceneNode("Node2");
	//	node->addChild(node2);
	//	node2->setPosition(10, 0, 0);
	//	node2->pitch(Ogre::Radian(Ogre::Math::HALF_PI));
	//	node2->attachObject(ent2);
	//
	//	Ogre::Entity* ent3 = mSceneMgr->createEntity("MyEntity3", "Sinbad.mesh");
	//	Ogre::SceneNode* node3 = mSceneMgr->createSceneNode("Node3");
	//	node->addChild(node3);
	//	node3->translate(20, 0, 0);
	//	node3->yaw(Ogre::Degree(90.0f));
	//	node3->attachObject(ent3);
	//
	//	Ogre::Entity* ent4 = mSceneMgr->createEntity("MyEntity4", "Sinbad.mesh");
	//	Ogre::SceneNode* node4 = mSceneMgr->createSceneNode("Node4");
	//	node->addChild(node4);
	//	node4->setPosition(30, 0, 0);
	//	node4->roll(Ogre::Radian(Ogre::Math::HALF_PI));
	//	node4->attachObject(ent4);
	// Part 2>

	//<Part 3 - Scaling a scene node
	//	node->setPosition(10, 10, 0);
	//
	//	Ogre::Entity* ent2 = mSceneMgr->createEntity("MyEntity2", "Sinbad.mesh");
	//	Ogre::SceneNode* node2 = node->createChildSceneNode("node2");
	//	node2->setPosition(10, 0, 0);
	//	node2->attachObject(ent2);
	//	node2->scale(2.0f, 2.0f, 2.0f);
	//
	//	Ogre::Entity* ent3 = mSceneMgr->createEntity("MyEntity3", "Sinbad.mesh");
	//	Ogre::SceneNode* node3 = node->createChildSceneNode("node3", Ogre::Vector3(
	//			20, 0, 0));
	//	node3->attachObject(ent3);
	//	node3->scale(0.2f, 0.2f, 0.2f);
	// Part 3>

	//<Part 4 - Using a scene graph the clever way
	//	//	node->setPosition(10,10,0);
	//
	//	Ogre::Entity* ent2 =
	//			mSceneMgr->createEntity("MyEntitysNinja", "ninja.mesh");
	//	Ogre::SceneNode* node2 = node->createChildSceneNode("node2");
	//	node2->setPosition(10, 0, 0);
	//	node2->setScale(0.02f, 0.02f, 0.02f);
	//	node2->attachObject(ent2);
	//
	//	node->setPosition(40, 10, 0);
	//	node->yaw(Ogre::Degree(180.0f));
	// Part 4>

	//<Part 5 - Different spaces in a scene
	//	node->setPosition(0, 0, 400);
	//	node->yaw(Ogre::Degree(180.0f));
	//
	//	Ogre::Entity* ent2 = mSceneMgr->createEntity("MyEntity2", "Sinbad.mesh");
	//	Ogre::SceneNode* node2 = node->createChildSceneNode("node2");
	//	node2->setPosition(10, 0, 0);
	//	node2->translate(0, 0, 10);
	//	node2->attachObject(ent2);
	//	Ogre::Entity* ent3 = mSceneMgr->createEntity("MyEntity3", "Sinbad.mesh");
	//	Ogre::SceneNode* node3 = node->createChildSceneNode("node3");
	//	node3->setPosition(20, 0, 0);
	//	//	node3->translate(0, 0, 10);
	//	node3->translate(0, 0, 10, Ogre::Node::TS_WORLD);
	//	node3->attachObject(ent3);
	// Part 5>

	//<Part 6 - Translating in local space
	//	node->setPosition(0, 0, 400);
	//	node->yaw(Ogre::Degree(180.0f));
	//
	//	Ogre::Entity* ent2 = mSceneMgr->createEntity("MyEntity2", "Sinbad.mesh");
	//	Ogre::SceneNode* node2 = node->createChildSceneNode("node2");
	//	node2->yaw(Ogre::Degree(45));
	//	node2->translate(0, 0, 20);
	//	node2->attachObject(ent2);
	//
	//	Ogre::Entity* ent3 = mSceneMgr->createEntity("MyEntity3", "Sinbad.mesh");
	//	Ogre::SceneNode* node3 = node->createChildSceneNode("node3");
	//	node3->yaw(Ogre::Degree(45));
	//	node3->translate(0, 0, 20, Ogre::Node::TS_LOCAL);
	//	node3->attachObject(ent3);
	// Part 6>

	//<Part 7 - Rotating in different spaces
	Ogre::Entity* ent2 = mSceneMgr->createEntity("MyEntity2", "sinbad.mesh");
	Ogre::SceneNode* node2 =
			mSceneMgr->getRootSceneNode()->createChildSceneNode("Node2");
	node2->setPosition(10, 0, 0);
	node2->yaw(Ogre::Degree(90));
	node2->roll(Ogre::Degree(90));
	node2->attachObject(ent2);

	Ogre::Entity* ent3 = mSceneMgr->createEntity("MyEntity3", "Sinbad.mesh");
	Ogre::SceneNode* node3 = node->createChildSceneNode("node3");
	node3->setPosition(20, 0, 0);
	node3->yaw(Ogre::Degree(90), Ogre::Node::TS_WORLD);
	node3->roll(Ogre::Degree(90), Ogre::Node::TS_WORLD);
	node3->attachObject(ent3);

	// Part 7>
}

