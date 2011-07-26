//|||||||||||||||||||||||||||||||||||||||||||||||

#include "Game/ElyApp.h"

#include "Game/MenuState.h"
#include "Game/GameState.h"
#include "Game/PauseState.h"

//|||||||||||||||||||||||||||||||||||||||||||||||

ElyApp::ElyApp()
{
	m_pAppStateManager = 0;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

ElyApp::~ElyApp()
{
	delete m_pAppStateManager;
	delete OgreFramework::getSingletonPtr();
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void ElyApp::startDemo()
{
	new OgreFramework();
	if (!OgreFramework::getSingletonPtr()->initOgre("AdvancedOgreFramework", 0,
			0))
		return;

	OgreFramework::getSingletonPtr()->m_pLog->logMessage("Demo initialized!");

	m_pAppStateManager = new AppStateManager();

	MenuState::create(m_pAppStateManager, "MenuState");
	GameState::create(m_pAppStateManager, "GameState");
	PauseState::create(m_pAppStateManager, "PauseState");

	m_pAppStateManager->start(m_pAppStateManager->findByName("MenuState"));
}

//|||||||||||||||||||||||||||||||||||||||||||||||
