//|||||||||||||||||||||||||||||||||||||||||||||||

#ifndef GAME_STATE_HPP
#define GAME_STATE_HPP

//|||||||||||||||||||||||||||||||||||||||||||||||

#include "GameState.h"

#include "DotSceneLoader.h"

#include <OgreSubEntity.h>
#include <OgreMaterialManager.h>

//|||||||||||||||||||||||||||||||||||||||||||||||

namespace framework
{
enum QueryFlags
{
	OGRE_HEAD_MASK = 1 << 0, CUBE_MASK = 1 << 1
};

//|||||||||||||||||||||||||||||||||||||||||||||||

/**
 * \brief The PlayState class is one of the actual GameState
 * class implementations.
 *
 * By inheriting from GameState it is assured that all states have
 * the same common functions as enter(), exit(), pause(),
 * resume() or update().
 * - DECLARE_GAMESTATE_CLASS(PlayState): This line calls the macro
 * defined in GameState.h, making this class a valid game state
 */
class PlayState: public GameState
{
public:

	/**
	 * \brief Constructor. Initializing values.
	 */
	PlayState();

	/**
	 * \brief Destructor.
	 */
	virtual ~PlayState();

	DECLARE_GAMESTATE_CLASS(PlayState)

	/**
	 * \brief Basic setup of the scene and the GUI.
	 *
	 * Always called when starting this state and does, among others,
	 * the following steps:
	 * -# creates SceneManager
	 * -# creates a RaySceneQuery
	 * -# creates/sets a Camera
	 * -# sets input callbacks
	 * -# builds GUI
	 * -# calls createScene() to fill the scene with content
	 */
	void enter();

	/**
	 * \brief Fills the scene with content.
	 *
	 * I does:
	 * -# create a Light
	 * -# instantiate the DotSceneLoader
	 * -# load a DotScene (CubeScene.xml) from the ResourceGroup General
	 * -# retrieve the Entities from the DotScene and set the QueryMaskFlag
	 * (so that they can't be selected with the mouse)
	 * -# put the OgreHead into the scene and also set a QueryMaskFlag,
	 * so that it actually can be selected with the mouse
	 * -# get the current material from the head, clone it and
	 * change the cloned one to red
	 */
	void createScene();

	/**
	 * \brief Executes on exit this state.
	 *
	 * Similar to the pause() function, but also destroys the Camera,
	 * the SceneManager and the RaySceneQuery.
	 */
	void exit();

	/**
	 * \brief Called by the app state manager when the game
	 * state is to be paused.
	 *
	 * @return Whether or not successful.
	 */
	bool pause();

	/**
	 * \brief Called by the app state manager when the game
	 * state is to be resumed.
	 *
	 * It rebuilds the GUI and sets the camera active.
	 */
	void resume();

	/**
	 * \brief Moves the camera.
	 *
	 * Translate the camera by the vector assembled in getInput()
	 * (10 times faster if Shift is pressed).
	 */
	void moveCamera();

	/**
	 * \brief Gets input.
	 *
	 * If we are not in chat mode, we want to move the camera
	 * via the keys, so in this case assemble a translation vector
	 * that is then applied to the camera in moveCamera().
	 */
	void getInput();

	/**
	 * \brief Builds GUI.
	 *
	 * Inserts all needed GUI elements in the SDKTrayManager.
	 */
	void buildGUI();

	/**
	 * \brief Handles key pressed input.
	 *
	 * Handles the buffered input:
	 * -# if we are in settings mode (can be toggled via [Tab] )
	 * then all the key hits have to be injected as they should
	 * be processed by the GUI
	 * -# check for [Escape] , [Tab] to switch input modes and
	 * [Return] / [Enter] when we are in chat mode in order to
	 * send the typed in text to the message box
	 * -# if we a) are not in the chat mode or b) we are in chat
	 * mode, but [O] was not pressed, we can pass the unhandled
	 * input to the OgreFramework class (without this check, e.g.
	 * typing in an [O] in chat mode would also be passed to the
	 * OgreFramework resulting in also toggling the Overlays for
	 * each typed [O] )
	 *
	 * @param keyEventRef The key event.
	 * @return Whether or not successful.
	 */
	bool keyPressed(const OIS::KeyEvent &keyEventRef);

