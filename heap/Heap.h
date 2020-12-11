/*******************************************************************************
 *
 * Copyright (c) 2016, 2017 Tamás Seller. All rights reserved.
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

#ifndef HEAP_H_
#define HEAP_H_

#include "ubiquitous/Trace.h"
#include "ubiquitous/Error.h"

class HeapInternalsTest;

namespace pet {

/**
 * Trace tag.
 *
 * Use this class as an identifier to enable tracing of heap events.
 */
class AllHeapsTrace: public pet::Trace<AllHeapsTrace> {};

/**
 * Common base for the heap host and the policies.
 *
 * Provides the constants and mappings that are essential for
 * the internal work of both parties. Also provides the Block
 * type that is the basis of the communication between the two.
 *
 * @tparam	SizeType The type to be used by the heap the express
 * 			the size of the free and used blocks in an internal
 * 			representation (ie. not necessarily bytes).
 *
 * @tparam	alignmentBits The number of **bits** of alignment. That
 * 			is the low bits that an address needs to have zeroed
 * 			out to satisfy the alignment criterion.
 */
template<class SizeType, unsigned int alignmentBits>
class HeapBase {
protected:
	/**
	 * SizeType MSB mask.
	 *
	 * A mask that has only the most significant bit set for the type used to
	 * represent block sizes internally. Irresponsibly assumes that a byte is 8 bits.
	 */
    static constexpr unsigned int sizeMsb = 1 << (sizeof(SizeType) * 8 - 1);

    /**
     * Heap block handle.
     *
     * Enables access to a (free or used) heap block.
     *
     * @see	Heap for the details.
     */
    struct Block {
    		/**
    	     * The offset of the next field from the start of the payload.
    	     * @see	Heap for the details.
    	     */
            static constexpr int nextFieldIdx = -1;

    		/**
    	     * Pointer to the payload.
    	     * @see	Heap for the details.
    	     */
            SizeType *ptr;

    		/**
    	     * Create from payload pointer.
    	     * @see	Heap for the details.
    	     */
            inline Block(void* address);

			/**
			 * Get the size of the payload of this block.
			 * @see	Heap for the details.
			 */
            inline unsigned int getSize() const;
    };
};

/**
 * Inheritance helper.
 *
 * Base class disambiguator for the Heap host. Needed because
 * both the host and the policies need to inherit from the HeapBase.
 */
template<class SizeType, unsigned int alignmentBits, bool useChecksum>
class HeapHostBase: protected HeapBase<SizeType, alignmentBits>
{
protected:
	using HeapBase<SizeType, alignmentBits>::sizeMsb;

	/**
	 * Alignment **bytes**
	 *
	 * Alignment of the heap in the regular sense.
	 */
    static constexpr unsigned int alignmentBytes = 1 << (alignmentBits);

    /**
     * Round up to alignment.
     *
     * Used internally to enforce alignment of various addresses and sizes.
     *
     * @param	x The address/size to be aligned.
     * @return	The lowest value that is:
     * 				1. Not smaller than _x_ and
     * 				2. Satisfies the alignment criterion.
     */
    static constexpr unsigned int align(unsigned int x);

    /**
     * Bytes to internal size.
     *
     * Converts a size from bytes to the internal representation.
     *
     * @param 	x The size (in bytes) to be converted to internal units.
     * @return	The result of the conversion.
     */
    static constexpr unsigned int encode(unsigned int x);

    /**
     * Internal size to bytes.
     *
     * Converts a size from the internal representation to bytes.
     *
     * @param 	x The size (in internal units) to be converted to bytes.
     * @return	The result of the conversion.
     */
    static constexpr unsigned int decode(unsigned int x);

