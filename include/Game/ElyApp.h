//|||||||||||||||||||||||||||||||||||||||||||||||

#ifndef OGRE_DEMO_HPP
#define OGRE_DEMO_HPP

//|||||||||||||||||||||||||||||||||||||||||||||||

#include "AdvancedOgreFramework.h"
#include "AppStateManager.h"

//|||||||||||||||||||||||||||||||||||||||||||||||

class ElyApp
{
public:
	ElyApp();
	~ElyApp();

	void startDemo();

private:
	AppStateManager* m_pAppStateManager;
};

//|||||||||||||||||||||||||||||||||||||||||||||||

#endif

//|||||||||||||||||||||||||||||||||||||||||||||||
