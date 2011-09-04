//|||||||||||||||||||||||||||||||||||||||||||||||

#ifndef OGRE_DEMO_HPP
#define OGRE_DEMO_HPP

//|||||||||||||||||||||||||||||||||||||||||||||||

#include "Framework/AdvancedOgreFramework.h"
#include "Framework/GameStateManager.h"

//|||||||||||||||||||||||||||||||||||||||||||||||

class Ely
{
public:
	Ely();
	~Ely();

	void go();

private:
	GameStateManager* m_pGameStateManager;
};

//|||||||||||||||||||||||||||||||||||||||||||||||

#endif

//|||||||||||||||||||||||||||||||||||||||||||||||
