/*
 * Example.cpp
 *
 *  Created on: Mar 25, 2011
 *      Author: marco
 */

#include "Example.h"

void Example::createScene()
{
	//	Ogre::ManualObject* manual = mSceneMgr->createManualObject("Quad");

	//	manual->begin("BaseWhiteNoLighting", RenderOperation::OT_TRIANGLE_LIST);
	//	manual->begin("MyMaterial1", RenderOperation::OT_TRIANGLE_LIST);
	//	manual->begin("MyMaterial3", RenderOperation::OT_TRIANGLE_LIST);
	//	manual->begin("MyMaterial4", RenderOperation::OT_TRIANGLE_LIST);
	//	manual->begin("MyMaterial5", RenderOperation::OT_TRIANGLE_LIST);
	//	manual->begin("MyMaterial6", RenderOperation::OT_TRIANGLE_LIST);
	//	manual->begin("MyMaterial7", RenderOperation::OT_TRIANGLE_LIST);
	//	manual->begin("MyMaterial8", RenderOperation::OT_TRIANGLE_LIST);
	//	manual->begin("MyMaterial11", RenderOperation::OT_TRIANGLE_LIST);
	//	manual->begin("MyMaterial13", RenderOperation::OT_TRIANGLE_LIST);
	//	manual->begin("MyMaterial14", RenderOperation::OT_TRIANGLE_LIST);
	//	manual->begin("MyMaterial15", RenderOperation::OT_TRIANGLE_LIST);
	//0
	//	manual->position(5.0, 0.0, 0.0);
	//	//	manual->textureCoord(0, 2);
	//	//	manual->textureCoord(2, 2);
	//	//	manual->textureCoord(1.0, 1.0);
	//	manual->colour(0, 0, 1);
	//	//1
	//	manual->position(-5.0, 10.0, 0.0);
	//	//	manual->textureCoord(2, 0);
	//	//	manual->textureCoord(0, 0);
	//	manual->colour(0, 1, 0);
	//	//2
	//	manual->position(-5.0, 0.0, 0.0);
	//	//	manual->textureCoord(2, 2);
	//	//	manual->textureCoord(0, 2);
	//	//	manual->textureCoord(0, 1.0);
	//	manual->colour(0, 1, 0);
	//	//3
	//	manual->position(5.0, 10.0, 0.0);
	//	//	manual->textureCoord(0, 0);
	//	//	manual->textureCoord(2, 0);
	//	//	manual->textureCoord(1.0, 0);
	//	manual->colour(0, 0, 1);
	//
	//	manual->index(0);
	//	manual->index(1);
	//	manual->index(2);
	//
	//	manual->index(0);
	//	manual->index(3);
	//	manual->index(1);
	//
	//	manual->end();
	//	manual->convertToMesh("Quad");

	//	Ogre::Entity * ent = mSceneMgr->createEntity("Quad");
	Ogre::Entity* ent = mSceneMgr->createEntity("Entity1", "Sinbad.mesh");
	//	ent->setMaterialName("MyMaterial14");
	ent->setMaterialName("MyMaterial17");
	Ogre::SceneNode* node =
			mSceneMgr->getRootSceneNode()->createChildSceneNode("Node1");
	node->attachObject(ent);

	//	ent = mSceneMgr->createEntity("Quad");
	//	ent->setMaterialName("MyMaterial12");
	//	node = mSceneMgr->getRootSceneNode()->createChildSceneNode("Node2",
	//			Ogre::Vector3(15, 0, 0));
	//	node->attachObject(ent);

	mCamera->setPosition(0.0, 5.0, 20.0);
}

