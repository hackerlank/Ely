/*
 * Example.cpp
 *
 *  Created on: Mar 25, 2011
 *      Author: marco
 */

#include "Example.h"

using namespace Ogre;

#if !(OGRE_VERSION_MINOR < 4)
//using namespace OIS;
#endif //OGRE_VERSION not heihort
using namespace OgreOde;
using namespace OgreOde_Prefab;
using namespace OgreOde_Loader;

const Ogre::Real SimpleScenes::KEY_DELAY = 0.5;
const Ogre::Real SimpleScenes::STEP_RATE = 0.01;

/*
 Constructor; initailise stuff
 */
SimpleScenes::SimpleScenes(OgreOde::World *world)
{
	_world = world;
	_mgr = _world->getSceneManager();
	_world->setCollisionListener(this);
	_space = _world->getDefaultSpace();
	_key_delay = SimpleScenes::KEY_DELAY;
	_last_node = 0;

	dotOgreOdeLoader = new OgreOde_Loader::DotLoader(world);

	_ragdollFactory = new OgreOde_Prefab::RagdollFactory();
	Ogre::Root::getSingletonPtr()->addMovableObjectFactory(_ragdollFactory);
	setInfoText(StringUtil::BLANK);
}

/*
 Called by OgreOde whenever a collision occurs, so
 that we can modify the contact parameters
 */
bool SimpleScenes::collision(OgreOde::Contact* contact)
{
	// Check for collisions between things that are connected and ignore them
	OgreOde::Geometry * const g1 = contact->getFirstGeometry();
	OgreOde::Geometry * const g2 = contact->getSecondGeometry();

	if (g1 && g2)
	{
		const OgreOde::Body * const b1 = g1->getBody();
		const OgreOde::Body * const b2 = g2->getBody();
		if (b1 && b2 && OgreOde::Joint::areConnected(b1, b2))
			return false;
	}

	// Set the friction at the contact
	contact->setCoulombFriction(OgreOde::Utility::Infinity);

	contact->setBouncyness(0.1);

	//contact->setCoulombFriction(5.0);
	//contact->setFrictionMode(OgreOde::Contact::Flag_SurfaceIsBouncy);

	//contact->setCoulombFriction(OgreOde::Utility::Infinity);

	// Yes, this collision is valid
	return true;
}

/*
 Handle key presses
 */
#if (OGRE_VERSION_MINOR < 4)
void SimpleScenes::frameEnded(Real time, InputReader* input)
#else
void SimpleScenes::frameEnded(Real time, OIS::Keyboard* input,
		OIS::Mouse* mouse)
#endif //OGRE_VERSION not heihort
{
	_key_delay += time;

	std::for_each(RagdollList.begin(), RagdollList.end(), std::mem_fun(
			&OgreOde_Prefab::Ragdoll::update));

}

/*
 Utility method to set the information string in the UI
 */
void SimpleScenes::setInfoText(const String& text)
{
	OverlayManager::getSingleton().getOverlayElement("OgreOdeDemos/Info")->setCaption(
			String("Info: ") + text);
}

static const String xmlNames[] =
{ "zombie", "zombie", "ninja", "robot" };
static const String meshNames[] =
{ "zombie_small.mesh", "zombie_small.mesh", "ninja.mesh", "robot.mesh" };
static const String meshAnimation[] =
{ "Walk1", "Walk1", "Walk", "Walk" };
static const Ogre::Vector3 meshScale[] =
{ Ogre::Vector3(1.0, 1.0, 1.0), Ogre::Vector3(1.0, 1.0, 1.0), Ogre::Vector3(
		1.0, 1.0, 1.0), Ogre::Vector3(1.0, 1.0, 1.0) };
static const String ragdollFile[] =
{ "zombie.ogreode", "zombie_auto.ogreode", "ninja.ogreode", "robot.ogreode" };
static int sSelectedMesh = 0;
static int _ragdoll_count = 0;

/*
 Create a ragdoll
 */
