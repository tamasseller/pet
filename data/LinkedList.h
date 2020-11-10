/*******************************************************************************
 *
 * Copyright (c) 2016, 2017 Tamás Seller. All rights reserved.
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

#ifndef LINKEDLIST_H_
#define LINKEDLIST_H_

#include "platform/Compiler.h"

#include "meta/Utility.h"

namespace pet {

/**
 * Singly linked list.
 *
 * Stores an arbitrary type, that has an adequate next pointer, in a singly chained list.
 * The elements are linked only in one way, the forward direction, that is the same as
 * the direction in which the iterator traverses the elements. So every element has only
 * a *next* pointer.
 *
 * @tparam	Element The type of the elements to be stored. It is required to have an adequate
 * 			next pointer, that is of type _Element*_.
 *
 * @note	Basically one object can be part of only one list at a time, this limitation
 * 			can be lifted by inheriting from multiple baseclasses that contain the next
 * 			pointers and casting the element to different base types, and instantiating
 * 			the lists with these different types. Ugly...
 *
 * @note 	An object can be added to a list only once.
 */
template<class Ptr>
class LinkedPtrList {
	Ptr first = nullptr;
public:
	/**
	 * Forward iterator.
	 *
	 * Represents a state of the iteration over the elements.
	 * That means it has a current element that is either valid or not.
	 * An iterator over an empty list has no element to address.
	 * Also iterating over the end yields the same state.
	 *
	 * @warning	Modifying the list while iterating through it with an iterator results in undefined behavior.
	 * 			This condition is not checked, because there is no zero-overhead way to do it, so avoiding
	 * 			this usage is entirely up to the user.
	 */
	class Iterator {
		friend LinkedPtrList;
		Ptr* prevsNext;

		really_inline Iterator(Ptr* prevsNext);
	public:
		/**
		 * Current element or NULL.
		 *
		 * @return The current element or NULL if over the end of the list.
		 */
		really_inline const Ptr &current() const;

		/**
		 * Take a step.
		 *
		 * Moves the iterator forward over the list or does nothing if already of the end.
		 */
		really_inline void step();

		/**
		 * Insert before current element.
		 *
		 * Adds an element **that is not already in the list** before the current elements.
		 * The iterator will stand at the added element after the operation.
		 * If called on an over-the-end iterator, appends to the end.
		 *
		 * @warning	The behavior is undefined (and probably wrong) if called with an element
		 * 			that is already in the list and **in this method it is not checked**.
		 */
		really_inline void insert(Ptr elem) const;

		/**
		 * Remove current element.
		 *
		 * Removes the current element.
		 * The iterator will stand at the next element after the operation.
		 * Does nothing on an over-the-end iterator.
		 */
		really_inline Ptr remove() const;

		/**
		 * Operators for STL style iterator usage.
		 */
		inline Ptr operator*() const;
		inline bool operator==(const Iterator& o) const;
		inline bool operator!=(const Iterator& o) const;
		inline Iterator& operator++();
	};

	/**
	 * Fast, unchecked add to front.
	 *
	 * Adds an element at the first position.
	 * It is not check whether the supplied element is member of the container.
	 *
	 * @warning The behavior is unspecified if the has already been element added.
	 *
	 * @param	elem Is a pointer to the element to be added.
	 */
	inline void fastAdd(Ptr elem);

	/**
	 * Add to front.
	 *
	 * Adds an element at the first position.
	 * The list is searched to check that it is not already added.
	 *
	 * @param	elem is a pointer to the element to add.
	 * @return 	True if successful.
	 */
	inline bool add(Ptr elem);

	/**
	 * Add to back.
	 *
	 * Adds an element at the last position.
	 * The list is searched to check that it is not already added.
	 *
	 * @param	elem is a pointer to the element to add.
	 * @return 	True if successful.
	 */
	inline bool addBack(Ptr elem);

	/**
	 * Remove an element.
	 *
	 * Searches an element by reference and if found deletes it from the list.
	 *
	 * @param	elem is a pointer to the element to remove.
	 * @return 	True if successful.
	 */
	inline bool remove(Ptr elem);

	/**
	 * Remove all elements.
	 */
	inline void clear();

	/**
	 * Remove all elements.
	 */
	inline bool isEmpty() const;

