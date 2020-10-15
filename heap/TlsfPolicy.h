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

#ifndef TLSFPOLICY_H_
#define TLSFPOLICY_H_

#include "heap/Heap.h"
#include "data/DoubleList.h"

class TlsfPolicyInternalsTest;

namespace pet {

/**
 * Two-level segregated fit allocator policy.
 *
 * This is a dynamic memory management policy for Heap host class. Together they
 * form a real-time heap, capable of allocating and reclaiming a full or partial
 * block with constant time complexity considering the whole internal state of
 * the heap. That is, the upper limit of the time it takes to carry out the
 * operation is a fixed constant, no matter how many or how small free/used
 * blocks there are in the free store.
 *
 * It achieves this predictability by never iterating over the stored elements.
 * Instead it employs a hash-table like structure where buckets are assigned
 * according to the block sizes. This assignment is done in a logarithmic-then-linear
 * fashion, so smaller sized blocks are distinguished more accurately. Upon an
 * allocation request such a bucket is selected that is known to contain blocks
 * that have a size greater or equal to requested one. It is always the first
 * block that is removed from that list.
 *
 * @dot digraph G { rankdir=LR; h [shape=record, label =
 * 		"Size|0|<l1>1|...|<l2>30|31|<l3>32, 33|34, 35|...|<l4>63, 64|65, ..., 68|<l5>69, ..., 72|..."]
 * 		edge[dir=both]
 * 		y[label="1 byte\nblock"]
 * 		z[label="1 byte\nblock"]
 * 		h:l1 -> "1 byte\nblock" -> y -> z
 * 		h:l2 -> "30 byte\nblock"
 * 		h:l3 -> "33 byte\nblock" -> "32 byte\nblock"
 * 		x[label="63 byte\nblock"]
 * 		h:l4 -> "63 byte\nblock" -> "64 byte\nblock" ->x
 * 		h:l5 -> "70 byte\nblock" -> "72 byte\nblock"
 * } @enddot
 *
 * It can be seen that for the first few (32 for this implementation) slots
 * this scheme is optimally accurate, for bigger sizes the precision
 * logarithmically decreases. As the blocks that are too small to be split
 * are served optimally there is no waste introduced by this technique.
 * Larger sized blocks can be split so the leftover space can be reused.
 * However the fragmentation may be increased slightly by the imprecise matches,
 * but in practice the effect is negligible.
 *
 * Adding an item to the store is done by first determining the bucket for the
 * size then adding the new element to the beginning of the list.
 * During block merges the Heap host may request the deletion of an arbitrary
 * entry from this data structure, so to be able to that in constant time the
 * lists of the buckets need to be linked in both directions.
 *
 * @see http://www.gii.upv.es/tlsf/files/ecrts04_tlsf.pdf for the original papaer.
 */
template <class SizeType, unsigned int alignmentBits>
class TlsfPolicy: private HeapBase<SizeType, alignmentBits> {
		friend ::TlsfPolicyInternalsTest;

        /** The Block of HeapBase is used here as the common representation with the Heap host */
        using Block = typename HeapBase<SizeType, alignmentBits>::Block;

        class FreeBlock;

        /**
         * List of free blocks.
         *
         * This class represents a bucket, that contains
         * the blocks of the associated size range.
         */
        class FreeList: protected pet::DoubleList<FreeBlock> {
        public:
        	/**
        	 * Add front.
        	 *
        	 * Adds an element to the front of the list.
        	 *
        	 * @param	block The free block to be added.
        	 */
        	inline void add(FreeBlock* block);

        	/**
        	 * Remove arbitrary element.
        	 *
        	 * Removes the specified element from the list.
        	 *
        	 * @param	block The free block to be removed.
        	 */
        	inline void remove(FreeBlock* block);

        	/**
        	 * Remove first.
        	 *
        	 * Removes the first element of the list.
        	 *
        	 * @return	The element that used to be the first.
        	 */
        	inline FreeBlock* removeHead();

        	/**
        	 * Is empty?
        	 *
        	 * Tells whether the list has any elements in it.
        	 *
        	 * @return	True if there is no stored element, false otherwise.
        	 */
        	inline bool isEmpty();
        };

        /**
         * Free block header.
         *
         * This lives in the payload section of a freed up block.
         */
        class FreeBlock {
        	friend pet::DoubleList<FreeBlock>;
			FreeBlock *next, *prev;
        };

        /**
         * Free block database.
         *
         * Contains the lists for each block size category
         * and also a two layered bitmap cache.
         */
    	class Index {
           	unsigned short flMap = 0;
           	unsigned short slMap[16] = {0,};
           	FreeList blocks[256];

       		static inline unsigned int getLogMap(unsigned int size);
    	public:
       		/**
       		 * Hash value.
       		 *
       		 * Bucket identifier, as structured hash value, contains
       		 * the logarithmic and the linear part separately.
       		 */
           	class Entry {
           		friend ::TlsfPolicyInternalsTest;
           		friend Index;
           		unsigned short fl, sl;
           	public:
           		/**
           		 * Is this a valid entry?
           		 *
           		 * Check if this object represents a valid entry in the table of free lists.
           		 *
           		 * @return	True if it can be used as an argument to getListFor (ie it is valid), false otherwise.
           		 */
           		bool isValid();

