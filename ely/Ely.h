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
 * \file /Ely/ely/Ely.h
 *
 * \date 30 8, 2013
 * \author consultit
 */

#ifndef ELY_H_
#define ELY_H_

#include <load_prc_file.h>
#include <Rocket/Core.h>
#include "Game/GameManager.h"
#include "Game/GameAIManager.h"
#include "Game/GameAudioManager.h"
#include "Game/GameBehaviorManager.h"
#include "Game/GameControlManager.h"
#include "Game/GamePhysicsManager.h"
#include "Game/GameSceneManager.h"
#include "ObjectModel/ComponentTemplateManager.h"
#include "ObjectModel/ObjectTemplateManager.h"

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
	_chain_->set_frame_sync(_framesync_);
#define GAMESUBMANAGER(_managertype_,_manager_,_sort_,_prio_,_chain_) \
	_managertype_* _manager_ = new _managertype_(_sort_, _prio_, #_chain_);

///Common ely definitions
namespace ely
{

/**
 * \brief Abstract class interface for libRocket event listeners.
 *
 * A concrete derived class must implement:
 * \code
 * virtual void ProcessEvent(Rocket::Core::Event& event);
 * \endcode
 */
class EventListener: public Rocket::Core::EventListener,
		Rocket::Core::ReferenceCountable
{
public:
	EventListener(const Rocket::Core::String& value, PandaFramework* framework) :
			Rocket::Core::ReferenceCountable(0), mValue(value), mFramework(
					framework)
	{
		PRINT_DEBUG(
				"Creating EventListener: " << mValue.CString() << " " << this);
	}
	virtual ~EventListener()
	{
		PRINT_DEBUG(
				"Destroying EventListener: " << mValue.CString() << " " << this);
	}

	virtual void OnAttach(Rocket::Core::Element* ROCKET_UNUSED(element))
	{
		PRINT_DEBUG(
				"Attaching EventListener: " << mValue.CString() << " " << this);
		AddReference();
	}

	virtual void OnDetach(Rocket::Core::Element* ROCKET_UNUSED(element))
	{
		PRINT_DEBUG(
				"Detaching EventListener: " << mValue.CString() << " " << this);
		RemoveReference();
	}

protected:
	virtual void OnReferenceDeactivate()
	{
		delete this;
	}

	Rocket::Core::String mValue;
	PandaFramework* mFramework;
};

/**
 * \brief Abstract class interface for libRocket event listener instancers.
 *
 * A concrete derived class must implement:
 * \code
 * virtual Rocket::Core::EventListener* InstanceEventListener(
 const Rocket::Core::String& value, Rocket::Core::Element* element);
 * \endcode
 * This interface is designed so that method would create a ely::EventListener
 * derived class.\n
 */
class EventListenerInstancer: public Rocket::Core::EventListenerInstancer
{
public:
	EventListenerInstancer(PandaFramework* framework) :
			mFramework(framework)
	{
		PRINT_DEBUG("Creating EventListenerInstancer " << " " << this);
	}
	virtual ~EventListenerInstancer()
	{
		PRINT_DEBUG("Destroying EventListenerInstancer " << " " << this);
	}

	virtual void Release()
	{
		delete this;
	}

protected:
	PandaFramework* mFramework;
};
}

#endif /* ELY_H_ */
