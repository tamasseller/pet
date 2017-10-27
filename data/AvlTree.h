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

namespace pet {

/**
 * Self balancing in-memory binary search tree.
 *
 * An implementation of the [AVL tree](https://en.wikipedia.org/wiki/AVL_tree) algorithm.
 * Extends the functionality of the BinaryTree with the possibility to add and remove elements.
 * A compromise is made between managing the shape of the tree to stay as spread out as possible
 * and minimizing the operations needed for the add and remove operations. The AVL algorithm and
 * the [Red-black tree](https://en.wikipedia.org/wiki/Red%E2%80%93black_tree) are the two most
 * commonly used self-balancing tree algorithms. For most cases they perform roughly equally,
 * however elaborated benchmarks show that AVL tree is better if the access pattern is read-heavy
 * because it is slightly slower to modify the tree than it is with the RB tree however it results
 * in a more spread out tree.
 */

class AvlTree: public BinaryTree{
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
		/// The height of the smaller subtree.
		unsigned short smallsize;

		/// The height of the bigger subtree.
		unsigned short bigsize;

		inline void* operator new( size_t sz, void* here ) {return here;}
	};

private:
	/**
	 * Update tree structure around node.
	 *
	 * Goes up the tree from _node_ and does rotations to
	 * satisfy balancing the criteria.
	 */
	inline void normalize(Node *node);

	/**
	 * Rotate big to parent.
	 *
	 *      B                    D
	 *    /   \                /   \
	 *  A       D     =>     B       E
	 *         / \          / \
	 *        C   E        A   C
	 */
	inline void rotateBigToParent(Node* str);

	/**
	 * Rotate small to parent.
	 *
	 *        D                B
	 *      /   \            /   \
	 *    B       E   =>   A       D
	 *   / \                      / \
	 *  A   C                    C   E
	 */
	inline void rotateSmallToParent(Node* str);

	/**
	 * Get the parent's reference to this node.
	 *
	 * Returns a pointer either the matching child pointer of the
	 * parent or the to the root pointer of the tree.
	 */
	inline BinaryTree::Node** getParentBackref(Node* str);

	/**
	 * Get maximal depth under this node.
	 *
	 * Returns the maximal distance of the leafs from the specified
	 * node in its subtree. It does not iterate over the subtree,
	 * but uses the stored values instead.
	 */
	inline int subtreeSize(Node* str);

	/**
	 * Update the stored subtree sizes.
	 *
	 * Sets the subtree size counters according the children's report.
	 */
	inline void updateSubtreeSizes(Node* i);

	/**
	 * Remove worker.
	 *
	 * Removes the specified node given its parent's child pointer.
	 */
	inline void doRemove(Node*, BinaryTree::Node**);
public:
	inline AvlTree() {
	    root = nullptr;
	}

	/**
	 * Add an element at a given position.
	 *
	 * This is the addition only, the result of the prior search needs to be fed into it.
	 *
	 * @param	node the node to be inserted.
	 * @param	pos the position to insert it at.
	 */
	inline void insert(Position pos, Node* node);

	/**
	 * Remove the element from a given position.
	 *
	 * This is the remove operation only, the result of the prior search needs to be fed into it.
	 *
	 * @param	pos the position of the element to be deleted.
	 */
	inline void remove(Position pos);

	/**
	 * Removes an element.
	 *
	 * For this variant no search is needed, however it is the responsibility of the user the ensure
	 * that the node passed in the argument is actually contained in the list. It is undefined (but
	 * probably pretty bad) what happens when this condition does not hold.
	 *
	 * @param	node the node to be deleted.
	 */
	inline void remove(Node *node);
};

inline int AvlTree::subtreeSize(Node* str)
{
	/*
	 * The size of this subtree is the depth of the deeper child's plus one.
	 */
    return ((str->smallsize > str->bigsize) ? str->smallsize : str->bigsize) + 1;
}

inline void AvlTree::updateSubtreeSizes(Node* i){
	i->smallsize = (i->small) ? subtreeSize((Node*)i->small) : 0;
	i->bigsize = (i->big) ? subtreeSize((Node*)i->big) : 0;
}

inline typename BinaryTree::Node** AvlTree::getParentBackref(Node* str)
{
	/*
	 * If there is a parent its matching child pointer is returned,
	 */
    if(BinaryTree::Node* parent = str->parent)
    	return ((parent->small == str) ? &parent->small : &parent->big);

    /*
     * Otherwise _str_ is the root, so the tree's root pointer is returned.
     */
   	return &root;
}

inline void AvlTree::rotateSmallToParent(Node* str)
{
	/*
	 *   <parent>
	 *       \
	 *      <str>
	 *      /   \
	 * <small>   Z
	 *   / \
	 *  X   Y
	 */
	BinaryTree::Node** root_parent_which = getParentBackref(str);
    Node* small = (Node*)str->small;

	/*
	 *   <parent>
	 *       \
	 *        \     <str>
	 *         \    /   \
	 *         <small>   Z
	 *           / \
	 *          X   Y
	 */
    *root_parent_which = small;
    small->parent = str->parent;

	/*
	 *   <parent>
	 *       \
	 *      <small>   <str>
	 *        /        / \
	 *       X        Y   Z
	 *
	 * Y := small->big
	 */
    str->small = small->big;
    if(small->big) small->big->parent = str;

    /*
	 *   <parent>
	 *       \
	 *      <small>
	 *       /   \
	 *      X    <str>
	 *           /  \
	 *          Y    Z
	 */
    small->big = str;
    str->parent = small;

    /*
     * The subtree sizes for both _small_ and _str_ are needed
     * to be updated. However the subtrees of X, Y and Z have
     * not been touched, so their subtree sizes can be used.
     */
    updateSubtreeSizes(str);
    updateSubtreeSizes(small);
}

