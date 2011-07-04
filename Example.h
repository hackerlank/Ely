/*
 * Example.h
 *
 *  Created on: Mar 25, 2011
 *      Author: marco
 */

#ifndef EXAMPLE_H_
#define EXAMPLE_H_

#include "OGRE/Ogre.h"
#include <OIS/OIS.h>

class MyFrameListener: public Ogre::FrameListener
{
public:
	MyFrameListener(Ogre::RenderWindow *win, Ogre::Camera* cam,
			Ogre::SceneNode* node);
	~MyFrameListener();

	bool frameStarted(const Ogre::FrameEvent& evt);
	bool frameEnded(const Ogre::FrameEvent& evt);
	bool frameRenderingQueued(const Ogre::FrameEvent& evt);
	void setNode(Ogre::SceneNode *node)
	{
		_node = node;
	}
	void setEntAnim(Ogre::Entity *ent)
	{
		_aniState = ent->getAnimationState("RunBase");
		_aniState->setLoop(false);
		_aniStateTop = ent->getAnimationState("RunTop");
		_aniStateTop->setLoop(false);
	}

private:
	OIS::InputManager* _InputManager;
	OIS::Keyboard* _Keyboard;
	OIS::Mouse* _Mouse;
	Ogre::Camera* _Cam;
	float _movementspeed;
	float _WalkingSpeed;
	Ogre::SceneNode* _node;
	float _rotation;
	Ogre::AnimationState* _aniState;
	Ogre::AnimationState* _aniStateTop;

};

class MyApplication
{
private:
	Ogre::SceneManager* _sceneManager;
	Ogre::Root* _root;
	MyFrameListener* _listener;
	bool _keepRunning;
	Ogre::SceneNode* _SinbadNode;

public:
	MyApplication();
	~MyApplication();

	void loadResources();
	int startup();
	void createScene();
	void renderOneFrame();
	inline bool keepRunning();

};

inline bool MyApplication::keepRunning()
{
	return _keepRunning;
}

#endif /* EXAMPLE_H_ */
