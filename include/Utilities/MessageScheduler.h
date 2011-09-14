/**
 * \file MessageScheduler.h
 *
 * \date 14/set/2011
 * \author Marco Paone
 */

#ifndef MESSAGESCHEDULER_H_
#define MESSAGESCHEDULER_H_

#include "Utilities/Message.h"

namespace utils
{

// TODO STEVEN Use C:\Work\Oge\WORK\oge\src\oge\utilities\OgeScheduler.cpp

/**
 * @todo Make multi-thread:
 *      - Four separate-rotating-concurrent queues should be used for different
 *        priorities.
 *      - Add MT locks.
 * @todo Clean up so that it could be used as singleton (StateManager).
 *
 * @author Mehdi Toghianifar
 */
class MessageScheduler
{
public:
	/**
	 * Map<int type, std::vector<MessageMethod> methods> used to associate
	 * a message type with the methods able to treat it.
	 */
	// TODO try to use a multimap: is it faster? and safer see below
	//    typedef std::map<int, std::vector<MessageDelegate> > MessageDelegatesMap;    TO REMOVE?
	typedef std::vector<MessageHook> MessageHookVector;
	typedef std::map<int, MessageHookVector> MessageHookMap;

private:
	struct Task // TODO: Rename!
	{
		Message message;
		/// Message priority, priority increase with value
//_ TODO Put priority in Message
		int priority;
		///
//_            MessageDispatcher* receiver;
		/**
		 * Elapsed Time: delta time used by the receiving method. This can be
		 * used differently depending on the policy needed
		 */
		double elapsedTime;

		/// Used to send general/system messages (like Shutdown, ...)
		inline Task(const Message& _message, int _priority) :
				message(_message), priority(_priority), elapsedTime(0)
		{
		}
		/// Operator < is used by the std::sort algorithm.
		inline bool operator<(const Task& rhs) const
		{
			return priority < rhs.priority;
		}
		inline void increasePriority()
		{
			++priority;
			assert(Message::IMMEDIATE >= priority);
		}
		inline void decreasePriority()
		{
			--priority;
			assert(Message::LOW <= priority);
		}

	private:
		Task(); //: receiver(0)    {}
	};

private:
	mutable boost::mutex mSchedulerMutex;
	/// Time limit to process batchs of messages. Default: 10ms
	double mTimeLimit;

	/// WIP
	double mAvgMsgProcessingTime;

	typedef std::deque<Task> MessageQueue; // WIP
	MessageQueue mMessageQueue_TimeLimited;

	// Maps all the message handlers
	MessageHookMap mHooksMap; // NOTE/TODO: Should be OPTIMISED

public:
	/**
	 * @note Messages hooks are called by the scheduler.
	 * @note Do not use messages hooks where possible. They decrease
	 *  performance of the scheduler.
	 */
	inline void addMessageHook(int messageType, const MessageHook& hook)
	{
		boost::lock_guard<boost::mutex> guard(mSchedulerMutex);
		mHooksMap[messageType].push_back(hook);

	}
	inline void removeMessageHook(int messageType, const MessageHook& hook)
	{
		boost::lock_guard<boost::mutex> guard(mSchedulerMutex);
		MessageHookMap::iterator it = mHooksMap.find(messageType);
		if (mHooksMap.end() != it)
		{
			MessageHookVector::iterator vit = it->second.begin();
			for (; vit != it->second.end(); vit++)
			{
				if (*vit == hook)
				{
					it->second.erase(vit);
					return;
				}
			}
		}
		else
		{
			std::cerr << "Message hook not be found: " << messageType
					<< std::endl;
		}
	}
	inline void removeAllMessageHooks()
	{
		boost::lock_guard<boost::mutex> guard(mSchedulerMutex);
		mHooksMap.clear();

	}
	inline bool isMessageHookPresent(int messageType, const MessageHook& hook)
	{
		boost::lock_guard<boost::mutex> guard(mSchedulerMutex);

		MessageHookMap::iterator it = mHooksMap.find(messageType);
		if (mHooksMap.end() == it)
		{
			return false;
		}

		MessageHookVector::iterator vit = it->second.begin();
		for (; vit != it->second.end(); vit++)
		{
			if (*vit == hook)
			{
				return true;
			}
		}

		return false;
	}

