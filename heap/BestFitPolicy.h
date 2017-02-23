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

#ifndef BESTFITPOLICY_H_
#define BESTFITPOLICY_H_

#include "heap/Heap.h"
#include "data/LinkedList.h"

namespace pet {

/**
 * Singly linked list based best-fit allocator policy.
 *
 * This is a dynamic memory management policy for Heap host class. Together they
 * form a non-realtime heap, capable of allocating and reclaiming a full or partial
 * block with linear time complexity by the number of free blocks.
 */

template <class SizeType, unsigned int alignmentBits>
class BestFitPolicy: protected HeapBase<SizeType, alignmentBits>
{
	using typename HeapBase<SizeType, alignmentBits>::Block;

	struct FreeBlock {
		FreeBlock* next;
	};

	pet::LinkedList<FreeBlock> freeStore;

protected:
	/** @copydoc pet::TlsfPolicy::freeHeaderSize */
	static constexpr unsigned int freeHeaderSize = sizeof(FreeBlock);

	/** @copydoc pet::TlsfPolicy::add(Block */
	inline void add(Block block);

	/** @copydoc pet::TlsfPolicy::remove */
	inline void remove(Block block);

	/** @copydoc pet::TlsfPolicy::findAndRemove */
	inline Block findAndRemove(unsigned int size);

	/** @copydoc pet::TlsfPolicy::update */
	inline void update(unsigned int oldSize, Block block);
};


/**
 * Heap with BestFitPolicy.
 *
 * Facade to provide nicer usage, with automatically matching redundant parameters.
 */

template<class SizeType, unsigned int alignmentBits, bool cheksummingOn = false>
using BestFitHeap = Heap<BestFitPolicy<SizeType, alignmentBits>, SizeType, alignmentBits, cheksummingOn>;

////////////////////////////////////////////////////////////////////////////////////////

template <class SizeType, unsigned int alignmentBits>
inline void BestFitPolicy<SizeType, alignmentBits>::add(Block block)
{
	freeStore.add((FreeBlock *)block.ptr);
}

template <class SizeType, unsigned int alignmentBits>
inline void BestFitPolicy<SizeType, alignmentBits>::remove(Block block)
{
	freeStore.remove((FreeBlock *)block.ptr);
}

template <class SizeType, unsigned int alignmentBits>
inline typename BestFitPolicy<SizeType, alignmentBits>::Block
BestFitPolicy<SizeType, alignmentBits>::findAndRemove(unsigned int size)
{
	unsigned int bestSize = 0xffffffff;
	FreeBlock* best = 0;
	for(auto it = freeStore.iterator(); it.current(); it.step()) {
		unsigned int currSize = Block(it.current()).getSize();
		if(currSize == size) {
			FreeBlock* ret = it.current();
			it.remove();
			return ret;
		} else if (currSize > size && currSize < bestSize) {
			bestSize = currSize;
			best = it.current();
		}
	}

	freeStore.remove(best);
	return best;
}

template <class SizeType, unsigned int alignmentBits>
inline void BestFitPolicy<SizeType, alignmentBits>::update(unsigned int oldSize, Block block){}

}


#endif /* BESTFITPOLICY_H_ */
