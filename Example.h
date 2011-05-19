/*
 * Example.h
 *
 *  Created on: Mar 25, 2011
 *      Author: marco
 */

#ifndef EXAMPLE_H_
#define EXAMPLE_H_

#include "ExampleApplication.h"

class CompositorListener1: public Ogre::CompositorInstance::Listener
{
public:
	CompositorListener1();

	virtual void notifyMaterialRender(uint32 pass_id, MaterialPtr &mat);
	void setNumber(float num)
	{
		number = num;
	}
	float getNumber()
	{
		return number;
	}

private:
	float number;

};

class Example8FrameListener: public Ogre::FrameListener
{
public:
	Example8FrameListener(RenderWindow* win, CompositorListener1* listener);
	~Example8FrameListener();

	virtual bool frameStarted(const Ogre::FrameEvent &evt);

private:
	OIS::InputManager* _man;
	OIS::Keyboard* _key;
	CompositorListener1* _listener;
};

class Example: public ExampleApplication
{
public:
	Example();
	~Example();

protected:

	virtual void createScene();
	virtual void createFrameListener();

private:
	CompositorListener1* compListener;
	Ogre::FrameListener* FrameListener;
};

#endif /* EXAMPLE_H_ */