	/// @note Do not use the returned container other than the scheduler thread.
	inline const MessageHookMap& _getMessageHookMap()
	{
		return mHooksMap;
	}

	/**
	 *
	 * @param sender Pointer to the sender: used to avoid infinite loops by
	 *        detecting if a message is send to its creator.
	 */
	inline void postMessage(int type, const Any& param_1 = Any(),
			const Any& param_2 = Any(), void* sender = 0, const Any& receiver =
					Any(), Message::Priority priority = Message::DEFAULT)
	{
		postMessage(Message(type, param_1, param_2, sender, receiver),
				priority);
	}
	/**
	 *
	 */
	inline void postMessage(const Message& message, Message::Priority priority =
			Message::DEFAULT)
	{ // TODO: Make threadsafe!
		if (priority == Message::IMMEDIATE)
		{
			processMessageImmediate(message);
		}
		else
		{
			boost::lock_guard<boost::mutex> guard(mSchedulerMutex);
			mMessageQueue_TimeLimited.push_back(Task(message, priority));
		}
	}
	/**
	 * Timed message with:
	 * - a start time at which the message must be processed
	 * - an end time after which is should no
	 * - a duration time which determine how long this message should be processed.
	 */
	//TODO    inline void postMessage_Timed(const Message& message,
	//                                      double start = 0, double end = 0, double duration = 0,
	//                                      MessageDispatcher* receiver = 0);
	/**
	 * Messages are processed based on their sequence: 1,2,3,... (& a priority for each sequence?)
	 */
	//TODO    inline void postMessage_Sequenced(const Message& message,
	//                                          int sequenceIndex,
	//                                          MessageDispatcher* receiver = 0,
	//                                          Message::Priority priority = Message::DEFUALT);
	/**
	 * Message are processed based on their sequence: 1,2,3,... and time constraints
	 */
	//TODO    inline void postMessage_TimedSequenced(const Message& message, ..., MessageDispatcher* receiver = 0); ???
	//============ Hmm... semi-internal methods << NEXT: From config file?
	/**
	 * @note WIP
	 */
	inline void setAvgMessageProcessingTime(double avgTime)
	{
		mAvgMsgProcessingTime = avgTime;
	}
	/**
	 * Get the time limit that scheduler uses to process the messages.
	 * @return Time limit in miliseconds.
	 */
	double getSchedulerTimeLimit() const
	{
		return mTimeLimit;
	}
	/**
	 * Set a time limit that scheduler uses to process messages.
	 * @param timeLimit Time limit in miliseconds. 1ms will be used if the
	 *        value was less than 1ms.
	 */
	void setSchedulerTimeLimit(double timeLimit);
	/**
	 * Smoothly change the time limit of the scheduler, by interpolating
	 * between the new and old values.
	 * @param suggestedTimeLimit Time limit in miliseconds. 1ms will be used
	 *        if value was less than 1ms.
	 */
	void affectSchedulerTimeLimit(double suggestedTimeLimit);
	/**
	 * Force immediate processing of messages of all queues. Used during object creation.
	 * @note Should not be used because interferes with the concurrent follow of the engine!
	 */
	//TODO???    void _forceProcessMessages(const MessageDispatcher* receiver);
	//TODO???    void _forceProcessMessages(int messageType);
	/**
	 * Removes messages from the queue. One usage ot this method is when
	 * the receiver object was deleted.
	 */
	void discardMessages(const Any& receiver);
	// TODO     void discardMessages(int messageType, const ObjectId& receiver);
	// TODO?    void discardMessages(int messageType);
	/*
	 * Dispatch depending on their type to the registered MessageDelegate
	 * @param message The Message
	 * @param immediate If immediate then the message should be processed immediately
	 * @note In OGE engine this method is implemened in ObjectManager
	 */
	virtual void _dispatchMessage(const Message& message,
			bool immediate = false) = 0;

protected:
	MessageScheduler();
	~MessageScheduler();

	/**
	 * Process tasks of the message queue.
	 * @return false if no message was left in the queue
	 */
	bool processMessages(double tickTime);
	bool processMessageImmediate(const Message& message);
	void resetScheduler();

private:
	bool emptyMessageHook(const Message& message);
};

} /* namespace utils */
#endif /* MESSAGESCHEDULER_H_ */
