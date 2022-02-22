/*******************************************************************************
 *
 * Copyright (c) 2022 Tamás Seller. All rights reserved.
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

#ifndef PLATFORM_CM_BARE_SHAREDATOMICLIST_H_
#define PLATFORM_CM_BARE_SHAREDATOMICLIST_H_

#include "platform/Atomic.h"

#include <cstdint>

namespace pet {

/**
 * Intrusive multi-writer, single-reader linked list.
 *
 * It also permits race on the items to be inserted between readers.
 */
class SharedAtomicList
{
public:
    /**
     * Base class for the contained elements.
     */
	class Element
	{
		/**
		 * Value used in the next field for elements that are not part of any list.
		 */
		static constexpr uintptr_t invalid = -1u;

		/**
		 * Next pointer.
		 *
		 * It is set to null if this is the last element in the list, and to
		 * **invalid** if it is not contained in any list.
		 */
		pet::Atomic<Element*> nextShalElem = (Element*)invalid;

		/// Access to the next field is provided to the members of the **SharedAtomicList** class.
		friend class SharedAtomicList;

	public:
		inline Element() = default;
	};

	class Reader
	{
		/// The first, next-one-to-be-read element of the re-reversed (original order) list.
		Element* current;

		/// Construct from pointer to first element (for internal use).
		inline Reader(Element* current): current(current) {}

		/// Access to the initializing constructor is provided for **SharedAtomicList**.
		friend class SharedAtomicList;

	public:
		/// Default move constructor.
		inline Reader(Reader&&) = default;

		/// Default move assignment operator.
		inline Reader& operator =(Reader&&) = default;

		/// Get pointer to the next in-order element or null if there is none.
		really_inline Element* peek() const {
			return current;
		}

		/**
		 * Drop the current element.
		 *
		 * NOTE: must not be called of empty, i.e. if _peek_ returns null.
		 */
		inline void pop()
		{
			/*
			 * The iterator needs to be updated via atomic operation, because with
			 * clearing the next field argument, the ownership is transfered back
			 * to the writer side.
			 */
			current = current->nextShalElem.swap((Element*)Element::invalid);
		}
	};

private:
	/// Pointer to the first element currently contained in the list.
	pet::Atomic<Element*> first;

public:
	/**
	 * Insert an element into the list.
	 *
	 * Returns true if the element was inserted, false if it is contained in a
	 * list (which may be another instance then this one).
	 */
	bool push(Element* element);

	/**
	 * Take over current contents of the list and return it as a reader object.
	 */
	Reader read();
};

}

#endif /* PLATFORM_CM_BARE_SHAREDATOMICLIST_H_ */
