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

#ifndef TREEMAP_H_
#define TREEMAP_H_

#include "data/AvlTree.h"
#include "pool/Pool.h"
#include "pool/Arena.h"

#include <stddef.h>

namespace container {

/**
 * AVL tree backed key-value store template.
 *
 * Wraps an AVL tree, provides KV store like facade, with convenience method for reading the content.
 * This is a base for actual maps, thus provides no methods for modifying the content to avoid hard-wiring
 * specific memory management policies, that is left to the next level.
 *
 * @tparam	KeyType the type of the keys, has to be copy constructible.
 * @tparam	ValueType the type of the values, has to be copy constructible.
 *
 * @note 	Uses the subtraction operator on the KeyType to obtain the ordering over the keys.
 */
template <class KeyType, class ValueType>
class ImmutableTreeMap: protected AvlTree {
protected:
	/**
	 * Subtraction backed comparator.
	 *
	 * This is the comparator that is provided by the ImmutableTreeMap to
	 * the Avl tree as the definition of the ordering over the set of Nodes.
	 *
	 * @param	node The node to be compared.
	 * @param	key The key to be compared.
	 */
	static int comparator(BinaryTree::Node* node, const KeyType &key) {
		return ((Node*)node)->key - key;
	}

public:
	/**
	 * Base of the contained nodes.
	 *
	 * This is the container for the user data.
	 *
	 * @note 	The user key and value data is embedded (copied in).
	 */
	class Node: public AvlTree::Node {
		friend ImmutableTreeMap;
		ValueType value;
		const KeyType key;
	public:
		/**
		 * Create a disconnected node.
		 *
		 * Create a disconnected node with specified key and value.
		 *
		 * @param	k The key.
		 * @param	v The value.
		 */
		Node(const KeyType &k, const ValueType &v): value(v), key(k) {}

		/** Placement new operator */
	    inline void* operator new( size_t sz, void* here ) {return here;}

		/**
		 * Set the value of a node.
		 *
		 * @param	v The value.
		 */
		inline void setValue(const ValueType &v) {
			value = v;
		}
	};

	/**
	 * KV map iterator facade.
	 *
	 * Provides KV map specific access to the iteration state.
	 *
	 * @warning	Modifying the list while iterating through it with an iterator results in undefined behavior.
	 * 			This condition is not checked, because there is no zero-overhead way to do it, so avoiding
	 * 			this usage is entirely up to the user.
	 */
	class Iterator: protected AvlTree::Iterator {
		friend ImmutableTreeMap;
		inline Iterator(const AvlTree::Iterator &it): AvlTree::Iterator(it) {}
	public:
		/**
		 * Take a step.
		 *
		 * Steps the iterator towards the next greater key or does nothing if already reached the end.
		 */
		inline void step() {
			AvlTree::Iterator::step();
		}

		/**
		 * Current _key_ or NULL.
		 *
		 * @return The current _key_ which the iterator is at or NULL if over the end.
		 */
		inline const KeyType* currentKey() {
			Node* current = (Node*)AvlTree::Iterator::current();

			if(!current)
				return NULL;

			return (KeyType*)&current->key;
		}

		/**
		 * Current _value_ or NULL.
		 *
		 * @return The current _value_ which the iterator is at or NULL if over the end.
		 */
		inline ValueType* currentValue() {
			Node* current = (Node*)AvlTree::Iterator::current();

			if(!current)
				return NULL;

			return (KeyType*)&current->value;
		}
	};

	/**
	 * Initial in-order iterator.
	 *
	 * @return An iterator that points to the foremost possible location.
	 */
	inline Iterator iterator() {
		return Iterator(BinaryTree::iterator());
	}

	/**
	 * Get value for a key.
	 *
	 * Convenience method for looking up a key in the store.
	 *
	 * @param	key	The key to be looked up.
	 * @return	Pointer to the mutable value inside the node or NULL if not found.
	 */
	ValueType* get(const KeyType key) const{
		BinaryTree::Position pos = BinaryTree::seek<KeyType, ImmutableTreeMap<KeyType, ValueType>::comparator>(key);
		if(pos.getNode())
			return &((Node*)pos.getNode())->value;

		return 0;
	}