void SimpleScenes::createRagDoll()
{
	if (_key_delay < SimpleScenes::KEY_DELAY)
		return;

	_key_delay = 0.0;

	OgreOde_Prefab::Ragdoll
			*_ragdoll =
					static_cast<OgreOde_Prefab::Ragdoll *> (dotOgreOdeLoader->loadObject(
							ragdollFile[sSelectedMesh],
							xmlNames[sSelectedMesh], "zombie"
									+ StringConverter::toString(
											_ragdoll_count++)));

	_ragdoll->setCastShadows(true);

	SceneNode *_ragdoll_node = _mgr->getRootSceneNode()->createChildSceneNode(
			_ragdoll->getName() + "Node");
	_ragdoll_node->attachObject(_ragdoll);

	//_ragdoll_node->yaw(Degree(rand() % 360));
	//_ragdoll_node->pitch(Degree(rand() % 360));


	_ragdoll_node->setOrientation(Quaternion(Radian(
			OgreOde::Utility::randomReal() * 10.0 - 5.0), Ogre::Vector3(
			OgreOde::Utility::randomReal() * 2.0 - 1.0,
			OgreOde::Utility::randomReal() * 2.0 - 1.0,
			OgreOde::Utility::randomReal() * 2.0 - 1.0)));

	_ragdoll_node->setPosition(Vector3((OgreOde::Utility::randomReal() * 10.0)
			- 5.0, OgreOde::Utility::randomReal() + 5,
			(OgreOde::Utility::randomReal() * 10.0) - 5.0));

	_ragdoll_node->setScale(meshScale[sSelectedMesh]);

	_ragdoll->getAnimationState(meshAnimation[sSelectedMesh])->setEnabled(false);

	// Create the ragdoll
	_ragdoll->takePhysicalControl(_world, _space, false);
	_ragdoll->setSelfCollisions(true);

	RagdollList.push_back(_ragdoll);

}
/*
 Create a randomly sized box, sphere or capsule for dropping on things
 */
