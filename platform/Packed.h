/*******************************************************************************
 *
 * Copyright (c) 2020 Seller Tamás. All rights reserved.
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

#ifndef PACKED_H_
#define PACKED_H_

#include "Compiler.h"

#if defined(PET_COMPILER_IS_GCC)

#define     PACKED              __attribute__((packed))
#define     BEFORE_PACKED()
#define     AFTER_PACKED()

#elif defined(PET_COMPILER_IS_MSVC)

#define     PACKED              __declspec(empty_bases)
#define     BEFORE_PACKED()     __pragma(pack(push, 1))
#define     AFTER_PACKED()      __pragma(pack(pop))

#endif

#endif /* PACKED_H_ */
