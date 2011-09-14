//|||||||||||||||||||||||||||||||||||||||||||||||

#include "Game/Ely.h"

#include "Framework/MenuState.h"
#include "Framework/PlayState.h"
#include "Framework/PauseState.h"

//|||||||||||||||||||||||||||||||||||||||||||||||
namespace game
{
Ely::Ely()
{
	m_pGameStateManager = 0;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

Ely::~Ely()
{
	delete m_pGameStateManager;
	delete framework::OgreFramework::getSingletonPtr();
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void Ely::go()
{
	new framework::OgreFramework();
	if (!framework::OgreFramework::getSingletonPtr()->initOgre("AdvancedOgreFramework", 0,
			0))
		return;

	framework::OgreFramework::getSingletonPtr()->m_pLog->logMessage("Demo initialized!");

	m_pGameStateManager = new framework::GameStateManager();

	framework::MenuState::create(m_pGameStateManager, "MenuState");
	framework::PlayState::create(m_pGameStateManager, "PlayState");
	framework::PauseState::create(m_pGameStateManager, "PauseState");

	m_pGameStateManager->start(m_pGameStateManager->findByName("MenuState"));
}
}
//|||||||||||||||||||||||||||||||||||||||||||||||
