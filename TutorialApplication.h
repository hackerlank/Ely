/*
 -----------------------------------------------------------------------------
 Filename:    TutorialApplication.h
 -----------------------------------------------------------------------------

 This source file is part of the
 ___                 __    __ _ _    _
 /___\__ _ _ __ ___  / / /\ \ (_) | _(_)
 //  // _` | '__/ _ \ \ \/  \/ / | |/ / |
 / \_// (_| | | |  __/  \  /\  /| |   <| |
 \___/ \__, |_|  \___|   \/  \/ |_|_|\_\_|
 |___/
 Tutorial Framework
 http://www.ogre3d.org/tikiwiki/
 -----------------------------------------------------------------------------
 */
#ifndef __TutorialApplication_h_
#define __TutorialApplication_h_

#include "BaseApplication.h"
#include <deque>

class TutorialApplication: public BaseApplication
{
public:
	TutorialApplication(void);
	virtual ~TutorialApplication(void);

protected:
	virtual void createScene(void);
	virtual void createFrameListener(void);
	virtual bool nextLocation(void);
	virtual bool frameRenderingQueued(const Ogre::FrameEvent &evt);

	Ogre::Real mDistance; // The distance the object has left to travel
	Ogre::Vector3 mDirection; // The direction the object is moving
	bool mWalking; // Whether or not the robot is moving
	Ogre::Vector3 mDestination; // The destination the object is moving towards

	Ogre::AnimationState *mAnimationState; // The current animation state of the object

	Ogre::Entity *mEntity; // The Entity we are animating
	Ogre::SceneNode *mNode; // The SceneNode that the Entity is attached to
	std::deque<Ogre::Vector3> mWalkList; // The list of points we are walking to

	static Ogre::Real mWalkSpeed; // The speed at which the object is moving

};

#endif // #ifndef __TutorialApplication_h_
