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

#if defined (__GNUC__)
#if (__ARM_ARCH == 7)
#ifndef clz
#define clz __builtin_clz
#endif
#else
template<typename Type>
static inline unsigned int clz(Type data) {
	static constexpr unsigned int bits = sizeof(data) * 8;
	unsigned int shift, count = bits - 1;
	Type value;

	if (data == 0)
		return bits;

	for (shift = bits / 2; shift; shift /= 2) {
		value = data >> shift;

		if (value) {
			data = value;
			count = count - shift;
		}
	}

	return count;
}
#endif // __ARM_ARCH == 7

#define	really 		__attribute__((always_inline))
#define section(x) 	__attribute((section(#x)))

#endif // __GNUC__

#endif /* COMPILER_H_ */