OgreOde::Body* SimpleScenes::createRandomObject(
		OgreOde::Geometry::Class objectClass)
{
	if (_key_delay < SimpleScenes::KEY_DELAY)
		return 0;

	String
			typeName =
					(objectClass == OgreOde::Geometry::Class_Box) ? "crate"
							: (objectClass == OgreOde::Geometry::Class_Sphere) ? "ball"
									: (objectClass
											== OgreOde::Geometry::Class_Capsule) ? "capsule"
											: (objectClass
													== OgreOde::Geometry::Class_TriangleMesh) ? "gun"
													: "unknown";

	// Create the visual representation (the Ogre entity and scene node)
	String name = typeName + StringConverter::toString(
			(unsigned int) _bodies.size());
	Entity* entity = _mgr->createEntity(name, typeName + ".mesh");
	entity->setQueryFlags(GEOMETRY_QUERY_MASK);

	SceneNode* node = _mgr->getRootSceneNode()->createChildSceneNode(name);
	node->attachObject(entity);
	//entity->setNormaliseNormals(true);
	entity->setCastShadows(true);

	// Pick a size
	Vector3 size((OgreOde::Utility::randomReal() * 0.5 + 0.1) * 2.0,
			(OgreOde::Utility::randomReal() * 0.5 + 0.1) * 2.0,
			(OgreOde::Utility::randomReal() * 0.5 + 0.1) * 2.0);

	// Create a body associated with the node we created
	OgreOde::Body* body = new OgreOde::Body(_world);
	node->attachObject(body);

	// Set the mass and geometry to match the visual representation
	OgreOde::Geometry* geom = 0;
	switch (objectClass)
	{
	case OgreOde::Geometry::Class_Box:
	{
		size *= 1.75;

		OgreOde::BoxMass mass(1.0, size);
		mass.setDensity(5.0, size);

		geom = (OgreOde::Geometry*) new OgreOde::BoxGeometry(size, _world,
				_space);
		node->setScale(size.x * 0.1, size.y * 0.1, size.z * 0.1);
		body->setMass(mass);
	}
		break;

	case OgreOde::Geometry::Class_Sphere:
	{
		OgreOde::SphereMass mass(1.0, size.x);
		mass.setDensity(5.0, size.x);

		geom = (OgreOde::Geometry*) new OgreOde::SphereGeometry(size.x, _world,
				_space);
		node->setScale(size.x * 0.2, size.x * 0.2, size.x * 0.2);
		body->setMass(mass);
	}
		break;

	case OgreOde::Geometry::Class_Capsule:
	{
		size.x *= 0.5;

		OgreOde::CapsuleMass mass(1.0, size.x, Vector3::UNIT_Z, size.y);
		mass.setDensity(5.0, size.x, Vector3::UNIT_Z, size.y);

		geom = (OgreOde::Geometry*) new OgreOde::CapsuleGeometry(size.x,
				size.y, _world, _space);
		node->setScale(size.x, size.x, (size.y + (size.x * 2.0)) * 0.25);
		body->setMass(mass);
	}
		break;

	case OgreOde::Geometry::Class_TriangleMesh:
	{
		OgreOde::EntityInformer ei(entity, node->_getFullTransform());
		geom = ei.createStaticTriangleMesh(_world, _space);

		OgreOde::BoxMass mass(1.0, ei.getSize());
		mass.setDensity(5.0, ei.getSize());

		//node->setScale(size.x,size.x,(size.y + (size.x * 2.0)) * 0.25);
		body->setMass(mass);
	}
		break;
	}

	// Tie the collision geometry to the physical body
	geom->setBody(body);
	entity->setUserAny(Ogre::Any(geom));

	// Keep track of the body
	_bodies.push_back(body);
	_geoms.push_back(geom);

	_key_delay = 0.0;

	// If we created something position and orient it randomly
	if (body)
	{
		body->setOrientation(Quaternion(Radian(OgreOde::Utility::randomReal()
				* 10.0 - 5.0), Ogre::Vector3(OgreOde::Utility::randomReal()
				* 2.0 - 1.0, OgreOde::Utility::randomReal() * 2.0 - 1.0,
				OgreOde::Utility::randomReal() * 2.0 - 1.0)));
		body->setPosition(Vector3(
				(OgreOde::Utility::randomReal() * 10.0) - 5.0,
				OgreOde::Utility::randomReal() + 5,
				(OgreOde::Utility::randomReal() * 10.0) - 5.0));

		// Set the last body we created to be looked at
		_last_node = static_cast<SceneNode*> (body->getParentNode());
	}

	return body;
}

/*
 Destructor, delete everything we're keeping track of
 */
SimpleScenes::~SimpleScenes(void)
{
	// Stop listening for collisions
	if (_world->getCollisionListener() == this)
		_world->setCollisionListener(0);

	// Delete all the joints
	for (std::vector<OgreOde::Joint*>::iterator i = _joints.begin(); i
			!= _joints.end(); ++i)
	{
		delete (*i);
	}

	// Run through the list of bodies we're monitoring
	for (std::vector<OgreOde::Body*>::iterator i = _bodies.begin(); i
			!= _bodies.end(); ++i)
	{
		// Get the node this body controls
		SceneNode* node = static_cast<SceneNode*> ((*i)->getParentNode());
		if (node)
		{
			// Get its name and remember all the things attached to it
			String name = node->getName();
			int num = node->numAttachedObjects();
			for (int cur = 0; cur < num; cur++)
			{
				MovableObject* obj = node->getAttachedObject(cur);
				if (obj->getMovableType() != OgreOde::Body::MovableType)
					clearList.push_back(obj);
			}

			// Destroy the node by name
			_mgr->getRootSceneNode()->removeAndDestroyChild(name);
		}

		// Delete the body
		delete (*i);
	}

	// Remove all the entities we found attached to scene nodes we're controlling
	for (std::vector<MovableObject*>::iterator i = clearList.begin(); i
			!= clearList.end(); ++i)
	{
		if ((*i)->getMovableType() == "Entity")
		{
			MovableObject* m = static_cast<MovableObject*> (*i);
			//_mgr->destroyMovableObject(m->getName(), m->);
			_mgr->destroyMovableObject(m);
		}
		else if ((*i)->getMovableType() == "ParticleSystem")
		{
			_mgr->destroyParticleSystem(static_cast<ParticleSystem*> (*i));
		}
	}

	// Delete all the collision geometries
	for (std::vector<OgreOde::Geometry*>::iterator i = _geoms.begin(); i
			!= _geoms.end(); ++i)
	{
		delete (*i);
	}
	// Remove all the entities we found attached to scene nodes we're controlling
	for (std::vector<OgreOde_Prefab::Ragdoll *>::iterator i =
			RagdollList.begin(); i != RagdollList.end(); ++i)
	{

		assert ((*i)->getParentNode ());
		assert ((*i)->getParentNode ()->getParent());
		(static_cast<SceneNode*> ((*i)->getParentNode()->getParent()))->removeAndDestroyChild(
				(*i)->getParentNode()->getName());

		_mgr->destroyMovableObject((*i)->getName(),
				OgreOde_Prefab::RagdollFactory::FACTORY_TYPE_NAME);
		//_mgr->destroyMovableObject(m);
	}
	Ogre::Root::getSingletonPtr()->removeMovableObjectFactory(_ragdollFactory);
	delete _ragdollFactory;
}

