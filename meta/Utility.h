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

#ifndef UTILITY_H_
#define UTILITY_H_

#include <stddef.h>

namespace pet
{

namespace detail {
    template<typename T> struct remove_reference { typedef T type; };
    template<typename T> struct remove_reference<T&> { typedef T type; };
    template<typename T> struct remove_reference<T&&> { typedef T type; };

    struct false_type { static constexpr auto value = false; };
    struct true_type { static constexpr auto value = true; };
    template<typename> struct is_lvalue_reference: public false_type { };
    template<typename _Tp> struct is_lvalue_reference<_Tp&>: public true_type { };
}

template<typename T>
constexpr T&& forward(typename detail::remove_reference<T>::type& t) {
    return static_cast<T&&>(t);
}

template<typename T>
constexpr T&& forward(typename detail::remove_reference<T>::type&& t)
{
    static_assert(!detail::is_lvalue_reference<T>::value, "template argument substituting T is an lvalue reference type");
    return static_cast<T&&>(t);
}

template<typename T>
constexpr typename detail::remove_reference<T>::type&& move(T&& t) {
    return static_cast<typename detail::remove_reference<T>::type&&>(t);
}

}

#endif /* UTILITY_H_ */
