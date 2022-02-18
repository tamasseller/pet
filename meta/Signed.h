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

#ifndef PET_META_SIGNED_H_
#define PET_META_SIGNED_H_

namespace pet {

namespace detail {
    template<class T> struct SignedVersionOf { using type = T; };
    template<> struct SignedVersionOf<char> { using type = signed char; };
    template<> struct SignedVersionOf<unsigned char> { using type = signed char; };
    template<> struct SignedVersionOf<unsigned short> { using type = signed short; };
    template<> struct SignedVersionOf<unsigned int> { using type = signed int; };
    template<> struct SignedVersionOf<unsigned long> { using type = signed long; };
    template<> struct SignedVersionOf<unsigned long long> { using type = signed long; };
}

template<class T> using Signed = typename detail::SignedVersionOf<T>::type;

}

#endif /* PET_META_SIGNED_H_ */
