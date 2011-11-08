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
 * \file /Ely/include/utilities/thread/Atomic.h
 *
 * \date 08/nov/2011
 * \author marco
 */

#ifndef ATOMIC_H_
#define ATOMIC_H_

namespace ely
{

/**
 * \brief ely::Atomic consistent Atomic interface using boost::atomic
 */
template<typename T>
class Atomic
{
protected:
	boost::atomic<T> mValue;
public:
	inline Atomic(const T& value)
	{
		mValue = value;
	}
	inline Atomic& operator=(const Atomic<T>& value)
	{
		mValue = value.mValue;
		return *this;
	}
	inline Atomic& operator=(const T& value)
	{
		mValue = value;
		return *this;
	}
	inline operator T() const
	{
		return mValue;
	}
	inline const Atomic& operator++() //prefix
	{
		++mValue;
		return *this;
	}
	inline T operator++(int) //postfix
	{
		return mValue++;
	}
	inline const Atomic& operator--() //prefix
	{
		--mValue;
		return *this;
	}
	inline T operator--(int) //postfix
	{
		return mValue--;
	}
	inline Atomic& operator+=(const Atomic& value)
	{
		mValue += value;
		return *this;
	}
	inline Atomic& operator+=(const T& value)
	{
		mValue += value;
		return *this;
	}
	inline Atomic& operator-=(const Atomic& value)
	{
		mValue -= value;
		return *this;
	}
	inline Atomic& operator-=(const T& value)
	{
		mValue -= value;
		return *this;
	}
};

} /* namespace ely */
#endif /* ATOMIC_H_ */
