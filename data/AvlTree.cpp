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

#include "data/AvlTree.h"

using namespace container;

AvlTree::AvlTree() {
	root = 0;
}

inline int AvlTree::subtree_size(Node* subtreeroot)
{
	if(subtreeroot->smallsize > subtreeroot->bigsize)
		return subtreeroot->smallsize;
	else
		return subtreeroot->bigsize;
}

inline void AvlTree::calc_sizes(Node* i){
	if(i->small)
		i->smallsize = subtree_size((Node*)i->small)+1;
	else
		i->smallsize = 0;
	if(i->big)
		i->bigsize = subtree_size((Node*)i->big)+1;
	else
		i->bigsize = 0;
}

inline AvlTree::Node** AvlTree::get_root_parent_which(Node* str){
	if(str->parent){
		if(str->parent->small == str)
			return (Node**)&str->parent->small;
		else
			return (Node**)&str->parent->big;
	}else
		return (AvlTree::Node**)&root;
}

void AvlTree::rotate_s2p(Node* str){
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

void AvlTree::rotate_b2p(Node* str){
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

void AvlTree::normalize(Node *node){
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

void AvlTree::insert(Position pos, AvlTree::Node* node)
{
	*pos.origin = node;

	node->small = (Node*)0;
	node->big = (Node*)0;
	node->parent = (Node*)pos.parent;

	normalize(node);
}

void AvlTree::doRemove(Node* node, BinaryTree::Node** which)
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

void AvlTree::remove(Position pos){
	doRemove((Node*)pos.getNode(), pos.origin);
}

void AvlTree::remove(Node* node){
	doRemove(node, (BinaryTree::Node**)get_root_parent_which(node));
}
