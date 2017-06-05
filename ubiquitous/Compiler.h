/*******************************************************************************
 *
 * Copyright (c) 2016, 2017 Seller Tamás. All rights reserved.
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

#ifndef COMPILER_H_
#define COMPILER_H_

#if defined (__GNUC__) && (__ARM_ARCH == 7)
#define clz __builtin_clz
#else
static inline unsigned int clz(unsigned int data) {
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


#define	really __attribute__((always_inline))

#endif /* COMPILER_H_ */
