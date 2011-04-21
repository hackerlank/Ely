/*
 * Example.cpp
 *
 *  Created on: Mar 25, 2011
 *      Author: marco
 */

#include "Example.h"

void Example::createScene()
{
	// <Part 1
	//	std::cout << mSceneMgr->getTypeName() << "::" << mSceneMgr->getName()
	//			<< std::endl;
	// Part 1>

	Ogre::Plane plane(Vector3::UNIT_Y, -10);
	Ogre::MeshManager::getSingleton().createPlane("plane",
			ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 1500,
			1500, 200, 200, true, 1, 5, 5, Vector3::UNIT_Z);

	Ogre::Entity* ent = mSceneMgr->createEntity("GrassPlane", "plane");
	mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(ent);
	ent->setMaterialName("Examples/GrassFloor");

	Ogre::Light* light = mSceneMgr->createLight("Light1");
	light->setType(Ogre::Light::LT_DIRECTIONAL);
	light->setDirection(Ogre::Vector3(1, -1, 0));

	Ogre::ManualObject* manual = mSceneMgr->createManualObject("grass");

	manual->begin("Examples/GrassBlades", RenderOperation::OT_TRIANGLE_LIST);
	//	manual->begin("Logo", RenderOperation::OT_TRIANGLE_LIST);
	//1 Quad
	manual->position(5.0, 0.0, 0.0);
	manual->textureCoord(1, 1);
	manual->position(-5.0, 10.0, 0.0);
	manual->textureCoord(0, 0);
	manual->position(-5.0, 0.0, 0.0);
	manual->textureCoord(0, 1);
	//
	//	manual->position(5.0, 0.0, 0.0);
	//	manual->textureCoord(1, 1);
	manual->position(5.0, 10.0, 0.0);
	manual->textureCoord(1, 0);
	//	manual->position(-5.0, 10.0, 0.0);
	//	manual->textureCoord(0, 0);
	//2 Quad
	manual->position(2.5, 0.0, 4.3);
	manual->textureCoord(1, 1);
	manual->position(-2.5, 10.0, -4.3);
	manual->textureCoord(0, 0);
	manual->position(-2.0, 0.0, -4.3);
	manual->textureCoord(0, 1);
	//
	//	manual->position(2.5, 0.0, 4.3);
	//	manual->textureCoord(1, 1);
	manual->position(2.5, 10.0, 4.3);
	manual->textureCoord(1, 0);
	//	manual->position(-2.5, 10.0, -4.3);
	//	manual->textureCoord(0, 0);
	//3 Quad
	manual->position(2.5, 0.0, -4.3);
	manual->textureCoord(1, 1);
	manual->position(-2.5, 10.0, 4.3);
	manual->textureCoord(0, 0);
	manual->position(-2.0, 0.0, 4.3);
	manual->textureCoord(0, 1);
	//
	//	manual->position(2.5, 0.0, -4.3);
	//	manual->textureCoord(1, 1);
	manual->position(2.5, 10.0, -4.3);
	manual->textureCoord(1, 0);
	//	manual->position(-2.5, 10.0, 4.3);
	//	manual->textureCoord(0, 0);

	//1 Quad
	manual->index(0);
	manual->index(1);
	manual->index(2);
	manual->index(0);
	manual->index(3);
	manual->index(1);
	//2 Quad
	manual->index(4);
	manual->index(5);
	manual->index(6);
	manual->index(4);
	manual->index(7);
	manual->index(5);
	//3 Quad
	manual->index(8);
	manual->index(9);
	manual->index(10);
	manual->index(8);
	manual->index(11);
	manual->index(9);

	manual->end();
	manual->convertToMesh("BladesOfGrass");

	//	Ogre::SceneNode* grassNode =
	//			mSceneMgr->getRootSceneNode()->createChildSceneNode("GrassNode2");
	//	grassNode->attachObject(manual);

	Ogre::StaticGeometry* field = mSceneMgr->createStaticGeometry(
			"FieldOfGrass");

	for (int i = 0; i < 50; i++)
	{
		for (int j = 0; j < 50; j++)
		{
			Ogre::Entity * ent = mSceneMgr->createEntity("BladesOfGrass");
			//			Ogre::SceneNode* node =
			//					mSceneMgr->getRootSceneNode()->createChildSceneNode(
			//							Ogre::Vector3(i * 3, -10, j * 3));
			//			node->attachObject(ent);
			field->addEntity(ent, Ogre::Vector3(i * 3, -10, j * 3));

			//			std::cout << node->getName() << "::" << ent->getName() << std::endl;
		}
	}
	field->build();
}

// <Part 1
//void Example::chooseSceneManager(void)
//{
//	ResourceGroupManager::getSingleton().addResourceLocation(
//			"/REPOSITORY/KProjects/usr/share/OGRE/media/packs/chiropteraDM.pk3",
//			"Zip",
//			ResourceGroupManager::getSingleton().getWorldResourceGroupName(),
//			true);
//	ResourceGroupManager::getSingleton().initialiseResourceGroup(
//			ResourceGroupManager::getSingleton().getWorldResourceGroupName());
//	mSceneMgr = mRoot->createSceneManager("BspSceneManager");
//	mSceneMgr->setWorldGeometry("maps/chiropteradm.bsp");
//
//}
// Part 1>
