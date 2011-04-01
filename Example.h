/*
 * Example.h
 *
 *  Created on: Mar 25, 2011
 *      Author: marco
 */

#ifndef EXAMPLE_H_
#define EXAMPLE_H_

// Include the OgreOde interface which includes Ogre itself
// We'll need the OgreOde definitions
#include "OgreOde_Core.h"
#include "OgreOde_Prefab.h"
#include "OgreOde_Loader.h"
#include "ExampleApplication.h"

#if !(OGRE_VERSION_MINOR < 4)
//using namespace OIS;
#endif //OGRE_VERSION not heihort
#if !(OGRE_VERSION <  ((1 << 16) | (3 << 8) | 0))
#include "OIS/OIS.h"
namespace OIS
{
class Keyboard;
class Mouse;
}
;
#endif //OGRE_VERSION not heihort
enum QueryFlags
{
	ANY_QUERY_MASK = 1 << 0,
	ZOMBIE_QUERY_MASK = 1 << 1,
	GEOMETRY_QUERY_MASK = 1 << 2,
	VEHICLE_QUERY_MASK = 1 << 3,
	STATIC_GEOMETRY_QUERY_MASK = 1 << 4
};
/*
 The base Test class, is also able to listen for collisions and thus change the contact properties
 */
class SimpleScenes: public OgreOde::CollisionListener,
		public OgreOde::StepListener
{
public:
	static const Ogre::Real KEY_DELAY;
	static const Ogre::Real STEP_RATE;

	// Constructor/destructor
	SimpleScenes(OgreOde::World *world);
	virtual ~SimpleScenes();

	// Useful methods that will be handy for all tests
	void createRagDoll();
	OgreOde::Body* createRandomObject(OgreOde::Geometry::Class objectClass);
	void updateScene();
	const Ogre::SceneNode* getLastNode()
	{
		return _last_node;
	}
	void setInfoText(const Ogre::String& text);

	// The things we'll have to override in derived classes
	// Actual tests must provide a name but everything else can default
	virtual bool collision(OgreOde::Contact* contact);
	virtual const Ogre::String& getKeys()
	{
		return Ogre::StringUtil::BLANK;
	}
	virtual const Ogre::String& getName() = 0;
	virtual void addForcesAndTorques()
	{
	}

#if (OGRE_VERSION_MINOR <  4)
	virtual void frameStarted(Ogre::Real time, Ogre::InputReader* input)
	{};
	virtual void frameEnded(Ogre::Real time, Ogre::InputReader* input);

#else
	virtual void frameStarted(Ogre::Real time, OIS::Keyboard* input,
			OIS::Mouse* mouse)
	{
	}
	;
	virtual void frameEnded(Ogre::Real time, OIS::Keyboard* input,
			OIS::Mouse* mouse);

#endif //OGRE_VERSION not heihort
	// If we register this with a stepper it'll get told every time the world's about to be stepped
	bool preStep(Ogre::Real time)
	{
		addForcesAndTorques();
		return true;
	}

protected:
	// Keep track of what created us
	Ogre::SceneManager* _mgr;
	OgreOde::World* _world;
	OgreOde::Space* _space;
	OgreOde_Prefab::RagdollFactory* _ragdollFactory;

	// Whatever the derived class sets this to will be what's looked at by the camera
	Ogre::SceneNode* _last_node;

	// Keep track of the things we create so we can
	// delete them automatically when we switch scenes
	std::vector<OgreOde::Body*> _bodies;
	std::vector<OgreOde::Geometry*> _geoms;
	std::vector<OgreOde::Joint*> _joints;

	std::vector<Ogre::MovableObject*> clearList;
	std::vector<OgreOde_Prefab::Ragdoll *> RagdollList;

	Ogre::Real _key_delay;
	OgreOde_Loader::DotLoader *dotOgreOdeLoader;
};

class SimpleScenesApplication;

/*
 A frame listener that does default processing before deferring to the main application
 */
class SimpleScenesFrameListener: public ExampleFrameListener
{
public:
	// Standard example constructor with one additional parameter
	SimpleScenesFrameListener(SimpleScenesApplication* demo, RenderWindow* win,
			Camera* cam) :
		ExampleFrameListener(win, cam), _demo(demo)
	{
		mMoveSpeed = 15.0;
	}
	~SimpleScenesFrameListener()
	{
	}

	bool frameStarted(const FrameEvent& evt);
	bool frameEnded(const FrameEvent& evt);

protected:
	// Keep track of the application that created us
	SimpleScenesApplication* _demo;
};

/*
 The test application, based on the Ogre example application for consistency
 */
class SimpleScenesApplication: public ExampleApplication
{
	// The frame listener gets access to some special things
	friend class SimpleScenesFrameListener;

public:
	// Standard constructor/destructor
	SimpleScenesApplication() :
		ExampleApplication()
	{
		_test = 0;
		_plane = 0;
		_stepper = 0;
		_world = 0;

		_time_elapsed = 0.0;
		_time_step = SimpleScenes::STEP_RATE;
		_looking = _chasing = false;
		_paused = false;
	}
	~SimpleScenesApplication();

protected:
	// Override stuff from the base class
	void createScene(void);
	void createFrameListener(void)
	{
		mFrameListener = new SimpleScenesFrameListener(this, mWindow, mCamera);
		mRoot->addFrameListener(mFrameListener);
	}
	void chooseSceneManager(void)
	{
		mSceneMgr = mRoot->createSceneManager(ST_GENERIC, "basicsm");
	}

	// Add the standard resources, plus our own pack
	void setupResources(void)
	{
		ExampleApplication::setupResources();
		ResourceGroupManager *rsm = ResourceGroupManager::getSingletonPtr();
		StringVector groups = rsm->getResourceGroups();

		FileInfoListPtr finfo =
				ResourceGroupManager::getSingleton().findResourceFileInfo(
						"Bootstrap", "axes.mesh");

		const bool isSDK = (!finfo->empty()) && StringUtil::startsWith(
				finfo->begin()->archive->getName(),
				"../../media/packs/ogrecore.zip", true);

		{

			const String groupName("OgreOde");
			if (std::find(groups.begin(), groups.end(), String(groupName))
					== groups.end())
			{
				if (isSDK)
				{
					rsm->addResourceLocation("../../../ogreode/demos/Media",
							"FileSystem", groupName);
				}
				else
				{
					rsm->addResourceLocation(
							"../../../../../ogreaddons/ogreode/demos/Media",
							"FileSystem", groupName);
				}
			}
		}

		{
			const String groupName("Kuniverse");
			if (std::find(groups.begin(), groups.end(), String(groupName))
					== groups.end())
			{
				rsm->createResourceGroup(groupName);
				String folderBase("../../../../../lib/cvs/KUniverse/medias/");
				if (isSDK)
				{
					folderBase = ("../../../../../lib/cvs/KUniverse/medias/");
				}
				else
				{
					folderBase = "../../../../../../KUniverse/medias/";
				}

				rsm->addResourceLocation(folderBase, "FileSystem", groupName);
				rsm->addResourceLocation(folderBase + "gfx/3d/entities/bship",
						"FileSystem", groupName);
				rsm->addResourceLocation(folderBase + "gfx/3d/entities/bull",
						"FileSystem", groupName);
				rsm->addResourceLocation(folderBase
						+ "gfx/3d/entities/mothership", "FileSystem", groupName);
				rsm->addResourceLocation(folderBase + "gfx/3d/entities/tank",
						"FileSystem", groupName);
				rsm->addResourceLocation(
						folderBase + "gfx/3d/entities/trooper", "FileSystem",
						groupName);
				rsm->addResourceLocation(folderBase + "gfx/3d/buildings",
						"FileSystem", groupName);

				rsm->addResourceLocation(folderBase + "gfx/3d/envir",
						"FileSystem", groupName);
				rsm->addResourceLocation(folderBase + "gfx/3d/envir/fish",
						"FileSystem", groupName);
				rsm->addResourceLocation(folderBase + "gfx/3d/envir/grass",
						"FileSystem", groupName);
				rsm->addResourceLocation(folderBase + "gfx/3d/envir/metal",
						"FileSystem", groupName);
				rsm->addResourceLocation(folderBase + "gfx/3d/envir/palm",
						"FileSystem", groupName);
				rsm->addResourceLocation(folderBase + "gfx/3d/envir/rock",
						"FileSystem", groupName);
				rsm->addResourceLocation(folderBase + "gfx/3d/envir/sky",
						"FileSystem", groupName);
				rsm->addResourceLocation(folderBase + "gfx/3d/envir/water",
						"FileSystem", groupName);
				rsm->addResourceLocation(folderBase + "gfx/3d/envir/clouds",
						"FileSystem", groupName);

				rsm->addResourceLocation(folderBase + "gfx/3d/effects/",
						"FileSystem", groupName);
				rsm->addResourceLocation(folderBase + "gfx/3d/effects/smoke",
						"FileSystem", groupName);

				rsm->addResourceLocation(folderBase + "gfx/gui/", "FileSystem",
						groupName);

				rsm->addResourceLocation(folderBase + "sound/", "FileSystem",
						groupName);
				rsm->addResourceLocation(folderBase + "video", "FileSystem",
						groupName);

			}
		}

	}