/*
 The frame listener is informed before every frame
 */
bool SimpleScenesFrameListener::frameStarted(const FrameEvent& evt)
{

	// Do the default demo input handling and keep our UI display
	// in sync with the other stuff (frame rate, logo, etc)
	bool show = mStatsOn;
	bool bOK = ExampleFrameListener::frameStarted(evt);
	if (mStatsOn != show)
	{
		Overlay* pOver = (Overlay*) OverlayManager::getSingleton().getByName(
				"OgreOdeDemos/Overlay");
		if (pOver)
		{
			if (mStatsOn)
				pOver->show();
			else
				pOver->hide();
		}
	}
	// Tell the demo application that it needs to handle input
#if (OGRE_VERSION_MINOR < 4)
	_demo->frameStarted(evt,mInputDevice);
#else
	_demo->frameStarted(evt, mKeyboard, mMouse);
#endif //OGRE_VERSION not heihort
	// Quit or carry on according to the normal demo input
	return bOK;
}

/*
 The frame listener is informed after every frame
 */
bool SimpleScenesFrameListener::frameEnded(const FrameEvent& evt)
{
	// Tell our demo that the frame's ended before doing default processing

#if (OGRE_VERSION_MINOR < 4)
	_demo->frameEnded(evt,mInputDevice);
#else
	_demo->frameEnded(evt, mKeyboard, mMouse);
#endif //OGRE_VERSION not heihort
	return ExampleFrameListener::frameEnded(evt);
}

/*
 Create the scene from an Ogre point of view
 and create the common OgreOde things we'll need
 */
