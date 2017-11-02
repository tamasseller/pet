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

#ifndef PRIOQUEUE_H_
#define PRIOQUEUE_H_

#include "algorithm/MinMaxHeap.h"

namespace pet {

/**
 * Common base class for the priority queues implemented over an array.
 */
template<class T, size_t N, class Child>
class ArrayBackedPrioQueueBase: public IndexedPrioQueueBase<Child>
{
    protected:
		friend PrioQueueBase<Child>;
		friend IndexedPrioQueueBase<Child>;
		typedef size_t ElementId;


        /// The backing-store array.
        T data[N];

        /// Default implementation of the comparator, can be overridden by the _Child_.
        static bool compare(const T& a, const T& b) {
            return a < b;
        }


        /// Default implementation of the element swap, can be overridden by the _Child_.
        static void swap(T& a, T& b) {
            const T temp = a;
            a = b;
            b = temp;
        }

        /// Comparator implementation required by the PrioQueueBase.
        inline bool compareElement(size_t a, size_t b) {
            return static_cast<Child*>(this)->compare(this->data[a], this->data[b]);
        }

        /// Element swap implementation required by the PrioQueueBase.
        inline void swapElement(size_t a, size_t b) {
            static_cast<Child*>(this)->swap(this->data[a], this->data[b]);
        }

    public:
        /**
         * Access the extremum.
         *
         * It is an error to call this method on an empty heap.
         */
        T& extreme() {
            return data[0];
        }
};

/**
 * Fixed capacity, variably sized priority queue.
 *
 * Regular, array backed min/max heap, that can contain any number
 * of elements between zero and the size of the array.
 */
template<class T, size_t N, class Child>
class GenericPrioQueue: public ArrayBackedPrioQueueBase<T, N, Child> {
        template<class, class> friend class PrioQueueBase;
        friend IndexedPrioQueueBase<Child>;

        /// The number of elements contained.
        size_t size = 0;

        /// Element count getter implementation required by the PrioQueueBase.
        inline size_t getIndexLimit() {
            return size;
        }

    public:
        /**
         * Allocate a free data block.
         *
         * Returns the immediate next element after the last element
         * of the heap in the data storage.
         *
         * Returns null if there is no more space available.
         *
         */
        T *allocate() {
            if(size >= N)
                return nullptr;

            return this->data + size++;
        }

        /**
         * Re-establish heap property after change.
         *
         * Updates the internal state by swapping the elements in
         * the required way to enforce the heap property, provided
         * that the only element not complying to the rules is the
         * one that the method is called on.
         *
         * Returns false if the element is not a member of the heap.
         */
        bool update(const T* t) {
            ptrdiff_t diff = t - this->data;

            if(diff < 0 || size <= diff)
                return false;

            PrioQueueBase<Child>::refresh(diff);

            return true;
        }

        /**
         * Remove internal element and restore heap property.
         *
         * Moves the last element to the place of the one given
         * in the parameter, decreases size and updates the heap.
         *
         * Returns false if the element is not a member of the heap.
         */
        bool remove(const T* t) {
            ptrdiff_t diff = t - this->data;

            if(diff < 0 || size <= diff || !size)
                return false;

            this->swapElement(diff, size - 1);
            size--;
            update(t);

            return true;
        }

        /**
         * Add a new element, with updated ordering.
         *
         * Puts the new element after the current last one (greatest
         * index), increases size then updates the ordering.
         *
         * Returns false if the element is not a member of the heap.
         *
         */
        bool insert(const T& t) {
            if(T* slot = allocate()) {
                *slot = t;
                return update(slot);
            }

            return false;
        }

        /// Returns true if there are no elements.
        bool isEmpty() {
            return !size;
        }
};

/**
 * Usage helper for the GenericPrioQueue.
 *
 * The purpose of this helper template is to enable the user
 * to instantiate the min/max heap without having to subclass
 * it. It works by specifying _void_ as the Child parameter,
 * and specializing the template for this case, such that
 * the partial specialization is the empty subclass itself.
 */
template<class T, size_t N, class Child = void>
struct PrioQueue: GenericPrioQueue<T, N, Child> {};

/// Empty subclass specialization, see the generic template for details.
template<class T, size_t N>
struct PrioQueue<T, N, void>: GenericPrioQueue<T, N, PrioQueue<T, N, void>> {};


/**
 * Fixed capacity, fixed size sized priority queue.
 *
 * Array backed min/max heap, that always contains the
 * predefined number of elements. Elements can not be
 * inserted or removed but replaced only.
 */
template<class T, size_t N, class Child>
class GenericPrioQueueWindow: public ArrayBackedPrioQueueBase<T, N, Child>
{
        template<class, class> friend class PrioQueueBase;
        friend IndexedPrioQueueBase<Child>;

        /// Element count getter implementation required by the PrioQueueBase.
        inline static constexpr size_t getIndexLimit() {
            return n;
        }

    public:
        /// The capacity of this heap window.
        static constexpr size_t n = N;

        /**
         * Access the data container directly.
         *
         * This method can be used to fill the heap
         * with initial data conveniently.
         */
        inline T (&access())[n] {
            return this->data;
        }

        /**
         * Re-establish heap property after change.
         *
         * Updates the internal state by swapping the elements in
         * the required way to enforce the heap property, provided
         * that the only element not complying to the rules is the
         * one that the method is called on.
         *
         * Returns false if the element is not a member of the heap.
         */
        inline void refresh(T* data) {
            PrioQueueBase<Child>::refresh(data - this->data);
        }

        /**
         * Replace an element and reconstruct the order.
         *
         * This method writes the required value, then
         * executes the _refresh_ method.
         */
        inline void replace(T* data, const T& fresh) {
            *data = fresh;
            refresh(data);
        }

        /**
         * Decides whether the an element belongs to this heap.
         *
         * Returns true if the element is in the data storage.
         */
        inline bool contains(T* data) {
            ptrdiff_t diff = data - this->data;
            return 0 <= diff && diff < n;
        }
};

template<class T, size_t N, class Child = void>
struct PrioQueueWindow: GenericPrioQueueWindow<T, N, Child> {};

template<class T, size_t N>
struct PrioQueueWindow<T, N, void>: GenericPrioQueueWindow<T, N, PrioQueueWindow<T, N, void>> {};

}

#endif /* PRIOQUEUE_H_ */
