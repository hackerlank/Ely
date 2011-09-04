//|||||||||||||||||||||||||||||||||||||||||||||||

#ifndef MENU_STATE_HPP
#define MENU_STATE_HPP

//|||||||||||||||||||||||||||||||||||||||||||||||

#include "GameState.h"

//|||||||||||||||||||||||||||||||||||||||||||||||

/**
 * \brief The MenuState class is one of the actual GameState
 * class implementations.
 *
 * By inheriting from GameState it is assured that all states have
 * the same common functions as enter(), exit(), pause(),
 * resume() or update().
 * - DECLARE_GAMESTATE_CLASS(MenuState): This line calls the macro
 * defined in GameState.h, making this class a valid game state
 */
class MenuState: public GameState
{
public:

	/**
	 * \brief Constructor. Initializing values.
	 */
	MenuState();

	/**
	 * \brief Destructor.
	 */
	virtual ~MenuState();

	DECLARE_GAMESTATE_CLASS(MenuState)

	/**
	 * \brief Called when starting this state.
	 *
	 * Always called when starting this state and does, among others,
	 * the following steps:
	 * -# creates SceneManager
	 * -# creates/sets a Camera
	 * -# sets input callbacks
	 * -# builds GUI
	 * -# calls createScene() to fill the scene with content
	 */
	void enter();

	/**
	 * \brief Fill the scene with content (no content in the MenuState here).
	 */
	void createScene();

	/**
	 * \brief Destroys the Camera and the SceneManager as well
	 * as the GUI elements.
	 */
	void exit();

	/**
	 * \brief Handles key pressed input.
	 *
	 * Quits on Escape and forwards the unhandled input to
	 * the OgreFramework class
	 *
	 * @param keyEventRef The key event.
	 * @return Whether or not successful.
	 */
	bool keyPressed(const OIS::KeyEvent &keyEventRef);

	/**
	 * \brief Handles key released input.
	 *
	 * Only forwards to the OgreFramework class.
	 *
	 * @param keyEventRef The key event.
	 * @return Whether or not successful.
	 */
	bool keyReleased(const OIS::KeyEvent &keyEventRef);

	/**
	 * \brief Handles mouse moved input.
	 *
	 * Injects the mouse movements to the GUI .
	 *
	 * @param evt The mouse event.
	 * @return Whether or not successful.
	 */
	bool mouseMoved(const OIS::MouseEvent &evt);

	/**
	 * \brief Handles mouse pressed input.
	 *
	 * Injects the mouse clicks to the GUI.
	 *
	 * @param evt The mouse event.
	 * @return Whether or not successful.
	 */
	bool mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id);

	/**
	 * \brief Handles mouse released input.
	 *
	 * Injects the mouse clicks to the GUI.
	 *
	 * @param evt The mouse event.
	 * @return Whether or not successful.
	 */
	bool mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID id);

	/**
	 * \brief Callback function that gets triggered when a button is hit.
	 *
	 * @param button The hit button.
	 */
	void buttonHit(OgreBites::Button* button);

	/**
	 * \brief The update function of the state.
	 *
	 * Updates the GUI and checks whether the state is to be quit.
	 *
	 * @param timeSinceLastFrame Time interval since last update.
	 */
	void update(double timeSinceLastFrame);

private:
	bool m_bQuit;
};

//|||||||||||||||||||||||||||||||||||||||||||||||

#endif

//|||||||||||||||||||||||||||||||||||||||||||||||
