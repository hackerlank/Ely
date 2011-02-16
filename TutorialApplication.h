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

#include <CEGUISystem.h>
#include <CEGUISchemeManager.h>
#include <RendererModules/Ogre/CEGUIOgreRenderer.h>

class TutorialApplication: public BaseApplication
{
public:
	TutorialApplication(void);
	virtual ~TutorialApplication(void);

protected:
	virtual void createScene(void);

	virtual void chooseSceneManager(void);
	virtual void createFrameListener(void);

	virtual bool frameRenderingQueued(const Ogre::FrameEvent& arg);

	virtual bool mouseMoved(const OIS::MouseEvent& arg);
	virtual bool
			mousePressed(const OIS::MouseEvent& arg, OIS::MouseButtonID id);
	virtual bool mouseReleased(const OIS::MouseEvent& arg,
			OIS::MouseButtonID id);

	Ogre::SceneNode *mCurrentObject; //pointer to our currently selected object
	Ogre::RaySceneQuery* mRayScnQuery; //pointer to our ray scene query
	CEGUI::Renderer* mGUIRenderer; //our CEGUI renderer

	bool bLMouseDown, bRMouseDown; //true if mouse buttons are held down
	int mCount; //number of objects created
	float mRotateSpeed; //the rotation speed for the camera


};

#endif // #ifndef __TutorialApplication_h_