    /**
     * Heap block handle.
     *
     * Enables access to a (free or used) heap block, with added
     * functionality available only for the HeapHost.
     *
     * @see	Heap for the details.
     */
    struct Block: HeapBase<SizeType, alignmentBits>::Block {
    		using HeapBase<SizeType, alignmentBits>::Block::getSize;
    		using HeapBase<SizeType, alignmentBits>::Block::ptr;
    		using HeapBase<SizeType, alignmentBits>::Block::nextFieldIdx;
    		/**
    	     * Static block header size.
    	     * @see	Heap for the details.
    	     */
            static constexpr unsigned int headerSize = (useChecksum ? 3 : 2) * sizeof(SizeType);

        	/**
             * The offset of the previous field from the start of the payload.
             * @see	Heap for the details.
             */
            static constexpr int prevFieldIdx = -2;

        	/**
             * The offset of the field used for storing checksum of
             * the other fields, if checksumming is enabled.
             * @see	Heap for the details.
             */
            static constexpr int checksumFieldIdx = -3;

    		/**
    	     * Fall-through create from payload pointer.
    	     * @see	Heap for the details.
    	     */
            inline Block(void* address);

            /**
			 * Fall-through copy constructor
			 * @see	Heap for the details.
			 */
            inline Block(const typename HeapBase<SizeType, alignmentBits>::Block &other);

    		/**
    	     * Has (physically) previous block? (Or is it the first one?)
    	     * @see	Heap for the details.
    	     */
            inline bool hasPrev() const;

    		/**
    	     * Get (physically) previous block.
    	     * @see	Heap for the details.
    	     */
            inline Block getPrev() const;

    		/**
    	     * Set (physically) previous block.
    	     * @see	Heap for the details.
    	     */
            inline void setPrev(const Block &prev) const;

    		/**
    	     * Is this a free block?
    	     * @see	Heap for the details.
    	     */
            inline bool isFree() const;

    		/**
    	     * Set the block free or used.
    	     * @see	Heap for the details.
    	     */
            inline void setFree(bool free) const;

    		/**
    	     * Has next (physical) block? (Or is it the last one?)
    	     * @see	Heap for the details.
    	     */
            inline bool hasNext(Block end) const;

            /**
			 * Set (physically) next block.
			 * @note Affects the size.
			 * @see	Heap for the details.
			 */
			inline void setNext(const Block &next) const;

			/**
			 * Get (physically) next block.
			 * @see	Heap for the details.
			 */
			inline Block getNext() const;

			/**
			 * Set the size of the payload of this block.
			 * @note Affects the next block property.
			 * @see	Heap for the details.
			 */
            inline void setSize(SizeType size) const;

			/**
			 * Helper that splits this block in two, returns the leftover.
			 * @see	Heap for the details.
			 */
            inline Block split(unsigned int size);

			/**
			 * Helper that merges the other block into this.
			 *
			 * After the operation this block has a length so that,
			 * its end is the same as the end of the other.
			 * @see	Heap for the details.
			 */
            inline void merge(Block other);

            /**
             * Update the prev size field of the next block, if present.
             * @see	Heap for the details.
             */
            inline void updateNext(Block end);

            /**
             * Helper to update the checksum field.
             * @see	Heap for the details.
             */
            inline void updateChecksum();

            /**
             * Helper to check the validity of the checksum field.
             * @see	Heap for the details.
             */
            inline bool checkChecksum();
    };
};

