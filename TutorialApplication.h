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

class TutorialApplication: public BaseApplication
{
public:
	TutorialApplication(void);
	virtual ~TutorialApplication(void);

protected:
	virtual void createScene(void);
	virtual void chooseSceneManager(void);
	virtual void createFrameListener(void);
	//frame listener
	virtual bool frameRenderingQueued(const Ogre::FrameEvent &evt);
	//mouse listener
	virtual bool mouseMoved(const OIS::MouseEvent &arg);
	virtual bool
			mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id);
	virtual bool mouseReleased(const OIS::MouseEvent &arg,
			OIS::MouseButtonID id);
protected:
	Ogre::RaySceneQuery *mRaySceneQuery;// The ray scene query pointer
	bool mLMouseDown, mRMouseDown; // True if the mouse buttons are down
	int mCount; // The number of robots on the screen
	Ogre::SceneNode *mCurrentObject; // The newly created object
	CEGUI::Renderer *mGUIRenderer; // CEGUI renderer
	float mRotateSpeed;

};

#endif // #ifndef __TutorialApplication_h_