	// The frame listener will call these

#if (OGRE_VERSION_MINOR < 4)
	void frameStarted(const FrameEvent& evt,InputReader* mInputDevice);
	void frameEnded(const FrameEvent& evt,InputReader* mInputDevice);
#else
	void frameStarted(const FrameEvent& evt, OIS::Keyboard* input,
			OIS::Mouse* mouse);
	void frameEnded(const FrameEvent& evt, OIS::Keyboard* input,
			OIS::Mouse* mouse);
#endif //OGRE_VERSION not heihort
protected:
	OgreOde::World* _world;
	OgreOde::StepHandler* _stepper;
	OgreOde::InfinitePlaneGeometry* _plane;
	SimpleScenes* _test;

	Real _delay, _time_elapsed, _time_step;
	Light* _spot;
	bool _looking, _chasing, _paused;
};

/*
 The box stacking test extends the base test class
 */
class SimpleScenes_BoxStack: public SimpleScenes
{
public:
	// Standard constructor/destructor
	SimpleScenes_BoxStack(OgreOde::World *world) :
		SimpleScenes(world)
	{
	}
	virtual ~SimpleScenes_BoxStack()
	{
	}

	// Return our name for the test application to display
	virtual const String& getName()
	{
		static String name = "Test Box Stack";
		return name;
	}

	// Return a description of the keys that the user can use in this test
	virtual const String& getKeys()
	{
		static String keys =
				"Z - Sphere, X - Box, C - Capsule, G - Ragdoll, T - Trimesh";
		return keys;
	}

#if (OGRE_VERSION_MINOR < 4)
	// Handle the user's key presses
	virtual void frameEnded(Real time,InputReader* input)
	{
		// Do default key handling
		SimpleScenes::frameEnded(time,input);
#else

	virtual void frameEnded(Real time, OIS::Keyboard* input, OIS::Mouse* mouse)
	{
		// Do default processing
		SimpleScenes::frameEnded(time, input, mouse);
#endif
		OgreOde::Body* body = 0;

		// Create random objects, that method will stop them being created too often
		if (input->isKeyDown(OIS::KC_Z))
			body = createRandomObject(OgreOde::Geometry::Class_Sphere);
		else if (input->isKeyDown(OIS::KC_X))
			body = createRandomObject(OgreOde::Geometry::Class_Box);
		else if (input->isKeyDown(OIS::KC_C))
			body = createRandomObject(OgreOde::Geometry::Class_Capsule);
		else if (input->isKeyDown(OIS::KC_T))
			body = createRandomObject(OgreOde::Geometry::Class_TriangleMesh);
		else if (input->isKeyDown(OIS::KC_G))
			createRagDoll();

	}
};

//------------------------------------------------------------------------------------------------
static const String carNames[] =
{ "Jeep", "JeepSway", "Subaru" };
static const String carFileNames[] =
{ "jeep.ogreode", "jeep.ogreode", "subaru.ogreode" };
static int sSelectedCar = 1;
static int maxNumCar = 3;

/*
 Buggy test extends from the base test class
 */
class SimpleScenes_Buggy: public SimpleScenes
{
public:
	//------------------------------------------------------------------------------------------------
	// Constructor also needs to create the scene
	SimpleScenes_Buggy(OgreOde::World *world) :
		SimpleScenes(world), vehicle(0)
	{
		// Create the vehicle from the config file
		changeCar();

		// Move the vehicle
		vehicle->setPosition(Vector3(0, 0.82898, 0));

		// Initially (i.e. in the config file) it's rear wheel drive
		_drive = 'R';
		setInfoText(carNames[sSelectedCar] + "Rear wheel drive");

		// Create a box to jump over, the visual version
		Entity* entity = _mgr->createEntity("Jump", "crate.mesh");
		//entity->setNormaliseNormals(true);
		entity->setCastShadows(true);

		SceneNode* node = _mgr->getRootSceneNode()->createChildSceneNode(
				entity->getName());
		node->attachObject(entity);
		node->setPosition(Vector3(0, 0.3, -5));
		node->setOrientation(Quaternion(Radian(0.4), Vector3(1, 0, 0)));
		node->setScale(0.3, 0.1, 0.4);

		// Create the physical version (just static geometry, it can't move so
		// it doesn't need a body) and keep track of it
		OgreOde::EntityInformer ei(entity, Matrix4::getScale(node->getScale()));
		OgreOde::Geometry *geom = ei.createSingleStaticBox(_world, _space);
		entity->setUserAny(Ogre::Any(this));
		_geoms.push_back(geom);

		// The car is what we'll want to look at
		_last_node = vehicle->getSceneNode();
	}

	//------------------------------------------------------------------------------------------------
	// Override the destructor since there's some special deleting to do
	virtual ~SimpleScenes_Buggy()
	{
		delete vehicle;

		// Destroy the jump manually since it's not associated with
		// any body it won't get deleted automatically
		_mgr->destroySceneNode("Jump");
		_mgr->destroyEntity("Jump");

		// Geometries and Joints will get deleted by the base class
	}

	//------------------------------------------------------------------------------------------------
	// Return our name
	virtual const String& getName()
	{
		static String name = "Test Buggy";
		return name;
	}

	//------------------------------------------------------------------------------------------------
	// Tell the user what keys they can use
	virtual const String& getKeys()
	{
		static String keys =
				"I/K - Accelerate/Brake, J/L - Turn, X - Change drive mode, N - Change Car";
		return keys;
	}

	//------------------------------------------------------------------------------------------------
	void changeCar()
	{
		sSelectedCar = (sSelectedCar + 1) % maxNumCar;

		delete vehicle;
		vehicle
				= static_cast<OgreOde_Prefab::Vehicle *> (dotOgreOdeLoader->loadObject(
						carFileNames[sSelectedCar], carNames[sSelectedCar]));

		// Move the vehicle
		vehicle->setPosition(Vector3(0, 0.82898, 0));
	}
	//------------------------------------------------------------------------------------------------
#if (OGRE_VERSION_MINOR < 4)
	// Handle the user's key presses
	virtual void frameEnded(Real time,InputReader* input)
	{
		// Do default key handling
		SimpleScenes::frameEnded(time,input);
#else

	virtual void frameEnded(Real time, OIS::Keyboard* input, OIS::Mouse* mouse)
	{
		// Do default processing
		SimpleScenes::frameEnded(time, input, mouse);
#endif
		// Tell the vehicle what digital inputs are being pressed; left, right, power and brake
		// There are equivalent methods for analogue controls, current you can't change gear so
		// you can't reverse!
		vehicle->setInputs(input->isKeyDown(OIS::KC_J), input->isKeyDown(
				OIS::KC_L), input->isKeyDown(OIS::KC_I), input->isKeyDown(
				OIS::KC_K));

		// Update the vehicle, you need to do this every time step
		vehicle->update(time);
		if (_key_delay > SimpleScenes::KEY_DELAY)
		{

			if ((input->isKeyDown(OIS::KC_N)))
			{
				changeCar();
				_key_delay = 0.0;
			}
			// Change the drive mode between front, rear and 4wd
			if ((input->isKeyDown(OIS::KC_X)))
			{
				switch (_drive)
				{
				// Switch from rear to front
				case 'R':
					_drive = 'F';

					vehicle->getWheel(0)->setPowerFactor(1);
					vehicle->getWheel(1)->setPowerFactor(1);
					vehicle->getWheel(2)->setPowerFactor(0);
					vehicle->getWheel(3)->setPowerFactor(0);

					setInfoText(carNames[sSelectedCar] + "Front wheel drive");
					break;

					// Switch from front to all
				case 'F':
					_drive = '4';

					vehicle->getWheel(0)->setPowerFactor(0.6);
					vehicle->getWheel(1)->setPowerFactor(0.6);
					vehicle->getWheel(2)->setPowerFactor(0.4);
					vehicle->getWheel(3)->setPowerFactor(0.4);

					setInfoText(carNames[sSelectedCar] + "All wheel drive");
					break;

					// Switch from all to rear
				case '4':
					_drive = 'R';

					vehicle->getWheel(0)->setPowerFactor(0);
					vehicle->getWheel(1)->setPowerFactor(0);
					vehicle->getWheel(2)->setPowerFactor(1);
					vehicle->getWheel(3)->setPowerFactor(1);

					setInfoText(carNames[sSelectedCar] + "Rear wheel drive");
					break;
				}
				_key_delay = 0.0;
			}
		}
	}

	//------------------------------------------------------------------------------------------------
	// Override the collision callback to set our own parameters
	bool collision(OgreOde::Contact* contact)
	{
		// If the base class doesn't think the collision needs to
		// happen then we won't do anything either
		if (!SimpleScenes::collision(contact))
			return false;

		if (!OgreOde_Prefab::Vehicle::handleTyreCollision(contact))
		{
			// Set the floor to be a bit slippy
			contact->setCoulombFriction(12.0);
		}
		return true;
	}

protected:
	// Keep track of the things we need to delete manually or change according to user input
	OgreOde_Prefab::Vehicle *vehicle;
	char _drive;
};

/*
 The chain test extends the base test class
 */
class SimpleScenes_Chain: public SimpleScenes
{
public:
	// Standard constructor, creates everything in the demo
	SimpleScenes_Chain(OgreOde::World *world) :
		SimpleScenes(world)
	{
		// Set up the sizes of stuff
		Real link_size = 0.4/*, geom_size = 0.4*/, adjust = 0.1;
		OgreOde::Body* last_body = 0;
		int num_links = 10;

		_first_body = 0;
		_force = 100;

		// Create all the links in the chain
		for (int i = num_links - 1; i >= 0; i--)
		{
			Real np = (Real) i * (link_size + adjust);
			String name = String("Sphere_") + StringConverter::toString(i);

			// Visuals
			Entity* entity = _mgr->createEntity(name, "ball.mesh");
			//entity->setNormaliseNormals(true);
			entity->setCastShadows(true);

			SceneNode* node = _mgr->getRootSceneNode()->createChildSceneNode(
					entity->getName());
			node->attachObject(entity);
			node->setScale(link_size * 0.2, link_size * 0.2, link_size * 0.2);
			node->setPosition(Vector3(np, np + 0.4, np));

			// Physicals
			OgreOde::EntityInformer ei(entity, Matrix4::getScale(
					node->getScale()));
			OgreOde::Body* body = ei.createSingleDynamicSphere(1.0, _world,
					_space);
			_bodies.push_back(body);
			_geoms.push_back(body->getGeometry(0));

			// Join the current body to the last one (if there was a last one)
			if (!_first_body)
			{
				_last_node = node;
				_first_body = body;
			}

			if (last_body)
			{
				OgreOde::BallJoint* joint = new OgreOde::BallJoint(_world);
				joint->attach(body, last_body);
				Real ja = ((Real) i + 0.5) * (link_size + adjust);
				joint->setAnchor(Vector3(ja, ja + 0.4, ja));

				_joints.push_back(joint);
			}

			last_body = body;
		}

		// Create some static boxes to bump into
		createBox(0, 2, Vector3(4, 1, 4));
		createBox(1, 2, Vector3(-4, 1, 4));
		createBox(2, 2, Vector3(-4, 1, -4));
		createBox(3, 2, Vector3(4, 1, -4));
	}