inline void AvlTree::rotateBigToParent(Node* str){
	/*
	 * Does the inverse of _rotateSmallToParent_.
	 */
	BinaryTree::Node** root_parent_which = getParentBackref(str);
    Node* big = (Node*)str->big;

    *root_parent_which = big;
    big->parent = str->parent;

    if(big->small)
        big->small->parent = str;
    str->big = big->small;

    big->small = str;
    str->parent = big;

    updateSubtreeSizes(str);
    updateSubtreeSizes(big);
}

inline void AvlTree::normalize(Node *node)
{
	/*
	 * Normalization is done by going up the tree from
	 * the specified node. And checking if the balancing
	 * criteria are still met. If not it is restored
	 * employing tree rotations.
	 */
    for(Node *i=node; i; ) {
        Node *next = static_cast<Node*>(i->parent);
        Node *big = static_cast<Node*>(i->big);
        Node *small = static_cast<Node*>(i->small);

        updateSubtreeSizes(i);

        if(i->bigsize > i->smallsize + 1) {
            /*
             * If the subtree of the currently considered
             * node leans towards the big side too much,
             * its subtree needs to be checked to be either
             * balanced or big heavy. If it is not, than
             * needs to be rotated to straighten it out.
             *
             * A          A
             *   \         \           B
             *     C  ->    B    ->   / \
             *    /          \       A   C
             *   B            C
             */
            if(big->smallsize > big->bigsize)
                rotateSmallToParent(big);

            rotateBigToParent(i);
        } else if(i->smallsize > i->bigsize + 1) {
        	/*
        	 * Small heavy subtree is treated in the exact
        	 * inverse way of the other case.
        	 */
            if(small->bigsize > small->smallsize)
                rotateBigToParent(small);

            rotateSmallToParent(i);
        }

        i = next;
    }
}

inline void AvlTree::insert(Position pos, AvlTree::Node* node)
{
	/*
	 * Establish the bi-directional parent-child link.
	 */
    *pos.origin = node;
    node->parent = (Node*)pos.parent;

    /*
     * Zero out children, to signify a leaf node.
     */
    node->small = node->big = nullptr;

    /*
     * Enforce the AVL tree invariant.
     */
    normalize(node);
}

inline void AvlTree::doRemove(Node* node, BinaryTree::Node** which)
{
    if(node->small && node->big){
    	/*
    	 * An internal node with two children is replaced with the
    	 * previous element in order, which is either a leaf or an
    	 * internal node that has no greater child, because if it
    	 * had, then it would not be the next in order, but the
    	 * child (or the smallest element in its subtree) would be.
    	 *
    	 *             <node>                 <node>
    	 *             /   \                  /   \
    	 *   <replacement>  X    or         ...    X
    	 *    /                             /  \
    	 *  ...                          ...   <replacement>
    	 */
        BinaryTree::Node *origParent, *replacement = node->small;

        if(!replacement->big){
        	/*
        	 * In the first case the _replacement_ is bound to the
        	 * parent and the greater child of the node, which are
        	 * common operations for both cases, the only difference
        	 * being that the normalization needs to start from the
        	 * _replacement_ not its original parent, because it is
        	 * the node that is being removed.
        	 */
            origParent = replacement;
        }else{
        	/*
        	 * In the second case the greatest element of the
        	 * smaller subtree must be found.
        	 */
            while(replacement->big)
                replacement = replacement->big;

            /*
             * The structure of the tree is only modified at the
             * point where the replacement is taken from, so the
             * later normalization needs to start at its parent.
             */
            origParent = replacement->parent;

            /*
             * The replacement is the greater child of its parent.
             * It needs to be cut out from its place by attaching
             * its smaller child as the greater child of the parent.
             */
            replacement->parent->big = (Node*)replacement->small;
            if(replacement->small)
            	replacement->small->parent = replacement->parent;

            /*
             * The smaller child of the node is inherited by the
             * replacement.
             */
            replacement->small = node->small;
            replacement->small->parent = replacement;
        }

        /*
         * Attach X as the big child of _replacement_ in both directions.
         */
        replacement->big = node->big;
        node->big->parent = replacement;

        /*
         * Attach _replacement_ as the same child as node have been,
         * to its parent, in both directions.
         */
        replacement->parent = node->parent;
        *which = replacement;

        /*
         * Enforce tree invariant, with the starting place
         * depending on the previous operations.
         */
        normalize((Node*)origParent);
    }else{
    	/*
    	 * If the node has only one child or it is a leaf, it is simply
    	 * cut off and its child is attached to the parent in its place.
    	 */
        if(node->small){
            *which = node->small;
            node->small->parent = node->parent;
            normalize((Node*)node->parent);
        }else{
            *which = node->big;
            if(node->big)
                node->big->parent = node->parent;
        }

        /*
         * Enforce tree invariant, starting
         * at the parent of the removed node.
         */
        normalize((Node*)node->parent);
    }
}

inline void AvlTree::remove(Position pos){
    doRemove((Node*)pos.getNode(), pos.origin);
}

inline void AvlTree::remove(Node* node){
    doRemove(node, getParentBackref(node));
}


}

#endif /* SRC_AVLTREE_H_ */