void SimpleScenesApplication::createScene(void)
{
	MovableObject::setDefaultQueryFlags(ANY_QUERY_MASK);

#ifndef _DEBUG
	// Set up shadowing
	mSceneMgr->setShadowTechnique(SHADOWTYPE_TEXTURE_MODULATIVE);
	mSceneMgr->setShadowColour(ColourValue(0.5, 0.5, 0.5));
	mSceneMgr->setShadowFarDistance(30);

	if (StringUtil::startsWith(mRoot->getRenderSystem()->getName(), "direct"))
		mSceneMgr->setShadowTextureSettings(1024, 2);
	else
		mSceneMgr->setShadowTextureSettings(512, 2);
#endif

	// Add some default lighting to the scene
	mSceneMgr->setAmbientLight(ColourValue(0.25, 0.25, 0.25));

	// Create a directional light to shadow and light the bodies
	_spot = mSceneMgr->createLight("Spot");
	_spot->setType(Light::LT_DIRECTIONAL);
	_spot->setDirection(-0.40824828, -0.81649655, -0.40824828);
	_spot->setCastShadows(true);
	_spot->setSpecularColour(1, 1, 0.8);

	// Give us some sky
	mSceneMgr->setSkyBox(true, "kk3d/DesertVII", 5000, true);

	// Position and orient the camera
	mCamera->setPosition(13, 4.5, 0);
	mCamera->lookAt(0, 0.5, 0);
	mCamera->setNearClipDistance(0.5);

	// Create the ODE world
	_world = new OgreOde::World(mSceneMgr);

	_world->setGravity(Vector3(0, -9.80665, 0));
	_world->setCFM(10e-5);
	_world->setERP(0.8);
	_world->setAutoSleep(true);
	_world->setAutoSleepAverageSamplesCount(10);
	_world->setContactCorrectionVelocity(1.0);

	// Create something that will step the world, but don't do it automatically

	const int _stepper_mode_choice = 2;
	const int _stepper_choice = 0;
	const Ogre::Real time_scale = Ogre::Real(1.0);
	const Ogre::Real max_frame_time = Ogre::Real(1.0 / 4);
	const Ogre::Real frame_rate = Ogre::Real(1.0 / 60);

	StepHandler::StepModeType stepModeType;

	switch (_stepper_mode_choice)
	{
	case 0:
		stepModeType = StepHandler::BasicStep;
		break;
	case 1:
		stepModeType = StepHandler::FastStep;
		break;
	case 2:
		stepModeType = StepHandler::QuickStep;
		break;

	default:
		stepModeType = StepHandler::QuickStep;
		break;
	}

	switch (_stepper_choice)
	{
	case 0:
		_stepper = new OgreOde::StepHandler(_world, StepHandler::QuickStep,
				_time_step, max_frame_time, time_scale);

		break;
	case 1:
		_stepper = new OgreOde::ExactVariableStepHandler(_world, stepModeType,
				_time_step, max_frame_time, time_scale);

		break;
	case 2:
		_stepper = new OgreOde::ForwardFixedStepHandler(_world, stepModeType,
				_time_step, max_frame_time, time_scale);

		break;
	case 3:
	default:
		_stepper = new OgreOde::ForwardFixedInterpolatedStepHandler(_world,
				stepModeType, _time_step, frame_rate, max_frame_time,
				time_scale);
		break;
	}

	//_stepper->setAutomatic(OgreOde::StepHandler::AutoMode_PostFrame, mRoot);
	//_stepper->setAutomatic(OgreOde::Stepper::AutoMode_PreFrame, mRoot);

	Root::getSingleton().setFrameSmoothingPeriod(5.0f);
	//Root::getSingleton().setFrameSmoothingPeriod(0.0f);

	// Create a default plane to act as the ground
	_plane = new OgreOde::InfinitePlaneGeometry(Plane(Vector3(0, 1, 0), 0),
			_world, _world->getDefaultSpace());

	// Use a load of meshes to represent the floor
	int i = 0;
	StaticGeometry* s;
	s = mSceneMgr->createStaticGeometry("StaticFloor");
	s->setRegionDimensions(Vector3(160.0, 100.0, 160.0));
	// Set the region origin so the center is at 0 world
	s->setOrigin(Vector3::ZERO);
	for (Real z = -80.0; z <= 80.0; z += 20.0)
	{
		for (Real x = -80.0; x <= 80.0; x += 20.0)
		{
			String name = String("Plane_") + StringConverter::toString(i++);

			Entity* entity = mSceneMgr->createEntity(name, "plane.mesh");
			entity->setQueryFlags(STATIC_GEOMETRY_QUERY_MASK);
			entity->setUserAny(Ogre::Any(_plane));
			entity->setCastShadows(false);
			s->addEntity(entity, Vector3(x, 0, z));
		}
	}
	s->build();
	//SceneNode* mPlaneNode = mSceneMgr->getRootSceneNode()->createChildSceneNode(name);

	// Load up our UI and display it
	Overlay* pOver = (Overlay*) OverlayManager::getSingleton().getByName(
			"OgreOdeDemos/Overlay");
	pOver->show();

	// Initialise stuff
	_test = 0;
	_delay = 1.0;
}

/*
 The frame listener will notify us when a frame's
 about to be started, so we can update stuff
 */

#if (OGRE_VERSION_MINOR < 4)
void SimpleScenesApplication::frameStarted(const FrameEvent& evt,InputReader* mInputDevice)
#else
void SimpleScenesApplication::frameStarted(const FrameEvent& evt,
		OIS::Keyboard* mKeyboard, OIS::Mouse* mMouse)
