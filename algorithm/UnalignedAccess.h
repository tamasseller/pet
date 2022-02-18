/*******************************************************************************
 *
 * Copyright (c) 2021 Tamás Seller. All rights reserved.
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

#ifndef PET_ALGORITHM_UNALIGNEDACCESS_H_
#define PET_ALGORITHM_UNALIGNEDACCESS_H_

namespace pet {

template<class T>
inline void unalignedWrite(void* addr, const T& v)
{
    struct __attribute__((packed)) D {
        T v;
    };

    ((D*)addr)->v = v;
}

template<class T>
inline auto unalignedRead(const void* addr)
{
    struct __attribute__((packed)) D {
        T v;
    };

    return ((D*)addr)->v;
}

}

#endif /* PET_ALGORITHM_UNALIGNEDACCESS_H_ */
