/*
 -----------------------------------------------------------------------------
 Filename:    TutorialApplication.cpp
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
#include <CEGUISystem.h>
#include <CEGUISchemeManager.h>
#include <RendererModules/Ogre/CEGUIOgreRenderer.h>

#include "TutorialApplication.h"

//-------------------------------------------------------------------------------------
TutorialApplication::TutorialApplication(void)
{
}
//-------------------------------------------------------------------------------------
TutorialApplication::~TutorialApplication(void)
{
	// We created the query, and we are also responsible for deleting it.
	mSceneMgr->destroyQuery(mRaySceneQuery);
}

//-------------------------------------------------------------------------------------
void TutorialApplication::createScene(void)
{
	// Set ambient light
	mSceneMgr->setAmbientLight(Ogre::ColourValue(0.5, 0.5, 0.5));
	mSceneMgr->setSkyDome(true, "Examples/CloudySky", 5, 8);

	// World geometry
	mSceneMgr->setWorldGeometry("terrain.cfg");

	// Set camera look point
	mCamera->setPosition(40, 100, 580);
	mCamera->pitch(Ogre::Degree(-30));
	mCamera->yaw(Ogre::Degree(-45));

	// CEGUI setup
	mGUIRenderer = &CEGUI::OgreRenderer::bootstrapSystem();
	// Mouse
	CEGUI::SchemeManager::getSingleton().create(
			(CEGUI::utf8*) "TaharezLook.scheme");
	CEGUI::MouseCursor::getSingleton().setImage("TaharezLook", "MouseArrow");
}
void TutorialApplication::createFrameListener(void)
{
	BaseApplication::createFrameListener();
	// Setup default variables
	mCount = 0;
	mCurrentObject = NULL;
	mLMouseDown = false;
	mRMouseDown = false;

	// Reduce rotate speed
	mRotateSpeed = .1;

	// Create RaySceneQuery
	mRaySceneQuery = mSceneMgr->createRayQuery(Ogre::Ray());
}
void TutorialApplication::chooseSceneManager(void)
{
	// Use the terrain scene manager.
	mSceneMgr = mRoot->createSceneManager(Ogre::ST_EXTERIOR_CLOSE);
}

bool TutorialApplication::frameRenderingQueued(const Ogre::FrameEvent &evt)
{
	// Process the base frame listener code.  Since we are going to be
	// manipulating the translate vector, we need this to happen first.
	if (!BaseApplication::frameRenderingQueued(evt))
		return false;

	// Setup the scene query
	Ogre::Vector3 camPos = mCamera->getPosition();
	Ogre::Ray cameraRay(Ogre::Vector3(camPos.x, 5000.0f, camPos.z),
			Ogre::Vector3::NEGATIVE_UNIT_Y);
	mRaySceneQuery->setRay(cameraRay);

	// Perform the scene query
	Ogre::RaySceneQueryResult &result = mRaySceneQuery->execute();
	Ogre::RaySceneQueryResult::iterator itr = result.begin();
	// Get the results, set the camera height
	if (itr != result.end() && itr->worldFragment)
	{
		Ogre::Real terrainHeight = itr->worldFragment->singleIntersection.y;
		if ((terrainHeight + 10.0f) > camPos.y)
			mCamera->setPosition(camPos.x, terrainHeight + 10.0f, camPos.z);
	}

	return true;

}
bool TutorialApplication::mouseMoved(const OIS::MouseEvent &arg)
{
	// Update CEGUI with the mouse motion
	CEGUI::System::getSingleton().injectMouseMove(arg.state.X.rel,
			arg.state.Y.rel);

	// If we are dragging the left mouse button.
	if (mLMouseDown)
	{
		CEGUI::Point mousePos =
				CEGUI::MouseCursor::getSingleton().getPosition();
		Ogre::Ray mouseRay = mCamera->getCameraToViewportRay(mousePos.d_x
				/ float(arg.state.width), mousePos.d_y
				/ float(arg.state.height));
		mRaySceneQuery->setRay(mouseRay);

		Ogre::RaySceneQueryResult &result = mRaySceneQuery->execute();
		Ogre::RaySceneQueryResult::iterator itr = result.begin();

		if (itr != result.end() && itr->worldFragment)
			mCurrentObject->setPosition(itr->worldFragment->singleIntersection);
	} // if
	// If we are dragging the right mouse button.
	else if (mRMouseDown)
	{
		mCamera->yaw(Ogre::Degree(-arg.state.X.rel * mRotateSpeed));
		mCamera->pitch(Ogre::Degree(-arg.state.Y.rel * mRotateSpeed));
	} // else if
	return true;
}
bool TutorialApplication::mousePressed(const OIS::MouseEvent &arg,
		OIS::MouseButtonID id)
{
	// Left mouse button down
	if (id == OIS::MB_Left)
	{
		// Setup the ray scene query, use CEGUI's mouse position
		CEGUI::Point mousePos =
				CEGUI::MouseCursor::getSingleton().getPosition();
		Ogre::Ray mouseRay = mCamera->getCameraToViewportRay(mousePos.d_x
				/ float(arg.state.width), mousePos.d_y
				/ float(arg.state.height));
		mRaySceneQuery->setRay(mouseRay);
		// Execute query
		Ogre::RaySceneQueryResult &result = mRaySceneQuery->execute();
		Ogre::RaySceneQueryResult::iterator itr = result.begin();
		// Get results, create a node/entity on the position
		if (itr != result.end() && itr->worldFragment)
		{
			char name[16];
			sprintf(name, "Robot%d", mCount++);
			Ogre::Entity *ent = mSceneMgr->createEntity(name, "robot.mesh");
			mCurrentObject
					= mSceneMgr->getRootSceneNode()->createChildSceneNode(
							std::string(name) + "Node",
							itr->worldFragment->singleIntersection);
			mCurrentObject->attachObject(ent);
			mCurrentObject->setScale(0.1f, 0.1f, 0.1f);
		} // if

		mLMouseDown = true;
	} // if
	// Right mouse button down
	else if (id == OIS::MB_Right)
	{
		CEGUI::MouseCursor::getSingleton().hide();
		mRMouseDown = true;
	} // else if

	return true;
}
bool TutorialApplication::mouseReleased(const OIS::MouseEvent &arg,
		OIS::MouseButtonID id)
{
	// Left mouse button up
	if (id == OIS::MB_Left)
	{
		mLMouseDown = false;
	} // if
	// Right mouse button up
	else if (id == OIS::MB_Right)
	{
		CEGUI::MouseCursor::getSingleton().show();
		mRMouseDown = false;
	} // else if
	return true;
}

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif

#ifdef __cplusplus
extern "C"
{
#endif

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
#else
int main(int argc, char *argv[])
#endif
{
	// Create application object
	TutorialApplication app;

	try
	{
		app.go();
	} catch (Ogre::Exception& e)
	{
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
		MessageBox( NULL, e.getFullDescription().c_str(), "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
		std::cerr << "An exception has occured: "
				<< e.getFullDescription().c_str() << std::endl;
#endif
	}

	return 0;
}

#ifdef __cplusplus
}
#endif
