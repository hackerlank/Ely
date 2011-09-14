//|||||||||||||||||||||||||||||||||||||||||||||||

#ifndef OGRE_FRAMEWORK_HPP
#define OGRE_FRAMEWORK_HPP

//|||||||||||||||||||||||||||||||||||||||||||||||

#include <OGRE/OgreCamera.h>
#include <OGRE/OgreEntity.h>
#include <OGRE/OgreLogManager.h>
#include <OGRE/OgreOverlay.h>
#include <OGRE/OgreOverlayElement.h>
#include <OGRE/OgreOverlayManager.h>
#include <OGRE/OgreRoot.h>
#include <OGRE/OgreViewport.h>
#include <OGRE/OgreSceneManager.h>
#include <OGRE/OgreRenderWindow.h>
#include <OGRE/OgreConfigFile.h>

#include <OIS/OISEvents.h>
#include <OIS/OISInputManager.h>
#include <OIS/OISKeyboard.h>
#include <OIS/OISMouse.h>

#include <SdkTrays.h>

//|||||||||||||||||||||||||||||||||||||||||||||||

namespace framework
{

/**
 * \brief This class wraps the Ogre functionality.
 *
 * It is a Ogre::Singleton.
 * It has the needed functions to power up Ogre and offers
 * the standard behavior such as making screenshots and toggling
 * and filling the Ogre Debug Overlays. It contains most of
 * the Ogre related variables:
 * - Root
 * - RenderWindow
 * - Viewport
 * - Log
 * - Timer
 * - InputManager / Keyboard / Mouse
 * - SDKTrays Manager
 * It also offers functions to handle input, but that
 * is almost not used here, as each application state needs a
 * different input behavior and therefore this is directly modeled
 * in the application states. However, everything common for the whole
 * application can be put here.
 */
class OgreFramework: public Ogre::Singleton<OgreFramework>,
		OIS::KeyListener,
		OIS::MouseListener
{
public:

	/**
	 * \brief Constructor.
	 */
	OgreFramework();

	/**
	 * \brief Destructor, clearing up.
	 */
	virtual
	~OgreFramework();

	/**
	 * \brief Powers up Ogre.
	 *
	 * Performs the following steps:
	 * -# creates the log manager
	 * -# creates the Root
	 * -# creates the RenderWindow and the Viewport
	 * -# powers up OIS
	 * -# if there was no MouseListener or KeyboardListener
	 * passed as a parameter, uses the ones from this class,
	 * otherwise the passed ones (however you still can use both,
	 * by calling the OgreFramework class input functions when you
	 * handle input elsewhere)
	 * -# loads resources
	 * -# start Timer
	 * -# set up the SDKTrayManager
	 * -# create and show the debug overlay
	 * \note After running this function you still won't see anything
	 * on the screen as there is no Camera and no SceneManager.
	 * Those are members of the individual application states!
	 *
	 * @param wndTitle Title of the window.
	 * @param pKeyListener The KeyListener.
	 * @param pMouseListener The MouseListener.
	 * @return Whether or not successful.
	 */
	bool initOgre(Ogre::String wndTitle, OIS::KeyListener *pKeyListener = 0,
			OIS::MouseListener *pMouseListener = 0);

	/**
	 * \brief Updates the application main state.
	 *
	 * Is called once per frame by the GameStateManager to update
	 * everything directly related to application (Ogre and others engine).
	 * In this application framework all updates of the game world are taken
	 * care of by the game states (through the update method), but if there
	 * would be some central application update (for Ogre and others engine)
	 * task, this would go in here.
	 *
	 * @param timeSinceLastFrame Time interval since last update.
	 */
	void updateOgre(double timeSinceLastFrame);

	/**
	 * \brief Handles key pressed input.
	 *
	 * Handles to buffered input, common for the whole application.
	 *
	 * @param keyEventRef The key event.
	 * @return Whether or not successful.
	 */
	bool keyPressed(const OIS::KeyEvent &keyEventRef);

	/**
	 * \brief Handles key released input.
	 *
	 * Handles to buffered input, common for the whole application.
	 *
	 * @param keyEventRef The key event.
	 * @return Whether or not successful.
	 */
	bool keyReleased(const OIS::KeyEvent &keyEventRef);

	/**
	 * \brief Handles mouse moved input.
	 *
	 * Handles to buffered input, common for the whole application.
	 *
	 * @param evt The mouse event.
	 * @return Whether or not successful.
	 */
	bool mouseMoved(const OIS::MouseEvent &evt);

	/**
	 * \brief Handles mouse pressed input.
	 *
	 * Handles to buffered input, common for the whole application.
	 *
	 * @param evt The mouse event.
	 * @param id The mouse button ID.
	 * @return Whether or not successful.
	 */
	bool mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id);

	/**
	 * \brief Handles mouse released input.
	 *
	 * Handles to buffered input, common for the whole application.
	 *
	 * @param evt The mouse event.
	 * @param id The mouse button ID.
	 * @return Whether or not successful.
	 */
	bool mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID id);

	Ogre::Root* m_pRoot;
	Ogre::RenderWindow* m_pRenderWnd;
	Ogre::Viewport* m_pViewport;
	Ogre::Log* m_pLog;
	Ogre::Timer* m_pTimer;

	OIS::InputManager* m_pInputMgr;
	OIS::Keyboard* m_pKeyboard;
	OIS::Mouse* m_pMouse;

	OgreBites::SdkTrayManager* m_pTrayMgr;

private:
	OgreFramework(const OgreFramework&);
	OgreFramework&
	operator=(const OgreFramework&);
};

}
//|||||||||||||||||||||||||||||||||||||||||||||||

#endif

//|||||||||||||||||||||||||||||||||||||||||||||||
