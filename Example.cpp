/*
 * Example.cpp
 *
 *  Created on: Mar 25, 2011
 *      Author: marco
 */

#include "Example.h"

Example::Example()
{
	FrameListener = NULL;
}

Example::~Example()
{
	if (FrameListener)
	{
		delete FrameListener;
	}
}

void Example::createScene()
{
	Ogre::Plane plane(Vector3::UNIT_Y, -10);
	Ogre::MeshManager::getSingleton().createPlane("plane",
			ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 1500,
			1500, 200, 200, true, 1, 5, 5, Vector3::UNIT_Z);
	Ogre::Entity* ent = mSceneMgr->createEntity("LightPlaneEntity", "plane");
	mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(ent);
	ent->setMaterialName("Examples/BeachStones");

	mSceneMgr->setShadowTechnique(SHADOWTYPE_STENCIL_ADDITIVE);
	//	mSceneMgr->setShadowTechnique(SHADOWTYPE_STENCIL_MODULATIVE);

	Ogre::Light* light = mSceneMgr->createLight("Light1");
	light->setType(Ogre::Light::LT_DIRECTIONAL);
	light->setDirection(Ogre::Vector3(1, -1, 0));

	Ogre::SceneNode* node = mSceneMgr->createSceneNode("Node1");
	mSceneMgr->getRootSceneNode()->addChild(node);
	_SinbadEnt = mSceneMgr->createEntity("Sinbad", "Sinbad.mesh");
	_SinbadNode = node->createChildSceneNode("SinbadNode");
	_SinbadNode->setScale(3.0f, 3.0f, 3.0f);
	_SinbadNode->setPosition(Ogre::Vector3(0.0f, 4.8f, 0.0f));
	_SinbadNode->attachObject(_SinbadEnt);

	Ogre::Entity* sword1 = mSceneMgr->createEntity("Sword1", "Sword.mesh");
	Ogre::Entity* sword2 = mSceneMgr->createEntity("Sword2", "Sword.mesh");
	_SinbadEnt->attachObjectToBone("Handle.L", sword1);
	_SinbadEnt->attachObjectToBone("Handle.R", sword2);

	Ogre::AnimationStateSet* set = _SinbadEnt->getAllAnimationStates();
	Ogre::AnimationStateIterator iter = set->getAnimationStateIterator();
	while (iter.hasMoreElements())
	{
		std::cout << iter.getNext()->getAnimationName() << std::endl;
	}

}

void Example::createCamera()
{
	mCamera = mSceneMgr->createCamera("MyCamera1");
	mCamera->setPosition(0, 100, 200);
	mCamera->lookAt(0, 0, 0);
	mCamera->setNearClipDistance(5);
}

void Example::createFrameListener(void)
{
	FrameListener = new Example4FrameListener(_SinbadNode, _SinbadEnt, mWindow,
			mCamera);
	mRoot->addFrameListener(FrameListener);
}

/*----------------------------------------------------*/

Example4FrameListener::Example4FrameListener(Ogre::SceneNode *node,
		Ogre::Entity* ent, RenderWindow* win, Ogre::Camera* cam) :
	_PolyMode(Ogre::PM_SOLID), _speed(40.0)
{
	_node = node;

	_ent = ent;
	//	_aniState = _ent->getAnimationState("Dance");
	_aniState = _ent->getAnimationState("RunBase");
	//	_aniState->setEnabled(true);
	_aniState->setLoop(false);
	_aniStateTop = _ent->getAnimationState("RunTop");
	//	_aniStateTop->setEnabled(true);
	_aniStateTop->setLoop(false);
	_WalkingSpeed = 50.0f;
	_rotation = 0.0f;

	size_t windowHnd = 0;
	std::stringstream windowHndStr;
	win->getCustomAttribute("WINDOW", &windowHnd);
	windowHndStr << windowHnd;
	OIS::ParamList pl;
	pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));
	_man = OIS::InputManager::createInputSystem(pl);
	_key = static_cast<OIS::Keyboard*> (_man->createInputObject(
			OIS::OISKeyboard, false));
	_mouse = static_cast<OIS::Mouse*> (_man->createInputObject(OIS::OISMouse,
			false));

	_Cam = cam;

	_timer.reset();
}

