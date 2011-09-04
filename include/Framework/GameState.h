//|||||||||||||||||||||||||||||||||||||||||||||||

#ifndef APP_STATE_HPP
#define APP_STATE_HPP

//|||||||||||||||||||||||||||||||||||||||||||||||

#include "AdvancedOgreFramework.h"

class GameState;

//|||||||||||||||||||||||||||||||||||||||||||||||

/**
 * \brief Allows a GameStateManager to receive callbacks from a GameState.
 *
 * This class will later be inherited by the game state manager,
 * but has to be defined here due to design reasons.
 */
class GameStateListener
{
public:

	/**
	 * \brief Constructor.
	 */
	GameStateListener()
	{
	}

	/**
	 * \brief Destructor.
	 */
	virtual ~GameStateListener()
	{
	}

	/**
	 * \brief  Store a game state to manage.
	 *
	 * Function to later add a new state to the manager.
	 *
	 * @param stateName The state name.
	 * @param state The game state to manage.
	 */
	virtual void manageGameState(Ogre::String stateName, GameState* state) = 0;

	/**
	 * \brief Find a state by name.
	 *
	 * @param stateName The state name.
	 * @return The game state to find.
	 */
	virtual GameState* findByName(Ogre::String stateName) = 0;

	/**
	 * \brief Request a change to state.
	 *
	 * Exits the current game state and starts the one specified as
	 * the parameter.
	 *
	 * @param state The game state to change to.
	 */
	virtual void changeGameState(GameState *state) = 0;

	/**
	 * \brief Push state onto the stack, making it current.
	 *
	 * Puts a new game state on the active state stack that will then
	 * be executed.
	 *
	 * @param state The game state to push.
	 * @return Whether or not successful.
	 */
	virtual bool pushGameState(GameState* state) = 0;

	/**
	 * \brief Pop a game state off the stack.
	 *
	 * Removes the top active state from the stack, which results
	 * in returning to the one below
	 */
	virtual void popGameState() = 0;

	/**
	 * \brief Pause current state.
	 */
	virtual void pauseGameState() = 0;

	/**
	 * \brief Cause a shutdown.
	 *
	 * Well, guess what happens here...
	 */
	virtual void shutdown() = 0;

	/**
	 * \brief Unwind stack and push a state.
	 *
	 * Removes all current app states from the stack and moves
	 * to the given new state
	 *
	 * @param state The game state to push.
	 */
	virtual void popAllAndPushGameState(GameState* state) = 0;
};

//|||||||||||||||||||||||||||||||||||||||||||||||

/**
 * \brief Inherit this class to make a game state capable of being
 * managed by the game state manager.
 *
 * The second class is the game state blueprint from which each actual
 * application state will inherit:
 * - some functions to enter, exit, pause, resume and update the state
 * - some functions to call other states (orders the manager to start them)
 * - a pointer to the manager (which is also a GameStateListener)
 * - own Camera and SceneManager
 * Be sure to use DECLARE_GAMESTATE_CLASS(class)in your public section.
 * For each application state we want to use in our program,
 * we create an own class inheriting GameState. So each game state has
 * the same basic functions like enter, exit, pause, resume and update
 * as well its own Camera and SceneManager.
 *
 */
class GameState: public OIS::KeyListener,
		public OIS::MouseListener,
		public OgreBites::SdkTrayListener
{
public:
	/**
	 * \brief Do not inherit this directly!
	 * Use DECLARE_GAMESTATE_CLASS (class) to do it for you.
	 *
	 * @param parent The GameStateListener (i.e. GameStateManager) that
	 * manage this state.
	 * @param name The name of the state.
	 */
	static void create(GameStateListener* parent, const Ogre::String name)
	{
	}

	/**
	 * \brief Destroy self.
	 */
	void destroy()
	{
		delete this;
	}

	/**
	 * \brief Inherit to supply game state enter code.
	 *
	 * A game state needs to be able to load graphics, resources ...
	 * and initialize itself.
	 */
	virtual void enter() = 0;

	/**
	 * \brief Inherit to supply state exit code.
	 *
	 * A game state needs to clean up resources when it’s done.
	 */
	virtual void exit() = 0;

	/**
	 * \brief Inherit to supply pause code.
	 *
	 * Inherit only if this game state can be paused.
	 *
	 * @return True for successful pause, or false to deny pause.
	 */
	virtual bool pause()
	{
		return true;
	}

	/**
	 * \brief Inherit to supply resume code. Inherit only if this
	 * game state can be paused.
	 */
	virtual void resume()
	{
	}

	/**
	 * \brief The update function of the state.
	 *
	 * This is the function to update the game world, i.e. all stuffs
	 * related to Ogre and the other engines.
	 *
	 * @param timeSinceLastFrame Time interval since last update.
	 */
	virtual void update(double timeSinceLastFrame) = 0;

protected:

	/**
	 * Constructor: This should be a private member of an inherited class.
	 */
	GameState()
	{
	}

	/**
	 * \brief Find a state by its name.
	 *
	 * @param stateName The state name.
	 * @return The game state.
	 */
	GameState* findByName(Ogre::String stateName)
	{
		return m_pParent->findByName(stateName);
	}

	/**
	 * \brief Request a change to game state.
	 *
	 * @param state The game state to change to.
	 */
	void changeGameState(GameState* state)
	{
		m_pParent->changeGameState(state);
	}

	/**
	 * \brief Push game state onto the stack.
	 *
	 * @param state The game state.
	 * @return Whether or not successful.
	 */
	bool pushGameState(GameState* state)
	{
		return m_pParent->pushGameState(state);
	}

	/**
	 * \brief Pop a game state off the stack.
	 */
	void popGameState()
	{
		m_pParent->popGameState();
	}

	/**
	 * \brief Cause a shutdown.
	 */
	void shutdown()
	{
		m_pParent->shutdown();
	}

	/**
	 * \brief Unwind stack and push a state.
	 *
	 * @param state The game state.
	 */
	void popAllAndPushGameState(GameState* state)
	{
		m_pParent->popAllAndPushGameState(state);
	}

	GameStateListener* m_pParent; ///< Stores the GameStateManager which is managing this state.

	Ogre::Camera* m_pCamera; ///< The camera used for this state.
	Ogre::SceneManager* m_pSceneMgr; ///< The scene manager used for this state.
	Ogre::FrameEvent m_FrameEvent; ///< The frame event used for this state.
};

//|||||||||||||||||||||||||||||||||||||||||||||||

/**
 * Create the game state. Inherit, Create your class, and have it managed.
 * It defines the macro DECLARE_GAMESTATE_CLASS via that you can later
 * create the game states.
 * \example
 *   \code
 *      static void MyGameStateClass::Create(GameStateListener *parent,
 *         const Ogre::String name)
 *      {
 *         myGameStateClass myGameState=new myGameStateClass();
 *         myGameState->parent=parent;
 *         parent->ManageGameState(name,myGameState);
 *      }
 *   \endcode
 *
 */
#define DECLARE_GAMESTATE_CLASS(T)										\
static void create(GameStateListener* parent, const Ogre::String name)	\
{																		\
	T* myGameState = new T();											\
	myGameState->m_pParent = parent;									\
	parent->manageGameState(name, myGameState);							\
}

//|||||||||||||||||||||||||||||||||||||||||||||||

#endif

//|||||||||||||||||||||||||||||||||||||||||||||||
