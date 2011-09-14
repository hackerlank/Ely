/**
 * \file MessageList.h
 *
 * \date 14/set/2011
 * \author Marco Paone
 */

#ifndef MESSAGELIST_H_
#define MESSAGELIST_H_

#include "Utilities/Prerequisites.h"
#include "Utilities/Message.h"

namespace utils
{

/**
 * List of messages
 *
 * @author Mehdi Toghianifar, Steven 'lazalong' Gay
 */
class MessageList
{
	/// Used by std::list to order the message by type
	struct lessType: public std::binary_function<Message, Message, bool>
	{
		/// functor for operator<
		bool operator()(const Message& _left, const Message& _right) const
		{
			return _left.getType() < _right.getType();
		}
	};

public:
	typedef std::set<Message, lessType> Set;
	typedef Set::iterator Iter;
	typedef Set::const_iterator ConstIter;

private:
	mutable Message temp; /// Enhances the speed of the find() methods
	mutable ConstIter iter; /// Enhances the speed of the find() methods

public:
	Set list;

public:
	virtual ~MessageList()
	{
		clear();
	}

	inline void clear()
	{
		list.clear();
		iter = list.end();
	}

	/**
	 * Add a message to the list
	 * @param messageType for example an ObjectMessage::SET_POSITION
	 * @param param_1 first message parameter (default Any() which indicate an empty param)
	 * @param param_2 second message parameter (default Any() which indicate an empty param)
	 */
	inline void add(int messageType, const Any& param_1 = Any(),
			const Any& param_2 = Any())
	{
		add(Message(messageType, param_1, param_2));
	}
	/**
	 * Add a message to the list
	 */
	inline void add(Message message)
	{
		assert(list.empty() || list.end() == list.find(message));
		list.insert(message);
	}
	/**
	 * Return true if the message type is found in the list.
	 *
	 * @param messageType for example an ObjectMessage::SET_POSITION
	 */
	inline bool find(int messageType) const
	{
		temp.setType(messageType);
		return list.find(temp) != list.end();
	}
	/**
	 * Get the message param in the list. Return true if found.
	 *
	 * @note The other overloaded method that accepts reference to the
	 *       pointer of the template type, is faster than this method.
	 * @note The parameter is cast so if the message param
	 *       is not of the right type a segfault can occur.
	 * @param messageType for example an ObjectMessage::SET_POSITION
	 * @param param_1 the first parameter of the message.
	 */
	template<typename T1> inline
	bool find(int messageType, T1& param_1) const
	{
		temp.setType(messageType);
		if ((iter = list.find(temp)) == list.end())
			return false;
		param_1 = FastAnyCast(T1, iter->getParam_1());
		return true;
	}
	/**
	 * Get the message param in the list. Return true if found.
	 *
	 * @note The other overloaded method that accepts reference to the
	 *       pointer of the template type, is faster than this method.
	 * @note The parameters is cast so if the message param
	 *       is not of the right type a segfault can occur.
	 * @param messageType for example an ObjectMessage::SET_POSITION
	 * @param param_1 the first parameter of the message.
	 * @param param_2 the first parameter of the message.
	 */
	template<typename T1, typename T2> inline
	bool find(int messageType, T1& param_1, T2& param_2) const
	{
		temp.setType(messageType);
		if ((iter = list.find(temp)) == list.end())
			return false;
		param_1 = FastAnyCast(T1, iter->getParam_1());
		param_2 = FastAnyCast(T2, iter->getParam_2());
		return true;
	}
	/**
	 * Get the message param in the list. Return true if found.
	 *
	 * @note The parameter is cast so if the message param
	 *       is not of the right type null will be returned.
	 * @param messageType for example an ObjectMessage::SET_POSITION
	 * @param param_1 the first parameter of the message.
	 */
	template<typename T1> inline
	bool find(int messageType, const T1* & param_1) const
	{
		temp.setType(messageType);
		if ((iter = list.find(temp)) == list.end())
			return false;
		param_1 = FastAnyCast(T1, &iter->getParam_1());
		assert(param_1);
		return (0 != param_1);
	}
	/**
	 * Get the message param in the list. Return true if found.
	 *
	 * @note The parameter is cast so if the message param
	 *       is not of the right type null will be returned.
	 * @param messageType for example an ObjectMessage::SET_POSITION
	 * @param param_1 the first parameter of the message.
	 * @param param_2 the first parameter of the message.
	 */
	template<typename T1, typename T2> inline
	bool find(int messageType, const T1* & param_1, const T2* & param_2) const
	{
		temp.setType(messageType);
		if ((iter = list.find(temp)) == list.end())
			return false;
		param_1 = FastAnyCast(T1, &iter->getParam_1());
		param_2 = FastAnyCast(T2, &iter->getParam_2());
		assert(param_1);
		assert(param_2);
		return (0 != param_1 && 0 != param_2);
	}
};

} /* namespace utils */
#endif /* MESSAGELIST_H_ */
