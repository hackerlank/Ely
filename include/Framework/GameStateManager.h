//|||||||||||||||||||||||||||||||||||||||||||||||

#ifndef APP_STATE_MANAGER_HPP
#define APP_STATE_MANAGER_HPP

//|||||||||||||||||||||||||||||||||||||||||||||||

#include "GameState.h"

//|||||||||||||||||||||||||||||||||||||||||||||||

namespace framework
{

/**
 * \brief The GameStateManager manages changes in the game states in the game.
 *
 * The class AppStateManager inherits from the class GameStateListener
 * shown above and mainly implements its abstract methods.
 * Furthermore it contains:
 * - a std::vector for all existing states (m_States)
 * - a std::vector for the active states, so the stack of those
 * states currently in use (m_ActiveStateStack)
 */
class GameStateManager: public GameStateListener
{
public:

	/**
	 * Holds information about the states in order to
	 * manage them properly and provide access.
	 */
	typedef struct
	{
		Ogre::String name;
		GameState* state;
	} state_info;

	/**
	 * \brief Constructor, constructs the GameStateManager.
	 *
	 * Must have all input, output, gui functions in order to manage states.
	 * Just setting the shutdown indicator.
	 */
	GameStateManager();

	/**
	 * \brief Destructor, cleans up the states before the instance dies.
	 *
	 * Exiting all active application states and emptying the std::vectors
	 */
	virtual ~GameStateManager();

	/**
	 * \brief  Store a game state to manage.
	 *
	 * Called from within the state creation macro and sets some
	 * information of the new state, as well as pushing it on the
	 * active states stack.
	 *
	 * @param stateName The state name.
	 * @param state The game state to manage.
	 */
	void manageGameState(Ogre::String stateName, GameState* state);

	/**
	 * \brief Find a state by name.
	 *
	 * @param stateName The state name.
	 * @return A pointer to the state with the respective name.
	 */
	GameState* findByName(Ogre::String stateName);

	/**
	 * \brief Start the game application main loop.
	 *
	 * Main loop of the application that does the following steps:
	 * -# change to the state specified
	 * -# start loop
	 * -# capture keyboard and mouse input
	 * -# update the current state (the top most of the stack)
	 * -# call the OgreFramework class to update and render
	 *
	 * @param state The game state to start.
	 */
	void start(GameState* state);

	/**
	 * \brief Change to a new game state.
	 *
	 * Exits the current state (if there is any) and starts the new
	 * specified one.
	 *
	 * @param state The game state to change to.
	 */
	void changeGameState(GameState* state);

	/**
	 * \brief Push game state onto the stack, making it current.
	 *
	 * Puts an new state on the top of the stack and starts it.
	 *
	 * @param state The game state to push.
	 * @return Whether or not successful.
	 */
	bool pushGameState(GameState* state);

	/**
	 * \brief Pop a game state off the stack.
	 *
	 * Removes the top most state and resumes the one below
	 * if there is one, otherwise shutdown the application.
	 */
	void popGameState();

	/**
	 * \brief Pause current state.
	 *
	 * Pauses the current game state and resumes the one in the
	 * stack below.
	 */
	void pauseGameState();

	/**
	 * \brief Exits the application.
	 *
	 * This is a special case function to cause a shutdown.
	 */
	void shutdown();

	/**
	 * \brief Unwind stack and push a state.
	 *
	 * Exits all existing game state on the stack and enters the
	 * given new state.
	 *
	 * @param state The game state to push.
	 */
	void popAllAndPushGameState(GameState* state);

protected:

	/**
	 * \brief This initializes a state to receive the events.
	 *
	 * Initializes a new state and links the input and SDKTrays
	 * callback on it, as well as resetting the Ogre statistics
	 * (FPS?, triangle count, batch count, ...).
	 *
	 * @param state the game state to initialize.
	 */
	void init(GameState *state);

	std::vector<GameState*> m_ActiveStateStack; ///< This is the stack where the active states are stored.
	std::vector<state_info> m_States; ///< This holds the states that are being managed.
	bool m_bShutdown; ///< If this is set to true, the game state manager prepares to exit.
};
}
//|||||||||||||||||||||||||||||||||||||||||||||||

#endif

//|||||||||||||||||||||||||||||||||||||||||||||||
