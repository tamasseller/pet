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

#ifndef TYPEMANIPULATION_H_
#define TYPEMANIPULATION_H_

namespace pet {

namespace detail {

template< class T > struct RreferenceRemover {typedef T type;};
template< class T > struct RreferenceRemover<T&> {typedef T type;};
template< class T > struct RreferenceRemover<T&&> {typedef T type;};


}

template<class T>
using removeReference = typename detail::RreferenceRemover<T>::type;

template<class T, class U> struct sameTypes { static constexpr bool value = false; };
template<class T> struct sameTypes<T, T> { static constexpr bool value = true; };

template<bool B, class T=void> struct enableIf {};
template<class T> struct enableIf<true, T> { typedef T type; };


}

#endif /* TYPEMANIPULATION_H_ */