           		/**
           		 * Different slot?
           		 *
           		 * Check if the other object signifies a different entry in the table of free lists.
           		 *
           		 * @return	True if it belongs to another list.
           		 */
           		bool operator !=(const Entry& other) const;
           	};

           	/**
           	 * Record block presence.
           	 *
           	 * Sets the bits in the lookup cache associated with the size category.
           	 *
           	 * @param sizeCategory The hash value identifying the bucket in question.
           	 */
        	inline void setBits(const Entry& sizeCategory);

           	/**
           	 * Record block absence.
           	 *
           	 * Clears the bits in the lookup cache associated with the size category.
           	 *
           	 * @param sizeCategory The hash value identifying the bucket in question.
           	 */
        	inline void resetBits(const Entry& sizeCategory);

           	/**
           	 * Get free list for size.
           	 *
           	 * Obtains the list containing the blocks for the given size.
           	 *
           	 * @param 	sizeCategory The hash value identifying the bucket in question.
           	 * @return	The free list for the bucket of the specified size.
           	 */
       		inline FreeList& getListFor(const Entry &sizeCategory);

       		/**
       		 * Direct hash mapping.
       		 *
       		 * Hashes the specified size resulting in the bucket identifier for that size.
       		 *
       		 * @param	size The size for wich the hash is to be calculated.
       		 * @return	The generated hash.
       		 */
       		static inline Entry getInsertionEntry(unsigned int size);

       		/**
       		 * Get a greater or equal bucket.
       		 *
       		 * Finds the right bucket (by direct mapping *and cache lookup*) out of
       		 * which a block with the specified size can be allocated.
       		 *
       		 * @return	The bucket identifier for the list or an invalid one if none found.
       		 */
       		inline Entry getGreaterEqualEntry(unsigned int size);
    	};

    	/** Free block database instance */
    	Index index;

protected:
        /**
         * Minimal required block size.
         *
         * No blocks with payload area smaller than this can be
         * handled, because when freed the payload area needs to
         * host the FreeBlock header.
         */
        static constexpr unsigned int freeHeaderSize = sizeof(FreeBlock);

    	/**
    	 * Add a block to the free store.
    	 *
    	 * Adds a free block into the right place in the accelerated allocation database.
    	 *
    	 * @note This method is part of the implicit Policy interface of the Heap host.
    	 *
    	 * @param 	block The block to be added.
    	 */
    	inline void add(Block block);
        inline void init(Block block) { add(block); }

    	/**
    	 * Remove a block from the free store.
    	 *
    	 * Removes a free block and updates all the necessary registries of the allocation database.
    	 *
    	 * @note This method is part of the implicit Policy interface of the Heap host.
    	 *
    	 * @param 	block The block to be remove.
    	 */
    	inline void remove(Block block);

    	/**
    	 * Block size change update.
    	 *
    	 * Updates free block size in the allocation database.
    	 *
    	 * @note This method is part of the implicit Policy interface of the Heap host.
    	 *
    	 * @param 	block The block whose size is to be updated.
    	 * @param	oldSize The size prior to the update.
    	 */
    	inline void update(unsigned int oldSize, Block block);