	/**
	 * Check whether a key exists.
	 *
	 * Convenience method for checking the existence of a key in the store.
	 *
	 * @param	key	The whose existence is to be checked.
	 */
	bool contains(const KeyType &key) const{
		return BinaryTree::seek<KeyType, comparator>(key).getNode() != NULL;
	}
};


/**
 * Pluggable memory policy KV store.
 *
 * Extends the the ImmutableTreeMap with methods that enable adding to and removing from the store.
 * There are several pre-configured variations, but it can be used with a completely user supplied policy too.
 *
 * @tparam	KeyType the type of the keys, has to be copy constructible.
 * @tparam	ValueType the type of the values, has to be copy constructible.
 * @tparam	Pool the memory management policy, has to provide acquire and release methods.
 *
 * @note 	Uses the subtraction operator on the KeyType to obtain the ordering over the keys.
 */
template <class KeyType, class ValueType, class Pool>
class TreeMap: public ImmutableTreeMap<KeyType, ValueType> {
	Pool pool;
public:
	/** The node is simply inherited from ImmutableTreeMap */
	typedef typename ImmutableTreeMap<KeyType, ValueType>::Node Node;

	/**
	 * Set mapping for key.
	 *
	 * Associates the specified value with the specified key in this map.
	 * If the map previously contained a mapping for the key, the old value is replaced.
	 *
	 * @param	key with which the specified value is to be associated
	 * @param 	value to be associated with the specified key
	 */
	bool put(const KeyType &key, const ValueType &value){
		BinaryTree::Position pos = BinaryTree::seek<KeyType, ImmutableTreeMap<KeyType, ValueType>::comparator>(key);
		if(pos.getNode()){
			((Node*)pos.getNode())->setValue(value);
		}else{
			auto ptr = pool.acquire();

			if(ptr.failed())
				return false;

			Node* nnode = new(ptr) Node(key, value);
			AvlTree::insert(pos, nnode);
		}
	}

	/**
	 * Remove key.
	 *
	 * Removes the mapping for the specified key from this map if present.
	 *
	 * @param 	key key whose mapping is to be removed from the map
	 * @return	True on success, false if not found.
	 */
	bool remove(const KeyType &key){
		BinaryTree::Position pos = BinaryTree::seek<KeyType, ImmutableTreeMap<KeyType, ValueType>::comparator>(key);
		if(pos.getNode()){
			Node* node = (Node*)pos.getNode();
			AvlTree::remove(pos);
			pool.release(node);
			return true;
		}
		return false;
	}

	/**
	 * Frees up the elements one by one.
	 *
	 * Calls the Pool::relase() method on each element.
	 *
	 * @note	If an arena type policy is used the compiler can easily collapse
	 * 			the iterative part of this to nothing, because then the release
	 * 			method has to be empty.
	 */
	~TreeMap() {
		BinaryTree::Node* node = BinaryTree::root;

		while(node) {
			if(node->small){
				BinaryTree::Node* small = node->small;
				node->small = 0;
				node = small;
			} else if(node->big) {
				BinaryTree::Node* big = node->big;
				node->big = 0;
				node = big;
			} else {
				BinaryTree::Node* parent = node->parent;
				pool.release((Node*)node);
				node = parent;
			}
		}

		AvlTree::root = 0;
	}
};

/**
 * Arena allocator backed KV store.
 *
 * A pre-configured TreeMap that uses the Arena allocator.
 *
 * @tparam	Allocator the low-level allocator that the arena uses.
 *
 * @see 	TreeMap for details.
 */
template <class KeyType, class ValueType, class Allocator>
using ArenaTreeMap = TreeMap<	KeyType,
								ValueType,
								mm::Arena<typename ImmutableTreeMap<KeyType, ValueType>::Node, Allocator>>;

/**
 * Dynamic pool backed KV store.
 *
 * A pre-configured TreeMap that uses the DynamicPool allocator.
 *
 * @tparam	Allocator the low-level allocator that the DynamicPool uses.
 *
 * @see 	TreeMap for details.
 */
template <class KeyType, class ValueType, class Allocator, unsigned int poolParam = 16>
using DynamicTreeMap = TreeMap<	KeyType,
								ValueType,
								mm::DynamicPool<typename ImmutableTreeMap<KeyType, ValueType>::Node, Allocator, poolParam>>;
}
#endif
