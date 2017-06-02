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

namespace pet {

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
class DoubleList: public pet::IterativeSearch<DoubleList<Element>, Element>::Decorator {
	Element* first = 0;
	Element* last = 0;
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

	really inline bool contains(Element* elem) const;

	/** @copydoc LinkedList::iterator */
	really inline Iterator iterator() const;

	/** @copydoc LinkedList::add */
	inline bool addFront(Element* elem);

	/** @copydoc LinkedList::fastAdd */
	really inline void fastAddFront(Element* elem);

	/** @copydoc LinkedList::addBack */
	inline bool addBack(Element* elem);

    really inline void fastAddBack(Element* elem);

	/** @copydoc LinkedList::remove */
	inline bool remove(Element* elem);

	inline void insertBefore(Element* elem, Iterator at);

	inline Element* front() const;
	inline Element* back() const;

	inline Element* popFront();
	inline Element* popBack();

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
really inline bool DoubleList<Element>::contains(Element* elem) const
{
    for(Iterator it=iterator(); it.current(); it.step())
        if(it.current() == elem)
            return true;

    return false;
}


template<class Element>
inline bool DoubleList<Element>::addFront(Element* elem)
{
    if(contains(elem))
        return false;

	fastAddFront(elem);
	return true;
}

template<class Element>
inline bool DoubleList<Element>::addBack(Element* elem)
{
    if(contains(elem))
        return false;

    fastAddBack(elem);
    return true;
}

template<class Element>
really inline void DoubleList<Element>::fastAddFront(Element* elem)
{
    if(first)
        first->prev = elem;

    elem->prev = 0;
    elem->next = first;
    first = elem;

    if(!last)
        last = first;
}

template<class Element>
really inline void DoubleList<Element>::fastAddBack(Element* elem)
{
    if(last)
        last->next = elem;

    elem->prev = last;
    elem->next = 0;

    last = elem;

    if(!first)
        first = last;
}

template<class Element>
really inline void DoubleList<Element>::fastRemove(Element* elem)
{
    Element *& prevsNext = (elem->prev) ? elem->prev->next : first;
    Element *& nextsPrev = (elem->next) ? elem->next->prev : last;

    prevsNext = elem->next;
    nextsPrev = elem->prev;
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
inline void DoubleList<Element>::insertBefore(Element* elem, Iterator at) {
	if(at.current()) {
		Element* &prevsNext = (at.current()->prev) ? at.current()->prev->next : first;
		Element* prev = (at.current()->prev) ? at.current()->prev : nullptr;

		prevsNext = elem;
		at.current()->prev = elem;

		elem->prev = prev;
		elem->next = at.current();
	} else
		this->fastAddBack(elem);
}

template<class Element>
really inline typename DoubleList<Element>::Iterator
DoubleList<Element>::iterator() const {
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

template<class Element>
inline Element* DoubleList<Element>::front() const
{
	return first;
}

template<class Element>
inline Element* DoubleList<Element>::back() const
{
	return last;
}

template<class Element>
inline Element* DoubleList<Element>::popFront()
{
	Element* ret = first;

	if(first)
		first = first->next;
	else
		return ret;

	if(first)
		first->prev = nullptr;
	else
		last = nullptr;

	return ret;
}

template<class Element>
inline Element* DoubleList<Element>::popBack()
{
	Element* ret = last;

	if(last)
		last = last->prev;
	else
		return ret;

	if(last)
		last->next = nullptr;
	else
		first = nullptr;

	return ret;
}

}
#endif /* DOUBLELIST_H_ */
