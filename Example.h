/*
 * Example.h
 *
 *  Created on: Mar 25, 2011
 *      Author: marco
 */

#ifndef EXAMPLE_H_
#define EXAMPLE_H_

#include "OgreOde_Core.h"

#include "ExampleApplication.h"

class ExampleFL : public ExampleFrameListener
{
protected:
  OgreOde::World* mWorld;
  OgreOde::StepHandler* mStepper;

public:
    ExampleFL(RenderWindow* win, Camera* cam, OgreOde::World* _world,
OgreOde::StepHandler* _stepper)
        : ExampleFrameListener(win, cam), mWorld(_world), mStepper(_stepper)
    {
    }
   //bool frameStarted(const FrameEvent& evt)
   bool frameEnded(const FrameEvent& evt)
   {
      Real time = evt.timeSinceLastFrame;
      if ( mStepper->step(time) )
            mWorld->synchronise();
      return ExampleFrameListener::frameEnded(evt);
      //return ExampleFrameListener::frameStarted(evt);
   }
};


class Example: public ExampleApplication
{
public:
	Example();
	virtual ~Example();

	virtual void createScene();
	virtual void createFrameListener(void);

protected:

	OgreOde::World *mWorld;
	OgreOde::Space *mSpace;
	OgreOde::StepHandler *mStepper;

	OgreOde::InfinitePlaneGeometry *mGround;
	OgreOde::Body *mBody;
	OgreOde::Geometry *mGeom;
	Ogre::SceneNode *mNode;
	Ogre::Entity *mEntity;
};

#endif /* EXAMPLE_H_ */
