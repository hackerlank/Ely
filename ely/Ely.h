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
 * \file /Ely/include/Ely.h
 *
 * \date 30 8, 2013
 * \author consultit
 */

#ifndef ELY_H_
#define ELY_H_

#include <load_prc_file.h>
#include "Game/GameManager.h"
#include "Game/GameAIManager.h"
#include "Game/GameAudioManager.h"
#include "Game/GameControlManager.h"
#include "Game/GamePhysicsManager.h"
#include "Game/GameSceneManager.h"
#include "ObjectModel/ComponentTemplateManager.h"
#include "ObjectModel/ObjectTemplateManager.h"

///Define a manager for a given subsystem:
///@param the manager type
///@param the manager variable
///@param the manager sort
///@param the manager priority
///@param the task chain variable
///@param the task chain number of threads and
///@param the task chain frame_sync flag
#define GAMESUBMANAGER(_managertype_,_manager_,_sort_,_prio_,_chain_,_threads_,_framesync_) \
	_chain_ = AsyncTaskManager::get_global_ptr()->make_task_chain(\
			#_managertype_#_chain_);\
	_chain_->set_num_threads(_threads_);\
	_chain_->set_frame_sync(_framesync_);\
	_managertype_* _manager_ = new _managertype_(_sort_, _prio_, #_managertype_#_chain_);

#endif /* ELY_H_ */