    	/**
    	 * Allocate.
    	 *
    	 * Finds a suitable block and removes it, updating all the necessary internal state variables.
    	 *
    	 * @note This method is part of the implicit Policy interface of the Heap host.
    	 *
    	 * @param 	size The size of the block to be found.
    	 * @return	The block or NULL if none found.
    	 */
    	inline Block findAndRemove(unsigned int size);
};

/**
 * Heap with the realtime TlsfPolicy.
 *
 * Facade to provide nicer usage, with automatically matching redundant parameters.
 */
template<class SizeType, unsigned int alignmentBits, bool cheksummingOn = false>
using TlsfHeap = Heap<TlsfPolicy<SizeType, alignmentBits>, SizeType, alignmentBits, cheksummingOn>;

////////////////////////////////////////////////////////////////////////////////////////

template <class SizeType, unsigned int alignmentBits>
inline void TlsfPolicy<SizeType, alignmentBits>::add(Block b)
{
	const unsigned int size = b.getSize();
	FreeBlock *block = (FreeBlock *)b.ptr;

	typename Index::Entry insEntry = Index::getInsertionEntry(size);
	index.getListFor(insEntry).add(block);
	index.setBits(insEntry);
}

template <class SizeType, unsigned int alignmentBits>
inline void TlsfPolicy<SizeType, alignmentBits>::remove(Block b)
{
	const unsigned int size = b.getSize();
	FreeBlock *block = (FreeBlock *)b.ptr;

	typename Index::Entry remEntry = Index::getInsertionEntry(size);
	FreeList &list = index.getListFor(remEntry);
	list.remove(block);

	if(list.isEmpty())
		index.resetBits(remEntry);
}

template <class SizeType, unsigned int alignmentBits>
inline typename TlsfPolicy<SizeType, alignmentBits>::Block
TlsfPolicy<SizeType, alignmentBits>::findAndRemove(unsigned int size) {
	typename Index::Entry findEntry = index.getGreaterEqualEntry(size);

	if(findEntry.isValid())
		return 0;

	FreeList &list = index.getListFor(findEntry);

	Block ret(list.removeHead());

	if(list.isEmpty())
		index.resetBits(findEntry);

	return ret;
}

template <class SizeType, unsigned int alignmentBits>
inline void TlsfPolicy<SizeType, alignmentBits>::update(unsigned int oldSize, Block b)
{
	const unsigned int size = b.getSize();
	FreeBlock *block = (FreeBlock *)b.ptr;

	typename Index::Entry oldEntry = Index::getInsertionEntry(oldSize);
	typename Index::Entry insEntry = Index::getInsertionEntry(size);

	if(oldEntry != insEntry) {
		FreeList &list = index.getListFor(oldEntry);
		list.remove(block);

		if(list.isEmpty())
			index.resetBits(oldEntry);

		index.getListFor(insEntry).add(block);
		index.setBits(insEntry);
	}
}

template <class SizeType, unsigned int alignmentBits>
inline void TlsfPolicy<SizeType, alignmentBits>::FreeList::add(FreeBlock *block)
{
	this->fastAddFront(block);
}

template <class SizeType, unsigned int alignmentBits>
inline typename TlsfPolicy<SizeType, alignmentBits>::FreeBlock *
TlsfPolicy<SizeType, alignmentBits>::FreeList::removeHead()
{
	FreeBlock* ret = this->iterator().current();
	this->fastRemove(ret);
	return ret;
}

template <class SizeType, unsigned int alignmentBits>
inline bool TlsfPolicy<SizeType, alignmentBits>::FreeList::isEmpty()
{
	return !this->iterator().current();
}

template <class SizeType, unsigned int alignmentBits>
inline void TlsfPolicy<SizeType, alignmentBits>::FreeList::remove(FreeBlock *block)
{
	this->fastRemove(block);
}

template <class SizeType, unsigned int alignmentBits>
inline void TlsfPolicy<SizeType, alignmentBits>::Index::setBits(const Entry& index){
	flMap |= 1 << index.fl;
	slMap[index.fl] |= 1 << index.sl;
}

template <class SizeType, unsigned int alignmentBits>
inline void TlsfPolicy<SizeType, alignmentBits>::Index::resetBits(const Entry& index){
	slMap[index.fl] &= ~(1 << index.sl);

	if(!slMap[index.fl])
		flMap &= ~(1 << index.fl);
}

template <class SizeType, unsigned int alignmentBits>
inline bool TlsfPolicy<SizeType, alignmentBits>::Index::Entry::isValid(){
	return fl == ((unsigned short)-1u);
}

template <class SizeType, unsigned int alignmentBits>
bool TlsfPolicy<SizeType, alignmentBits>::Index::Entry::operator !=(const Entry& other) const {
	return (fl != other.fl) || (sl != other.sl);
}

template <class SizeType, unsigned int alignmentBits>
inline typename TlsfPolicy<SizeType, alignmentBits>::FreeList&
TlsfPolicy<SizeType, alignmentBits>::Index::getListFor(const Entry &index){
	AllHeapsTrace::assert(index.sl < 16);
	AllHeapsTrace::assert(index.fl < 16);
	return blocks[index.sl + index.fl*16];
}

template <class SizeType, unsigned int alignmentBits>
inline unsigned int TlsfPolicy<SizeType, alignmentBits>::Index::getLogMap(unsigned int size) {
	if(!size)
		return 0;

	unsigned int lz ;
	if((lz = __builtin_clz(size)) > 28)
		return 0;
	else
		return 28-lz;
}


template <class SizeType, unsigned int alignmentBits>
inline typename TlsfPolicy<SizeType, alignmentBits>::Index::Entry
TlsfPolicy<SizeType, alignmentBits>::Index::getInsertionEntry(unsigned int size)
{
	Entry ret;

	ret.fl = getLogMap(size);

	if(ret.fl)
		size >>= ret.fl - 1;

	ret.sl = size & 0xf;

	return ret;
}

template <class SizeType, unsigned int alignmentBits>
inline typename TlsfPolicy<SizeType, alignmentBits>::Index::Entry
TlsfPolicy<SizeType, alignmentBits>::Index::getGreaterEqualEntry(unsigned int size)
{
	int logMap = getLogMap(size);

	if(logMap > 1)  // round up
		size += (1 << (logMap - 1)) - 1;

	Entry entry = getInsertionEntry(size);

	unsigned int newMask = slMap[entry.fl] & ~((1 << (entry.sl))-1);
	if(newMask){
		entry.sl = __builtin_ctz(newMask);
	}else {
		newMask = flMap & ~((1 << (entry.fl+1))-1);
		if(!newMask)
			entry.sl = entry.fl = ((unsigned short)-1u);
		else {
			entry.fl = __builtin_ctz(newMask);
			entry.sl = __builtin_ctz(slMap[entry.fl]);
		}
	}

	return entry;
}

}

#endif /* TLSFPOLICY_H_ */
