/*
 *   This file is part of Ely.
 *
 *   Ely is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   Ely is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with Ely.  If not, see <http://www.gnu.org/licenses/>.
 */
/**
 * \file /Ely/elygame/Ely.h
 *
 * \date 2013-08-30
 * \author consultit
 */

#ifndef ELYGAME_H_
#define ELYGAME_H_

#include <load_prc_file.h>
#include "Game/GameManager.h"
#include "Game/GameAIManager.h"
#include "Game/GameAudioManager.h"
#include "Game/GameBehaviorManager.h"
#include "Game/GameControlManager.h"
#include "Game/GameGUIManager.h"
#include "Game/GamePhysicsManager.h"
#include "Game/GameSceneManager.h"
#include "ObjectModel/ComponentTemplateManager.h"
#include "ObjectModel/ObjectTemplateManager.h"

#ifdef ELY_THREAD
///Define a manager for a given subsystem:
///@param _chain_ the task chain variable/name
///@param _threads_ the task chain number of threads
///@param _framesync_ the task chain frame_sync flag
///@param _managertype_ the manager type
///@param _manager_ the manager variable
///@param _sort_ the manager sort
///@param _prio_ the manager priority
#define TASKCHAIN(_chain_,_threads_,_framesync_) \
	AsyncTaskChain* _chain_ = AsyncTaskManager::get_global_ptr()->\
	make_task_chain(#_chain_);\
	_chain_->set_num_threads(_threads_);\
	_chain_->set_frame_sync(_framesync_)
#define GAMESUBMANAGER(_managertype_,_manager_,_managersmutex_,_managersvar_,\
		_completedmask_,_exiting_,_completedtasks_,_sort_,_prio_,_chain_) \
	_managertype_* _manager_ = new _managertype_(_managersmutex_,_managersvar_,\
			_completedmask_,_exiting_,_completedtasks_,_sort_, _prio_, #_chain_)
extern Mutex managersMutex;
extern ConditionVarFull managersVar;
extern unsigned long int completedTasks;
extern unsigned long int completedAllMask;
extern const unsigned long int exiting;
AsyncTask::DoneStatus fireManagers(GenericAsyncTask* task, void * data);
#endif

#endif /* ELYGAME_H_ */
