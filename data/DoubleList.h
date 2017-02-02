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

#ifndef DOUBLELIST_H_
#define DOUBLELIST_H_

#include "ubiquitous/Compiler.h"
#include "meta/FieldAccessor.h"
#include "algorithm/Iterate.h"

namespace container {

/**
 * Doubly linked list.
 *
 * Stores an arbitrary type, that has an adequate next and previous pointer, in a doubly chained list.
 * The elements are linked in both ways, the forward direction, that is the same as
 * the direction in which the iterator traverses the elements and the orher.
 *
 * @tparam	Element The type of the elements to be stored. It is required to have adequate
 * 			_next_ and _prev_ pointers of type _Element*_.
 *
 * @note	Basically one object can be part of only one list at a time, this limitation
 * 			can be lifted by inheriting from multiple baseclasses that contain the next
 * 			pointers and casting the element to different base types, and instantiating
 * 			the lists with these different types. Ugly...
 *
 * @note 	An object can be added to a list only once.
 */
template<class Element>
class DoubleList: public algorithm::IterativeSearch<DoubleList<Element>, Element>::Decorator {
	Element* first = 0;
public:
	/** @copydoc LinkedList::Iterator */
	class Iterator {
		friend DoubleList;
		Element* element;

		really inline Iterator(Element* element);
	public:
		/** @copydoc LinkedList::Iterator::current */
		really inline Element* current() const;

		/** @copydoc LinkedList::Iterator::step */
		really inline void step();
	};

	/** @copydoc LinkedList::iterator */
	really inline Iterator iterator();

	/** @copydoc LinkedList::add */
	inline bool add(Element* elem);

	/** @copydoc LinkedList::fastAdd */
	really inline void fastAdd(Element* elem);

	/** @copydoc LinkedList::addBack */
	inline bool addBack(Element* elem);

	/** @copydoc LinkedList::remove */
	inline bool remove(Element* elem);

	/**
	 * Fast, unchecked remove element.
	 *
	 * Removes an element of the list.
	 * It is not check whether the supplied element is a member of the container.
	 *
	 * @warning The behavior is unspecified if the element is not contained.
	 *
	 * @param	elem Is a pointer to the element to be removed.
	 */
	really inline void fastRemove(Element* elem);

};

template<class Element>
really inline void DoubleList<Element>::fastAdd(Element* elem)
{
	if(first)
		first->prev = elem;

	elem->prev = 0;
	elem->next = first;
	first = elem;
}


template<class Element>
inline bool DoubleList<Element>::add(Element* elem)
{
	for(Iterator it=iterator(); it.current(); it.step())
		if(it.current() == elem)
			return false;

	fastAdd(elem);
	return true;
}

template<class Element>
inline bool DoubleList<Element>::addBack(Element* elem)
{
	Iterator it=iterator();
	while(it.current()) {
		if(it.current() == elem)
			return false;

		if(!it.current()->next)
			break;

		it.step();
	}

	elem->next = 0;
	elem->prev = it.current();

	if(it.current())
		it.current()->next = elem;
	else
		first = elem;

	return true;
}

template<class Element>
really inline void DoubleList<Element>::fastRemove(Element* elem)
{
	if(elem->prev)
		elem->prev->next = elem->next;
	else
		first = elem->next;

	if(elem->next)
		elem->next->prev = elem->prev;
}

template<class Element>
inline bool DoubleList<Element>::remove(Element* elem)
{
	for(Iterator it = iterator(); it.current(); it.step()) {
		if(it.current() == elem) {
			fastRemove(elem);
			return true;
		}
	}

	return false;
}

template<class Element>
really inline typename DoubleList<Element>::Iterator
DoubleList<Element>::iterator() {
	return Iterator(first);
}

template<class Element>
really inline DoubleList<Element>::Iterator::Iterator(Element* element):element(element) {}

template<class Element>
really inline Element* DoubleList<Element>::Iterator::current() const
{
	return element;
}

template<class Element>
really inline void DoubleList<Element>::Iterator::step()
{
	if(element)
		element = element->next;
}

}
#endif /* DOUBLELIST_H_ */