Example4FrameListener::~Example4FrameListener()
{
	_man->destroyInputObject(_mouse);
	_man->destroyInputObject(_key);
	OIS::InputManager::destroyInputSystem(_man);
}

bool Example4FrameListener::frameStarted(const FrameEvent & evt)
{
	//<Part 1
	//	_node->translate(Ogre::Vector3(0.1, 0, 0));
	//Part 1>
	//<Part 2 - Modifying the code to be time based rather than frame based
	//	_node->translate(Ogre::Vector3(10, 0, 0) * evt.timeSinceLastFrame);
	//Part 2>

	Ogre::Vector3 direction(0, 0, 0);

	_key->capture();
	_mouse->capture();

	if (_key->isKeyDown(OIS::KC_ESCAPE))
	{
		return false;
	}
	if (_key->isKeyDown(OIS::KC_W))
	{
		direction = Ogre::Vector3(0, 0, -1);
	}
	if (_key->isKeyDown(OIS::KC_S))
	{
		direction = Ogre::Vector3(0, 0, 1);
	}
	if (_key->isKeyDown(OIS::KC_A))
	{
		direction = Ogre::Vector3(-1, 0, 0);
	}
	if (_key->isKeyDown(OIS::KC_D))
	{
		direction = Ogre::Vector3(1, 0, 0);
	}
	if (_key->isKeyDown(OIS::KC_R) && _timer.getMilliseconds() > 250)
	{
		if (_PolyMode == PM_SOLID)
		{
			_PolyMode = Ogre::PM_WIREFRAME;
		}
		else if (_PolyMode == PM_WIREFRAME)
		{
			_PolyMode = Ogre::PM_POINTS;
		}
		else if (_PolyMode == PM_POINTS)
		{
			_PolyMode = Ogre::PM_SOLID;
		}
		_Cam->setPolygonMode(_PolyMode);
		_timer.reset();
	}

	// <Part 3 - Adding movement to the model
	//	_node->translate(speed * direction * evt.timeSinceLastFrame);
	// Part 3>
	float rotX = _mouse->getMouseState().X.rel * evt.timeSinceLastFrame * -1;
	float rotY = _mouse->getMouseState().Y.rel * evt.timeSinceLastFrame * -1;
	_Cam->yaw(Ogre::Radian(rotX));
	_Cam->pitch(Ogre::Radian(rotY));
	_Cam->moveRelative(direction * evt.timeSinceLastFrame * _speed);

	bool walked = false;
	Ogre::Vector3 SinbadTranslate(0, 0, 0);
	if (_key->isKeyDown(OIS::KC_UP))
	{
		SinbadTranslate += Ogre::Vector3(0, 0, -1);
		_rotation = 3.14f;
		walked = true;
	}
	if (_key->isKeyDown(OIS::KC_DOWN))
	{
		SinbadTranslate += Ogre::Vector3(0, 0, 1);
		_rotation = 0.0f;
		walked = true;
	}
	if (_key->isKeyDown(OIS::KC_LEFT))
	{
		SinbadTranslate += Ogre::Vector3(-1, 0, 0);
		_rotation = -1.57f;
		walked = true;
	}
	if (_key->isKeyDown(OIS::KC_RIGHT))
	{
		SinbadTranslate += Ogre::Vector3(1, 0, 0);
		_rotation = 1.57f;
		walked = true;
	}

	if (walked)
	{
		_aniState->setEnabled(true);
		_aniStateTop->setEnabled(true);
		if (_aniState->hasEnded())
		{
			_aniState->setTimePosition(0.0f);
		}
		if (_aniStateTop->hasEnded())
		{
			_aniStateTop->setTimePosition(0.0f);
		}
		_aniState->addTime(evt.timeSinceLastFrame);
		_aniStateTop->addTime(evt.timeSinceLastFrame);
	}
	else
	{
		_aniState->setTimePosition(0.0f);
		_aniState->setEnabled(false);
		_aniStateTop->setTimePosition(0.0f);
		_aniStateTop->setEnabled(false);
	}
	_node->translate(SinbadTranslate * evt.timeSinceLastFrame * _WalkingSpeed);
	_node->resetOrientation();
	_node->yaw(Ogre::Radian(_rotation));

	//	_aniState->addTime(evt.timeSinceLastFrame);
	//	_aniStateTop->addTime(evt.timeSinceLastFrame);

	return true;
}

