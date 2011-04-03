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
	/** Create ambient light */
	mSceneMgr->setAmbientLight(ColourValue(1, 1, 1));

	/** Create point light, set its type and position */
	Light *light = mSceneMgr->createLight("Light1");
	light->setType(Light::LT_POINT);
	light->setPosition(Vector3(0, 150, 250));

	/** set the diffuse and specular colour */
	light->setDiffuseColour(1.0, 1.0, 1.0);
	light->setSpecularColour(1.0, 1.0, 1.0);

	mCamera->setPosition(Vector3(0, 120, 800));

	mWorld = new OgreOde::World(mSceneMgr);
	mWorld->setGravity(Ogre::Vector3(0, -9.80665, 0));
	mWorld->setCFM(10e-5);
	mWorld->setERP(0.8);
	mWorld->setAutoSleep(true);
	mWorld->setAutoSleepAverageSamplesCount(10);
	mWorld->setContactCorrectionVelocity(1.0);
	mSpace = mWorld->getDefaultSpace();

	const Real _time_step = 0.5;
	const Real time_scale = Real(1.7);
	const Real max_frame_time = Real(1.0 / 4);
	mStepper = new OgreOde::StepHandler(mWorld,
			OgreOde::StepHandler::QuickStep, _time_step, max_frame_time,
			time_scale);

	mGround = new OgreOde::InfinitePlaneGeometry(Plane(Ogre::Vector3(0, 1, 0),
			0), mWorld, mWorld->getDefaultSpace());
	// Use a load of meshes to represent the floor
	int i = 0;
	StaticGeometry* s = mSceneMgr->createStaticGeometry("StaticFloor");
	s->setRegionDimensions(Vector3(1600.0, 100.0, 1600.0));
	// Set the region origin so the center is at 0 world
	s->setOrigin(Vector3::ZERO);
	for (Real z = -800.0; z <= 800.0; z += 20.0)
	{
		for (Real x = -800.0; x <= 800.0; x += 20.0)
		{
			String name = String("Ground") + StringConverter::toString(i++);
			Entity *entity = mSceneMgr->createEntity(name, "plane.mesh");
			entity->setQueryFlags(1 << 4);
			entity->setUserAny(Any(mGround));
			entity->setCastShadows(false);
			s->addEntity(entity, Vector3(x, 0, z));
		}
	}
	s->build();

	mEntity = mSceneMgr->createEntity("crate", "crate.mesh");
	mEntity->setQueryFlags(1 << 2);
	mNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("crate");
	mNode->attachObject(mEntity);
	mEntity->setCastShadows(true);

	mBody = new OgreOde::Body(mWorld);
	mNode->attachObject(mBody);

	Vector3 size(10.0, 10.0, 10.0);
	OgreOde::BoxMass mMass(0.5, size);

	mMass.setDensity(5.0, size);
	mGeom = (OgreOde::Geometry*) new OgreOde::BoxGeometry(size, mWorld,
			mWorld->getDefaultSpace());
	mNode->setScale(size.x/5.0, size.y/5.0, size.z/5.0);
	mBody->setMass(mMass);
	mGeom->setBody(mBody);
	mEntity->setUserAny(Any(mGeom));

	mBody->setOrientation(Quaternion(Radian(5.0), Vector3(0, 0, 0)));
	mBody->setPosition(Vector3(0, 120, -20));

}

void Example::createFrameListener(void)
{
	mFrameListener = new ExampleFL(mWindow, mCamera, mWorld,
			mStepper);
	mRoot->addFrameListener(mFrameListener);
}

