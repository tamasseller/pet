/*******************************************************************************
 *
 * Copyright (c) 2016-2020 Seller Tamás. All rights reserved.
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

#define PET_COMPILER_IS_GCC

#if defined(__amd64__) || defined(__amd64) || defined(__x86_64__) || defined(__x86_64)
#define PET_TARGET_IS_PC
#elif defined(__arm__) && defined(__thumb__)
#if defined (__ARM_ARCH_6M__)
#define PET_TARGET_IS_CM0
#elif defined (__ARM_ARCH_7M__)
#define PET_TARGET_IS_CM3
#elif defined (__ARM_ARCH_7EM__)
#define PET_TARGET_IS_CM4
#endif
#endif

#define	    really_inline 		__attribute__((always_inline)) inline
#define     section(x) 			__attribute((section(#x)))

#elif defined(_MSC_VER)

#define PET_COMPILER_IS_MSVC
#define PET_TARGET_IS_PC

#define	    really_inline 		__forceinline

#endif 

template <class C, class M>
static constexpr inline size_t unsafeOffsetof(M C::* member) {
    return reinterpret_cast<const char*>(&(reinterpret_cast<C*>(0)->*member)) - reinterpret_cast<const char*>(reinterpret_cast<M*>(0));
}

#endif /* COMPILER_H_ */
