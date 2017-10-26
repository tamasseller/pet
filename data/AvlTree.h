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

namespace detail {

struct TrivialAccessor {
};

}

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

template<class Accessor> class BasicAvlTree;
using AvlTree = BasicAvlTree<detail::TrivialAccessor>;

template<class Accessor>
class BasicAvlTree: public BinaryTree{
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
	inline void normalize(Node *node);
	inline void rotate_b2p(Node* str);
	inline void rotate_s2p(Node* str);

	inline Node** get_root_parent_which(Node* str);
	inline void calc_sizes(Node* i);
	inline int subtree_size(Node* subtreeroot);
	inline void doRemove(Node*, BinaryTree::Node**);
public:
	inline BasicAvlTree() {
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

template<class Accessor>
inline int BasicAvlTree<Accessor>::subtree_size(Node* subtreeroot)
{
    if(subtreeroot->smallsize > subtreeroot->bigsize)
        return subtreeroot->smallsize;
    else
        return subtreeroot->bigsize;
}

template<class Accessor>
inline void BasicAvlTree<Accessor>::calc_sizes(Node* i){
    if(i->small)
        i->smallsize = subtree_size((Node*)i->small)+1;
    else
        i->smallsize = 0;
    if(i->big)
        i->bigsize = subtree_size((Node*)i->big)+1;
    else
        i->bigsize = 0;
}

template<class Accessor>
inline typename BasicAvlTree<Accessor>::Node** BasicAvlTree<Accessor>::get_root_parent_which(Node* str){
    if(str->parent){
        if(str->parent->small == str)
            return (Node**)&str->parent->small;
        else
            return (Node**)&str->parent->big;
    }else
        return (BasicAvlTree::Node**)&root;
}

template<class Accessor>
inline void BasicAvlTree<Accessor>::rotate_s2p(Node* str){
    Node** root_parent_which = get_root_parent_which(str);
    Node* small = (Node*)str->small;

    *root_parent_which = small;
    small->parent = str->parent;

    if(small->big)
        small->big->parent = str;
    str->small = small->big;

    small->big = str;
    str->parent = small;

    calc_sizes(str);
    calc_sizes(small);
}

template<class Accessor>
inline void BasicAvlTree<Accessor>::rotate_b2p(Node* str){
    Node** root_parent_which = get_root_parent_which(str);
    Node* big = (Node*)str->big;

    *root_parent_which = big;
    big->parent = str->parent;

    if(big->small)
        big->small->parent = str;
    str->big = big->small;

    big->small = str;
    str->parent = big;

    calc_sizes(str);
    calc_sizes(big);
}

template<class Accessor>
inline void BasicAvlTree<Accessor>::normalize(Node *node){
    Node *i=node;
    while(i){
        Node *next = (Node*)i->parent;
        calc_sizes(i);
        if(i->bigsize > i->smallsize+1){
            if(((Node*)i->big)->smallsize > ((Node*)i->big)->bigsize){
                rotate_s2p((Node*)i->big);
            }
            rotate_b2p(i);
        }
        else if(i->smallsize > i->bigsize+1){
            if(((Node*)i->small)->bigsize > ((Node*)i->small)->smallsize){
                rotate_b2p((Node*)i->small);
            }
            rotate_s2p(i);
        }
        i = next;
    }
}

template<class Accessor>
inline void BasicAvlTree<Accessor>::insert(Position pos, BasicAvlTree::Node* node)
{
    *pos.origin = node;

    node->small = (Node*)0;
    node->big = (Node*)0;
    node->parent = (Node*)pos.parent;

    normalize(node);
}

template<class Accessor>
inline void BasicAvlTree<Accessor>::doRemove(Node* node, BinaryTree::Node** which)
{
    if(node->small && node->big){
        Node* replacement = node;
        replacement = (Node*)replacement->small;
        while(replacement->big)
            replacement = (Node*)replacement->big;

        Node* orig_parent;
        if(replacement->parent != node){
            replacement->parent->big = (Node*)replacement->small;
            if(replacement->small)
                replacement->small->parent = replacement->parent;
            orig_parent = (Node*)replacement->parent;

            replacement->small = (Node*)node->small;
            replacement->small->parent = replacement;
        }else{
            orig_parent = replacement;
        }

        replacement->big = (Node*)node->big;
        replacement->big->parent = replacement;

        replacement->parent = (Node*)node->parent;
        *which = replacement;

        normalize(orig_parent);
    }else{
        if(node->small){
            *which = node->small;
            node->small->parent = node->parent;
            normalize((Node*)node->parent);
        }else{
            *which = node->big;
            if(node->big){
                node->big->parent = node->parent;
                normalize((Node*)node->parent);
            }else{
                normalize((Node*)node->parent);
            }
        }
    }
}

template<class Accessor>
inline void BasicAvlTree<Accessor>::remove(Position pos){
    doRemove((Node*)pos.getNode(), pos.origin);
}

template<class Accessor>
inline void BasicAvlTree<Accessor>::remove(Node* node){
    doRemove(node, (BinaryTree::Node**)get_root_parent_which(node));
}


}

#endif /* SRC_AVLTREE_H_ */
