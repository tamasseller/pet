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

#ifndef TREE_H_
#define TREE_H_

#include "algorithm/Iterate.h"

#include "ubiquitous/Trace.h"

namespace pet {

class BinaryTreeTrace: public pet::Trace<BinaryTreeTrace> {};

/**
 * Generic in-memory binary search tree.
 *
 * Represents a binary tree in which every node can directly access both of its children and its parent.
 * Mainly useful as a base of other trees as it provides method only for searching and does not for adding
 * and removing elements.
 */
class BinaryTree {
public:
	/**
	 * Base of the contained nodes.
	 *
	 * It contains the three required pointers: smaller child, greater child and the parent.
	 * For the root node the parent is NULL. For a leaf both children are NULL. For every other
	 * at most one child can be NULL.
	 *
	 * @note 	The user has to derive its data type from this class.
	 */
	class Node {
	public:
		Node *small;	//!< The smaller child
		Node *big;		//!< The bigger child
		Node *parent;	//!< The parent

		/** Create a disconnected node */
		inline Node(): small(0), big(0), parent(0) {}
	};

	/**
	 * In-order iterator.
	 *
	 * Traverses the tree following the natural ordering of the elements, it does not do any
	 * comparisons, the structure of the tree already encodes the order.
	 *
	 * @warning	Modifying the container while iterating through it with an iterator results in
	 * 			undefined behavior. This condition is not checked, because there is no
	 * 			zero-overhead way to do it. Avoiding misusage is entirely up to the user.
	 */
	class Iterator {
			/// The node the iterator is at.
			Node* currentNode;

		public:

		/**
		 * Current node or NULL.
		 *
		 * @return The current state of the iteration: the current node or NULL if over the end.
		 */
		inline Node *current() const {
			return currentNode;
		}

		/**
		 * Take a step.
		 *
		 * Steps the iterator or does nothing if already finished.
		 */
		inline void step();

		/**
		 * Creates an iterator at a position.
		 *
		 * Creates an iterator whit the specified node as the current position of it.
		 *
		 * @param first The node to start with.
		 */
		Iterator(Node* first): currentNode(first){}
	};

protected:
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
	 *
	 * There are several different configurations regarding the validity and
	 * meaning of these pointers:
	 *
	 *  1. The found element is the root or if an empty tree is traversed:
	 *
	 *    - there is no parent,
	 *    - the back reference (origin) is the _root_ pointer of the tree.
	 *
	 *  2. The node is found and is not the root of the tree:
	 *
	 *    - the parent pointer is set to the nodes parent,
	 *    - the origin points to either the _small_ or _big_ pointer of
	 *      the parent, in accordance with the actual configuration of
	 *      the found node and its parent.
	 *
	 *  3. The node is not found:
	 *
	 *    - the parent pointer points at the leaf where the search terminated,
	 *    - the origin is set the same way as above.
	 *
	 * In all cases, this object provides sufficient information for:
	 *
	 *  - finding a node or determining its absence,
	 *  - deleting a node that is found,
	 *  - adding a new node that is not found.
	 */
	struct Position {
		/// The parent of the node found or to be inserted or NULL for the root
		Node *parent = nullptr;

		/// A pointer to the child pointer of parent or to the root pointer if it is the root
		Node **origin = 0;

		/// Get the node or null if not present.
		inline Node *getNode ()const{return *origin;}

		inline Position (Node* parent, Node** origin): parent(parent), origin(origin) {}
	};

	/**
	 * The root node of the tree.
	 *
	 * Points at the root of the tree, for an empty tree this field is null.
	 */
	Node *root;

public:
	/**
	 * Initial in-order iterator.
	 *
	 * @return An iterator that points to the foremost possible location.
	 */
	inline Iterator iterator() const {
		Node* first = root;

		if(first) {
			/*
			 * Descend to the smallest node.
			 */
			while(first->small)
				first = first->small;
		}

		return Iterator(first);
	}

	/**
	 * In-order iterator from node.
	 *
	 * @return An iterator that points to the node specified.
	 */
	inline Iterator iterator(Node* start) const {
		return Iterator(start);
	}

	/**
	 * Find an element by comparison.
	 *
	 * @tparam	KeyType the type of the key
	 * @tparam	comp a pointer to a method that can compare a node to the key.
	 * @param	key the key to look for.
	 *
	 * @note	The comparator must return a *positive* value if the *node* is *greater*,
	 * 			zero if equal, negative otherwise.
	 */
	template <typename KeyType, int (*const comp)(Node*, const KeyType&)>
	inline Position seek(const KeyType& key) const{
		/*
		 * Initialize the return value with the result for an empty tree,
		 * because the following loop will fall through in that case and
		 * simply return this initial value.
		 */
		Position ret(nullptr, (BinaryTree::Node**)&root);

		/*
		 * Descend starting from the root:
		 *
		 *  - take the smaller of greater link depending on key comparison,
		 *  - terminate on equality
		 *  - also exit if a leaf is reached.
		 */
		for(Node *it = root; it; it = *ret.origin){
			if(int cmp = comp(it, key)) {
				/*
				 * If the comparison function reports non equality take
				 * the smaller or greater child depending on the result
				 */
				ret.origin = (cmp > 0) ? &it->small : &it->big;

				/*
				 * Update the parent pointer now, because this information
				 * is lost if terminating due to falling off a leaf.
				 */
				ret.parent = it;
			} else {
				/*
				 * Exit on equality.
				 */
				break;
			}
		}

		return ret;
	}
};

inline void BinaryTree::Iterator::step()
{
    if(!currentNode)
        return;

    if(currentNode->big) {
        /*
         * The small subtree is considered done, so at any node that has a
         * greater child the next one is smallest node in the subtree of that.
         *
         *   -> 1            1
         *        \            \
         *          3   =>       3
         *         /            /
         *        2         -> 2
         */
        currentNode = currentNode->big;

        while(currentNode->small)
        	currentNode = currentNode->small;

    } else {
        /*
         * If there is no greater child, the next node can be found by climbing
         * back up. As long as stepping back along a greater child link, it is
         * sure that the next element is smaller, thus it is considered already
         * visited. Once stepped back through a smaller child link we have reached
         * the next node in order.
         *
         *       3         -> 3
         *     /            /
         *   1       =>   1
         *    \            \
         *  -> 2            2
         */
        while(true) {
        	// Save the previous node.
        	Node* prev = currentNode;

        	// Step to the parent.
            currentNode = currentNode->parent;

            // The root must have been visited already.
            if(!currentNode)
            	break;

            // Done if stepped along small child link.
			if(currentNode->small == prev)
				break;

            BinaryTreeTrace::assert(currentNode->big == prev);
        }
    }
}

}

#endif /* TREE_H_ */