/**
 * Policy based heap.
 *
 * Flexible heap implementation, that:
 *
 *	- can use an arbitrary policy for storing and selecting free blocks on allocation,
 *	- has configurable block storage overhead vs maximal allocatable size tradeoff.
 *	- has configurable alignment.
 *
 * In general a dynamic memory heap (not to be mistaken for the binary heap or min/max heap
 * data structure) takes care of the dynamic memory segment allocation from a continuous
 * memory area. Commonly it is implemented by introducing metadata inline with business
 * content. This implementation does so as well.
 *
 * The responsibilities of the heap include:
 *
 *	- keeping track of the currently unused memory,
 *	- serving allocation request so that any block of memory handed over to the
 *	  application is never assigned again, until it is released by the application,
 *	- integrating back released memory block into the free block storage.
 *
 * This task obviously requires the storage of additional metadata along with the actual
 * user content. The most scalable and common implementation scatters metadata throughout
 * the whole heap space in the form of headers before each block of memory.
 *
 * There are two different sets of information that needs to be stored along each block,
 * depending on whether it is free or used. However the used blocks header is a subset of
 * the free one's. The common metadata to be describes the location of the immediate neighbors
 * of the given block.
 *
 * @dot digraph G {
 * h[shape = plaintext, label = <<table border="0" cellborder="1" cellspacing="1"><tr>
 * 		<td port="n1">next</td><td port="p1" SIDES="TB">prev</td><td bgcolor="#90ee90">Free space</td>
 * 		<td port="n2">next</td><td port="p2" SIDES="TB">prev</td><td bgcolor="#ffc0cb">User data</td>
 * 		<td port="n3">next</td><td port="p3" SIDES="TB">prev</td><td bgcolor="#90ee90">Free space</td>
 * 		<td port="n4">next</td><td port="p4" SIDES="TB">prev</td><td bgcolor="#ffc0cb">User data</td>
 * 		<td port="n5">next</td><td port="p5" SIDES="TB">prev</td><td bgcolor="#ffc0cb">User data</td>
 * </tr></table>>]
 * h:n1:s -> h:n2:s;
 * h:n2:s -> h:n3:s;
 * h:n3:s -> h:n4:s;
 * h:n4:s -> h:n5:s;
 * h:p2:n -> h:n1:n;
 * h:p3:n -> h:n2:n;
 * h:p4:n -> h:n3:n;
 * h:p5:n -> h:n4:n;
 * } @enddot
 *
 * As these blocks cover the heapspace continuously (ie. without gaps) it is sufficient to
 * know the size of the block to get to the next one, also conversely knowing the size of
 * the previous block is. This implementation achieves the storage of the immediate neighbors'
 * location by storing the sizes instead of pointers. This is beneficial because this way the
 * sizes of the header fields depend on the maximal allowed size of a block and not on the
 * size of an arbitrary memory address. This comes in handy especially for 32bit microcontrollers,
 * where it is typical to have much less memory than the 4GB address space, so only a small
 * portion of the available space is utilized, nevertheless using pointers would require the
 * storage of the full four bytes of the address. Also the sizes are not expressed as bytes,
 * but instead shifted down by the alignment bits, to save those because otherwise they would
 * be wasted as fixed zeroes. This means that as the alignment get higher so does the
 * maximal block size.
 *
 * The heap needs to match the size of the segments to the needs of the application. Every
 * requested block has to be at least as big as the specified size, but the actually
 * allocated space can exceed the amount that was asked for. However the additional space
 * is basically wasted, because the application can only take the specified length granted,
 * so it can not use the surplus. In order for the block sizes to be as close as possible
 * to the actual needs of the application, the heap needs to slice and rejoin free blocks.
 *
 * Upon allocation the free store is queried for a block that is big enough to provide the
 * required amount of memory. If the found block is bigger (by enough) than the required
 * size, it is split and the leftover is inserted back into the free store.
 *
 * If during any operation a free block is created next to another free block they are
 * instantly joined. Obviously, used blocks can lay next to each other, but if any block
 * gets freed up, it is always checked for the possibility of merging. So this implementation
 * guarantees that there can be no pairs of adjacent free blocks, thus always counting
 * continuous free blocks as such.
 *
 * @note 	This class is directly responsible for only the tasks of splitting and joining
 * 			the blocks. The free store, including the search algorithm for adequate free
 * 			blocks is implemented by the pluggable policy. All of the operations carried out
 * 			in this class are of linear time complexity, thus preserving the possible real-time
 * 			characteristics of the supplied policy.
 *
 * @warning This heap implementation is not guarded for **concurrent accesses** in itself. If it
 *			is used in multi-threaded/interrupt context **locking has to be added externally**.
 *
 * @see		TlsfPolicy.h
 *
 * @tparam	Policy the free block storage/selection policy to be incorporated.
 * @tparam	SizeType the (unsigned integral) type to be used for the internal
 * 			representation of sizes, this parameter tunes the storage overhead
 * 			against the maximal size of blocks.
 * @tparam	alignmentBits The number of **bits** of alignment. That
 * 			is the low bits that an address needs to have zeroed
 * 			out to satisfy the alignment criterion.
 * @tparam	useChecksum If true the static part of the heap blocks are checksummed
 * 			and before every access to a block its checksum is validated. Reports
 * 			error through the Trace<AllHeapsTrace>. If false the checks are optimized
 * 			away.
 */

