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

#ifndef FIELDACCESSOR_H_
#define FIELDACCESSOR_H_

#include "ubiquitous/Compiler.h"
#include "meta/MemberPointers.h"

namespace meta {
#ifndef DOXYGEN_SHOULD_SKIP_THIS
namespace detail {
	/**
	 * @internal
	 *
	 * Recursively step through a field path, dereferencing each element.
	 *
	 * This splits the first argument from the list and dereferences the base with that.
	 * Passes on the result and the remaining elements to the next step of the process.
	 */
	template <class T, class... U>
	struct AccessorHelper {
		really inline static constexpr valueTypeOf<lastOf<U...>> &access(baseTypeOf<T> &b, const T& field, const U&... otherFields) {
			return AccessorHelper<U...>::access(b.*field, otherFields...);
		}
	};

	/**
	 * @internal
	 *
	 * Iteration stopper.
	 *
	 * In the last iteration simply the dereferencing takes place, the value is returned.
	 * Ideally the whole recursion construct gets collapsed into a single load instruction.
	 * The additional inlining attributes are provided to assist the compiler in doing so.
	 */
	template <class T>
	struct AccessorHelper<T> {
		really inline static constexpr valueTypeOf<T> &access(baseTypeOf<T> &b, const T& field) {
			return b.*field;
		}
	};
}
#endif /* DOXYGEN_SHOULD_SKIP_THIS */

/**
 * Access a data member by field path parameter.
 *
 * Enables a declarative description of field accesses.
 * An example usage is the findByFields of the see LinkedList.
 * The idea here is that this will result in a single load instruction,
 * thus it does not introduce any overhead at all.
 *
 * @see LinkedList
 */

template <class... T>
really inline static constexpr valueTypeOf<lastOf<T...>> &access(baseTypeOf<firstOf<T...>> &b, const T&... fields) {
	return detail::AccessorHelper<T...>::access(b, fields...);
}

} // namespace meta

#endif /* FIELDACCESSOR_H_ */
