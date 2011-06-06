/*
 * Example.h
 *
 *  Created on: Mar 25, 2011
 *      Author: marco
 */

#ifndef EXAMPLE_H_
#define EXAMPLE_H_

#include "Ogre.h"
#include "OIS.h"

class MyFrameListener: public Ogre::FrameListener
{
public:
	MyFrameListener(Ogre::RenderWindow *win);
	~MyFrameListener();

	bool frameStarted(const Ogre::FrameEvent& evt);
	bool frameEnded(const Ogre::FrameEvent& evt);
	bool frameRenderingQueued(const Ogre::FrameEvent& evt);

private:
	OIS::InputManager* _InputManager;
	OIS::Keyboard* _Keyboard;

};

class MyApplication
{
private:
	Ogre::SceneManager* _sceneManager;
	Ogre::Root* _root;
	MyFrameListener* _listener;

public:
	MyApplication();
	~MyApplication();

	void loadResources();
	int startup();
	void createScene();

};

#endif /* EXAMPLE_H_ */
