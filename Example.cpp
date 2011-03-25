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
	Ogre::Entity* ent = mSceneMgr->createEntity("MyEntity", "Sinbad.mesh");
	mSceneMgr->getRootSceneNode()->attachObject(ent);
}

