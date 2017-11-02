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

#ifndef BINARYHEAP_H_
#define BINARYHEAP_H_

#include "algorithm/MinMaxHeap.h"

namespace pet {

class HeapNode {
	public:
		HeapNode *parent;		//!< The parent
		HeapNode *children[2];	//!< The children

		/** Create a disconnected node */
		inline HeapNode(): children{nullptr, nullptr}, parent(nullptr) {}
};

template<bool (*compare)(HeapNode*, HeapNode*)>
class BinaryHeap: PrioQueueBase<BinaryHeap<compare>, HeapNode*> {
	friend class PrioQueueBase<BinaryHeap<compare>, HeapNode*>;
	using Node = HeapNode;

	Node* root;
	size_t nElements;

    /// Helper to find the parent.
	static constexpr inline Node* parent(Node* n) {
    	return n->parent;
    }

    /// Helper to find one child.
    static constexpr inline Node* leftChild(Node* n) {
    	return n->children[0];
    }

    /// Helper to find the other child.
    static constexpr inline Node* rightChild(Node* n) {
    	return n->children[1];
    }

    static constexpr inline Node* moveUp(Node* current, Node* parent) {
		return current;
	}

    static constexpr inline Node* moveDown(Node* parent, Node* child) {
		return parent;
	}


	static constexpr inline bool isValid(Node* n) {
    	return n != nullptr;
    }

    static constexpr inline bool compareElement(Node* a, Node* b) {
         return compare(a, b);
    }

    static inline void swapPointers(Node* &a, Node* &b) {
    	Node* const temp = a;
    	a = b;
    	b = temp;
    }

    inline void swapElement(Node* a, Node* b) {
    	*(a->parent ? (a->parent->children + (a->parent->children[0] == a ? 0 : 1)) : &root) = b;
    	*(b->parent ? (b->parent->children + (b->parent->children[0] == b ? 0 : 1)) : &root) = a;
    	swapPointers(a->parent, b->parent);

    	for(int i=0; i<2; i++) {
        	swapPointers(a->children[i], b->children[i]);

        	if(a->children[i])
        		a->children[i]->parent = a;

        	if(b->children[i])
				b->children[i]->parent = b;
    	}
    }

    struct Position {
    	Node* parent;
    	Node** backLink;
    };

    Position findLastPlace()
    {
		size_t path = nElements, low = 0, high = 31;
    	Position ret{nullptr, &root};

		while(low != high){
			unsigned int mid = (low + high + 1) >> 1;
			if(~((1 << mid) - 1) & path)
				low = mid;
			else
				high = mid - 1;
		}

		while(low) {
			const size_t currentBit = 1 << (low-- - 1);
			ret.parent = *ret.backLink;
			ret.backLink = ret.parent->children + ((path & currentBit) ? 1 : 0);
		}

    	return ret;
    }

public:
    void insert(Node* n) {
    	nElements++;
    	Position pos = findLastPlace();
    	n->children[0] = n->children[1] = nullptr;
    	*pos.backLink = n;
    	n->parent = pos.parent;
    	this->heapUp(n);
    }

    const Node* extreme() {
    	return root;
    }

    const void remove(Node* n) {
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

    const void pop()
    {
    	if(!root)
    		return;

    	remove(root);
    }

};

}

#endif /* BINARYHEAP_H_ */
