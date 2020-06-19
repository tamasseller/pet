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

#ifndef ATOMIC_IMPL_H_
#define ATOMIC_IMPL_H_

#include "../x86-64-common/AtomicCommon.h"

namespace pet {

namespace detail
{
    struct CasHolder
    {
        template<class T>
        static inline bool cas(volatile T *ptr, T oldval, T newval) {
            return __sync_bool_compare_and_swap(ptr, oldval, newval);
        }
    };
}

template<class Data>
using Atomic = IntelArchCommon::Atomic<Data, detail::CasHolder>;

}

#endif /* ATOMIC_IMPL_H_ */
