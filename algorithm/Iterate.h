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

#ifndef ITERATE_H_
#define ITERATE_H_

#include "ubiquitous/Compiler.h"
#include "meta/FieldAccessor.h"

namespace pet {

/**
 * Find by value.
 *
 * Generic iterative search over sequentially iterable containers.
 *
 * @tparam	Container The type of the underlying container.
 * @tparam	Element The type of the elements stored in the container.
 */
template<class Container, class Element>
class IterativeSearch {
public:
	/**
	 * Find by value.
	 *
	 * Iterates through a container and compares one field of each element with a
	 * value and returns the first one found to be matching.
	 *
	 * @param	container the container to operate on.
	 * @param	value is the expected value
	 * @param	fields are pointer-to-members that specify the field to be accessed,
	 * 			if the required field is not directly the member of the element type,
	 * 			but a member of a member then multiple fields are needed to be specified.
	 * 			These have to belong the consecutive embedded objects types and are
	 * 			accessed by dereferencing them respectively.
	 * @return 	The first matching element or NULL if not found.
	 *
	 * @see		The unit test of this class provides some examples of the usage.
	 */
	template<class... T>
	really_inline static Element* findByFields(Container* container, pet::finalTypeOf<T...> value, const T&... fields)
	{
		for(typename Container::Iterator it = container->iterator(); it.current(); it.step())
			if(pet::access(*it.current(), fields...) == value)
				return it.current();

		return 0;
	}

	/**
	 *	Container decorator.
	 *
	 *	This is a helper used for internalizing the functionality of the IterativeSearch.
	 *
	 *	@see For details see IterativeSearch.
	 */
	class Decorator {
	public:
		/**
		 * Find by value.
		 *
		 * @param	value the expected value.
		 * @param	fields pointer-to-members that specify the field to be accessed.
		 *
		 * @see For details see IterativeSearch.
		 */
		template<class... T>
		really_inline Element* findByFields(pet::finalTypeOf<T...> value, const T&... fields) const
		{
			return IterativeSearch<Container, Element>::findByFields((Container*) this, value, fields...);
		}
	};
};

}

#endif /* ITERATE_H_ */
