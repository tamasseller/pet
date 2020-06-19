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

#ifndef ATOMIC_H_
#define ATOMIC_H_

#include "Compiler.h"

#if defined(PET_COMPILER_IS_MSVC) && defined(PET_TARGET_IS_PC)


#include "msvc-x86-64/Atomic.h"

#elif defined(PET_COMPILER_IS_GCC) && defined(PET_TARGET_IS_PC)

#include "gcc-x86-64/Atomic.h"

#elif defined(PET_COMPILER_IS_GCC) && defined(PET_TARGET_IS_CM0)

#include "gcc-cortex-m0/Atomic.h"

#endif

#endif /* ATOMIC_H_ */