	// Let the user throw the chain around
#if (OGRE_VERSION_MINOR < 4)
	// Handle the user's key presses
	virtual void frameEnded(Real time,InputReader* input)
	{
		// Do default key handling
		SimpleScenes::frameEnded(time,input);
#else

	virtual void frameEnded(Real time, OIS::Keyboard* input, OIS::Mouse* mouse)
	{
		// Do default processing
		SimpleScenes::frameEnded(time, input, mouse);
#endif
		_force_to_apply = Ogre::Vector3::ZERO;

		Vector3 right = _mgr->getCamera("PlayerCam")->getRight();
		Vector3 forward = right.crossProduct(Vector3::UNIT_Y);

		// Up
		if (input->isKeyDown(OIS::KC_X))
			_force_to_apply += Ogre::Vector3::UNIT_Y * _force;

		// Left/right
		if (input->isKeyDown(OIS::KC_J))
			_force_to_apply -= right * _force;
		if (input->isKeyDown(OIS::KC_L))
			_force_to_apply += right * _force;

		// Forward/back
		if (input->isKeyDown(OIS::KC_K))
			_force_to_apply += forward * _force;
		if (input->isKeyDown(OIS::KC_I))
			_force_to_apply -= forward * _force;

		_mgr->getCamera("PlayerCam");
	}

	// Apply the forces before every time step
	void addForcesAndTorques()
	{
		// Apply the force we calculated in the key handler
		_first_body->addForce(_force_to_apply);
	}

	// Return our name for the test application to display
	virtual const String& getName()
	{
		static String name = "Test Chain";
		return name;
	}

	// Tell the user what keys they can use
	virtual const String& getKeys()
	{
		static String keys = "J/L, I/K, X - Throw the chain around";
		return keys;
	}

	// Override the collision callback to set our own parameters
	bool collision(OgreOde::Contact* contact)
	{
		// Set the floor to be a bit slippy
		contact->setCoulombFriction(10.0);
		return true;
	}

	// Use the destructor to delete the crate scene nodes
	// everything else gets deleted automatically
	~SimpleScenes_Chain()
	{
		for (int i = 0; i < 4; i++)
		{
			if (i != 2)
			{
				String name = String("Crate_") + StringConverter::toString(i);
				_mgr->destroySceneNode(name);
				_mgr->destroyEntity(name);
			}
		}
	}

protected:
	// Utility method to create a static box
	void createBox(int id, Real size, const Ogre::Vector3& position)
	{
		// Visual
		String name = String("Crate_") + StringConverter::toString(id);

		Entity* entity = _mgr->createEntity(name, "crate.mesh");
		//		entity->setNormaliseNormals(true);
		entity->setCastShadows(true);

		SceneNode* node = _mgr->getRootSceneNode()->createChildSceneNode(
				entity->getName());
		node->attachObject(entity);
		node->setScale(size * 0.1, size * 0.1, size * 0.1);
		node->setPosition(position);

		// Make one of them dynamic, the others are static
		OgreOde::EntityInformer ei(entity, Matrix4::getScale(node->getScale()));
		if ((position.x < 0) && (position.z < 0))
		{
			_box_body = ei.createSingleDynamicBox(2.0, _world, _space);
			_bodies.push_back(_box_body);
			_geoms.push_back(_box_body->getGeometry(0));
		}
		else
		{
			// Collision geometry
			_geoms.push_back(ei.createSingleStaticBox(_world, _space));
		}
	}

protected:
	// Keep track of the top of the chain so we can throw it around
	OgreOde::Body *_first_body, *_box_body;
	Real _force;
	Vector3 _force_to_apply;
};

/*
 The box stacking test extends the box stacking demo, but adds a trimesh!
 */
class SimpleScenes_TriMesh: public SimpleScenes_BoxStack
{
public:
	// Standard constructor, but create a triangle mesh
	SimpleScenes_TriMesh(OgreOde::World *world) :
		SimpleScenes_BoxStack(world)
	{
		// Create the visual entity and scene node
		Entity* entity = _mgr->createEntity("Landscape", "landscape.mesh");

		SceneNode* node = _mgr->getRootSceneNode()->createChildSceneNode(
				entity->getName());
		node->attachObject(entity);
		node->setScale(Vector3(0.4, 0.3, 0.4));
		node->setPosition(Vector3(0, 3, 0));
		node->setOrientation(Quaternion(Degree(30), Vector3::UNIT_Y));

		//entity->setNormaliseNormals(true);
		//entity->setCastShadows(false);

		// Turn it into a triangle mesh static geometry
		OgreOde::EntityInformer ei(entity, node->_getFullTransform());
		OgreOde::Geometry *geom = ei.createStaticTriangleMesh(_world, _space);
		entity->setUserAny(Ogre::Any(geom));
		_geoms.push_back(geom);
	}

	// Destructor, manually destroy the entity and node, since they're
	// not associated with a body they won't get deleted automatically
	virtual ~SimpleScenes_TriMesh()
	{
		_mgr->destroySceneNode("Landscape");
		_mgr->destroyEntity("Landscape");
	}