template<class Policy, class SizeType, unsigned int alignmentBits, bool useChecksum = false>
class Heap:	public Policy,
			protected HeapHostBase<SizeType, alignmentBits, useChecksum>,
			protected pet::Trace<AllHeapsTrace>

{
	friend HeapInternalsTest;
	typedef HeapHostBase<SizeType, alignmentBits, useChecksum> Base;
    using Block = typename Base::Block;
    using Base::sizeMsb;
    using Base::alignmentBytes;
    using Base::align;
    using Base::encode;
    using Base::decode;

	Block end = 0; //!< Tricky one, points over the end, never dereferenced (ie. no setter or getter methods called on it).

    static constexpr unsigned int maxBlockSize = decode(~sizeMsb);

    bool canSplit(Block, unsigned int size);

	public:
    	/** @cond */
    	inline bool dump(void *start) {
			bool ok = true;
			Block block = (char*)start + Block::headerSize;
        	while(1) {
        		if(block.isFree())
        			info << "(";

        		info << decode(block.getSize());

        		if(block.isFree())
        			info << ")";

        		info << " ";

        		if(!block.hasNext(end))
        			break;
        		else
        			ok = ok && (block.getNext().getPrev().ptr == block.ptr);


        		block = block.getNext();
        	}

        	info << "\n";
        	return ok;
		}
		/** @endcond */

    	/**
    	 * Initialize the heap.
    	 *
    	 * Initializes the internal data structures of the heap on the given block of memory.
    	 *
    	 * @param	space The pointer to the start of the heap space.
    	 * @param	size The size of the heap space.
    	 */
		inline void init(void* space, unsigned int size);
		inline Heap(void* space, unsigned int size);

    	/**
    	 * Create an uninitialized heap, must be set up before use with the **init** method.
    	 */
		inline Heap() = default;

    	/**
    	 * Allocate memory.
    	 *
    	 * Allocates the specified amount of memory.
    	 * The allocated region is aligned to the specified bits (given as alignmentBits template argument).
    	 *
    	 * @param	size The amount (in bytes) to be allocated.
    	 * @return	A pointer to the start of the allocated region or NULL on failure.
    	 */
		inline pet::FailPointer<void> alloc(unsigned int size);

    	/**
    	 * Release used memory.
    	 *
    	 * Places back the given block into the free stores.
    	 *
    	 * @param	ptr	The pointer to the block that is to be freed.
    	 * 			It has to be a pointer returned by the method alloc, without any offset!
    	 */
		inline void free(void* ptr);

    	/**
    	 * Shrinks block.
    	 *
    	 * Splits the given block and frees up the end of it.
    	 * If a higher value is requested than the size of the block the size is not enlarged.
    	 * This method can **only shrink**, thus the name. It is not the same as stdlibc's
    	 * _realloc_, as it never moves the data.
    	 *
    	 * @param	ptr The pointer to the block that is to be split.
    	 * 			It has to be a pointer returned by the method alloc, without any offset!
    	 * @param 	shrunkSize The requested new size of the block.
    	 * @return	The new size of the block. Note that depending on the actual arguments
    	 * 			and the configuration of the Heap it can be more or less than to the
    	 * 			_shrunkSize_ parameter, however it is never smaller than the smaller
    	 * 			of the current size and the requested size.
    	 */
		inline unsigned int shrink(void* ptr, unsigned int shrunkSize);
};

////////////////////////////////////////////////////////////////////////////////////////////////

