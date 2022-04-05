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

#ifndef ATOMIC_H_
#define ATOMIC_H_

#include "Compiler.h"

#if defined(PET_COMPILER_IS_MSVC) && defined(PET_TARGET_IS_PC)
#include "msvc-x86-64/Atomic.h"
#elif defined(PET_COMPILER_IS_GCC) && defined(PET_TARGET_IS_PC)
#include "gcc-x86-64/Atomic.h"
#elif defined(PET_COMPILER_IS_GCC) && (defined(PET_TARGET_IS_CM0))
#include "gcc-armv6m/Atomic.h"
#elif defined(PET_COMPILER_IS_GCC) && defined(PET_TARGET_IS_CM3) || defined(PET_TARGET_IS_CM4)
#include "gcc-armv7m/Atomic.h"
#endif

namespace pet {

template<class Data>
struct Atomic: BaseAtomic<Data>
{
    using BaseAtomic<Data>::BaseAtomic;
    using BaseAtomic<Data>::operator Data;

    /**
     * Atomically increment.
     *
     * Returns previous value.
     */
    really_inline Data increment(Data v = Data(1))
    {
        return this->Atomic::operator()([v](Data old, Data &result, Data desired)
        {
            result = old + desired;
            return true;
        }, v);
    }

    /**
     * Atomically decrement.
     *
     * Returns previous value.
     */
    really_inline Data decrement(Data v = Data(1))
    {
        return this->Atomic::operator()([v](Data old, Data &result, Data desired)
        {
            result = old - desired;
            return true;
        }, v);
    }

    /**
     * Atomically set value.
     *
     * Returns previous value.
     */
    really_inline Data set(Data v)
    {
        return this->Atomic::operator()([](Data old, Data &result, Data desired)
        {
            result = desired;
            return true;
        }, v);
    }

    /**
     * Atomically set value if new one is greater.
     *
     * Returns previous value.
     */
    really_inline Data setIfGreater(Data v)
    {
        return this->Atomic::operator()([](Data old, Data &res, Data desired)
        {
            res = desired;
            return old < desired;
        }, v);
    }

    /**
     * Atomically set value if new one is greater.
     *
     * Returns previous value.
     */
    really_inline Data setIfLess(Data v)
    {
        return this->Atomic::operator()([](Data old, Data &res, Data desired)
        {
			res = desired;
			return desired < old;
        }, v);
    }

};

}

#endif /* ATOMIC_H_ */
