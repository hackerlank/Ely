//|||||||||||||||||||||||||||||||||||||||||||||||

#ifndef OGRE_DEMO_HPP
#define OGRE_DEMO_HPP

//|||||||||||||||||||||||||||||||||||||||||||||||

#include "Framework/AdvancedOgreFramework.h"
#include "Framework/AppStateManager.h"

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
