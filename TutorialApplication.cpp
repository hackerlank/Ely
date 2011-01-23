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
#include "TutorialApplication.h"
#define CAMERA_NAME "SceneCamera"

//-------------------------------------------------------------------------------------
TutorialApplication::TutorialApplication(void) :
	mPrimarySceneMgr(0), mSecondarySceneMgr(0), mDual(false)
{
}
//-------------------------------------------------------------------------------------
TutorialApplication::~TutorialApplication(void)
{

}
//-------------------------------------------------------------------------------------
//Local Functions
void TutorialApplication::setupViewport(Ogre::SceneManager *curr)
{
	mWindow->removeAllViewports();

	Ogre::Camera *cam = curr->getCamera(CAMERA_NAME); //The Camera
	Ogre::Viewport *vp = mWindow->addViewport(cam); //Our Viewport linked to the camera

	vp->setBackgroundColour(Ogre::ColourValue(0, 0, 0));
	cam->setAspectRatio(Ogre::Real(vp->getActualWidth()) / Ogre::Real(
			vp->getActualHeight()));
}

void TutorialApplication::dualViewport(Ogre::SceneManager *primarySceneMgr,
		Ogre::SceneManager *secondarySceneMgr)
{
	mWindow->removeAllViewports();

	Ogre::Viewport *vp = 0;
	Ogre::Camera *cam = primarySceneMgr->getCamera(CAMERA_NAME);
	vp = mWindow->addViewport(cam, 0, 0, 0, 0.5, 1);
	vp->setBackgroundColour(Ogre::ColourValue(0, 0, 0));
	cam->setAspectRatio(Ogre::Real(vp->getActualWidth()) / Ogre::Real(
			vp->getActualHeight()));

	cam = secondarySceneMgr->getCamera(CAMERA_NAME);
	vp = mWindow->addViewport(cam, 1, 0.5, 0, 0.5, 1);
	vp->setBackgroundColour(Ogre::ColourValue(0, 0, 0));
	cam->setAspectRatio(Ogre::Real(vp->getActualWidth()) / Ogre::Real(
			vp->getActualHeight()));
}

static void swap(Ogre::SceneManager *&first, Ogre::SceneManager *&second)
{
	Ogre::SceneManager *tmp = first;
	first = second;
	second = tmp;
}

//-------------------------------------------------------------------------------------
void TutorialApplication::createScene(void)
{
	// Setup the space SceneManager
	mPrimarySceneMgr->setSkyBox(true, "Examples/SpaceSkyBox");
	// Setup the Cloudy SceneManager
	mSecondarySceneMgr->setSkyDome(true, "Examples/CloudySky", 5, 8);
}
//-------------------------------------------------------------------------------------
void TutorialApplication::chooseSceneManager(void)
{
	mPrimarySceneMgr = mRoot->createSceneManager(Ogre::ST_GENERIC, "primary");
	mSecondarySceneMgr = mRoot->createSceneManager(Ogre::ST_GENERIC,
			"secondary");
}

//-------------------------------------------------------------------------------------
void TutorialApplication::createCamera()
{
	mPrimarySceneMgr->createCamera(CAMERA_NAME);
	mSecondarySceneMgr->createCamera(CAMERA_NAME);
}

//-------------------------------------------------------------------------------------
void TutorialApplication::createViewports()
{
	setupViewport(mPrimarySceneMgr);
}

//-------------------------------------------------------------------------------------
void TutorialApplication::createFrameListener(void)
{
	Ogre::LogManager::getSingletonPtr()->logMessage("*** Initializing OIS ***");
	OIS::ParamList pl;
	size_t windowHnd = 0;
	std::ostringstream windowHndStr;

	mWindow->getCustomAttribute("WINDOW", &windowHnd);
	windowHndStr << windowHnd;
	pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));

	mInputManager = OIS::InputManager::createInputSystem(pl);

	mKeyboard = static_cast<OIS::Keyboard*> (mInputManager->createInputObject(
			OIS::OISKeyboard, true));
	mMouse = static_cast<OIS::Mouse*> (mInputManager->createInputObject(
			OIS::OISMouse, true));

	mMouse->setEventCallback(this);
	mKeyboard->setEventCallback(this);

	//Set initial mouse clipping size
	windowResized(mWindow);

	//Register as a Window listener
	Ogre::WindowEventUtilities::addWindowEventListener(mWindow, this);

	mRoot->addFrameListener(this);
}

//-------------------------------------------------------------------------------------
bool TutorialApplication::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
	if (mWindow->isClosed())
		return false;

	if (mShutDown)
		return false;

	//Need to capture/update each device
	mKeyboard->capture();
	mMouse->capture();

	return true;
}

//-------------------------------------------------------------------------------------
bool TutorialApplication::keyPressed(const OIS::KeyEvent &arg)
{
	if (arg.key == OIS::KC_ESCAPE)
	{
		mShutDown = true;
	}
	else if (arg.key == OIS::KC_V)
	{
		mDual = !mDual;

		if (mDual)
			dualViewport(mPrimarySceneMgr, mSecondarySceneMgr);
		else
			setupViewport(mPrimarySceneMgr);
	}
	else if (arg.key == OIS::KC_C)
	{
		swap(mPrimarySceneMgr, mSecondarySceneMgr);

		if (mDual)
			dualViewport(mPrimarySceneMgr, mSecondarySceneMgr);
		else
			setupViewport(mPrimarySceneMgr);
	}

	return true;
}

//-------------------------------------------------------------------------------------
bool TutorialApplication::keyReleased(const OIS::KeyEvent &arg)
{
	return true;
}

//-------------------------------------------------------------------------------------
bool TutorialApplication::mouseMoved(const OIS::MouseEvent &arg)
{
	return true;
}

bool TutorialApplication::mousePressed(const OIS::MouseEvent &arg,
		OIS::MouseButtonID id)
{
	return true;
}

//-------------------------------------------------------------------------------------
bool TutorialApplication::mouseReleased(const OIS::MouseEvent &arg,
		OIS::MouseButtonID id)
{
	return true;
}
//-------------------------------------------------------------------------------------

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif

#ifdef __cplusplus
extern "C"
{
#endif

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT)
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
		MessageBox(NULL, e.getFullDescription().c_str(),
				"An exception has occured!", MB_OK | MB_ICONERROR
				| MB_TASKMODAL);
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
