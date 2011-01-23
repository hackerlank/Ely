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

//-------------------------------------------------------------------------------------
TutorialApplication::TutorialApplication(void)
{
}
//-------------------------------------------------------------------------------------
TutorialApplication::~TutorialApplication(void)
{

}
//-------------------------------------------------------------------------------------
void TutorialApplication::createScene(void)
{
	mRenderer = &CEGUI::OgreRenderer::bootstrapSystem();
	CEGUI::Imageset::setDefaultResourceGroup("Imagesets");
	CEGUI::Font::setDefaultResourceGroup("Fonts");
	CEGUI::Scheme::setDefaultResourceGroup("Schemes");
	CEGUI::WidgetLookManager::setDefaultResourceGroup("LookNFeel");
	CEGUI::WindowManager::setDefaultResourceGroup("Layouts");
	CEGUI::SchemeManager::getSingleton().create("TaharezLook.scheme");
	CEGUI::System::getSingleton().setDefaultMouseCursor("TaharezLook",
			"MouseArrow");
	CEGUI::WindowManager &wmgr = CEGUI::WindowManager::getSingleton();
	CEGUI::Window *sheet =
			wmgr.createWindow("DefaultWindow", "CEGUIDemo/Sheet");
	CEGUI::Window *quit = wmgr.createWindow("TaharezLook/Button",
			"CEGUIDemo/QuitButton");
	quit->setText("Quit");
	quit->setSize(CEGUI::UVector2(CEGUI::UDim(0.15, 0), CEGUI::UDim(0.05, 0)));
	sheet->addChildWindow(quit);
	CEGUI::System::getSingleton().setGUISheet(sheet);
	quit->subscribeEvent(CEGUI::PushButton::EventClicked,
			CEGUI::Event::Subscriber(&TutorialApplication::quit, this));

	mSceneMgr->setAmbientLight(Ogre::ColourValue(1, 1, 1));
	mSceneMgr->setSkyDome(true, "Examples/CloudySky", 5, 8);
	Ogre::Entity* ogreHead = mSceneMgr->createEntity("Head", "ogrehead.mesh");
	Ogre::SceneNode* headNode =
			mSceneMgr->getRootSceneNode()->createChildSceneNode(Ogre::Vector3(
					0, 0, -300));
	headNode->attachObject(ogreHead);
	Ogre::TexturePtr tex = mRoot->getTextureManager()->createManual("RTT",
			Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
			Ogre::TEX_TYPE_2D, 512, 512, 0, Ogre::PF_R8G8B8,
			Ogre::TU_RENDERTARGET);
	Ogre::RenderTexture *rtex = tex->getBuffer()->getRenderTarget();
	Ogre::Camera *cam = mSceneMgr->createCamera("RTTCam");
	cam->setPosition(100, -100, -400);
	cam->lookAt(0, 0, -300);
	Ogre::Viewport *v = rtex->addViewport(cam);
	v->setOverlaysEnabled(false);
	v->setClearEveryFrame(true);
	v->setBackgroundColour(Ogre::ColourValue::Black);
	CEGUI::Texture &guiTex = mRenderer->createTexture(tex);
	CEGUI::Imageset &imageSet = CEGUI::ImagesetManager::getSingleton().create(
			"RTTImageset", guiTex);
	imageSet.defineImage("RTTImage", CEGUI::Point(0.0f, 0.0f), CEGUI::Size(
			guiTex.getSize().d_width, guiTex.getSize().d_height), CEGUI::Point(
			0.0f, 0.0f));
	CEGUI::Window *si = CEGUI::WindowManager::getSingleton().createWindow(
			"TaharezLook/StaticImage", "RTTWindow");
	si->setSize(CEGUI::UVector2(CEGUI::UDim(0.5f, 0), CEGUI::UDim(0.4f, 0)));
	si->setPosition(CEGUI::UVector2(CEGUI::UDim(0.5f, 0), CEGUI::UDim(0.0f, 0)));
	si->setProperty("Image", CEGUI::PropertyHelper::imageToString(
			&imageSet.getImage("RTTImage")));
	sheet->addChildWindow(si);
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
	CEGUI::System &sys = CEGUI::System::getSingleton();
	sys.injectKeyDown(arg.key);
	sys.injectChar(arg.text);
	return true;
}
//-------------------------------------------------------------------------------------
bool TutorialApplication::keyReleased(const OIS::KeyEvent &arg)
{
	CEGUI::System::getSingleton().injectKeyUp(arg.key);
	return true;
}
//-------------------------------------------------------------------------------------
static inline CEGUI::MouseButton convertButton(OIS::MouseButtonID buttonID)
{
	switch (buttonID)
	{
	case OIS::MB_Left:
		return CEGUI::LeftButton;

	case OIS::MB_Right:
		return CEGUI::RightButton;

	case OIS::MB_Middle:
		return CEGUI::MiddleButton;

	default:
		return CEGUI::LeftButton;
	}
}
//-------------------------------------------------------------------------------------
bool TutorialApplication::mouseMoved(const OIS::MouseEvent &arg)
{
	CEGUI::System &sys = CEGUI::System::getSingleton();
	sys.injectMouseMove(arg.state.X.rel, arg.state.Y.rel);
	// Scroll wheel.
	if (arg.state.Z.rel)
		sys.injectMouseWheelChange(arg.state.Z.rel / 120.0f);
	return true;
}
//-------------------------------------------------------------------------------------
bool TutorialApplication::mousePressed(const OIS::MouseEvent &arg,
		OIS::MouseButtonID id)
{
	CEGUI::System::getSingleton().injectMouseButtonDown(convertButton(id));
	return true;
}
//-------------------------------------------------------------------------------------
bool TutorialApplication::mouseReleased(const OIS::MouseEvent &arg,
		OIS::MouseButtonID id)
{
	CEGUI::System::getSingleton().injectMouseButtonUp(convertButton(id));
	return true;
}
//-------------------------------------------------------------------------------------
bool TutorialApplication::quit(const CEGUI::EventArgs &e)
{
	mShutDown = true;
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
