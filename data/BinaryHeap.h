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

#ifndef BINARYHEAP_H_
#define BINARYHEAP_H_

#include "algorithm/MinMaxHeap.h"

namespace pet {

/**
 * A node of the BinaryHeap.
 */
struct HeapNode {
        /// Pointer to the parent of the node or null for the root.
        HeapNode *parent;

        /// Pointer to the children of the node or null if not exists.
        HeapNode *children[2];

        /// Create a disconnected node.
        inline HeapNode(): parent(nullptr), children{nullptr, nullptr} {}
};

/**
 * Intrusive binary heap, based on the generic PrioQueueBase logic.
 *
 * This class implements a binary heap (a binary tree that is heap
 * structured) according to the used supplied comparison function,
 * that is of the 'less' kind. Which means that it must return true
 * if the strict ordering relation between the elements is true.
 */
template<bool (*compare)(const HeapNode*, const HeapNode*)>
class BinaryHeap: PrioQueueBase<BinaryHeap<compare>, HeapNode*> {
    friend class PrioQueueBase<BinaryHeap<compare>, HeapNode*>;
    using Node = HeapNode;

    /// Swap operations do not change the target of ids (PrioQueueBase requirement).
    static constexpr bool areIdsStable = true;

    /// Helper to find the parent (PrioQueueBase requirement).
    static constexpr inline Node* parent(Node* n) {
        return n->parent;
    }

    /// Helper to find one child (PrioQueueBase requirement).
    static constexpr inline Node* leftChild(Node* n) {
        return n->children[0];
    }

    /// Helper to find the other child (PrioQueueBase requirement).
    static constexpr inline Node* rightChild(Node* n) {
        return n->children[1];
    }

    /// Helper to determine element reference validity (PrioQueueBase requirement).
    static constexpr inline bool isValid(Node* n) {
        return n != nullptr;
    }

    /// Element comparator, 'less' type (PrioQueueBase requirement).
    static constexpr inline bool compareElement(Node* a, Node* b) {
         return compare(a, b);
    }

    /// Pointer to the root node, or null if empty.
    Node* root = nullptr;

    /// The number of elements in the container (used for finding the last element).
    size_t nElements = 0;

    /// Exchange the contents of two pointers.
    static inline void swapPointers(Node* &a, Node* &b) {
        Node* const temp = a;
        a = b;
        b = temp;
    }

    /// Exchange the position of two element in the tree (PrioQueueBase requirement).
    inline void swapElement(Node* a, Node* b) {
        /*
         * Update parent backlinks to the swapped state.
         */
        *(a->parent ? (a->parent->children + (a->parent->children[0] == a ? 0 : 1)) : &root) = b;
        *(b->parent ? (b->parent->children + (b->parent->children[0] == b ? 0 : 1)) : &root) = a;

        /*
         * Swap the nodes parent pointers.
         */
        swapPointers(a->parent, b->parent);

        /*
         * Swap the children and update their parent pointers.
         */
        for(int i=0; i<2; i++) {
            swapPointers(a->children[i], b->children[i]);

            if(a->children[i])
                a->children[i]->parent = a;

            if(b->children[i])
                b->children[i]->parent = b;
        }
    }


    /**
     * In-tree position of an existing or new node.
     *
     * An instance of this type identifies an abstract position in the tree,
     * that can be obtained while searching for a given key. It describes
     * the position in the tree where the node is found, or where it needs
     * to be inserted.
     *
     * It contains:
     *
     *  - a pointer to the parent node if there is any
     *  - and a pointer to the pointer that is to be pointed at the node.
     */
    struct Position {
        Node* parent;
        Node** backLink;
    };

    /**
     * Find the Position of the last element defined by the number of elements.
     */
    Position findLastPlace()
    {
        size_t path = nElements, low = 0, high = 31;
        Position ret{nullptr, &root};

        /*
         * Halving search for the bit position of the most significant one bit.
         */
        while(low != high){
            size_t mid = (low + high + 1) >> 1;
            if(~((1 << mid) - 1) & path)
                low = mid;
            else
                high = mid - 1;
        }

        // ^^^ TODO replace with clz based approach ^^^

        /*
         * Follow the path defined by the number of elements to reach the
         * parent of the last element. At every bit position, zero bits mean
         * 'left' turn one bits mean 'right'.
         */
        while(low) {
            const size_t currentBit = 1 << (low-- - 1);
            ret.parent = *ret.backLink;
            ret.backLink = ret.parent->children + ((path & currentBit) ? 1 : 0);
        }

        return ret;
    }

public:
    /**
     * Access the extremum, null for empty heap.
     */
    Node* extreme() {
        return root;
    }

    /**
     * Insert a node at an adequate position according the
     * comparison of elements.
     */
    void insert(Node* n) {
        nElements++;
        Position pos = findLastPlace();
        n->children[0] = n->children[1] = nullptr;
        *pos.backLink = n;
        n->parent = pos.parent;
        this->heapUp(n);
    }

    /**
     * Remove an arbitrary element and restore heap property.
     *
     * @NOTE It is an error to call this method on a node that
     *       is not contained in this heap (or it is contained
     *       in an other heap). This is not checked, as there
     *       is no zero-overhead way of doing it.
     */
    void remove(Node* n) {
        Position pos = findLastPlace();
        nElements--;

        Node* subst = *pos.backLink;
        *pos.backLink = nullptr;

        if(n == subst)
            return;

        subst->parent = n->parent;
        *(n->parent ? (n->parent->children + (n->parent->children[0] == n ? 0 : 1)) : &root) = subst;

        for(int i=0; i<2; i++) {
            subst->children[i] = n->children[i];

            if(subst->children[i])
                subst->children[i]->parent = subst;
        }

        this->heapDown(subst);
    }

    /**
     * Discard the extremum and restore heap property.
     *
     * @NOTE Emptiness is checked, nothing is done on an empty heap.
     */
    void pop()
    {
        if(!root)
            return;

        remove(root);
    }

    /**
     * Restore the heap property after updating an arbitrary element.
     *
     * @NOTE It is an error to call this method on a node that
     *       is not contained in this heap (or it is contained
     *       in an other heap). This is not checked, as there
     *       is no zero-overhead way of doing it.
     */
    void update(Node* n) {
        this->refresh(n);
    }
};

}

#endif /* BINARYHEAP_H_ */