	/**
	 * \brief Handles key released input.
	 *
	 * Just passes the input on to the OgreFramework class.
	 *
	 * @param keyEventRef The key event.
	 * @return Whether or not successful.
	 */
	bool keyReleased(const OIS::KeyEvent &keyEventRef);

	/**
	 * \brief Handles mouse moved input.
	 *
	 * Handle the mouse movement:
	 * -# injects the movement to the GUI
	 * -# if the right mouse button is pressed, manipulates the camera
	 *
	 * @param arg The mouse event.
	 * @return Whether or not successful.
	 */
	bool mouseMoved(const OIS::MouseEvent &arg);

	/**
	 * \brief Handles mouse pressed input.
	 *
	 * Checks which button was pressed and sets the internal
	 * bool values as well as injecting the left click to the
	 * GUI and calls the function onLeftPressed() for object
	 * selection with the mouse.
	 *
	 * @param arg The mouse event.
	 * @param id The mouse button ID.
	 * @return Whether or not successful.
	 */
	bool mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id);

	/**
	 * \brief Handles mouse released input.
	 *
	 * The same as above (mousePressed), but for the mouse pressed.
	 *
	 * @param arg The mouse event.
	 * @param id The mouse button ID.
	 * @return Whether or not successful.
	 */
	bool mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id);

	/**
	 * \brief Perform object selection.
	 *
	 * It does:
	 * -# if there is a selected object (which would be stored in
	 * m_pCurrentObject) hide its bounding box now and set another
	 * material for the Ogre head (the Ogre head is the only
	 * selectable object in our scene due to the QueryMask)
	 * -# get the position of our mouse cursor
	 * -# create a Ray from the camera to this position and check
	 * for hits
	 * -# iterate over all hits
	 * -# if we hit a movable object, show its bounding box and store
	 * it into m_pCurrentObject as well as applying a different
	 * material to it
	 *
	 * @param evt The mouse event.
	 */
	void onLeftPressed(const OIS::MouseEvent &evt);

	/**
	 * \brief Callback function triggered when a selection menu changes.
	 *
	 * As a result when change the polygon mode of the camera.
	 *
	 * @param menu The selected menu.
	 */
	void itemSelected(OgreBites::SelectMenu* menu);

	/**
	 * \brief Update the whole state.
	 *
	 * It does:
	 * -# update the GUI
	 * -# leave and remove this state from the active state stack if
	 * requested
	 * -# update the information in the details panel
	 * -# determine the move scale depending on the time passed
	 * since the last frame
	 * -# reset the translation vector for the camera
	 * -# get the unbuffered input and move the camera
	 *
	 * @param timeSinceLastFrame Time interval since last update.
	 */
	void update(double timeSinceLastFrame);

private:
	Ogre::SceneNode* m_pOgreHeadNode;
	Ogre::Entity* m_pOgreHeadEntity;
	Ogre::MaterialPtr m_pOgreHeadMat;
	Ogre::MaterialPtr m_pOgreHeadMatHigh;

	OgreBites::ParamsPanel* m_pDetailsPanel;
	bool m_bQuit;

	Ogre::Vector3 m_TranslateVector;
	Ogre::Real m_MoveSpeed;
	Ogre::Degree m_RotateSpeed;
	float m_MoveScale;
	Ogre::Degree m_RotScale;

	Ogre::RaySceneQuery* m_pRSQ;
	Ogre::SceneNode* m_pCurrentObject;
	Ogre::Entity* m_pCurrentEntity;
	bool m_bLMouseDown, m_bRMouseDown;
	bool m_bSettingsMode;
};
}
//|||||||||||||||||||||||||||||||||||||||||||||||

#endif

//|||||||||||||||||||||||||||||||||||||||||||||||