	// Return our name for the test application to display
	virtual const String& getName()
	{
		static String name = "Test Triangle Mesh";
		return name;
	}
};

/*
 The crash test extends the base test class
 */
class SimpleScenes_Crash: public SimpleScenes
{
public:
	// Standard constructor/destructor
	SimpleScenes_Crash(OgreOde::World *world) :
		SimpleScenes(world)
	{
		_rocket_node_explosion
				= _mgr->getRootSceneNode()->createChildSceneNode(
						"rocket_node_explosion_node");

		_rocketParticles_explosion = _mgr->createParticleSystem(
				"rocket_node_explosion", "myExplosionTemplate");
		_rocketParticles_smoke_white = _mgr->createParticleSystem(
				"rocketParticles_smoke_white", "myExplosionSmokewhiteTemplate");
		_rocketParticles_smoke = _mgr->createParticleSystem(
				"rocketParticles_smoke", "myExplosionSmokeTemplate");

		_rocket_node_explosion->attachObject(_rocketParticles_explosion);
		_rocket_node_explosion->attachObject(_rocketParticles_smoke_white);
		_rocket_node_explosion->attachObject(_rocketParticles_smoke);

		_rocketParticles_explosion->setSpeedFactor(0.0);
		_rocketParticles_smoke_white->setSpeedFactor(0.0);
		_rocketParticles_smoke->setSpeedFactor(0.0);

		_rocket_node_explosion->setVisible(false);

		// create helicopter
		{
			// Main bit of the helicopter
			_apache_body
					= _mgr->createEntity("Apache Body", "apache_body.mesh");

			_apache_body->setQueryFlags(VEHICLE_QUERY_MASK);

			_apache_body->setCastShadows(true);

			_apache_body_node = _mgr->getRootSceneNode()->createChildSceneNode(
					"Apache Body");
			_apache_body_node->attachObject(_apache_body);
			_apache_body_node->setPosition(0, 1.2, 0);

			// Main rotor
			_main_rotor = _mgr->createEntity("Main Rotor", "main_rotor.mesh");
			_main_rotor->setCastShadows(true);

			_main_rotor_node = _apache_body_node->createChildSceneNode(
					"Main Rotor");
			_main_rotor_node->attachObject(_main_rotor);
			_main_rotor_node->setPosition(0, 0.987322, 0.573885);

			// Tail rotor
			_tail_rotor = _mgr->createEntity("Tail Rotor", "tail_rotor.mesh");
			_tail_rotor->setCastShadows(true);

			_tail_rotor_node = _apache_body_node->createChildSceneNode(
					"Tail Rotor");
			_tail_rotor_node->attachObject(_tail_rotor);
			_tail_rotor_node->setPosition(0.174927, 0.173132, -3.50708);

			// Set up the parameters for the main rotor
			_main_min = 2.0;
			_main_max = 90.0;
			_main_spd = _main_min;
			_main_response = 40.0;
			_main_scale = 1.0;

			// Set up the parameters for the tail rotor
			_tail_min = -80.0;
			_tail_max = 80.0;
			_tail_response = 90.0;
			_tail_scale = 0.5;
			_tail_spd = 0.0;

			// Other simulation parameters
			_rotate_scale = 20.0;
			_thrust = 50.0;
			_linear_damp = 12.5;
			_angular_damp = 25.0;
			_thrust_offs = Ogre::Vector3(0, 0.1, 0);
			_fire_rate = 0.5;
			_fire_time = _fire_rate;
			_fire_force = 250.0;
			_fire_mass = 0.1;
			_fire_size = 0.25;
			_rockets = 0;

			// Create a space for the helicopter to live in
			_apache_space = new OgreOde::SimpleSpace(_world, _space);
			_apache_space->setInternalCollisions(false);
			_apache_space->setAutoCleanup(false);

			// Create the physical body of the helicopter
			OgreOde::BoxMass apache_mass(1.5, Vector3(1.4, 1.46, 4.24));
			_apache = new OgreOde::Body(_world);
			_apache_body_node->attachObject(_apache);
			_apache->setMass(apache_mass);

			_apache->setDamping(_linear_damp, _angular_damp);

			_bodies.push_back(_apache);
			_last_node = _apache_body_node;

			// Create the geometry of the helicopter from several bits

			// Main body
			OgreOde::TransformGeometry* trans = new OgreOde::TransformGeometry(
					_world, _apache_space);
			trans->setBody(_apache);

			_apache_body->setUserAny(Ogre::Any(trans));

			OgreOde::BoxGeometry* geom = new OgreOde::BoxGeometry(Vector3(1.4,
					1.46, 4.24), world, 0);
			trans->setEncapsulatedGeometry(geom);
			geom->setPosition(Vector3(0.013, 0.014, 1.052));
			_geoms.push_back(geom);
			_geoms.push_back(trans);

			// Left wing
			trans = new OgreOde::TransformGeometry(_world, _apache_space);
			geom
					= new OgreOde::BoxGeometry(Vector3(0.75, 0.55, 1.05),
							world, 0);
			trans->setBody(_apache);
			trans->setEncapsulatedGeometry(geom);
			geom->setPosition(Vector3(-1.089, -0.486, 0.446));
			_geoms.push_back(geom);
			_geoms.push_back(trans);

			// Right wing
			trans = new OgreOde::TransformGeometry(_world, _apache_space);
			geom
					= new OgreOde::BoxGeometry(Vector3(0.75, 0.55, 1.05),
							world, 0);
			trans->setBody(_apache);
			trans->setEncapsulatedGeometry(geom);
			geom->setPosition(Vector3(1.089, -0.505, 0.446));
			_geoms.push_back(geom);
			_geoms.push_back(trans);

			// Tail boom
			trans = new OgreOde::TransformGeometry(_world, _apache_space);
			geom
					= new OgreOde::BoxGeometry(Vector3(0.73, 0.66, 3.28),
							world, 0);
			trans->setBody(_apache);
			trans->setEncapsulatedGeometry(geom);
			geom->setPosition(Vector3(0, -0.533, -2.104));
			_geoms.push_back(geom);
			_geoms.push_back(trans);

			// Tail flipper (probably not the technically correct name!)
			trans = new OgreOde::TransformGeometry(_world, _apache_space);
			geom
					= new OgreOde::BoxGeometry(Vector3(1.61, 0.30, 0.45),
							world, 0);
			trans->setBody(_apache);
			trans->setEncapsulatedGeometry(geom);
			geom->setPosition(Vector3(-0.014, -0.622, -3.778));
			_geoms.push_back(geom);
			_geoms.push_back(trans);

			// Tail fin
			trans = new OgreOde::TransformGeometry(_world, _apache_space);
			geom
					= new OgreOde::BoxGeometry(Vector3(0.30, 1.27, 0.96),
							world, 0);
			trans->setBody(_apache);
			trans->setEncapsulatedGeometry(geom);
			geom->setPosition(Vector3(0, -0.042, -3.491));
			_geoms.push_back(geom);
			_geoms.push_back(trans);

			// Rear wheel - a separate body connected by a ball joint, doesn't have a SceneNode
			OgreOde::SphereMass wheel_mass(0.02, 0.06);
			_rear_wheel = new OgreOde::Body(_world);
			_rear_wheel->setMass(wheel_mass);
			_rear_wheel->setPosition(Vector3(0.0, 0.068, -3.937));
			OgreOde::BallJoint* ball_joint = new OgreOde::BallJoint(_world);
			ball_joint->attach(_apache, _rear_wheel);
			ball_joint->setAnchor(_rear_wheel->getPosition());
			OgreOde::SphereGeometry* sphere = new OgreOde::SphereGeometry(0.06,
					_world, _apache_space);
			sphere->setBody(_rear_wheel);
			_geoms.push_back(sphere);
			_bodies.push_back(_rear_wheel);
			_joints.push_back(ball_joint);

			// Left front wheel - connected by a hinge joint so it can only roll forward
			OgreOde::SphereMass front_wheel_mass(0.03, 0.17);
			_left_front = new OgreOde::Body(_world);
			_left_front->setMass(front_wheel_mass);
			_left_front->setPosition(Vector3(-0.699, 0.199, 1.163));
			OgreOde::HingeJoint* hinge_joint = new OgreOde::HingeJoint(_world);
			hinge_joint->attach(_apache, _left_front);
			hinge_joint->setAnchor(_left_front->getPosition());
			hinge_joint->setAxis(Vector3::UNIT_X);
			sphere = new OgreOde::SphereGeometry(0.17, _world, _apache_space);
			sphere->setBody(_left_front);
			_geoms.push_back(sphere);
			_bodies.push_back(_left_front);
			_joints.push_back(hinge_joint);

			// Right front wheel - connected by a hinge joint, doesn't have a SceneNode
			_right_front = new OgreOde::Body(_world);
			_right_front->setMass(front_wheel_mass);
			_right_front->setPosition(Vector3(0.699, 0.185, 1.163));
			hinge_joint = new OgreOde::HingeJoint(_world);
			hinge_joint->attach(_apache, _right_front);
			hinge_joint->setAnchor(_right_front->getPosition());
			hinge_joint->setAxis(Vector3::UNIT_X);
			sphere = new OgreOde::SphereGeometry(0.17, _world, _apache_space);
			sphere->setBody(_right_front);

			_geoms.push_back(sphere);
			_bodies.push_back(_right_front);
			_joints.push_back(hinge_joint);
		}

		// Create the wall of boxes to crash into, make it
		// smaller in Debug mode for performance reasons
		int i = 0;
#ifndef _DEBUG
		for (Real y = 0.0; y <= 5.0; y += 1.0)
		{
			for (Real x = -6.0 + (y * 0.5); x <= 6.0 - (y * 0.5); x += 1.0)
			{
#else
				for (Real y = 0.0;y <= 1.0;y += 1.0)
				{
					for (Real x = -2.0 + (y * 0.5);x <= 2.0 - (y * 0.5);x += 1.0)
					{
#endif
				// Create the Ogre box
				String name = String("Box_") + StringConverter::toString(i);
				Entity* box = _mgr->createEntity(name, "crate.mesh");
				box->setQueryFlags(GEOMETRY_QUERY_MASK);
				box->setCastShadows(true);

				SceneNode* node =
						_mgr->getRootSceneNode()->createChildSceneNode(name);
				node->attachObject(box);
				node->setScale(0.1, 0.1, 0.1);

				// Set the position and rotate that, then rotate the box by the same amount
				Quaternion q;
				q.FromAngleAxis(Degree(45), Vector3::UNIT_Y);
				Vector3 pos(x, y + 0.5, -10.0);
				node->setPosition(q * pos);
				node->setOrientation(q);

				// Create a box for ODE and attach it to the Ogre version
				OgreOde::Body* body = new OgreOde::Body(_world, name + "Body");
				node->attachObject(body);
				body->setMass(OgreOde::BoxMass(0.01, Vector3(1, 1, 1)));
				body->setDamping(0.005, 0.005);

				OgreOde::BoxGeometry* geom = new OgreOde::BoxGeometry(Vector3(
						1.0, 1.0, 1.0), _world, _space);
				// Tie the collision geometry to the physical body
				geom->setBody(body);
				box->setUserAny(Ogre::Any(geom));

				// Keep track of the ODE objects for house keeping
				_bodies.push_back(body);
				_geoms.push_back(geom);

				i++;
			}
		}
	}
	virtual ~SimpleScenes_Crash()
	{
		// Destroy the non-standard scene nodes that make up the helicopter
		_apache_body_node->removeAndDestroyChild("Main Rotor");
		_apache_body_node->removeAndDestroyChild("Tail Rotor");

		// Remove the entities that represent the scene nodes we destroyed
		_mgr->destroyEntity("Main Rotor");
		_mgr->destroyEntity("Tail Rotor");

		// Delete the space in which the helicopter lived
		delete _apache_space;
	}

	// Return our name for the test application to display
	virtual const String& getName()
	{
		static String name = "Test Crash";
		return name;
	}

	// Return a description of the keys that the user can use in this test
	virtual const String& getKeys()
	{
		static String keys =
				"I/K - Throttle, J/L - Turn, G/B/V/N - Thrust, X - Fire";
		return keys;
	}

#if (OGRE_VERSION_MINOR < 4)
	// Handle the user's key presses
	virtual void frameEnded(Real time,InputReader* input)
	{
		// Do default key handling
		SimpleScenes::frameEnded(time,input);
#else

	virtual void frameEnded(Real time, OIS::Keyboard* input, OIS::Mouse* mouse)
	{
		// Do default processing
		SimpleScenes::frameEnded(time, input, mouse);
#endif

		// Throttle up or down
		if (input->isKeyDown(OIS::KC_I))
			_main_spd += (_main_response * time);
		else if (input->isKeyDown(OIS::KC_K))
			_main_spd -= (_main_response * time);

		// Thrust left, right, forward, or back
		_thrust_force = Ogre::Vector3::ZERO;
		if (input->isKeyDown(OIS::KC_G))
			_thrust_force.z += _thrust;
		if (input->isKeyDown(OIS::KC_B))
			_thrust_force.z -= _thrust;
		if (input->isKeyDown(OIS::KC_V))
			_thrust_force.x += _thrust;
		if (input->isKeyDown(OIS::KC_N))
			_thrust_force.x -= _thrust;

		// Clamp the main rotor speed
		_main_spd = std::max(_main_min, std::min(_main_max, _main_spd));

		// Turn left or right or automatically stop the tail rotor
		if (input->isKeyDown(OIS::KC_J))
			_tail_spd += (_tail_response * time);
		else if (input->isKeyDown(OIS::KC_L))
			_tail_spd -= (_tail_response * time);
		else
		{
			if (_tail_spd < 0.0)
			{
				_tail_spd += (_tail_response * time);
				if (_tail_spd > 0.0)
					_tail_spd = 0.0;
			}
			else if (_tail_spd > 0.0)
			{
				_tail_spd -= (_tail_response * time);
				if (_tail_spd < 0.0)
					_tail_spd = 0.0;
			}
		}

		// Clamp the tail rotor speed
		_tail_spd = std::max(_tail_min, std::min(_tail_max, _tail_spd));

		// Rotate the tail rotor scene node
		_tail_rotor_node->rotate(Vector3::UNIT_X, Radian(_tail_spd * time
				* _rotate_scale));

		// If there's some power being applied to the main rotor then...
		if ((_main_spd > _main_min) || (!input->isKeyDown(OIS::KC_K)))
		{
			// Rotate the main rotor scene node
			_main_rotor_node->rotate(Vector3::UNIT_Y, Radian(_main_spd * time
					* _rotate_scale));
		}

		// Fire rockets. Woo!
		_fire_time += time;
		if ((_fire_time > _fire_rate) && (input->isKeyDown(OIS::KC_X)))
		{
			_fire_time = 0.0;
			_rockets++;

			// Create a rocket node and attach a particle system to it
			const String name(String("Rocket_") + StringConverter::toString(
					_rockets));
			ParticleSystem * rocketParticles = _mgr->createParticleSystem(name,
					"OgreOdeDemos/Rocket");
			SceneNode * rocket_node =
					_mgr->getRootSceneNode()->createChildSceneNode(name);
			rocket_node->attachObject(rocketParticles);

			// Alternate firing between the left and right pods and
			// convert it from body coordinates to world
			// Do the same with the firing "force" vector
			const Vector3 pos(_apache->getPointWorldPosition(Vector3(1.35
					* ((_rockets & 1) ? -1.0 : 1.0), -0.55, 0.95)));
			const Vector3 force(_apache->getVectorToWorld(Vector3(0, 0,
					_fire_force)));

			rocket_node->setPosition(pos);

			// Create a sphere for the physical body
			OgreOde::Body* rocket_body = new OgreOde::Body(_world);
			OgreOde::SphereGeometry* rocket_geom = new OgreOde::SphereGeometry(
					_fire_size, _world, _space);

			rocket_body->setMass(OgreOde::SphereMass(_fire_mass, _fire_size));
			rocket_body->setAffectedByGravity(false);
			rocket_geom->setBody(rocket_body);

			rocket_node->attachObject(rocket_body);

			_bodies.push_back(rocket_body);
			_geoms.push_back(rocket_geom);

			// Fire it off by applying an initial force
			rocket_body->addForce(force);

			// Initialise the rocket's life span to zero
			_rocket_list[rocket_geom] = 0.0f;
		}

		// Check all the rockets
		for (std::map<OgreOde::SphereGeometry*, Real>::iterator i =
				_rocket_list.begin(); i != _rocket_list.end();)
		{
			// Increase the time it's lived
			float *rocketTime = &(i->second);
			*rocketTime += time;

			// If it had it's emitter disabled (see below) more than 2 seconds ago then kill it
			if ((*rocketTime < 0.0))
			{
				// Get the geometry's body and kill any particle system attached to the same node
				killParticleSystem(i->first);

				OgreOde::Body * const body = i->first->getBody();
				// Manually remove the body from the list of managed bodies
				for (std::vector<OgreOde::Body*>::iterator bi = _bodies.begin(); bi
						!= _bodies.end();)
				{
					if ((*bi) == body)
						bi = _bodies.erase(bi);
					else
						++bi;
				}

				// Manually delete the geometry from the list of managed geometries
				for (std::vector<OgreOde::Geometry*>::iterator gi =
						_geoms.begin(); gi != _geoms.end();)
				{
					if ((*gi) == i->first)
						gi = _geoms.erase(gi);
					else
						++gi;
				}

				// Delete the actual body and geometry
				delete body;
				delete i->first;

				// Erase the rocket from the hash map
				_rocket_list.erase(i++);
			}
			else
			{
				// If the rocket has been alive for more than 2 seconds then kill its emitters (so it fades away)
				if (*rocketTime > 3.0)
				{
					OgreOde::SphereGeometry* sphereGeom = i->first;
					killEmitters(sphereGeom, rocketTime);
				}
				++i;
			}
		}
	}

	// Override the base collision callback, 'cos we don't want the "if connected" check doing
	bool collision(OgreOde::Contact* contact)
	{
		OgreOde::Geometry* firstGeom = contact->getFirstGeometry();
		OgreOde::Geometry* secondGeom = contact->getSecondGeometry();

		OgreOde::SphereGeometry* rocketGeom = 0;
		OgreOde::Geometry* otherGeom = 0;
		float *time = 0;
		std::map<OgreOde::SphereGeometry*, Real>::iterator li;
		if (firstGeom->getClass() == OgreOde::Geometry::Class_Sphere
				|| secondGeom->getClass() == OgreOde::Geometry::Class_Sphere)
		{

			// If a rocket's hit something then stop emitting particles
			// check if it's a sphere (rocket proxy Geom...)
			if (firstGeom->getClass() == OgreOde::Geometry::Class_Sphere)
			{
				li = _rocket_list.find(
						static_cast<OgreOde::SphereGeometry*> (firstGeom));
				if (li != _rocket_list.end())
				{
					rocketGeom = li->first;
					time = &(li->second);
					otherGeom = secondGeom;
				}
			}

			if (secondGeom->getClass() == OgreOde::Geometry::Class_Sphere)
			{
				std::map<OgreOde::SphereGeometry*, Real>::iterator
						li =
								_rocket_list.find(
										static_cast<OgreOde::SphereGeometry*> (secondGeom));
				if (li != _rocket_list.end())
				{
					rocketGeom = li->first;
					time = &(li->second);
					otherGeom = firstGeom;
				}
			}
			if (rocketGeom && *time > 0)
			{
				// prevent collision against Apache
				if (otherGeom->getSpace() == _apache_space)
				{
					return false;
				}

				// there, Add an Explosion Effect (could make two sphere, one that destroy, one for the wave effect only.)
				const Vector3 explosionCenter(contact->getPosition());
				Ogre::SphereSceneQuery* explosionAreaQuery =
						_mgr->createSphereQuery(Sphere(explosionCenter, 20),
								0xFFFFFFFF);

				// asking for movable physic object (no particles system or static geometry (here the plane))
				explosionAreaQuery->setQueryMask(GEOMETRY_QUERY_MASK
						| VEHICLE_QUERY_MASK);
				explosionAreaQuery->setQueryTypeMask(
						SceneManager::ENTITY_TYPE_MASK);

				const SceneQueryResult& result = explosionAreaQuery->execute();
				if (!result.movables.empty())
				{
					SceneQueryResultMovableList::const_iterator i =
							result.movables.begin();
					while (i != result.movables.end())
					{
						//GEOMETRY_QUERY_MASK

						Ogre::Any a = (*i)->getUserAny();
						if (!a.isEmpty())
						{
							const String typeInfo = String(a.getType().name());

							OgreOde::Geometry *affectedGeom = 0;
							if (a.getType() == typeid(OgreOde::BoxGeometry))
							{
								affectedGeom = (OgreOde::Geometry *) any_cast<
										OgreOde::BoxGeometry *> (a);
							}
							else if (a.getType()
									== typeid(OgreOde::TransformGeometry))
							{
								affectedGeom = (OgreOde::Geometry *) any_cast<
										OgreOde::TransformGeometry *> (a);
							}
							else if (a.getType() == typeid(OgreOde::Geometry))
							{
								affectedGeom
										= any_cast<OgreOde::Geometry *> (a);
							}

							assert (affectedGeom != rocketGeom);
							assert (affectedGeom);
							OgreOde::Body *affectedBody =
									affectedGeom->getBody();
							assert (affectedBody);
							Vector3 forceAreaDirection =
									(*i)->getParentNode()->getPosition()
											- explosionCenter;
							const Real forcePower =
									forceAreaDirection.squaredLength() / 5;
							forceAreaDirection.normalise();

							// Apply the main rotor force
							affectedBody->addForce(forceAreaDirection
									* forcePower);
						}

						++i;
					}
				}

				// remove particle and rocket geom
				killEmitters(rocketGeom, time);

				// add explosion particles
				_rocket_node_explosion->setPosition(explosionCenter);
				_rocket_node_explosion->setVisible(true);

				resetParticleSystem(_rocketParticles_explosion, true, 0.1);
				resetParticleSystem(_rocketParticles_smoke_white, true, 0.3);
				resetParticleSystem(_rocketParticles_smoke, true, 0.6);
			}

		}
		// Set the friction at the contact
		contact->setCoulombFriction(5.0);

		// Yes, this collision is valid
		return true;
	}

	// Will get called just before each time step, since a timestep zeros the force accumulators
	void addForcesAndTorques()
	{
		// Apply the main rotor force
		_apache->addForce(Vector3(0, _main_spd * _main_scale, 0));

		// Apply the tail rotor torque
		_apache->addRelativeTorque(Vector3(0, _tail_spd * _tail_scale, 0));

		// Apply the thrust force
		_apache->addRelativeForceAtRelative(_thrust_force, _thrust_offs);

	}

protected:

	// Kill the emitters for any particle systems attached to the same node
	// as the body with which the specified geometry is associated (if that makes sense!)
	void killEmitters(OgreOde::SphereGeometry* geom, Real* time = 0)
	{
		// Find the body
		OgreOde::Body* body = geom->getBody();
		if (body)
		{
			// Find the associated scene node
			SceneNode* node = static_cast<SceneNode*> (body->getParentNode());
			if (node)
			{
				// Kill the emitters of any attached particle systems
				for (int i = 0; i < node->numAttachedObjects(); i++)
				{
					MovableObject* obj = node->getAttachedObject(i);
					if (obj->getMovableType() == "ParticleSystem")
					{
						static_cast<ParticleSystem*> (obj)->removeAllEmitters();
					}
				}
			}

			// Set the life span value so that we can easily tell
			// that its had its emitters deleted
			if (time)
				*time = -100.0;
		}
	}

	void resetParticleSystem(Ogre::ParticleSystem *ps, bool enable,
			const Ogre::Real delay)
	{
		const unsigned short numEmitters = ps->getNumEmitters();
		for (unsigned short i = 0; i < numEmitters; i++)
		{
			Ogre::ParticleEmitter *pe = ps->getEmitter(i);
			pe->setEnabled(enable);
			pe->setStartTime(delay);
		}

		ps->setSpeedFactor(enable ? 1.0 : 0.0);
	}
	// Kill the particle system and scene node associated with this geometry
	void killParticleSystem(OgreOde::SphereGeometry* geom)
	{
		// Find the body
		OgreOde::Body* body = geom->getBody();
		if (body)
		{
			// Find the scene node
			SceneNode* node = static_cast<SceneNode*> (body->getParentNode());
			if (node)
			{
				MovableObject* obj = 0;
				for (int i = 0; i < node->numAttachedObjects(); i++)
				{
					// If this object isn't an OgreOde::Body then it must be
					// (in our demo) a particle system that we need to delete
					MovableObject* can_obj = node->getAttachedObject(i);
					if (can_obj->getMovableType() != "OgreOde::Body")
						obj = can_obj;
				}

				// Delete the node
				_mgr->getRootSceneNode()->removeAndDestroyChild(node->getName());

				// If we found a particle system then delete it
				if (obj)
					_mgr->destroyParticleSystem(obj->getName());
			}
		}
	}

protected:
	// Things we'll control and delete manually
	Entity *_apache_body, *_main_rotor, *_tail_rotor;
	SceneNode *_apache_body_node, *_main_rotor_node, *_tail_rotor_node;

	Real _rotate_scale;

	// Physical things we'll need to apply forces to and delete manually
	OgreOde::Body *_apache, *_rear_wheel, *_left_front, *_right_front;
	OgreOde::SimpleSpace* _apache_space;

	// The worst helicopter flight model in the world!
	Real _main_spd, _main_response, _main_scale, _main_min, _main_max;
	Real _tail_spd, _tail_response, _tail_scale, _tail_min, _tail_max;
	Real _thrust, _linear_damp, _angular_damp;
	Vector3 _thrust_offs, _thrust_force;

	// Stuff to help us shoot things
	Real _fire_rate, _fire_time, _fire_force, _fire_mass, _fire_size;
	std::map<OgreOde::SphereGeometry*, Real> _rocket_list;
	int _rockets;

	SceneNode * _rocket_node_explosion;
	ParticleSystem * _rocketParticles_explosion;
	ParticleSystem * _rocketParticles_smoke_white;
	ParticleSystem * _rocketParticles_smoke;
};

/*
 The joints test extends the base test class
 */
class SimpleScenes_Joints: public SimpleScenes
{
public:

	// Create the scene, just a load of differently hinged doors
	SimpleScenes_Joints(OgreOde::World *world) :
		SimpleScenes(world)
	{
		left = right = up = false;

		// Create the player's ball
		Entity* entity = _mgr->createEntity("Ball", "ball.mesh");
		//		entity->setNormaliseNormals(true);
		entity->setCastShadows(true);

		SceneNode* node = _mgr->getRootSceneNode()->createChildSceneNode(
				entity->getName());

		node->attachObject(entity);
		node->setScale(0.2, 0.2, 0.2);
		node->setPosition(0, 1.2, 0);

		OgreOde::EntityInformer ei(entity, Matrix4::getScale(node->getScale()));
		ball_body = ei.createSingleDynamicSphere(5.0, _world, _space);
		_last_node = static_cast<SceneNode*> (ball_body->getParentNode());
		_bodies.push_back(ball_body);
		_geoms.push_back(ball_body->getGeometry(0));

		// Make the ball stay in the X/Y plane
#ifdef OGREODE_PLANARJOINT
		OgreOde::PlanarJoint* joint2d = new OgreOde::PlanarJoint();
		joint2d->attach(ball_body);
		_joints.push_back(joint2d);
#endif

		// Create the free standing door
		entity = _mgr->createEntity("Free_Door", "door.mesh");
		//		entity->setNormaliseNormals(true);
		entity->setCastShadows(true);

		node
				= _mgr->getRootSceneNode()->createChildSceneNode(
						entity->getName());

		node->attachObject(entity);
		node->setPosition(15, 4, 0);
		node->setOrientation(Quaternion(Degree(90), Vector3(0, 1, 0)));
		node->setScale(2, 2, 2);

		OgreOde::EntityInformer
				ei1(entity, Matrix4::getScale(node->getScale()));
		OgreOde::Body* body = ei1.createSingleDynamicBox(20.0, _world, _space);
		_bodies.push_back(body);
		_geoms.push_back(body->getGeometry(0));

		// Create a door hinged at the left hand side
		entity = _mgr->createEntity("Left_Door", "door.mesh");
		//		entity->setNormaliseNormals(true);
		entity->setCastShadows(true);

		node
				= _mgr->getRootSceneNode()->createChildSceneNode(
						entity->getName());

		node->attachObject(entity);
		node->setPosition(-10, 4.01, 1.5);
		node->setOrientation(Quaternion(Degree(-90), Vector3(0, 1, 0)));
		node->setScale(2, 2, 2);

		OgreOde::EntityInformer
				ei2(entity, Matrix4::getScale(node->getScale()));
		body = ei2.createSingleDynamicBox(20.0, _world, _space);
		body->setDamping(0, 20);

		OgreOde::HingeJoint* joint = new OgreOde::HingeJoint(_world);
		joint->attach(body);
		joint->setAxis(Vector3::UNIT_Y);
		joint->setAnchor(Vector3(-10, 4.01, 3.5));

		_bodies.push_back(body);
		_geoms.push_back(body->getGeometry(0));
		_joints.push_back(joint);

		// Create a door hinged at the right hand side
		entity = _mgr->createEntity("Right_Door", "door.mesh");
		//		entity->setNormaliseNormals(true);
		entity->setCastShadows(true);

		node
				= _mgr->getRootSceneNode()->createChildSceneNode(
						entity->getName());

		node->attachObject(entity);
		node->setPosition(-15, 4.01, -1.5);
		node->setOrientation(Quaternion(Degree(90), Vector3(0, 1, 0)));
		node->setScale(2, 2, 2);

		OgreOde::EntityInformer
				ei3(entity, Matrix4::getScale(node->getScale()));
		body = ei3.createSingleDynamicBox(20.0, _world, _space);
		body->setDamping(0, 20);

		joint = new OgreOde::HingeJoint(_world);
		joint->attach(body);
		joint->setAxis(Vector3::UNIT_Y);
		joint->setAnchor(Vector3(-15, 4.01, -3.5));

		_bodies.push_back(body);
		_geoms.push_back(body->getGeometry(0));
		_joints.push_back(joint);

		// Create a door hinged at the top
		entity = _mgr->createEntity("Top_Door", "door.mesh");
		//		entity->setNormaliseNormals(true);
		entity->setCastShadows(true);

		node
				= _mgr->getRootSceneNode()->createChildSceneNode(
						entity->getName());

		node->attachObject(entity);
		node->setPosition(10, 4.1, 0);
		node->setOrientation(Quaternion(Degree(90), Vector3(0, 1, 0)));
		node->setScale(2, 2, 2);

		OgreOde::EntityInformer
				ei4(entity, Matrix4::getScale(node->getScale()));
		body = ei4.createSingleDynamicBox(20.0, _world, _space);
		body->setDamping(0, 20);

		joint = new OgreOde::HingeJoint(_world);
		joint->attach(body);
		joint->setAxis(Vector3::UNIT_Z);
		joint->setAnchor(Vector3(10, 8.1, 0));

		_bodies.push_back(body);
		_geoms.push_back(body->getGeometry(0));
		_joints.push_back(joint);
	}

	// Standard destructor
	virtual ~SimpleScenes_Joints()
	{
	}

	// Return our name for the test application to display
	virtual const String& getName()
	{
		static String name = "Test Joints";
		return name;
	}

	// Return a description of the keys that the user can use in this test
	virtual const String& getKeys()
	{
		static String keys = "I - Bounce, J - Left, L - Right";
		return keys;
	}

	// Gets called every time the world is stepped, so we can apply forces every time step
	virtual void addForcesAndTorques()
	{
		// Apply a torque about the Z axis to roll the ball left
		if (left)
		{
			ball_body->wake();
			ball_body->addTorque(Vector3(0, 0, 50));
		}

		// Apply a torque about the Z axis to roll the ball right
		if (right)
		{
			ball_body->wake();
			ball_body->addTorque(Vector3(0, 0, -50));
		}

		// Apply an upwards force, or at least opposite to gravity
		if (up)
		{
			ball_body->wake();
			ball_body->addForce(_world->getGravity() * -10.0);
		}
	}

#if (OGRE_VERSION_MINOR < 4)
	// Handle the user's key presses
	virtual void frameEnded(Real time,InputReader* input)
	{
		// Do default key handling
		SimpleScenes::frameEnded(time,input);
#else

	virtual void frameEnded(Real time, OIS::Keyboard* input, OIS::Mouse* mouse)
	{
		// Do default processing
		SimpleScenes::frameEnded(time, input, mouse);
#endif

		// Set the key flags so we can apply forces at the right time
		left = input->isKeyDown(OIS::KC_J);
		right = input->isKeyDown(OIS::KC_L);
		up = input->isKeyDown(OIS::KC_I);
	}

	// Set up the collision properties
	virtual bool collision(OgreOde::Contact* contact)
	{
		contact->setBouncyness(0.8);
		contact->setCoulombFriction(OgreOde::Utility::Infinity);
		contact->setFrictionMode(OgreOde::Contact::Flag_FrictionPyramid);

		return true;
	}

protected:
	OgreOde::Body *ball_body;
	bool left, right, up;
};

static const String xmlZombieNames[] =
{ "zombie", "zombie", "ninja", "robot", "GingerBreadMan" };
static const String meshZombieNames[] =
{ "zombie_small.mesh", "zombie_small.mesh", "ninja.mesh", "robot.mesh",
		"GingerBreadMan.mesh" };
static const String meshZombieAnimation[] =
{ "Walk1", "Walk1", "Walk", "Walk", "Walk1" };
static const Ogre::Vector3 meshScale1[] =
{ Ogre::Vector3(1.0, 1.0, 1.0), Ogre::Vector3(1.0, 1.0, 1.0), Ogre::Vector3(
		1.0, 1.0, 1.0), Ogre::Vector3(1.0, 1.0, 1.0), Ogre::Vector3(1.0, 1.0,
		1.0), };
static const String ragdollFile1[] =
{ "zombie.ogreode", "zombie_auto.ogreode", "ninja.ogreode", "robot.ogreode",
		"gingerbreadman.ogreode" };
static size_t sSelectedMesh1 = 2;

#ifdef _DEBUG
static size_t sMaxRagdoll = 10;
#else //_DEBUG
static size_t sMaxRagdoll = 2;
#endif //_DEBUG
/*
 The zombie demo extends the base test class
 */
class SimpleScenes_Zombie: public SimpleScenes
{
private:
	typedef std::list<OgreOde_Prefab::Ragdoll *> RagDollList;

	struct ragDollData
	{
		size_t sSelectedMesh1;
		Ogre::Real timeSinceBirth;
		Ogre::Real timeSincePhysical;
	};

public:
	void createRagDoll()
	{

		_last_ragdoll = 0.0;

		OgreOde_Prefab::Ragdoll
				*ragdoll =
						static_cast<OgreOde_Prefab::Ragdoll *> (dotOgreOdeLoader->loadObject(
								ragdollFile1[sSelectedMesh1],
								xmlZombieNames[sSelectedMesh1], "zombie"
										+ StringConverter::toString(
												_ragdoll_count++)));

		ragdoll->setCastShadows(true);
		ragdoll->setQueryFlags(ZOMBIE_QUERY_MASK);

		Any a = ragdoll->getUserAny();
		assert (a.isEmpty ());
		{
			ragDollData *b = new ragDollData();

			b->timeSinceBirth = 0.0f;
			b->timeSincePhysical = 0.0f;
			b->sSelectedMesh1 = sSelectedMesh1;

			Any *newA = new Any(b);
			ragdoll->setUserAny(*newA);
		}

		// Add entity to the scene node
		SceneNode *ragdoll_node =
				_mgr->getRootSceneNode()->createChildSceneNode(
						ragdoll->getName() + "Node");
		ragdoll_node->attachObject(ragdoll);
		ragdoll_node->yaw(Degree(rand() % 360));
		ragdoll_node->setPosition(0.0, 0.0, 0);
		ragdoll_node->setScale(meshScale1[sSelectedMesh1]);

		_last_node = ragdoll_node;

		ragdoll->getAnimationState(meshZombieAnimation[sSelectedMesh1])->setEnabled(
				true);

		myRagDolls.push_back(ragdoll);

		sSelectedMesh1 = (sSelectedMesh1 + 1) % 4;
	}

	// Constructor
	SimpleScenes_Zombie(OgreOde::World *world) :
		SimpleScenes(world)
	{
		_over = (Overlay*) OverlayManager::getSingleton().getByName(
				"OgreOdeDemos/Target");
		_over->show();

		_gun = _mgr->createEntity("gun", "gun.mesh");
		_gun->setCastShadows(false);

		_gun_node = _mgr->getRootSceneNode()->createChildSceneNode("gun");
		_gun_node->attachObject(_gun);

		_camera = _mgr->getCamera("PlayerCam");

		_animation_speed = 1.0;
		_ragdoll_count = 0;
		createRagDoll();

		_shot_time = 0.0;

		myOdeRay = new OgreOde::RayGeometry(1000.0, _world, _space);
		mRayQuery = _mgr->createRayQuery(Ray());
		mRayQuery->setQueryMask(ZOMBIE_QUERY_MASK);
		mRayQuery->setQueryTypeMask(SceneManager::ENTITY_TYPE_MASK);
	}

	// Destructor
	virtual ~SimpleScenes_Zombie()
	{
		for (RagDollList::iterator zE = myRagDolls.begin(); zE
				!= myRagDolls.end(); ++zE)
		{
			assert ((*zE)->getParentNode ());
			assert ((*zE)->getParentNode ()->getParent());
			(static_cast<SceneNode*> ((*zE)->getParentNode()->getParent()))->removeAndDestroyChild(
					(*zE)->getParentNode()->getName());
			_mgr->destroyMovableObject((*zE)->getName(),
					OgreOde_Prefab::RagdollFactory::FACTORY_TYPE_NAME);

		}
		delete myOdeRay;

		_over->hide();
		_mgr->destroySceneNode("gun");
		_mgr->destroyEntity("gun");
		_mgr->destroyQuery(mRayQuery);
	}

	// Return our name for the test application to display
	virtual const String& getName()
	{
		static String name = "RagDoll Shooting Gallery";
		return name;
	}

	// Return a description of the keys that the user can use in this test
	virtual const String& getKeys()
	{
		static String keys = "X - Shoot";
		return keys;
	}

#if (OGRE_VERSION_MINOR < 4)
	virtual void frameStarted(Real time, InputReader* input)
#else
	virtual void frameStarted(Real time, OIS::Keyboard* input,
			OIS::Mouse* mouse)
#endif //OGRE_VERSION not heihort
	{
		_last_ragdoll += time;

		// Do default key handling
#if (OGRE_VERSION_MINOR < 4)
		SimpleScenes::frameStarted(time, input);
#else
		SimpleScenes::frameStarted(time, input, mouse);
#endif //OGRE_VERSION not heihort
		_gun_node->setOrientation(_camera->getOrientation());
		_gun_node->setPosition(_camera->getPosition()
				+ (_camera->getOrientation() * Ogre::Vector3(0.3, -0.15, -1.1)));

		for (RagDollList::iterator zE = myRagDolls.begin(); zE
				!= myRagDolls.end(); ++zE)
		{
			ragDollData *b = any_cast<ragDollData *> ((*zE)->getUserAny());
			if ((*zE)->isPhysical())
			{
				if (!(*zE)->isStatic())
				{

					b->timeSincePhysical += time;
					(*zE)->update();// RagDoll Activated
				}
			}
			else
			{
				(*zE)->getAnimationState(meshZombieAnimation[b->sSelectedMesh1])->addTime(
						time * _animation_speed);

				(*zE)->getParentNode()->translate(
						(*zE)->getParentNode()->getOrientation()
								* (Vector3::UNIT_Z * time * 2.5));

			}
		}

	}

#if (OGRE_VERSION_MINOR < 4)
	// Handle the user's key presses
	virtual void frameEnded(Real time,InputReader* input)
	{
		// Do default key handling
		SimpleScenes::frameEnded(time,input);
#else

	virtual void frameEnded(Real time, OIS::Keyboard* input, OIS::Mouse* mouse)
	{
		// Do default processing
		SimpleScenes::frameEnded(time, input, mouse);
#endif
		_shot_time -= time;

		if (_key_delay > 0 && input->isKeyDown(OIS::KC_C))
		{
			for (RagDollList::iterator zE = myRagDolls.begin(); zE
					!= myRagDolls.end(); ++zE)
			{
				if ((*zE)->isPhysical())
				{
					(*zE)->releasePhysicalControl();
					ragDollData *b = any_cast<ragDollData *> (
							(*zE)->getUserAny());
					(*zE)->getAnimationState(
							meshZombieAnimation[b->sSelectedMesh1])->setEnabled(
							true);
				}
			}

			_key_delay = SimpleScenes::KEY_DELAY;
		}

		if (_shot_time <= 0.0 &&
#if (OGRE_VERSION_MINOR < 4)
				(input->isKeyDown(KC_X) || input->getMouseButton (0)))
#else
				(input->isKeyDown(OIS::KC_X)
						|| mouse->getMouseState().buttonDown(OIS::MB_Left)))
#endif
		{

			Ray pickRay = _camera->getCameraToViewportRay(0.5, 0.5);
			myOdeRay->setDefinition(pickRay.getOrigin(), pickRay.getDirection());

			_shot_time = 0.2;

			mRayQuery->setRay(pickRay);
			const RaySceneQueryResult& result = mRayQuery->execute();
			if (!result.empty())
			{
				RaySceneQueryResult::const_iterator i = result.begin();

				//mRayQuery->setSortByDistance (true, 1);//only one hit
				myOdeRay->enable();
				while ((i != result.end()))
				{

					_last_ragdoll = 5.0;

					OgreOde_Prefab::Ragdoll * const radgoll =
							static_cast<OgreOde_Prefab::Ragdoll *> (i->movable);

					// uncomment to see nice debug mesh of animated mesh
					//#define _DEBUG_ZOMBIE_TRIMESH
#ifdef _DEBUG_ZOMBIE_TRIMESH
					// ray cast could be tested against that instead of ragdoll.
					// depending on complexity of mesh could be simpler
					OgreOde::EntityInformer ei(radgoll, radgoll->getParentNode ()->_getFullTransform());
					_geoms.push_back(ei.createStaticTriangleMesh(_world, _space));
#else //_DEBUG_ZOMBIE_TRIMESH
					bool wasPhysical = radgoll->isPhysical();
					if (!wasPhysical)
					{
						ragDollData * const b = any_cast<ragDollData *> (
								radgoll->getUserAny());
						radgoll->getAnimationState(
								meshZombieAnimation[b->sSelectedMesh1])->setEnabled(
								false);

						// Create the ragdoll
						radgoll->takePhysicalControl(_world, _space, false);
						radgoll->setSelfCollisions(true);

					}

					OgreOde::Body *hit_body;
					Ogre::Vector3 hit_point;

					bool is_hit = false;
					if (radgoll->pickRagdoll(myOdeRay, hit_body, hit_point))
					{
						if (hit_body)
						{
							hit_body->addForceAt(pickRay.getDirection()
									* 250000, hit_point);
							is_hit = true;
							ragDollData *b = any_cast<ragDollData *> (
									radgoll->getUserAny());
							b->timeSincePhysical = 0.0f;
						}
					}

					if (!is_hit && !wasPhysical)
					{
						radgoll->releasePhysicalControl();
						ragDollData *b = any_cast<ragDollData *> (
								radgoll->getUserAny());
						radgoll->getAnimationState(
								meshZombieAnimation[b->sSelectedMesh1])->setEnabled(
								true);
					}
#endif //_TESTTRIMESH
					++i;
				} // if results.
			}
			myOdeRay->disable();
		}

		for (RagDollList::iterator zE = myRagDolls.begin(); zE
				!= myRagDolls.end();)
		{

			OgreOde_Prefab::Ragdoll * const radgoll = (*zE);
			ragDollData *b = any_cast<ragDollData *> (radgoll->getUserAny());
			b->timeSinceBirth += time;

			// turn to stone to improve fps,
			// better way to do that would be to tweak
			// simulation parameters to be less jitty.
			// better auto sleep
			if (radgoll->isPhysical() && !radgoll->isStatic()
					&& radgoll->isAwake() && b->timeSincePhysical > 5.0)
			{
				//radgoll->turnToStone();
				radgoll->sleep();
			}

			if (b->timeSinceBirth > sMaxRagdoll)
			{
				(static_cast<SceneNode*> (radgoll->getParentNode()->getParent()))->removeAndDestroyChild(
						radgoll->getParentNode()->getName());
				_mgr->destroyMovableObject(radgoll->getName(),
						OgreOde_Prefab::RagdollFactory::FACTORY_TYPE_NAME);
				zE = myRagDolls.erase(zE);
			}
			else
			{
				++zE;
			}
		}
		if (_last_ragdoll > 5.0 && myRagDolls.size()
				< static_cast<size_t> (sMaxRagdoll))
			createRagDoll();

	}

protected:
	RagDollList myRagDolls;
	RaySceneQuery *mRayQuery;

	Real _animation_speed;

	Entity *_gun;
	SceneNode *_gun_node;

	OgreOde::RayGeometry *myOdeRay;

	Camera* _camera;
	Overlay* _over;

	Real _shot_time;

	Real _last_ragdoll;
	int _ragdoll_count;
};

#endif /* EXAMPLE_H_ */
