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
#include "SelectionBox.h"

#include <CEGUI.h>
#include <CEGUISchemeManager.h>
#include <RendererModules/Ogre/CEGUIOgreRenderer.h>

class TutorialApplication: public BaseApplication
{
public:
	enum QueryFlags
	{
		NINJA_MASK = 1 << 0, ROBOT_MASK = 1 << 1
	};
	TutorialApplication(void);
	virtual ~TutorialApplication(void);

protected:
	virtual void createScene(void);

	virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt);

	virtual bool mouseMoved(const OIS::MouseEvent& arg);
	virtual bool
			mousePressed(const OIS::MouseEvent& arg, OIS::MouseButtonID id);
	virtual bool mouseReleased(const OIS::MouseEvent& arg,
			OIS::MouseButtonID id);

	void performSelection(const Ogre::Vector2& first,
			const Ogre::Vector2& second);
	void deselectObjects();
	void selectObject(Ogre::MovableObject* obj);
	SelectionBox* mSelectionBox;

private:
	Ogre::Vector2 mStart, mStop;
	Ogre::PlaneBoundedVolumeListSceneQuery* mVolQuery;
	std::list<Ogre::MovableObject*> mSelected;
	bool mSelecting;
	CEGUI::OgreRenderer* mGUIRenderer;

	static void swap(float& x, float& y);

};

#endif // #ifndef __TutorialApplication_h_
