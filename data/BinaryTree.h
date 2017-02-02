/*******************************************************************************
 *
 * Copyright (c) 2016 Seller Tamás. All rights reserved.
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

namespace container {

class BinaryTreeTrace: public ubiq::Trace<BinaryTreeTrace> {};

/**
 * Generic in-memory binary search tree.
 *
 * Represents a binary tree in which every node can directly access both of its children and its parent.
 * Mainly useful as a base of other trees as it provides method only for searching and does not for adding
 * and removing elements.
 *
 * @note 	Genericity is *not* achieved by making this a class template make.
 * 			The functionality provided by this class does not rely on knowing the full type of the stored elements.
 * 			It is also beneficial for avoiding code bloat.
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
		Node *small;	//!< The smaller children
		Node *big;		//!< The bigger children
		Node *parent;	//!< The parent

		/** Create a disconnected node */
		Node(): small(0), big(0), parent(0) {}
	};

	/**
	 * In-order iterator.
	 *
	 * Traverses the tree following the natural ordering of the elements.
	 *
	 * @note It does not do any comparisons, the structure of the tree already encodes the order.
	 *
	 * @warning	Modifying the list while iterating through it with an iterator results in undefined behavior.
	 * 			This condition is not checked, because there is no zero-overhead way to do it, so avoiding
	 * 			this usage is entirely up to the user.
	 */
	class Iterator {
		bool goDown;
		Node* currentNode;
		public:

		/**
		 * Current node or NULL.
		 *
		 * @return The current state of the iteration: the current node or NULL if over the end.
		 */
		Node *current() const {
			return currentNode;
		}

		/**
		 * Take a step.
		 *
		 * Steps the iterator or does nothing if already finished.
		 */
		void step();

		/**
		 * Creates an iterator at a position.
		 *
		 * Creates an iterator whit the specified node as the current position of it.
		 *
		 * @param first The node to start with.
		 */
		Iterator(Node* first): goDown(first && first->big), currentNode(first){}
	};

protected:
	/**
	 * Full result of a search
	 */
	struct Position {
		Node *parent = 0;	//!< The parent of the node found or to be inserted or NULL for the root
		Node **origin = 0;	//!< A pointer to the child pointer of parent or to the root pointer if it is the root
		Node *getNode(){return *origin;} //!< Get the node or null if not present.
	};

	Node *root; //!< The root node of the tree.
public:
	/**
	 * Initial in-order iterator.
	 *
	 * @return An iterator that points to the foremost possible location.
	 */
	inline Iterator iterator() const {
		Node* first = root;

		if(first) {
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
	Position seek(const KeyType& key) const{
		Position ret;
		Node *it = root;
		while(it){
			int cmp = comp(it, key);
			if(cmp == 0)
				break;

			ret.parent = it;
			if(cmp > 0){
				ret.origin = &it->small;
			}else{
				ret.origin = &it->big;
			}
			it = *ret.origin;
		}

		if(!ret.origin)
			ret.origin = (BinaryTree::Node**)&root;

		return ret;
	}
};

}

#endif /* TREE_H_ */
