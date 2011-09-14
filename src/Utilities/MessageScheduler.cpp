/**
 * \file MessageScheduler.cpp
 *
 * \date 14/set/2011
 * \author Marco Paone
 */

#include "Utilities/MessageScheduler.h"

namespace utils
{

//-------------------------------------------------------------------------
MessageScheduler::MessageScheduler() :
		mTimeLimit(10.0), mAvgMsgProcessingTime(0.0)
{
	addMessageHook(Message::EMPTY,
			MessageHook(this, &MessageScheduler::emptyMessageHook));
}
//-------------------------------------------------------------------------
MessageScheduler::~MessageScheduler()
{
}
//-------------------------------------------------------------------------
void MessageScheduler::setSchedulerTimeLimit(double timeLimit)
{
	mTimeLimit = 1.0 < timeLimit ? timeLimit : 1.0;
}
//-------------------------------------------------------------------------
void MessageScheduler::affectSchedulerTimeLimit(double suggestedTimeLimit)
{
	if (suggestedTimeLimit < 1.0)
		suggestedTimeLimit = 1.0;
	mTimeLimit = 0.75 * mTimeLimit + 0.25 * suggestedTimeLimit; // NEXT: evaluate and make better
}
//-------------------------------------------------------------------------
bool MessageScheduler::processMessages(double tickTime)
{
	boost::lock_guard<boost::mutex> guard(mSchedulerMutex);

	// Sort messages into a descending order so front element of the queue has
	// the most priority.
	std::sort(mMessageQueue_TimeLimited.rbegin(),
			mMessageQueue_TimeLimited.rend());

	// The size of the queue could be changed while processing the messages (e.g.
	// their handler methods can post other messages). So before any processing,
	// we store 'current' number of the messages in this cycle and only try to
	// respond to these requests.
	size_t numTasks = mMessageQueue_TimeLimited.size();
	double approxProcessingTime = 0;

	// Process until time limit is reached or all done. All messages with
	// immediate priority are processed, anyhow.
	// NOTE: The scheduler might be reset as a result of a message. This
	//        conditions should be checked in every iteration.
	while (0 < numTasks-- && !mMessageQueue_TimeLimited.empty()
			&& (Message::IMMEDIATE <= mMessageQueue_TimeLimited.front().priority
					|| approxProcessingTime < mTimeLimit))
	{
		approxProcessingTime += mAvgMsgProcessingTime;

		Task &task = mMessageQueue_TimeLimited.front();

		//--------------- Process the message --------------
		// Note that while processing, other messages could be posted and pushed to the end of the queue.
		bool allowed = true;

		// Find and invode registerd hooks for message types that has no specific receiver.
		// TODO: DOC!
		//if (task.message.getReceiver().type() != typeid(String))  // TODO: ...==0) ie: String --> size_t
		//{
		MessageHookMap::const_iterator it = mHooksMap.find(
				task.message.getType());
		if (mHooksMap.end() != it)
		{
			MessageHookVector::const_iterator vit = it->second.begin();
			for (; vit != it->second.end(); vit++)
			{
				allowed = (*vit)(task.message); // NOTE: Not considered in processing time

				if (!allowed)
					break;
			}
		}
		//}

		if (allowed)
			_dispatchMessage(task.message);
		//--------------------------------------------------

		if (!mMessageQueue_TimeLimited.empty())
			// If MT, perhaps msgs should be poped a few ticks later, so that dispatcher sends their ptr
			mMessageQueue_TimeLimited.pop_front();
	}
	++numTasks; // decreased from its real value if time limit is reached.

	// Enhance the priority of unprocessed tasks (but NOT lately added messages).
	for (MessageQueue::iterator it = mMessageQueue_TimeLimited.begin(); // begins from front
	0 < numTasks-- && mMessageQueue_TimeLimited.end() != it; ++it) // ends to back of the queue
			{
		(*it).increasePriority();
	}

	return mMessageQueue_TimeLimited.empty() ? false : true;
}
//-------------------------------------------------------------------------
bool MessageScheduler::processMessageImmediate(const Message& message)
{

	MessageHookMap::const_iterator it = mHooksMap.find(message.getType());
	if (mHooksMap.end() != it)
	{
		MessageHookVector::const_iterator vit = it->second.begin();
		for (; vit != it->second.end(); vit++)
		{
			if (!((*vit)(message)))
				return false;
		}
	}

	_dispatchMessage(message, true);

	return true;
}
//-------------------------------------------------------------------------
void MessageScheduler::resetScheduler()
{
	boost::lock_guard<boost::mutex> guard(mSchedulerMutex);

	mMessageQueue_TimeLimited.clear();
}
//-------------------------------------------------------------------------
void MessageScheduler::discardMessages(const Any& receiver)
{
	boost::lock_guard<boost::mutex> guard(mSchedulerMutex);
	assert(!receiver.empty() && receiver.type() == typeid(String));

	unsigned int num = 0;
	int msgType = 0;
	for (MessageQueue::iterator it = mMessageQueue_TimeLimited.begin();
			mMessageQueue_TimeLimited.end() != it; ++it)
			{
		// TODO This part has the same problem... if ptr 0 then exception...
		if (FastAnyCast(String, (*it).message.getReceiver())
				== FastAnyCast(String, receiver))
		{
			// TODO Efficient?
			(*it).message.setReceiver(Any());
			msgType = (*it).message.getType();
			(*it).message.setType(Message::EMPTY); // Do not change the priority and so queue's order!
			++num;
		}
	}

}
//-------------------------------------------------------------------------
bool MessageScheduler::emptyMessageHook(const Message& message)
{
	return false;
}
//-------------------------------------------------------------------------

} /* namespace utils */
