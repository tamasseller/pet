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

#ifndef BESTFITPOLICY_H_
#define BESTFITPOLICY_H_

#include "data/LinkedList.h"
#include "heap/HeapBase.h"

namespace pet {

/**
 * Singly linked list based best-fit allocator policy.
 *
 * This is a dynamic memory management policy for Heap host class. Together they
 * form a non-realtime heap, capable of allocating and reclaiming a full or partial
 * block with linear time complexity by the number of free blocks.
 */

template <class SizeType>
class BestFitPolicy: protected HeapBase<SizeType>
{
    using typename HeapBase<SizeType>::Block;

    struct FreeBlock
    {
        FreeBlock* next;
    };

    pet::LinkedList<FreeBlock> freeStore;

protected:
    static constexpr auto freeHeaderSize = sizeof(FreeBlock);

    really_inline void init(Block block)
    {
        freeStore.clear();
        add(block);
    }

    really_inline void add(Block block) {
        freeStore.add((FreeBlock *)block.ptr);
    }

    really_inline void remove(Block block) {
        freeStore.remove((FreeBlock *)block.ptr);
    }

    really_inline void update(unsigned int oldSize, Block block) {}

    really_inline Block findAndRemove(unsigned int size, bool hot)
    {
        typename decltype(freeStore)::Iterator best = freeStore.end();
        unsigned int bestSize = -1u;

        for(auto it = freeStore.iterator(); it.current(); it.step())
        {
            unsigned int currSize = Block(it.current()).getSize();

            if(currSize == size)
            {
                return it.remove();
            }
            else if (currSize > size && currSize < bestSize)
            {
                bestSize = currSize;
                best = it;
            }
        }

        if(best != freeStore.end())
        {
            return best.remove();
        }

        return {nullptr};
    }

};

/**
 * Heap with BestFitPolicy.
 *
 * Facade to provide nicer usage, with automatically matching redundant parameters.
 */
template<class SizeType, unsigned int alignmentBits, bool cheksummingOn = false>
using BestFitHeap = Heap<BestFitPolicy<SizeType>, SizeType, alignmentBits, cheksummingOn>;

}

#endif /* BESTFITPOLICY_H_ */
