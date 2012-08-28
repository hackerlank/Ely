/*
 * GameSceneManager.cpp
 *
 *  Created on: 28/ago/2012
 *      Author: marco
 */

#include "Game/GameSceneManager.h"

GameSceneManager::GameSceneManager(int sort, int priority,
		const std::string& asyncTaskChain)
{
	// TODO Auto-generated constructor stub

}

GameSceneManager::~GameSceneManager()
{
	// TODO Auto-generated destructor stub
}

void GameSceneManager::addToSceneUpdate(SMARTPTR(Component)sceneComp)
{
}

void GameSceneManager::removeFromSceneUpdate(SMARTPTR(Component)sceneComp)
{
}

AsyncTask::DoneStatus GameSceneManager::update(GenericAsyncTask* task)
{
	//
	return AsyncTask::DS_cont;
}

ReMutex& GameSceneManager::getMutex()
{
	return mMutex;
}
