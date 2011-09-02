//|||||||||||||||||||||||||||||||||||||||||||||||

#include "Game/Ely.h"

#include "Framework/MenuState.h"
#include "Framework/PlayState.h"
#include "Framework/PauseState.h"

//|||||||||||||||||||||||||||||||||||||||||||||||

Ely::Ely()
{
	m_pGameStateManager = 0;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

Ely::~Ely()
{
	delete m_pGameStateManager;
	delete OgreFramework::getSingletonPtr();
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void Ely::startDemo()
{
	new OgreFramework();
	if (!OgreFramework::getSingletonPtr()->initOgre("AdvancedOgreFramework", 0,
			0))
		return;

	OgreFramework::getSingletonPtr()->m_pLog->logMessage("Demo initialized!");

	m_pGameStateManager = new GameStateManager();

	MenuState::create(m_pGameStateManager, "MenuState");
	PlayState::create(m_pGameStateManager, "GameState");
	PauseState::create(m_pGameStateManager, "PauseState");

	m_pGameStateManager->start(m_pGameStateManager->findByName("MenuState"));
}

//|||||||||||||||||||||||||||||||||||||||||||||||
