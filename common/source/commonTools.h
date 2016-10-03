/**
 * \file commonTools.h
 *
 * \date 2016-10-03
 * \author consultit
 */
#ifndef COMMONTOOLS_H_
#define COMMONTOOLS_H_

#include "genericAsyncTask.h"
#include "plist.h"

#ifdef _WIN32
#include <ciso646>
#define STRTOF (float)strtod
#else
#define STRTOF strtof
#endif

//continue if condition is true else return a value
#define CONTINUE_IF_ELSE_R(condition, return_value) \
  { \
    if (!(condition)) { \
      return return_value; \
    } \
  }
//continue if condition is true else return (void)
#define CONTINUE_IF_ELSE_V(condition) \
  { \
    if (!(condition)) { \
      return; \
    } \
  }

/**
 * An automatic Singleton Utility.
 *
 * \note This Singleton class is based on the article "An automatic
 * Singleton Utility" by Scott Bilas in "Game Programming Gems 1" book.
 * Non multi-threaded.
 */
template<typename T> class Singleton
{
	static T* ms_Singleton;

public:
	Singleton(void)
	{
		assert(!ms_Singleton);
		unsigned long int offset = (unsigned long int) (T*) 1
				- (unsigned long int) (Singleton<T>*) (T*) 1;
		ms_Singleton = (T*) ((unsigned long int) this + offset);
	}
	~Singleton(void)
	{
		assert(ms_Singleton);
		ms_Singleton = 0;
	}
	static T& GetSingleton(void)
	{
		assert(ms_Singleton);
		return (*ms_Singleton);
	}
	static T* GetSingletonPtr(void)
	{
		return (ms_Singleton);
	}
};

template<typename T> T* Singleton<T>::ms_Singleton = 0;

/**
 * A std::pair wrapper
 */
template<typename T1, typename T2> struct Pair
{
PUBLISHED:
	Pair() :
			mPair()
	{
	}
	Pair(const T1& first, const T2& second) :
			mPair(first, second)
	{
	}
	bool operator== (const Pair &other) const
	{
		return mPair == other.mPair;
	}
	INLINE void set_first(const T1& first)
	{
		mPair.first = first;
	}
	INLINE T1 get_first() const
	{
		return mPair.first;
	}
	INLINE void set_second(const T2& second)
	{
		mPair.second = second;
	}
	INLINE T2 get_second() const
	{
		return mPair.second;
	}

public:
	T1& first()
	{
		return mPair.first;
	}
	T2& second()
	{
		return mPair.second;
	}
private:
	pair<T1, T2> mPair;
};

/**
 * A pair that can be used with PT/CPT (C++ only)
 */
template<typename T1, typename T2> struct PairRC: public Pair<T1, T2>,
		public ReferenceCount
{
public:
	PairRC() :
			Pair<T1, T2>()
	{
	}
	PairRC(const T1& first, const T2& second) :
			Pair<T1, T2>(first, second)
	{
	}
};

/**
 * Template struct for generic Task Function interface
 *
 * The effective Tasks are composed by a Pair of an object and
 * a method (member function) doing the effective task.
 * To register a task:
 * 1) in class A define a (pointer to) TaskData member:
 * \code
 * 	SMARTPTR(TaskInterface<A>::TaskData) myData;
 * \endcode
 * 2) and a method (that will execute the real task) with signature:
 * \code
 * 	AsyncTask::DoneStatus myTask(GenericAsyncTask* task);
 * \endcode
 * 3) in code associate to myData a new TaskData referring to this
 * class instance and myTask, then create a new GenericAsyncTask
 * referring to taskFunction and with data parameter equal to
 * myData (reinterpreted as void*):
 * \code
 * 	myData = new TaskInterface<A>::TaskData(this, &A::myTask);
 * 	AsyncTask* task = new GenericAsyncTask("my task",
 * 							&TaskInterface<A>::taskFunction,
 * 							reinterpret_cast<void*>(myData.p()));
 * \endcode
 * 4) finally register the async-task to your manager:
 * \code
 * 	pandaFramework.get_task_mgr().add(task);
 * 	\endcode
 * From now on myTask will execute the task, while being able
 * to refer directly to data members of the A class instance.
 */
template<typename A> struct TaskInterface
{
	typedef AsyncTask::DoneStatus (A::*TaskPtr)(GenericAsyncTask* taskFunction);
	typedef PairRC<A*, TaskPtr> TaskData;
	static AsyncTask::DoneStatus taskFunction(GenericAsyncTask* task,
			void * data)
	{
		TaskData* appData = reinterpret_cast<TaskData*>(data);
		return ((appData->first())->*(appData->second()))(task);
	}
};

/**
 * Throwing event data.
 *
 * Data related to throwing events by components.
 */
