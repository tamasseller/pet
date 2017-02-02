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

#ifndef MEMBERPOINTERS_H_
#define MEMBERPOINTERS_H_

#include "meta/Packs.h"

namespace meta {
#ifndef DOXYGEN_SHOULD_SKIP_THIS
namespace detail {
	/**
	 * @internal
	 *
	 * Forward declaration of the helper.
	 *
	 * This is form is only used for specialization, never gets defined.
	 * It is only specialized for when T is a pointer-to-method type.
	 */
	template<class T> struct PointerToMemberTypeExtractorHelper;

	/**
	 * @internal
	 *
	 * Specialization of the helper.
	 *
	 * This is how type deduction is leveraged to access the details of the
	 * pinter-to-method type passed as type argument.
	 */
	template<class _FieldType, class _BaseType>
	struct PointerToMemberTypeExtractorHelper< _FieldType(_BaseType::*)>
	{
		typedef _FieldType FieldType;
		typedef _BaseType BaseType;
	};

	/**
	 * @internal
	 *
	 * Specialization selector.
	 *
	 * This is needed to give the compiler a chance to find the specialized version.
	 */
	template<class T>
	struct PointerToMemberTypeExtractorPublisher : public PointerToMemberTypeExtractorHelper<T> {};
}
#endif /* DOXYGEN_SHOULD_SKIP_THIS */

/**
 * Type of value of a pointer-to-method type.
 *
 * This tool enables the a short and human readable representation.
 */
template<class T>
using valueTypeOf = typename detail::PointerToMemberTypeExtractorPublisher<T>::FieldType;

/**
 * Type of the class for a pointer-to-method type.
 *
 * This tool enables the a short and human readable representation.
 */
template<class T>
using baseTypeOf = typename detail::PointerToMemberTypeExtractorPublisher<T>::BaseType;

/**
 * Type of value of the last pointer-to-method type from a pack of these.
 *
 * This tool is useful in the a scenario when there is a path of members
 * specified and one needs to get the final one.
 */
template <class... T>
using finalTypeOf = valueTypeOf<lastOf<T...>>;

} // namespace meta

#endif /* MEMBERPOINTERS_H_ */
