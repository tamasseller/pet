/*
 * AvlTreePolicy.h
 *
 *  Created on: 2016.11.17.
 *      Author: tamas.seller
 */

#ifndef AVLTREEPOLICY_H_
#define AVLTREEPOLICY_H_

#include "Heap.h"
#include "AvlTree.h"

class AvlPolicyInternalsTest;

namespace mm {

/**
 * Balanced binary search tree based best-fit allocator policy.
 *
 * This is a dynamic memory management policy for Heap host class. Together they
 * form a non-realtime heap, capable of allocating and reclaiming a full or partial
 * block with logarithmic time complexity by the number of free blocks.
 */

template <class SizeType, unsigned int alignmentBits>
class AvlTreePolicy: protected HeapBase<SizeType, alignmentBits>, protected container::AvlTree
{
	using typename HeapBase<SizeType, alignmentBits>::Block;

	static int sizeCompare(BinaryTree::Node* block, const unsigned int &size) {
		return (int)(Block(block).getSize() - size);
	}

protected:
	/** @copydoc mm::TlsfPolicy::freeHeaderSize */
	static constexpr unsigned int freeHeaderSize = sizeof(AvlTree::Node);

	/** @copydoc mm::TlsfPolicy::add */
	inline void add(Block block);

	/** @copydoc mm::TlsfPolicy::remove */
	inline void remove(Block block);

	/** @copydoc mm::TlsfPolicy::findAndRemove */
	inline Block findAndRemove(unsigned int size);

	/** @copydoc mm::TlsfPolicy::update */
	inline void update(unsigned int oldSize, Block block);
};

/**
 * Heap with AvlPolicy.
 *
 * Facade to provide nicer usage, with automatically matching redundant parameters.
 */
template<class SizeType, unsigned int alignmentBits, bool cheksummingOn = false>
using AvlHeap = Heap<AvlTreePolicy<SizeType, alignmentBits>, SizeType, alignmentBits, cheksummingOn>;

////////////////////////////////////////////////////////////////////////////////////////

template <class SizeType, unsigned int alignmentBits>
inline void AvlTreePolicy<SizeType, alignmentBits>::add(Block block)
{
	BinaryTree::Position pos = BinaryTree::seek<unsigned int, &AvlTreePolicy::sizeCompare>(block.getSize());

	while(pos.getNode()) {
		pos.parent = pos.getNode();
		pos.origin = &pos.getNode()->small;
	}

	insert(pos, new(block.ptr) AvlTree::Node);
}

template <class SizeType, unsigned int alignmentBits>
inline void AvlTreePolicy<SizeType, alignmentBits>::remove(Block block)
{
	AvlTree::remove((AvlTree::Node*) block.ptr);
}

template <class SizeType, unsigned int alignmentBits>
inline typename AvlTreePolicy<SizeType, alignmentBits>::Block
AvlTreePolicy<SizeType, alignmentBits>::findAndRemove(unsigned int size)
{
	BinaryTree::Position pos = BinaryTree::seek<unsigned int, &AvlTreePolicy::sizeCompare>(size);

	if(Node *node = (Node*)pos.getNode()) {
		remove(node);
		return node;
	}

	for(BinaryTree::Iterator it = iterator(pos.parent); it.current(); it.step()) {
		if(Block(it.current()).getSize() >= size) {
			AvlTree::remove((AvlTree::Node*) it.current());
			return it.current();
		}
	}

	return 0;
}

template <class SizeType, unsigned int alignmentBits>
inline void AvlTreePolicy<SizeType, alignmentBits>::
update(unsigned int oldSize, Block block)
{
	remove(block);
	add(block);
}

}

#endif /* AVLTREEPOLICY_H_ */
