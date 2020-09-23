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

#ifndef MINMAXHEAP_H_
#define MINMAXHEAP_H_

#include <stddef.h>

namespace pet {

/**
 * Generic priority queue (min/max heap) logic.
 *
 * It is a fully abstract logic that implements the ordering
 * operations of a min/max heap or priority queue.
 *
 * It can be used by providing the concrete details through
 * a CRTP relationship.
 *
 * It expects that:
 *
 *  - the elements are by instances of a user provided type (ElementId),
 *  - the CRTP child provides a methods to query the parent and children
 *    elements and validity of a given instance of this identifier type.
 *  - the CRTP child provides comparator and swap methods, that uses
 *    these identifiers as their arguments.
 *  - the CRTP child provides a further boolean value (_areIdsStable_)
 *    that is true if swap operations also swap the targets of the ids,
 *    so that the original identifers address the swapped elements (ie.
 *    they are pointers) and false if the identifiers held by the logic
 *    must be swapped to represent the actual state after the swap
 *    operation.
 *
 * The comparator method (_compareElement_) is expected to:
 *
 *  - take two identifiers as argument,
 *  - return a boolean that is true if the strict ordering relation holds
 *    for the two elements identified by the indices.
 *
 * The swapper (_swapElement_) method:
 *
 *  - takes two identifiers as argument,
 *  - swaps the relation of elements, to their parents and children.
 *
 * The validity query (_isValid_) method is required to return true if
 * the element identifier passed as its argument targets an element that
 * is an existing and usable (swappable, comparable) element. This
 * property is expected to be constant during the ordering operations.
 */
template<class Child, class ElementId = size_t>
class PrioQueueBase {
        /// Helper to step up along the swap path of elements.
        constexpr static inline ElementId moveUp(ElementId current, ElementId parent) {
            return Child::areIdsStable ? current : parent;
        }

        /// Helper to step down along the swap path of elements.
        constexpr static inline ElementId moveDown(ElementId parent, ElementId child) {
            return Child::areIdsStable ? parent : child;
        }

    protected:
        /**
         * Re-establish heap property recursively, going towards the root.
         *
         * This method is needed to be called after changing an element
         * to be smaller (according to the comparator) or after inserting
         * a new element on the bottom layer.
         */
        inline void heapUp(ElementId e) {
            auto* self = static_cast<Child*>(this);

            do {
                ElementId p = self->parent(e);

                if(!self->isValid(p) || !self->compareElement(e, p))
                    break;
                else
                    self->swapElement(e, p);

                e = self->moveUp(e, p);
            } while(self->isValid(e));
        }

        /**
         * Re-establish heap property recursively, going towards the leaves.
         *
         * This method is needed to be called after changing an element to
         * be greater for example after extracting the extremum (which is
         * contained at the root).
         */
        inline void heapDown(ElementId e) {
            auto* self = static_cast<Child*>(this);

            while(true) {
            	ElementId l = self->leftChild(e);
            	ElementId r = self->rightChild(e);

                if(!self->isValid(r)) {
                    if(self->isValid(l) && self->compareElement(l, e))
                        self->swapElement(l, e);

                    return;
                }

                if(self->compareElement(r, e)) {
                    if(self->compareElement(r, l)) {
                        self->swapElement(e, r);
                        e = self->moveDown(e, r);
                    } else {
                        self->swapElement(e, l);
                        e = self->moveDown(e, l);
                    }
                } else if(self->compareElement(l, e)) {
                    self->swapElement(e, l);
                    e = self->moveDown(e, l);
                } else
                    break;
            }
        }

