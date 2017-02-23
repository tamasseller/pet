/*******************************************************************************
 *
 * Copyright (c) 2016, 2017 Seller Tamás. All rights reserved.
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

#ifndef PACKS_H_
#define PACKS_H_

namespace pet {
#ifndef DOXYGEN_SHOULD_SKIP_THIS
namespace detail {
	/**
	 * First type parameter of the pack.
	 *
	 * This is rather trivial, splits the first element from the pack and returns that.
	 */
	template <class T, class... Ts>
	struct FirstArgumentPublisher {
		typedef T type;
	};

	/**
	 * Recursive iteration for the last element.
	 *
	 * Works its way down to the last element by spliting the first parameter in each step.
	 */
	template <class T, class... Ts>
	class LastArgumentWorker: protected LastArgumentWorker<Ts...> {
		protected:
			typedef typename LastArgumentWorker<Ts...>::type type;
	};

	/**
	 * Recursion stopper.
	 *
	 * When the recursive references consume all the inputs but the last,
	 * this one simply returns the last parameter, thus stopping the recursion.
	 */
	template <class T>
	class LastArgumentWorker<T> {
		protected:
			typedef T type;
	};

	/**
	 * Visibility control.
	 *
	 * This is last layer would not be strictly necessary, it is used for hiding
	 * the intermediate stages of the recursion.
	 */
	template<class... Types>
	class LastArgumentPublisher: protected LastArgumentWorker<Types...>
	{
		public:
			typedef typename LastArgumentWorker<Types...>::type type;
	};
}
#endif /* DOXYGEN_SHOULD_SKIP_THIS */

/**
 * Gets _last_ type from a type parameter pack.
 *
 * This meta-template hack provides for an easy to read expression of the last element of a type parameter pack.
 */
template<class... T>
using lastOf = typename detail::LastArgumentPublisher<T...>::type;

/**
 * Gets _first_ type from a type parameter pack.
 *
 * This meta-template hack provides for an easy to read expression of the first element of a type parameter pack.
 */
template<class... T>
using firstOf = typename detail::FirstArgumentPublisher<T...>::type;

}

#endif /* PACKS_H_ */
