/*******************************************************************************
 *
 * Copyright (c) 2020 Seller TamĂˇs. All rights reserved.
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

#ifndef MATH_H_
#define MATH_H_

namespace pet
{

template<class T>
static inline constexpr T max(const T& a, const T& b) {
    return a > b ? a : b;
}

template<class T>
static inline constexpr T min(const T& a, const T& b) {
    return a < b ? a : b;
}

template<class T, class... Ts>
static inline constexpr T max(const T& a, const Ts&... as) {
    return max(a, max(as...));
}

}

#endif /* MATH_H_ */