        /**
         * General heap property re-establisher.
         *
         * This method needs to be called after modifying the value of an
         * arbitrary element.
         */
        inline void refresh(ElementId idx) {
            auto* self = static_cast<Child*>(this);

            if(self->isValid(idx) && self->isValid(self->parent(idx)) && self->compareElement(idx, self->parent(idx)))
                heapUp(idx);
            else
                heapDown(idx);
        }
};

/**
 * Priority queue (min/max heap) logic, for array-like containers.
 *
 * It is a fully abstract logic that implements the ordering
 * operations of a min/max heap or priority queue, that is
 * implemented over random access storage.
 *
 * It can be used by providing the concrete details through
 * a CRTP relationship.
 *
 * It expects that:
 *
 *  - the elements are identified by a zero based index,
 *  - the CRTP child provides a method that returns the number of accessible elements.
 *  - the CRTP child provides a comparator method, that uses these indices
 *  - and that the CRTP child also provides a similar swap method.
 *
 * The comparator method (compareElement) is expected to:
 *
 *  - take two indices as argument,
 *  - return a boolean that is true if the strict ordering relation holds
 *    for the two elements identified by the indices.
 *
 * The swapper (swapElement) method:
 *
 *  - takes two indices as argument,
 *  - swaps the elements, such that after it returns the elements can be
 *    found under the old indices of each other.
 *
 * The element number getter (getIndexLimit) method is required to return
 * the currently available number of elements. This number is expected to
 * be constant during the ordering operations.
 *
 * Internals
 * ---------
 *
 * The heap structure is interpreted such, that every stored element is a
 * node in a full binary tree, that has all of its levels full except for
 * the last one, that - depending on the current number of elements - may
 * be filled only partially. The shape of the tree is never modified by the
 * ordering operations.
 *
 * The partially ordered property of the heap data structure means that
 * every node has a smaller (or greater, depending on the comparator
 * function) value than its children. If any value is changed, or a new
 * element is introduced (on the bottom layer) the elements are swapped
 * such that this property is satisfied.
 *
 * The indices are attached to the nodes such that on each layer elements
 * are numbered from left to right with incrementing indices. On the nth
 * layer the leftmost element has the index 2^n-1  (with the root level
 * being 0) It looks like this, in the common upside down representation,
 * where the root is at the top:
 *
 *                  0
 *                /   \
 *               1     2
 *              / \   / \
 *             3   4 5   6
 *
 */
template<class Child>
class IndexedPrioQueueBase: protected PrioQueueBase<Child, size_t> {
	friend class PrioQueueBase<Child, size_t>;

	/// Swap operations change the meaning of ids.
    static constexpr bool areIdsStable = false;

    /// Helper to find the index of a parent.
	constexpr static inline size_t parent(size_t n) {
        return ((n + 1) >> 1) - 1;
    }

    /// Helper to find the index of the left child.
    constexpr static inline size_t leftChild(size_t n) {
        return ((n + 1) << 1) - 1;
    }

    /// Helper to find the index of the right child.
    constexpr static inline size_t rightChild(size_t n) {
        return ((n + 1) << 1);
    }


    inline bool isValid(size_t n) {
    	auto* self = static_cast<Child*>(this);
    	return n < self->getIndexLimit();
    }
};


namespace detail {
    /// Comparison based on the < operator of the type.
    template<class T>
    static inline bool trivialCompare(const T& a,const T& b) {
        return a < b;
    }

    /// Worker for the heapSort and findMedian methods.
    template<class T, bool (compare)(const T& a,const T& b)>
    struct PrioQueueWorker: IndexedPrioQueueBase<PrioQueueWorker<T, compare>> {
            typedef PrioQueueBase<PrioQueueWorker<T, compare>> Base;

            /// Number of the already processed elements.
            size_t size = 0;

            /// Pointer to the buffer that is being sorted.
            T* data;

            /// Inverse comparator, needed due to order reversal of extraction.
            inline bool compareElement(size_t a, size_t b) {
                return !compare(this->data[a], this->data[b]);
            }

            /// 'Make heap' constructor.
            PrioQueueWorker(T* data, size_t n): data(data) {
                for(size = 2; size < n; size++) {
                    Base::heapUp(size - 1);
                }

                Base::heapUp(size - 1);
            }

            /// Implementation of the IndexedPrioQueueBase requirements.
            inline size_t getIndexLimit() {
                return size;
            }

            /// Implementation of the IndexedPrioQueueBase requirements.
            inline void swapElement(size_t a, size_t b) {
                const T temp = data[a];
                data[a] = data[b];
                data[b]= temp;
            }

            using Base::refresh;
    };
}

/**
 * Heap sort implementation based on the generic PrioQueueBase logic.
 */
template<class T, bool (compare)(const T& a,const T& b) = &detail::trivialCompare<T>>
void heapSort(T* data, size_t n) {
    for (detail::PrioQueueWorker<T, compare> heap(data, n); heap.size > 1; ) {
        heap.swapElement(0, --heap.size);
        heap.refresh(0);
    }
}

/**
 * Partial heap sort that stops when the median is found based on the generic PrioQueueBase logic.
 *
 * @NOTE This implementation only supports odd numbers of input data items.
 */
template<class T, bool (compare)(const T& a, const T& b) = &detail::trivialCompare<T>>
const T& findMedian(T* data, size_t n) {
    for (detail::PrioQueueWorker<T, compare> heap(data, n); heap.size > (n+1) / 2; ) {
        data[0] = data[--heap.size];
        heap.refresh(0);
    }

    return data[0];
}

}

#endif /* MINMAXHEAP_H_ */