	/**
	 * Get an all through iterator.
	 *
	 * @return 	An iterator that is at the foremost possible location
	 * 			(ie.: the first element if there is any).
	 */
	really_inline Iterator iterator();

	/**
	 * STL style iterator getter for the beginning of the list.
	 */
	really_inline Iterator begin();

	/**
	 * STL style iterator getter for the beginning of the list.
	 */
	really_inline Iterator end();

	/**
	 * Take over the content of another list.
	 *
	 * Drops the current content and takes over the other lists
	 * content, also sets the other list to be empty.
	 */
	really_inline void take(LinkedPtrList& other);
};

template<class Ptr>
inline void LinkedPtrList<Ptr>::fastAdd(Ptr elem)
{
	iterator().insert(elem);
}

template<class Ptr>
inline bool LinkedPtrList<Ptr>::add(Ptr elem)
{
	for(Iterator it=iterator(); it.current(); it.step())
		if(it.current() == elem)
			return false;

	iterator().insert(elem);
	return true;
}

template<class Ptr>
inline bool LinkedPtrList<Ptr>::addBack(Ptr elem)
{
	Iterator it = iterator();
	for(; it.current(); it.step())
		if(it.current() == elem)
			return false;

	it.insert(elem);
	return true;
}

template<class Ptr>
inline bool LinkedPtrList<Ptr>::remove(Ptr elem)
{
	for(Iterator it = iterator(); it.current(); it.step()) {
		if(it.current() == elem) {
			it.remove();
			return true;
		}
	}

	return false;
}

template<class Ptr>
inline void LinkedPtrList<Ptr>::clear() {
	first = nullptr;
}

template<class Ptr>
inline bool LinkedPtrList<Ptr>::isEmpty() const {
	return first == nullptr;
}

template<class Ptr>
inline void LinkedPtrList<Ptr>::take(LinkedPtrList& other) {
	first = other.first;
	other.clear();
}

template<class Ptr>
really_inline typename LinkedPtrList<Ptr>::Iterator
LinkedPtrList<Ptr>::iterator() {
	return Iterator(&first);
}

template<class Ptr>
really_inline typename LinkedPtrList<Ptr>::Iterator
LinkedPtrList<Ptr>::begin() {
	return Iterator(&first);
}

template<class Ptr>
really_inline typename LinkedPtrList<Ptr>::Iterator
LinkedPtrList<Ptr>::end() {
	return Iterator(nullptr);
}

template<class Ptr>
really_inline LinkedPtrList<Ptr>::Iterator::Iterator(Ptr* prevsNext):prevsNext(prevsNext) {}

template<class Ptr>
really_inline const Ptr &LinkedPtrList<Ptr>::Iterator::current() const
{
	return *this->prevsNext;
}

template<class Ptr>
really_inline void LinkedPtrList<Ptr>::Iterator::step()
{
	if(*this->prevsNext)
		this->prevsNext = &((*this->prevsNext)->next);
}

template<class Ptr>
inline Ptr LinkedPtrList<Ptr>::Iterator::operator*() const {
	return *this->prevsNext;
}

template<class Ptr>
inline bool LinkedPtrList<Ptr>::Iterator::operator==(const Iterator& o) const {
	return (this->prevsNext ? *this->prevsNext : nullptr) == (o.prevsNext ? *o.prevsNext : nullptr);
}

template<class Ptr>
inline bool LinkedPtrList<Ptr>::Iterator::operator!=(const Iterator& o) const {
	return !(*this == o);
}

template<class Ptr>
inline typename LinkedPtrList<Ptr>::Iterator& LinkedPtrList<Ptr>::Iterator::operator++()
{
	this->prevsNext = &((*this->prevsNext)->next);
	return *this;
}

template<class Ptr>
really_inline void LinkedPtrList<Ptr>::Iterator::insert(Ptr elem) const
{
	elem->next = pet::move(*this->prevsNext);
	*this->prevsNext = pet::move(elem);
}

template<class Ptr>
really_inline Ptr LinkedPtrList<Ptr>::Iterator::remove() const
{
	auto ret(pet::move(*this->prevsNext));

	if(ret)
		*this->prevsNext = ret->next;

	return ret;
}

template<class Element>
using LinkedList = LinkedPtrList<decltype(Element::next)>;

}

#endif /* LINKEDLIST_H_ */
