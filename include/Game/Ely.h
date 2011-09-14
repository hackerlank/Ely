//|||||||||||||||||||||||||||||||||||||||||||||||

#ifndef OGRE_DEMO_HPP
#define OGRE_DEMO_HPP

//|||||||||||||||||||||||||||||||||||||||||||||||

#include "Framework/AdvancedOgreFramework.h"
#include "Framework/GameStateManager.h"

//|||||||||||||||||||||||||||||||||||||||||||||||

namespace game
{
/**
 * \brief The Ely class is the central application organizing location.
 *
 * It only has one member variable:
 * -  the game state manager
 */
class Ely
{
public:

	/**
	 * \brief Constructor.
	 */
	Ely();

	/**
	 * \brief Destructor.
	 */
	~Ely();

	/**
	 * \brief Starts the application.
	 *
	 * It does:
	 * -# fires up the OgreFramework
	 * -# initializes Ogre (via the OgreFramework)
	 * -# create an GameStateManager instance
	 * -# creates the application states (MenuState, PlayState
	 * and PauseState)
	 * -# orders the GameStateManager to start its main loop function
	 */
	void go();

private:
	framework::GameStateManager* m_pGameStateManager; ///< The game state manager.
};
}
//|||||||||||||||||||||||||||||||||||||||||||||||

#endif

//|||||||||||||||||||||||||||||||||||||||||||||||
