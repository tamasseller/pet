/*******************************************************************************
 *
 * Copyright (c) 2016 Seller Tamás. All rights reserved.
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

#ifndef GENERAL_H_
#define GENERAL_H_

// GCC
#if 1
#define clz __builtin_clz
#else
// not tested, from https://community.arm.com/thread/6400
unsigned int clz(unsigned int data) {
	unsigned int shift, value, count = 31;

	if (data == 0)
		return 32;

	for (shift = 16; shift; shift >>= 1) {
		value = data >> shift;

		if (value) {
			data = value;
			count = count - shift;
		}
	}

	return count;
}
#endif



struct General {
	constexpr inline static unsigned int log2floorConst(const unsigned int x) {
		return ((x==0)?
					(32):
					((x==1)?
							0:
							(log2floorConst(x/2)+1)
					)
				);
	}

	__attribute__ ((always_inline))
	inline static unsigned int log2floor(unsigned int x) {
		return 31-clz(x);
	}
};

template<typename T>
void swap(T& a, T& b){
	T temp = a;
	a = b;
	b = temp;
}

#endif /* GENERAL_H_ */
