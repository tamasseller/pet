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

#ifndef CIRCULARBUFFER_H_
#define CIRCULARBUFFER_H_

#include <stddef.h>

namespace pet {

/**
 * True circular buffer.
 *
 * This is a real circular buffer, that has a single pointer into
 * the stored data, that can move around in a single direction.
 * It behaves like a FIFO buffer that has a fixed number of elements
 * in it, and it can be read and written only simultaneously.
 */
template<class T, size_t n>
struct CircularBuffer {

        /// Data container.
        T data[n];

        /// Index of the next element to be written.
        uint16_t idx = 0;

    public:

        /**
         * Access the data container directly.
         *
         * This method can be used to fill the buffer with initial
         * data conveniently.
         */
        inline T (&access())[n] {
            return data;
        }

        /// The element pointed to by the circularly moving pointer.
        inline T & front() {
            return data[idx];
        }

        /// Move the circular pointer by one unit.
        inline void advance() {
            idx++;

            if(idx >= n)
                idx = 0;
        }

        /**
         * Drop the oldest element and insert a new one.
         *
         * Replace the element currently selected by the pointer and advance the pointer.
         */
        inline void replace(const T &fresh) {
            data[idx] = fresh;
            advance();
        }
};

}

#endif /* CIRCULARBUFFER_H_ */
