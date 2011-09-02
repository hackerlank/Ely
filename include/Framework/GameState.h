//|||||||||||||||||||||||||||||||||||||||||||||||

#ifndef APP_STATE_HPP
#define APP_STATE_HPP

//|||||||||||||||||||||||||||||||||||||||||||||||

#include "AdvancedOgreFramework.h"

class GameState;

//|||||||||||||||||||||||||||||||||||||||||||||||

/**
 * \brief Allows a GameStateManager to receive callbacks from a GameState.
 */
class GameStateListener
{
public:
	GameStateListener()
	{
	}
	;
	virtual ~GameStateListener()
	{
	}
	;

	/**
	 * \brief  Store a game state to manage.
	 *
	 * @param stateName The state name.
	 * @param state The game state.
	 */
	virtual void manageGameState(Ogre::String stateName, GameState* state) = 0;

	/**
	 * \brief Find a state by name.
	 *
	 * @param stateName The state name.
	 * @return The game state.
	 */
	virtual GameState* findByName(Ogre::String stateName) = 0;

	/**
	 * \brief Request a change to state.
	 *
	 * @param state The game state.
	 */
	virtual void changeGameState(GameState *state) = 0;

	/**
	 * \brief Push state onto the stack so make it current.
	 *
	 * @param state The game state.
	 * @return If successful.
	 */
	virtual bool pushGameState(GameState* state) = 0;

	/**
	 * \brief Pop a game state off the stack.
	 */
	virtual void popGameState() = 0;

	/**
	 * \brief Pause current state.
	 */
	virtual void pauseGameState() = 0;

	/**
	 * \brief Cause a shutdown.
	 */
	virtual void shutdown() = 0;

	/**
	 * \brief Unwind stack and push a state.
	 *
	 * @param state The game state.
	 */
	virtual void popAllAndPushGameState(GameState* state) = 0;
};

//|||||||||||||||||||||||||||||||||||||||||||||||

class GameState: public OIS::KeyListener,
		public OIS::MouseListener,
		public OgreBites::SdkTrayListener
{
public:
	static void create(GameStateListener* parent, const Ogre::String name)
	{
	}
	;

	void destroy()
	{
		delete this;
	}

	virtual void enter() = 0;
	virtual void exit() = 0;
	virtual bool pause()
	{
		return true;
	}
	virtual void resume()
	{
	}
	;
	virtual void update(double timeSinceLastFrame) = 0;

protected:
	GameState()
	{
	}
	;

	GameState* findByName(Ogre::String stateName)
	{
		return m_pParent->findByName(stateName);
	}
	void changeGameState(GameState* state)
	{
		m_pParent->changeGameState(state);
	}
	bool pushGameState(GameState* state)
	{
		return m_pParent->pushGameState(state);
	}
	void popGameState()
	{
		m_pParent->popGameState();
	}
	void shutdown()
	{
		m_pParent->shutdown();
	}
	void popAllAndPushGameState(GameState* state)
	{
		m_pParent->popAllAndPushGameState(state);
	}

	GameStateListener* m_pParent;

	Ogre::Camera* m_pCamera;
	Ogre::SceneManager* m_pSceneMgr;
	Ogre::FrameEvent m_FrameEvent;
};

//|||||||||||||||||||||||||||||||||||||||||||||||

#define DECLARE_APPSTATE_CLASS(T)										\
static void create(GameStateListener* parent, const Ogre::String name)	\
{																		\
	T* myGameState = new T();											\
	myGameState->m_pParent = parent;										\
	parent->manageGameState(name, myGameState);							\
}

//|||||||||||||||||||||||||||||||||||||||||||||||

#endif

//|||||||||||||||||||||||||||||||||||||||||||||||
