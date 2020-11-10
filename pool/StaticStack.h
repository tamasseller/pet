/*******************************************************************************
 *
 * Copyright (c) 2020 Tamás Seller. All rights reserved.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *******************************************************************************/

#ifndef STATICSTACK_H_
#define STATICSTACK_H_

namespace pet {

template<class T, unsigned int size>
class StaticStack {
	uint16_t idx;
	T data[size];

public:
	class Iterator {
		T* state;
		friend StaticStack;
		inline Iterator(T* state): state(state) {}
	public:
		T* current();
		void step(StaticStack&);
	};

	inline bool push(const T& d);
	inline bool pop(T& d);
	inline void clear();

	inline Iterator iterator();
	inline StaticStack(): idx(0) {}
};

template<class T, unsigned int size>
inline bool StaticStack<T, size>::push(const T& d)
{
	if(idx < size) {
		data[idx++] = d;
		return true;
	}

	return false;
}

template<class T, unsigned int size>
inline bool StaticStack<T, size>::pop(T& d)
{
	if(idx > 0) {
		d = data[--idx];
		return true;
	}

	return false;
}

template<class T, unsigned int size>
inline void StaticStack<T, size>::clear()
{
	idx = 0;
}

template<class T, unsigned int size>
inline typename StaticStack<T, size>::Iterator StaticStack<T, size>::iterator()
{
	return idx ? data : 0;
}

template<class T, unsigned int size>
inline T* StaticStack<T, size>::Iterator::current()
{
	return state;
}

template<class T, unsigned int size>
inline void StaticStack<T, size>::Iterator::step(StaticStack& stack)
{
	if(state && state - stack.data < stack.idx - 1)
		state++;
	else
		state = 0;
}

}

#endif /* STATICSTACK_H_ */
