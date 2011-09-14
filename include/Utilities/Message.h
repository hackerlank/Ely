/**
 * \file Message.h
 *
 * \date 14/set/2011
 * \author Marco Paone
 */

#ifndef MESSAGE_H_
#define MESSAGE_H_

#include <boost/any.hpp>

#include "Utilities/Prerequisites.h"
#include "Utilities/fastdelegate.h"

namespace utils
{

/**
 * Contains information that is to be added to a MessageQueue for other
 * code to handle.
 *
 * @author Steven Gay, Christopher Jones, Mehdi Toghianifar
 */
class Message
{
public:
	/**
	 * @enum Message type used by the receiver to determine if he can handle
	 *       the message. If message type are created at run-time use
	 *       Type::LAST+x for a new type id.
	 *
	 * @brief Message types are associated to predefined oge methods.
	 *        The user can use additional int value to create custom message.
	 */
	enum Type
	{
		//Common
		EMPTY = 0, // Those msg will be ignored
		LAST
	// Used as marker for the highest predefined type value
	};
	/**
	 * @enum Priority Used by the scheduler to determine urgency of message
	 *
	 * @var HIGH   = Message must be processed at once
	 * @var MEDIUM = Message should be processed in the actual cycle
	 * @var LOW    = Message processing can be postponed
	 */
	enum Priority
	{
		LOW = 0, MEDIUM, HIGH, IMMEDIATE, DEFAULT = MEDIUM
	};
protected:
	/**
	 * Message type used to define what the message does
	 */
	int mType;
	/**
	 * First generic data. It must be type cast to the correct type
	 */
	Any mParam_1;
	/**
	 * Second generic data. It must be type cast to the correct type
	 */
	Any mParam_2;
	/**
	 * Sender can be either a pointer or a string id
	 * @note Also used to avoid infinite loops by detecting
	 *       if a message is send to its creator
	 */
	void* mSender;
	/**
	 * Receiver can be either a pointer or a string id
	 * TODO Should be a void* or size_t for efficiency
	 */
	Any mReceiver;

public:
	/**
	 * Default Constructor. Should not be used.
	 * @note NEXT: Public because concurrent queue needs it!
	 */
	Message() :
			mType(Message::EMPTY), mSender(0)
	{
	}
	/**
	 * Message constructor
	 *
	 * @param[in] type      Message type (usually the Game Time not the local time)
	 * @param[in] param_1   First optional data to send (default Any())
	 * @param[in] param_2   Second optional data to send (default Any())
	 * @param[in] sender    Message sender (default Any())
	 * @param[in] receiver  Message receiver (default Any())
	 */
	inline Message(int type, const Any& param_1 = Any(), const Any& param_2 =
			Any(), void* sender = 0, const Any& receiver = Any()) :
			mType(type), mParam_1(param_1), mParam_2(param_2), mSender(sender), mReceiver(
					receiver)
	{
	}
	/// Operator == defined for the std::find algorithm, hence ONLY compares the message type
	inline bool operator==(const Message& rhs) const
	{
		return rhs.mType == mType;
	}
	/// Destructor
	virtual ~Message()
	{
	}

	/// Operator used in the MessageList
	inline bool operator <(const Message& msg) const
	{
		//TODO implement message priorities
		//if ( mPriority > msg.priority )
		if (msg.mType < mType)
			return false;
		return true;
	}

	inline int getType() const
	{
		return mType;
	}
	inline void setType(int type)
	{
		mType = type;
	}

	inline Any& getParam_1()
	{
		return mParam_1;
	}
	;
	inline const Any& getParam_1() const
	{
		return mParam_1;
	}
	;
	inline void setParam_1(const Any& param)
	{
		mParam_1 = param;
	}
	;

	inline Any& getParam_2()
	{
		return mParam_2;
	}
	;
	inline const Any& getParam_2() const
	{
		return mParam_2;
	}
	;
	inline void setParam_2(const Any& param)
	{
		mParam_2 = param;
	}
	;

	inline const void* getSender() const
	{
		return mSender;
	}
	inline void setSender(void* sender)
	{
		mSender = sender;
	}

	inline const Any& getReceiver() const
	{
		return mReceiver;
	}
	inline void setReceiver(const Any& receiver)
	{
		mReceiver = receiver;
	}
};

/**
 * MessageMethod : Used by receivers to send the message
 * to the appropriate method
 */
typedef fastdelegate::FastDelegate1<const Message&, void> MessageDelegate;

/**
 * MessageHook : Used by receivers to catch the message
 * before its appropriate handlers
 *
 * @return False if the message should not be processed by the handlers
 */
typedef fastdelegate::FastDelegate1<const Message&, bool> MessageHook;

} /* namespace utils */
#endif /* MESSAGE_H_ */