class ThrowEventData
{
	struct Period
	{
		Period() :
				mPeriod(1.0 / 30.0)
		{
		}
		Period(float period) :
				mPeriod(period)
		{
		}
		Period& operator =(const Period& other)
		{
			mPeriod = other.mPeriod;
			return *this;
		}
		Period& operator =(float value)
		{
			mPeriod = value;
			return *this;
		}
		operator float() const
		{
			return mPeriod;
		}
	private:
		float mPeriod;
	};
	struct Frequency
	{
		Frequency() :
				mFrequency(30.0), mPeriod(1.0 / 30.0), mEventData(NULL)
		{
		}
		Frequency(float value) :
				mFrequency(value >= 0 ? value : -value)
		{
			mFrequency <= FLT_MIN ?
					mPeriod = FLT_MAX : mPeriod = 1.0 / mFrequency;
			mEventData = NULL;
		}
		Frequency& operator =(const Frequency& other)
		{
			mFrequency = other.mFrequency;
			mPeriod = other.mPeriod;
			return *this;
		}
		Frequency& operator =(float value)
		{
			value >= 0 ? mFrequency = value : mFrequency = -value;
			mFrequency <= FLT_MIN ?
					mPeriod = FLT_MAX : mPeriod = 1.0 / mFrequency;
			if (mEventData != NULL)
			{
				mEventData->mPeriod = mPeriod;
			}
			return *this;
		}
		operator float() const
		{
			return mFrequency;
		}
		void setEventData(ThrowEventData *eventData)
		{
			mEventData = eventData;
		}
	private:
		float mFrequency;
		Period mPeriod;
		ThrowEventData *mEventData;
	};

public:
	ThrowEventData() :
			mEnable(false), mEventName(string("")), mThrown(false), mTimeElapsed(
					0), mCount(0), mFrequency(30.0)
	{
		mFrequency.setEventData(this);
	}
	bool mEnable;
	string mEventName;
	bool mThrown;
	float mTimeElapsed;
	unsigned int mCount;
	Frequency mFrequency;
	Period mPeriod;

public:
	void write_datagram(Datagram &dg) const;
	void read_datagram(DatagramIterator &scan);
};

/**
 * Declarations for parameters management.
 */
typedef multimap<string, string> ParameterTable;
typedef multimap<string, string>::iterator ParameterTableIter;
typedef multimap<string, string>::const_iterator ParameterTableConstIter;
typedef map<string, ParameterTable> ParameterTableMap;
typedef pair<string, string> ParameterNameValue;

/**
 * Template function for conversion values to string.
 */
template<typename Type> string str(Type value)
{
	return static_cast<ostringstream&>(ostringstream().operator <<(value)).str();
}

/**
 * Parses a string composed by substrings separated by a character
 * separator.
 * \note all blanks are erased before parsing.
 * @param srcCompoundString The source string.
 * @param separator The character separator.
 * @return The substrings vector.
 */
pvector<string> parseCompoundString(
		const string& srcCompoundString, char separator);

/**
 * \brief Into a given string, replaces any occurrence of a character with
 * another character.
 * @param source The source string.
 * @param character Character to be replaced .
 * @param replacement Replaced character.
 * @return The result string.
 */
string replaceCharacter(const string& source, int character,
		int replacement);

/**
 * \brief Into a given string, erases any occurrence of a given character.
 * @param source The source string.
 * @param character To be erased character.
 * @return The result string.
 */
string eraseCharacter(const string& source, int character);

///ValueList template
template<typename Type>
class ValueList
{
PUBLISHED:
	ValueList(unsigned int size=0);
	ValueList(const ValueList &copy);
	INLINE ~ValueList();

	INLINE void operator =(const ValueList &copy);
	INLINE bool operator== (const ValueList &other) const;
	INLINE void add_value(const Type& value);
	bool remove_value(const Type& value);
	bool has_value(const Type& value) const;
	void add_values_from(const ValueList &other);
	void remove_values_from(const ValueList &other);
	INLINE void clear();
	INLINE int get_num_values() const;
	INLINE Type get_value(int index) const;
	MAKE_SEQ(get_values, get_num_values, get_value);
	INLINE Type operator [](int index) const;
	INLINE int size() const;
	INLINE void operator +=(const ValueList &other);
	INLINE ValueList operator +(const ValueList &other) const;

#ifndef CPPPARSER
public:
	operator plist<Type>() const;
	operator pvector<Type>() const;
#endif //CPPPARSER

private:
	pvector<Type> _values;
};

///Result values
#define RESULT_SUCCESS 0
#define RESULT_ERROR -1

///inline
#include "commonTools.I"

#endif /* COMMONTOOLS_H_ */
