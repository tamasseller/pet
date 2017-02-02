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

#ifndef SRC_AVLTREE_H_
#define SRC_AVLTREE_H_

#include "data/BinaryTree.h"

#include <stddef.h>

namespace container {

/**
 * Self balancing in-memory binary search tree.
 *
 * An implementation of the [AVL tree](https://en.wikipedia.org/wiki/AVL_tree) algorithm.
 * Extends the functionality of the BinaryTree with the possibility to add and remove elements.
 * A compromise is made between managing the shape of the tree to stay as spread out as possible
 * and minimizing the operations needed for the add and remove operations. The AVL algorithm and
 * the [Red-black tree](https://en.wikipedia.org/wiki/Red%E2%80%93black_tree) are the two most
 * commonly used self-balancing tree algorithms. For most cases they perform roughly equally,
 * however elaborated benchmarks show that AVL tree is better if the access pattern is read hevy
 * because it is slightly slower to modify the tree than it is with the RB tree however it results
 * in a more spread out tree.
 */
class AvlTree : public BinaryTree{
public:
	/**
	 * Base of the contained nodes.
	 *
	 * It contains additional info needed for the leveling algorithm.
	 *
	 * @note 	The user has to derive its data type from this class.
	 */

	class Node : public BinaryTree::Node{
	public:
		unsigned short smallsize;	//!< The height of the smaller subtree.
		unsigned short bigsize;		//!< The height of the bigger subtree.
		inline void* operator new( size_t sz, void* here ) {return here;}
	};

private:
	void normalize(Node *node);
	void rotate_b2p(Node* str);
	void rotate_s2p(Node* str);

	inline Node** get_root_parent_which(Node* str);
	inline void calc_sizes(Node* i);
	inline int subtree_size(Node* subtreeroot);
	inline void doRemove(Node*, BinaryTree::Node**);
public:
	AvlTree();
	/**
	 * Add an element at a given position.
	 *
	 * This is the addition only, the result of the prior search needs to be fed into it.
	 *
	 * @param	node the node to be inserted.
	 * @param	pos the position to insert it at.
	 */
	void insert(Position pos, Node* node);

	/**
	 * Remove the element from a given position.
	 *
	 * This is the remove operation only, the result of the prior search needs to be fed into it.
	 *
	 * @param	pos the position of the element to be deleted.
	 */
	void remove(Position pos);

	/**
	 * Removes an element.
	 *
	 * For this variant no search is needed, however it is the responsibility of the user the ensure
	 * that the node passed in the argument is actually contained in the list. It is undefined (but
	 * probably pretty bad) what happens when this condition does not hold.
	 *
	 * @param	node the node to be deleted.
	 */
	void remove(Node *node);
};

}

#endif /* SRC_AVLTREE_H_ */