template<class Policy, class SizeType, unsigned int alignmentBits, bool useChecksum>
inline Heap<Policy, SizeType, alignmentBits, useChecksum>::Heap(void* start, unsigned int size)  {
	init(start, size);
}

template<class Policy, class SizeType, unsigned int alignmentBits, bool useChecksum>
inline void Heap<Policy, SizeType, alignmentBits, useChecksum>::init(void* start, unsigned int size){
	end.ptr = (SizeType*)((char*)start + size + Block::headerSize);
	unsigned int initialIndent = align(Block::headerSize);
	Block first((char*)start + initialIndent);
	first.setFree(true);
	first.setNext(end);
	first.setPrev(first);
	first.updateChecksum();
	Policy::init(first);
	info << "Heap created at: " << start << " - " << (void*)(((char*)start) + size) << "\n";
}


template<class Policy, class SizeType, unsigned int alignmentBits, bool useChecksum>
inline pet::FailPointer<void> Heap<Policy, SizeType, alignmentBits, useChecksum>::alloc(unsigned int sizeParam)
{
    unsigned int size = sizeParam;

	if(size > maxBlockSize) {
		warn << "Heap::alloc(): Too large block requested !\n";
        return 0;
    }

    if(size < Policy::freeHeaderSize)
        size = Policy::freeHeaderSize;

    size = encode(align(size));

    Block ret(Policy::findAndRemove(size));

    if(ret.ptr == 0) {
    	warn << "Heap::alloc(): Can not allocate " << decode(size) << " bytes\n";
    	return 0;
    }

	assertThat(ret.checkChecksum(), "Heap corruption");

	if(canSplit(ret, size)) {
	    Block leftover(ret.split(size));
       	leftover.updateNext(end);
       	leftover.updateChecksum();
    	Policy::add(leftover);
	}

	info << "Heap::alloc(" << sizeParam << "): " << ret.ptr << "\n";

	ret.setFree(false);
	ret.updateChecksum();

    assertThat(ret.getSize() >= size, "Heap corruption");
    return ret.ptr;
}

template<class Policy, class SizeType, unsigned int alignmentBits, bool useChecksum>
inline void Heap<Policy, SizeType, alignmentBits, useChecksum>::free(void* r) {
	assertThat(r, "Heap::free(): Invalid argument\n");

    Block block(r);
	assertThat(block.checkChecksum(), "Heap corruption");
	assertThat(!block.isFree(), "Heap corruption (probably double free)");

	info << "Heap::free(" <<  r << "): " << decode(block.getSize()) << " freed\n";

    bool prevFree = block.hasPrev() && block.getPrev().isFree();
    bool nextFree = block.hasNext(end) && block.getNext().isFree();

    if(prevFree) {
        Block prev(block.getPrev());
        assertThat(prev.checkChecksum(), "Heap corruption");
        unsigned int oldSize = prev.getSize();

        if(nextFree) {
        	Block next(block.getNext());

        	assertThat(next.checkChecksum(), "Heap corruption");

            Policy::remove(next);

        	prev.merge(next);
        	prev.updateNext(end);
        } else {
        	if(block.hasNext(end))
        		assertThat(block.getNext().checkChecksum(), "Heap corruption");

        	prev.merge(block);
        	prev.updateNext(end);
        }

        Policy::update(oldSize, prev);
    } else {
        block.setFree(true);

        if(nextFree) {
        	Block next(block.getNext());
        	assertThat(next.checkChecksum(), "Heap corruption");
            Policy::remove(next);

        	block.merge(next);
        	block.updateNext(end);
        } else
        	block.updateChecksum();

        Policy::add(block);
    }
}