#endif //OGRE_VERSION not heihort
{
	// If we're looking at something then adjust the camera
	if (_test && _test->getLastNode())
	{
		if (_looking)
		{
			mCamera->lookAt(_test->getLastNode()->getPosition());

			if (_chasing)
			{
				// Thanks to Ahmed!
				const Ogre::Real followFactor = 0.1;
				const Ogre::Real camHeight = 5.0;
				const Ogre::Real camDistance = 7.0;

				Quaternion q = _test->getLastNode()->getOrientation();
				Vector3 toCam = _test->getLastNode()->getPosition();

				toCam.y += camHeight;
				toCam.z -= camDistance * q.zAxis().z;
				toCam.x -= camDistance * q.zAxis().x;

				mCamera->move((toCam - mCamera->getPosition()) * followFactor);
			}
		}
	}

	// Set the shadow distance according to how far we are from the plane that receives them
	mSceneMgr->setShadowFarDistance((fabs(mCamera->getPosition().y) + 1.0)
			* 3.0);

	// If we're running a test, tell it that a frame's ended
	if ((_test) && (!_paused))
	{
#if (OGRE_VERSION_MINOR < 4)
		_test->frameStarted(evt.timeSinceLastFrame, mInputDevice);
#else
		_test->frameStarted(evt.timeSinceLastFrame, mKeyboard, mMouse);
#endif //OGRE_VERSION not heihort
	}
}

/*
 The frame listener will let us know when a frame's ended. So we
 can do stuff that we can't do in a frame started event
 e.g. delete things that we can't delete at the start of a frame,
 presumably because some processing has already been done, leaving
 things dangling, like particle systems.
 */
#if (OGRE_VERSION_MINOR < 4)
void SimpleScenesApplication::frameEnded(const FrameEvent& evt,InputReader* myInput)
#else
void SimpleScenesApplication::frameEnded(const FrameEvent& evt,
		OIS::Keyboard* myInput, OIS::Mouse* mMouse)
