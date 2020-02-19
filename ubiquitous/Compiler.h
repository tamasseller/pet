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

#include <cstddef>

#ifndef COMPILER_H_
#define COMPILER_H_

#if defined (__GNUC__)

#define     clz 				__builtin_clz
#define	    really_inline 		__attribute__((always_inline)) inline
#define     section(x) 			__attribute((section(#x)))
#define     PACKED 				__attribute__((packed))
#define     BEFORE_PACKED()
#define     AFTER_PACKED()

#elif defined(_MSC_VER)

#include <intrin.h>
#include <cstdint>
static inline uint32_t clz(uint32_t x) {
    unsigned long r = 0;
    _BitScanReverse(&r, x);
    return (31 - r);
}

#define	    really_inline 		__forceinline
#define     PACKED              __declspec(empty_bases)
#define     BEFORE_PACKED() 	__pragma(pack(push, 1))
#define     AFTER_PACKED() 		__pragma(pack(pop))

#endif 

template <class C, class M>
static constexpr inline size_t unsafeOffsetof(M C::* member) {
    return reinterpret_cast<const char*>(&(reinterpret_cast<C*>(0)->*member)) - reinterpret_cast<const char*>(reinterpret_cast<M*>(0));
}

#endif /* COMPILER_H_ */