template<class Policy, class SizeType, unsigned int alignmentBits, bool useChecksum>
inline unsigned int Heap<Policy, SizeType, alignmentBits, useChecksum>::shrink(void* ptr, unsigned int shrunkSizeParam)
{
	assertThat(ptr, "Heap::shrink(): Null argument\n");
	info << "Heap::shrink(" <<  ptr << "): ";

    Block block(ptr);
    assertThat(block.checkChecksum(), "Heap corruption");

	unsigned int oldSize = decode(block.getSize());

    info << oldSize << " -> " << shrunkSizeParam;

    unsigned int shrunkSize = encode(align((shrunkSizeParam < Policy::freeHeaderSize) ?
    		Policy::freeHeaderSize :
    		shrunkSizeParam));

    if(block.getSize() < shrunkSize) {
    	info << " (enlargement requested)\n";
    	warn << "Heap::shrink(" <<  ptr << "): " << oldSize << " -> " << shrunkSizeParam;
    	warn << " (enlargement requested)\n";
    	return decode(block.getSize());
    }

	if(canSplit(block, shrunkSize)) {
	    Block leftover = block.split(shrunkSize);
	    block.updateChecksum();

	    if(leftover.hasNext(end)) {
        	Block next(leftover.getNext());
        	assertThat(next.checkChecksum(), "Heap corruption");

	    	if(next.isFree()) {
				Policy::remove(next);
				leftover.merge(next);
	    	} else {
		    	leftover.updateChecksum();
	    	}

    		leftover.updateNext(end);
	    } else
	    	leftover.updateChecksum();

    	Policy::add(leftover);
	}

    unsigned int newSize = decode(block.getSize());

    if(oldSize == newSize)
    	info << " (not changed)\n";
    else
    	info << " (shrunk to: " << newSize << ")\n";

    return newSize;
}

template<class Policy, class SizeType, unsigned int alignmentBits, bool useChecksum>
inline bool Heap<Policy, SizeType, alignmentBits, useChecksum>::canSplit(Block block, unsigned int size) {
    constexpr static unsigned int minEncodedFullBlockSize = encode(align(Block::headerSize + Policy::freeHeaderSize));
    unsigned int leftoverSize = block.getSize() - size;
    return leftoverSize >= minEncodedFullBlockSize;
}

template<class SizeType, unsigned int alignmentBits>
inline HeapBase<SizeType, alignmentBits>::Block::Block(void* address): ptr((SizeType*)address) {}

template<class SizeType, unsigned int alignmentBits>
inline unsigned int HeapBase<SizeType, alignmentBits>::Block::getSize() const  {
    return ptr[nextFieldIdx] & ~sizeMsb;
}

template<class SizeType, unsigned int alignmentBits, bool useChecksum>
inline HeapHostBase<SizeType, alignmentBits, useChecksum>::Block::
Block(const typename HeapBase<SizeType, alignmentBits>::Block &other):
 		HeapBase<SizeType, alignmentBits>::Block(other) {}

template<class SizeType, unsigned int alignmentBits, bool useChecksum>
inline HeapHostBase<SizeType, alignmentBits, useChecksum>::Block::Block(void* address):
	HeapBase<SizeType, alignmentBits>::Block((SizeType*)address) {}

template<class SizeType, unsigned int alignmentBits, bool useChecksum>
constexpr unsigned int HeapHostBase<SizeType, alignmentBits, useChecksum>::align(unsigned int x) {
        return (alignmentBytes > 1) ?
                (x + (alignmentBytes - 1)) & -alignmentBytes:
                x;
}

template<class SizeType, unsigned int alignmentBits, bool useChecksum>
inline void HeapHostBase<SizeType, alignmentBits, useChecksum>::Block::merge(Block other)
{
	setNext(other.getNext());
	other.setPrev(*this);

	other.updateChecksum();
	updateChecksum();
}

template<class SizeType, unsigned int alignmentBits, bool useChecksum>
inline void HeapHostBase<SizeType, alignmentBits, useChecksum>::Block::updateNext(Block end)
{
	if(hasNext(end)) {
		getNext().setPrev(*this);
		getNext().updateChecksum();
	}
}

template<class SizeType, unsigned int alignmentBits, bool useChecksum>
inline typename HeapHostBase<SizeType, alignmentBits, useChecksum>::Block
HeapHostBase<SizeType, alignmentBits, useChecksum>::Block::split(unsigned int size)
{
	Block oldNext = getNext();
	setSize(size);

	Block leftover(getNext());
	leftover.setPrev(*this);
	leftover.setNext(oldNext);
	leftover.setFree(true);
	return leftover;
}

