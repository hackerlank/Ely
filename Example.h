/*
 * Example.h
 *
 *  Created on: Mar 25, 2011
 *      Author: marco
 */

#ifndef EXAMPLE_H_
#define EXAMPLE_H_

#include <OGRE/Ogre.h>
#include <OIS/OIS.h>

class MyFrameListener: public Ogre::FrameListener
{
private:

	OIS::InputManager* _InputManager;
	OIS::Keyboard* _Keyboard;
	OIS::Mouse* _Mouse;
	Ogre::Camera* _Cam;
	Ogre::Viewport* _viewport;
	float _movementspeed;

	bool comp1, comp2, comp3;
	bool down1, down2, down3;

	float _WalkingSpeed;
	Ogre::SceneNode* _node;

	Ogre::AnimationState* _aniState;
	Ogre::AnimationState* _aniStateTop;
public:

	MyFrameListener(Ogre::RenderWindow* win, Ogre::Camera* cam,
			Ogre::Viewport* viewport, Ogre::SceneNode* node, Ogre::Entity* ent);

	virtual ~MyFrameListener();

	bool frameStarted(const Ogre::FrameEvent& evt);

};

class MyApplication
{
private:

	Ogre::SceneManager* _sceneManager;
	Ogre::Root* _root;

	Ogre::SceneNode* _SinbadNode;
	Ogre::Entity* _SinbadEnt;

	MyFrameListener* _listener;

	bool _keepRunning;
public:

	MyApplication();
	~MyApplication();

	void loadResources();
	int startup();
	void createScene();
	void renderOneFrame();
	bool keepRunning();
};


#endif /* EXAMPLE_H_ */