#endif //OGRE_VERSION not heihort
{
	Real time = evt.timeSinceLastFrame;

	// If we're running a test, tell it that a frame's ended
	// If we're running a test, tell it that a frame's ended
	if ((_test) && (!_paused))
	{
#if (OGRE_VERSION_MINOR < 4)
		_test->frameEnded(time, myInput);
#else
		_test->frameEnded(time, myInput, mMouse);
#endif //OGRE_VERSION not heihort
	}

	// Step the world and then synchronise the scene nodes with it,
	// we could get this to do this automatically, but we
	// can't be sure of what order the frame listeners will fire in
	if (_stepper->step(time))
	{
		_world->synchronise();
	}

	_delay += time;
	if (_delay > 1.0)
	{
		bool changed = false;

		// Switch the test we're displaying
		if (myInput->isKeyDown(OIS::KC_F1))
		{
			delete _test;
			_test = new SimpleScenes_BoxStack(_world);
			changed = true;
		}
		else if (myInput->isKeyDown(OIS::KC_F2))
		{
			delete _test;
			_test = new SimpleScenes_Chain(_world);
			changed = true;
		}
		else if (myInput->isKeyDown(OIS::KC_F3))
		{
			delete _test;
			_test = new SimpleScenes_Buggy(_world);
			changed = true;
		}
		else if (myInput->isKeyDown(OIS::KC_F4))
		{
			delete _test;
			_test = new SimpleScenes_TriMesh(_world);
			changed = true;
		}
		else if (myInput->isKeyDown(OIS::KC_F5))
		{
			delete _test;
			_test = new SimpleScenes_Crash(_world);
			changed = true;
		}
		else if (myInput->isKeyDown(OIS::KC_F6))
		{
			delete _test;
			_test = new SimpleScenes_Joints(_world);

			if (mCamera->getPosition().z < 10.0)
			{
				Vector3 pos = mCamera->getPosition();
				mCamera->setPosition(pos.x, pos.y, 10.0);
				mCamera->lookAt(0, 0, 0);
			}
			changed = true;
		}
		else if (myInput->isKeyDown(OIS::KC_F7))
		{
			delete _test;
			_test = new SimpleScenes_Zombie(_world);

			changed = true;
		}

		// If we changed the test...
		if ((_test) && (changed))
		{
			// Register it with the stepper, so we can (for example) add forces before each step
			_stepper->setStepListener(_test);

			// Set the UI to show the test's details
			OverlayManager::getSingleton().getOverlayElement(
					"OgreOdeDemos/Name")->setCaption("Name: "
					+ _test->getName());
			OverlayManager::getSingleton().getOverlayElement(
					"OgreOdeDemos/Keys")->setCaption("Keys: "
					+ _test->getKeys());

			_delay = 0.0;
		}

		// Switch shadows
		if (myInput->isKeyDown(OIS::KC_SPACE))
		{
			static Ogre::uint shadowtype = 0;
			shadowtype += 1;
			if (shadowtype > 5)
				shadowtype = 0;
			switch (shadowtype)
			{
			case 0:
				mSceneMgr->setShadowTechnique(SHADOWTYPE_NONE);
				break;
			case 1:
				mSceneMgr->setShadowTechnique(SHADOWTYPE_TEXTURE_MODULATIVE);
				mSceneMgr->setShadowColour(ColourValue(0.5, 0.5, 0.5));
				mSceneMgr->setShadowFarDistance(30);
				if (StringUtil::startsWith(mRoot->getRenderSystem()->getName(),
						"direct"))
					mSceneMgr->setShadowTextureSettings(1024, 2);
				else
					mSceneMgr->setShadowTextureSettings(512, 2);
				break;
			case 2:
				mSceneMgr->setShadowTechnique(SHADOWTYPE_STENCIL_ADDITIVE);
				break;
			case 3:
				mSceneMgr->setShadowTechnique(SHADOWTYPE_STENCIL_MODULATIVE);
				break;
			case 4:
				mSceneMgr->setShadowTechnique(SHADOWTYPE_TEXTURE_ADDITIVE);
				mSceneMgr->setShadowColour(ColourValue(0.5, 0.5, 0.5));
				mSceneMgr->setShadowFarDistance(30);
				if (StringUtil::startsWith(mRoot->getRenderSystem()->getName(),
						"direct"))
					mSceneMgr->setShadowTextureSettings(1024, 2);
				else
					mSceneMgr->setShadowTextureSettings(512, 2);
				break;
			default:
				mSceneMgr->setShadowTechnique(SHADOWTYPE_NONE);
				break;
			}
			_delay = 0.0;
		}

		// Look at the last object, chase it, or not
		if (myInput->isKeyDown(OIS::KC_M))
		{
			if (_looking)
			{
				if (_chasing)
				{
					_looking = _chasing = false;
				}
				else
				{
					_chasing = true;
				}
			}
			else
			{
				_looking = true;
			}
			_delay = 0.0;
		}

		// Switch debugging objects on or off
		if (myInput->isKeyDown(OIS::KC_E))
		{
			_world->setShowDebugGeometries(!_world->getShowDebugGeometries());
			_delay = 0.0;
		}

		// Switch debugging Contacts on or off
		if (myInput->isKeyDown(OIS::KC_B))
		{
			_world->setShowDebugContact(!_world->getShowDebugContact());
			_delay = 0.5;
		}

		// Pause or Run the simulation
		if (myInput->isKeyDown(OIS::KC_P))
		{
			_paused = !_paused;
			_delay = 0.0;

			_stepper->pause(_paused);

			const Ogre::Real timeSet = (_paused) ? 0.0 : 1.0;

			Ogre::SceneManager::MovableObjectIterator it =
					mSceneMgr->getMovableObjectIterator(
							ParticleSystemFactory::FACTORY_TYPE_NAME);
			while (it.hasMoreElements())
			{
				ParticleSystem* p = static_cast<ParticleSystem*> (it.getNext());
				p->setSpeedFactor(timeSet);
			}
		}
	}
}

/*
 Destructor, delete stuff we created
 */
SimpleScenesApplication::~SimpleScenesApplication(void)
{
	delete _test;
	delete _plane;
	delete _stepper;
	delete _world;
}