template<class SizeType, unsigned int alignmentBits, bool useChecksum>
constexpr unsigned int HeapHostBase<SizeType, alignmentBits, useChecksum>::encode(unsigned int x) {
    return x >> alignmentBits;
}

template<class SizeType, unsigned int alignmentBits, bool useChecksum>
constexpr unsigned int HeapHostBase<SizeType, alignmentBits, useChecksum>::decode(unsigned int x) {
    return x << alignmentBits;
}

template<class SizeType, unsigned int alignmentBits, bool useChecksum>
inline void HeapHostBase<SizeType, alignmentBits, useChecksum>::Block::setFree(bool free) const {
    if(free)
        ptr[nextFieldIdx] |= sizeMsb;
    else
    	ptr[nextFieldIdx] &= ~sizeMsb;
}

template<class SizeType, unsigned int alignmentBits, bool useChecksum>
inline bool HeapHostBase<SizeType, alignmentBits, useChecksum>::Block::isFree() const {
    return ptr[nextFieldIdx] & sizeMsb;
}

template<class SizeType, unsigned int alignmentBits, bool useChecksum>
inline bool HeapHostBase<SizeType, alignmentBits, useChecksum>::Block::hasNext(Block end) const {
    return getNext().ptr != end.ptr;
}

template<class SizeType, unsigned int alignmentBits, bool useChecksum>
inline typename HeapHostBase<SizeType, alignmentBits, useChecksum>::Block
HeapHostBase<SizeType, alignmentBits, useChecksum>::Block::getNext() const {
    return Block((char*)ptr + decode(getSize()) + headerSize);
}

template<class SizeType, unsigned int alignmentBits, bool useChecksum>
inline void HeapHostBase<SizeType, alignmentBits, useChecksum>::Block::setNext(const Block &next) const  {
    setSize(encode(((char*)next.ptr - (char*)ptr) - headerSize));
}

template<class SizeType, unsigned int alignmentBits, bool useChecksum>
inline void HeapHostBase<SizeType, alignmentBits, useChecksum>::Block::setSize(SizeType size) const {
	ptr[nextFieldIdx] &= sizeMsb;
	ptr[nextFieldIdx] |= size & ~sizeMsb;
}

template<class SizeType, unsigned int alignmentBits, bool useChecksum>
inline bool HeapHostBase<SizeType, alignmentBits, useChecksum>::Block::hasPrev() const {
    return ptr[prevFieldIdx];
}

template<class SizeType, unsigned int alignmentBits, bool useChecksum>
inline typename HeapHostBase<SizeType, alignmentBits, useChecksum>::Block
HeapHostBase<SizeType, alignmentBits, useChecksum>::Block::getPrev() const {
    return Block((char*)ptr - ptr[prevFieldIdx]);
}

template<class SizeType, unsigned int alignmentBits, bool useChecksum>
inline void HeapHostBase<SizeType, alignmentBits, useChecksum>::Block::setPrev(const Block &prev) const {
	ptr[prevFieldIdx] = (char*)ptr - (char*)prev.ptr;
}

template<class SizeType, unsigned int alignmentBits, bool useChecksum>
inline void HeapHostBase<SizeType, alignmentBits, useChecksum>::Block::updateChecksum() {
	if(useChecksum)
		ptr[checksumFieldIdx] = ptr[prevFieldIdx] + ptr[nextFieldIdx];
}

template<class SizeType, unsigned int alignmentBits, bool useChecksum>
inline bool HeapHostBase<SizeType, alignmentBits, useChecksum>::Block::checkChecksum() {
	if(useChecksum)
		return ptr[checksumFieldIdx] == ptr[prevFieldIdx] + ptr[nextFieldIdx];
	else
	    return true;
}
}

#endif /* HEAP_H_ */
