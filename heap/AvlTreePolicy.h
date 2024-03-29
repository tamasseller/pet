/*******************************************************************************
 *
 * Copyright (c) 2017 Tamás Seller. All rights reserved.
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
#ifndef AVLTREEPOLICY_H_
#define AVLTREEPOLICY_H_

#include "heap/Heap.h"
#include "data/AvlTree.h"

class AvlPolicyInternalsTest;

namespace pet {

/**
 * Balanced binary search tree based best-fit allocator policy.
 *
 * This is a dynamic memory management policy for Heap host class. Together they
 * form a non-realtime heap, capable of allocating and reclaiming a full or partial
 * block with logarithmic time complexity by the number of free blocks.
 */

template <class SizeType>
class AvlTreePolicy: protected HeapBase<SizeType>, protected pet::AvlTree
{
    using typename HeapBase<SizeType>::Block;

    static int sizeCompare(BinaryTree::Node* block, const unsigned int &size) {
        return (int)(Block(block).getSize() - size);
    }

protected:
    /** @copydoc pet::TlsfPolicy::freeHeaderSize */
    static constexpr uintptr_t freeHeaderSize = sizeof(AvlTree::Node);

    /** @copydoc pet::TlsfPolicy::init */
    inline void init(Block block)
    {
        root = nullptr;
        add(block);
    }

    /** @copydoc pet::TlsfPolicy::add */
    inline void add(Block block)
    {
        BinaryTree::Position pos = BinaryTree::seek<unsigned int, &AvlTreePolicy::sizeCompare>(block.getSize());

        while(pos.getNode()) {
            pos.parent = pos.getNode();
            pos.origin = &pos.getNode()->small;
        }

        insert(pos, new(block.ptr) AvlTree::Node);
    }

    /** @copydoc pet::TlsfPolicy::remove */
    inline void remove(Block block)
    {
        AvlTree::remove((AvlTree::Node*) block.ptr);
    }

    /** @copydoc pet::TlsfPolicy::findAndRemove */
    inline Block findAndRemove(unsigned int size, bool hot)
    {
        BinaryTree::Position pos = BinaryTree::seek<unsigned int, &AvlTreePolicy::sizeCompare>(size);

        if(Node *node = (Node*)pos.getNode())
        {
            remove(node);
            return node;
        }

        for(BinaryTree::Iterator it = iterator(pos.parent); it.current(); it.step())
        {
            if(Block(it.current()).getSize() >= size)
            {
                AvlTree::remove((AvlTree::Node*) it.current());
                return it.current();
            }
        }

        return 0;
    }

    /** @copydoc pet::TlsfPolicy::update */
    inline void update(unsigned int oldSize, Block block)
    {
        remove(block);
        add(block);
    }

};

/**
 * Heap with AvlPolicy.
 *
 * Facade to provide nicer usage, with automatically matching redundant parameters.
 */
template<class SizeType, unsigned int alignmentBits, bool cheksummingOn = false>
using AvlHeap = Heap<AvlTreePolicy<SizeType>, SizeType, alignmentBits, cheksummingOn>;

}

#endif /* AVLTREEPOLICY_H_ */
