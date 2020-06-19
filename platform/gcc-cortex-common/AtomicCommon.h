/*******************************************************************************
 *
 * Copyright (c) 2017 Seller Tamás. All rights reserved.
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

#ifndef ATOMICCOMMON_H_
#define ATOMICCOMMON_H_

namespace home {

namespace CortexCommon {

template<class Value, Value (*ldrex)(volatile Value*), bool (*strex)(volatile Value*, Value), void (*clrex)()>
class AtomicCommon
{
	volatile Value data;
public:
	inline AtomicCommon(): data(0) {}

	inline AtomicCommon(Value value) {
		data = value;
		clrex();        // TODO is this ok (for example: preempted between write and clrex)?
	}

	inline operator Value() {
		return data;
	}

	template<class Op, class... Args>
	inline Value operator()(Op&& op, Args... args)
	{
		Value old, result;
		do {
			old = ldrex(&this->data);

			if(!op(old, result, args...)) {
				clrex();
				break;
			}

		} while(!strex(&this->data, result));

		return old;
	}
};

}

}

#endif /* ATOMICCOMMON_H_ */
