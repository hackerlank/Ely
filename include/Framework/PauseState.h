//|||||||||||||||||||||||||||||||||||||||||||||||

#ifndef PAUSE_STATE_HPP
#define PAUSE_STATE_HPP

//|||||||||||||||||||||||||||||||||||||||||||||||

#include "GameState.h"

//|||||||||||||||||||||||||||||||||||||||||||||||
namespace framework
{
/**
 * \brief The PauseState class is one of the actual GameState
 * class implementations.
 *
 * By inheriting from GameState it is assured that all states have
 * the same common functions as enter(), exit(), pause(),
 * resume() or update().
 * - DECLARE_GAMESTATE_CLASS(PauseState): This line calls the macro
 * defined in GameState.h, making this class a valid game state
 */
class PauseState: public GameState
{
public:

	/**
	 * \brief Constructor. Initializing values.
	 */
	PauseState();

	/**
	 * \brief Destructor.
	 */
	virtual ~PauseState();

	DECLARE_GAMESTATE_CLASS(PauseState)

	/**
	 * \brief Setup the basic values and building the GUI.
	 */
	void enter();

	void createScene();
	void exit();

	bool keyPressed(const OIS::KeyEvent &keyEventRef);
	bool keyReleased(const OIS::KeyEvent &keyEventRef);

	bool mouseMoved(const OIS::MouseEvent &evt);
	bool mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id);
	bool mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID id);

	void buttonHit(OgreBites::Button* button);
	void yesNoDialogClosed(const Ogre::DisplayString& question, bool yesHit);

	void update(double timeSinceLastFrame);

private:
	bool m_bQuit;
	bool m_bQuestionActive;
};
}
//|||||||||||||||||||||||||||||||||||||||||||||||

#endif

//|||||||||||||||||||||||||||||||||||||||||||||||
