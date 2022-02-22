#include "SharedAtomicList.h"

bool pet::SharedAtomicList::push(Element* element)
{
	/*
	 * Read current first (will be compare-and-set later).
	 */
	auto f = (Element*)first;

	/*
	 * Try and set the next field of the element to the (possibly) current first
	 * one, if it is set to the invalid value. If the field's value is found to
	 * be different than the invalid value, the element is already in a list.
	 */
	if(!element->nextShalElem.compareAndSwap((Element*)Element::invalid, f))
	{
	    return false;
	}

    /*
     * Next the first pointer needs to be updated if it has not changed during
     * the first operation. If the first pointer changed, then the next field
     * also needs to be changed before publishing the element via setting the
     * first pointer to it.
     *
     * NOTE: At this point the ownership is already taken over the element, by
     * 		 setting its next pointer to a different value from **invalid**.
     */
    while(!first.compareAndSwap(f, element))
    {
        /*
         * Update the first pointer candidate that is used for the repeated
         * trial of atomic update and also for setting the next pointer of the element.
         */
        f = first;

        /*
         * Update the next pointer of the element before retrying atomic update
         * of the first pointer, which also publishes the element, thus the
         * exclusive ownership of this context is lost over the element.
         *
         * NOTE: Atomic access is not required here as the element is owned solely
         * 		 by this context.
         */
        element->nextShalElem = f;
    }

	return true;
}

pet::SharedAtomicList::Reader pet::SharedAtomicList::read()
{
	/*
	 * Take over the current list atomically.
	 */
	Element* current = first.swap(nullptr);

	/*
	 * The list is in reverse order, in order to enable sane
	 * behavior when used as a queue, the list is reversed.
	 */

	/// _prev_ is the previous element in the original (reversed) order.
	Element* prev = nullptr;

	// _current_ moves through the list in the original (reversed) order.
	while(current)
	{
		/*
		 * Get the original next, this is going to be _current_
		 * in the next iteration.
		 */
		Element* oldNext = current->nextShalElem;

		/*
		 * Set the next pointer of the current to be the other
		 * neighbor, thus reversing the order of elements.
		 *
		 * Atomic operations are not required here, as none of the
		 * involved values can be the special _invalid_ value.
		 */
		current->nextShalElem = prev;

		/*
		 * Set up state for the next iteration:
		 *   - _prev_ is going to be the _current_ of this run
		 *   - _current_ is going to be oldNext, the next in
		 *     the original ordering.
		 */
		prev = current;
		current = oldNext;
	}

	/*
	 * At this point _current_ is null and _prev_ holds the last
	 * element encountered before the end of the list in original
	 * (reversed) order, which is the first in the correct order.
	 */
	return prev;
}
