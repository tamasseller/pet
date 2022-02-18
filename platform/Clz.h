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

#ifndef CLZ_H_
#define CLZ_H_

#include "Compiler.h"

#if defined(PET_COMPILER_IS_GCC)

#ifndef clz
#define     clz                 __builtin_clz
#endif

#elif defined(PET_COMPILER_IS_MSVC)

#include <intrin.h>
#include <cstdint>
static inline uint32_t clz(uint32_t x) {
    unsigned long r = 0;
    _BitScanReverse(&r, x);
    return (31 - r);
}

#endif

template<typename N>
constexpr static inline size_t ilog2(N n) {
    return (n<2) ? 0 : 1 + ilog2((n + 1) / 2);
}

#endif /* CLZ_H_ */
