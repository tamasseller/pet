#ifndef PET_HEAP_HEAPBASE_H_
#define PET_HEAP_HEAPBASE_H_

/*******************************************************************************
 *
 * Copyright (c) 2016 - 2021 Tamás Seller. All rights reserved.
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

#include "ubiquitous/Trace.h"

#include <cstdint>

namespace pet {

/**
 * Trace tag.
 *
 * Use this class as an identifier to enable tracing of heap events.
 */
class AllHeapsTrace: public pet::Trace<AllHeapsTrace> { static constexpr const char* name = "HEAP"; };

/**
 * Common base for the heap host and the policies.
 *
 * Provides the constants and mappings that are essential for
 * the internal work of both parties. Also provides the Block
 * type that is the basis of the communication between the two.
 *
 * @tparam	SizeType The type to be used by the heap the express
 * 			the size of the free and used blocks in an internal
 * 			representation (ie. not necessarily bytes).
 */
template<class SizeType>
struct HeapBase
{
	/**
	 * SizeType MSB mask.
	 *
	 * A mask that has only the most significant bit set for the type used to
	 * represent block sizes internally. Recklessly assumes that a byte is 8 bits.
	 */
    static constexpr uintptr_t sizeMsb = uintptr_t (1) << (sizeof(SizeType) * 8 - 1);

    /**
     * Heap block handle.
     *
     * Enables access to a (free or used) heap block.
     *
     * @see	Heap for the details.
     */
    struct Block
    {
		/**
		 * The offset of the next field from the start of the payload.
		 * @see	Heap for the details.
		 */
		static constexpr int nextFieldIdx = -1;

		/**
		 * Pointer to the payload.
		 * @see	Heap for the details.
		 */
		SizeType *ptr;

		/**
		 * Create from payload pointer.
		 * @see	Heap for the details.
		 */
		inline Block(void* address): ptr((SizeType*)address) {}

		/**
		 * Get the size of the payload of this block.
		 * @see	Heap for the details.
		 */
		inline unsigned int getSize() const  {
		    return ptr[nextFieldIdx] & ~sizeMsb;
		}
    };
};

}

#endif /* PET_HEAP_HEAPBASE_H_ */
