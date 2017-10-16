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

#ifndef MEDIANFILTER_H_
#define MEDIANFILTER_H_

#include "data/CircularBuffer.h"
#include "data/PrioQueue.h"

#include <limits>

namespace pet {

/**
 * Fast rolling-window median filter.
 */
template<class T, size_t N>
class MedianFilter {
        static_assert(N % 2 == 1, "Even number of elements not supported.");
        struct swap;

        struct Entry;

        typedef Entry* Handle;

        struct Entry {
            Handle* backRef;
            T data;
        };

        static inline bool compare(const T& a, const T& b) {
            return a < b;
        }

        static inline void swap(Handle &a, Handle& b) {
            Handle temp = a;
            a = b;
            b = temp;
            a->backRef = &a;
            b->backRef = &b;
        }

        CircularBuffer<Entry, N> buffer;

        struct MinHeap: PrioQueueWindow<Handle, N/2, MinHeap>  {
            static inline bool compare(const Handle& a, const Handle& b) {
                return MedianFilter::compare(a->data, b->data);
            }

            static inline void swap(Handle& a, Handle& b) {
                MedianFilter::swap(a, b);
            }
        } bigOnes;

        struct MaxHeap: PrioQueueWindow<Handle, N/2, MaxHeap>  {
            static inline bool compare(const Handle& a, const Handle& b) {
                return MedianFilter::compare(b->data, a->data);
            }

            static inline void swap(Handle& a, Handle& b) {
                MedianFilter::swap(a, b);
            }
        } smallOnes;

        Handle median;

    public:

        MedianFilter(): median(buffer.access()) {
            buffer.access()[0].data = std::numeric_limits<T>::min();
            buffer.access()[0].backRef = &median;

            for(int i=0; i<smallOnes.n; i++) {
                Entry* e = buffer.access() + i * 2 + 1;

                smallOnes.access()[i] = e;
                e->backRef = smallOnes.access() + i;
                e->data = std::numeric_limits<T>::min();
            }

            for(int i=0; i<bigOnes.n; i++) {
                Entry* e = buffer.access() + i * 2 + 2;

                bigOnes.access()[i] = e;
                e->backRef = bigOnes.access() + i;
                e->data = std::numeric_limits<T>::max();
            }
        }

        void enterValue(const T& value) {
            // Storage for the element to be updated.
            Entry &place = buffer.front();
            buffer.advance();

            // Update data.
            place.data = value;

            if(place.backRef == &median) {
                if(compare(place.data, smallOnes.extreme()->data)) {
                    /*
                     *  If it was the median and the new value is smaller
                     *  than the biggest small, swap that with the new one
                     *  and update the small heap.
                     */
                    swap(median, smallOnes.extreme());
                    smallOnes.refresh(&smallOnes.extreme());
                } else if(compare(bigOnes.extreme()->data, place.data)) {
                    /*
                     *  If it was the median and the new value is bigger
                     *  than the smallest big, swap that with the new one
                     *  and update the big heap.
                     */
                    swap(median, bigOnes.extreme());
                    bigOnes.refresh(&bigOnes.extreme());
                }
            } else if(smallOnes.contains(place.backRef)){
                if(!compare(median->data, place.data)) {
                    /*
                     * If the updated element was in the small heap, and the new
                     * data is smaller than the median. update the small heap.
                     */
                    smallOnes.refresh(place.backRef);
                } else {
                    /*
                     * If the updated element was in the small heap, put the new
                     * data is bigger than the median. rotation is needed to be
                     * performed. First the median is swapped with the updated
                     * element and the small heap is updated, then we can be sure
                     * that the small heap and the median is consistent.
                     */
                    Handle *oldHandle = place.backRef;
                    swap(median, *oldHandle);
                    smallOnes.refresh(oldHandle);

                    /*
                     * But we still need to check if the median is greater then
                     * the smallest big one and if it is, then they need to be
                     * swapped and the big heap needs to be updated.
                     */
                    if(compare(bigOnes.extreme()->data, median->data)) {
                        swap(median, bigOnes.extreme());
                        bigOnes.refresh(&bigOnes.extreme());
                    }
                }
            } else {
                if(!compare(place.data, median->data)) {
                    /*
                     * If the updated element was in the big heap, and the new
                     * data is bigger than the median. update the big heap.
                     */
                    bigOnes.refresh(place.backRef);
                } else {
                    Handle *oldHandle = place.backRef;
                    swap(median, *oldHandle);
                    bigOnes.refresh(oldHandle);

                    if(compare(median->data, smallOnes.extreme()->data)) {
                        swap(median, smallOnes.extreme());
                        smallOnes.refresh(&smallOnes.extreme());
                    }
                }
            }
        }

        const T& getOutput() {
            return median->data;
        }
};

}

#endif /* MEDIANFILTER_H_ */
