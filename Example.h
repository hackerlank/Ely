/*
 * Example.h
 *
 *  Created on: Mar 25, 2011
 *      Author: marco
 */

#ifndef EXAMPLE_H_
#define EXAMPLE_H_

#include "ExampleApplication.h"

class Example: public ExampleApplication
{
public:
	Example();
	virtual ~Example();

protected:
	virtual void createScene();
	virtual void createCamera();
	virtual void createFrameListener(void);

private:
	Ogre::SceneNode* _SinbadNode;
	Ogre::Entity* _SinbadEnt;
	Ogre::FrameListener* FrameListener;
};

class Example4FrameListener: public Ogre::FrameListener
{
public:
	Example4FrameListener(Ogre::SceneNode* node, Ogre::Entity* ent,
			RenderWindow* win, Ogre::Camera* cam);
	virtual ~Example4FrameListener();

	virtual bool frameStarted(const FrameEvent& evt);

private:
	Ogre::SceneNode* _node;
	Ogre::Camera* _Cam;
	Ogre::PolygonMode _PolyMode;
	Ogre::Timer _timer;

	Ogre::Entity* _ent;
	Ogre::AnimationState* _aniState;
	Ogre::AnimationState* _aniStateTop;
	float _WalkingSpeed;
	float _rotation;

	OIS::InputManager* _man;
	OIS::Keyboard* _key;
	OIS::Mouse* _mouse;

	float _speed;

};

#endif /* EXAMPLE_H_ */
