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

#ifndef PET_DATA_PAIR_H_
#define PET_DATA_PAIR_H_

#include "meta/Utility.h"

namespace pet {

template<typename T, typename U>
struct Pair
{
	T first;
	U second;

	template<typename X, typename Y> Pair(X&& a, Y&& b): first(pet::forward<X>(a)), second(pet::forward<Y>(b)) { }
	template<typename X, typename Y> Pair(const Pair<X, Y>& p): Pair(p.first, p.second) { }
	template<typename X, typename Y> Pair(Pair<X, Y>&& p): Pair(pet::move(p.first), pet::move(p.second)) { }
};

template<typename T, typename U> inline static constexpr bool operator==(const Pair<T, U>& x, const Pair<T, U>& y) { return x.first == y.first && x.second == y.second; }
template<typename T, typename U> inline static constexpr bool operator< (const Pair<T, U>& x, const Pair<T, U>& y) { return x.first < y.first || (!(y.first < x.first) && x.second < y.second); }
template<typename T, typename U> inline static constexpr bool operator!=(const Pair<T, U>& x, const Pair<T, U>& y) { return !(x == y); }
template<typename T, typename U> inline static constexpr bool operator> (const Pair<T, U>& x, const Pair<T, U>& y) { return y < x; }
template<typename T, typename U> inline static constexpr bool operator<=(const Pair<T, U>& x, const Pair<T, U>& y) { return !(y < x); }
template<typename T, typename U> inline static constexpr bool operator>=(const Pair<T, U>& x, const Pair<T, U>& y) { return !(x < y); }

template<typename T, typename U> Pair(T, U) -> Pair<T, U>;

}

#endif /* PET_DATA_PAIR_H_ */
