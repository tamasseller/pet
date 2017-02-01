/*******************************************************************************
 *
 * Copyright (c) 2016 Seller Tamás. All rights reserved.
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

#ifndef RANGE_H_
#define RANGE_H_

namespace meta {
namespace detail {
	template<template <unsigned int> class F, unsigned int Min>
	struct Range {
		template<unsigned int N, unsigned int... theRest>
		struct Runner{
			static constexpr auto &value = Runner<N-1, N, theRest...>::value;
		};

		template<unsigned int... theRest>
		struct Runner<Min, theRest...>{
			static constexpr unsigned int value[] = {F<Min>::value, F<theRest>::value...};
		};
	};

	template<template <unsigned int> class F, unsigned int Min>
	template<unsigned int... theRest>
	constexpr unsigned int Range<F, Min>::Runner<Min, theRest...>::value[];

	template<unsigned int x> struct Identity {
		static constexpr unsigned int value = x;
	};
}

template<unsigned int Min, unsigned int Max>
using range = typename detail::Range<detail::Identity, Min>::template Runner<Max>;

template<template <unsigned int> class F, unsigned int Min, unsigned int Max>
using applyOverRange = typename detail::Range<F, Min>::template Runner<Max>;

}

#endif /* RANGE_H_ */
