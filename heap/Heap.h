/*******************************************************************************
 *
 * Copyright (c) 2016 - 2021 Tamás Seller. All rights reserved.
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

#include "heap/HeapBase.h"

#include "algorithm/Math.h"

#include "platform/Compiler.h"
#include "platform/Clz.h"

class HeapInternalsTest;

namespace pet {

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
            protected pet::Trace<AllHeapsTrace>
{
    static_assert(alignof(SizeType) <= (1 << alignmentBits));
    using Base = HeapBase<SizeType>;
    friend HeapInternalsTest;

    /**
     * Bytes to internal size.
     *
     * Converts a size from bytes to the internal representation, while rounding down.
     *
     * @param 	x The size (in bytes) to be converted to internal units.
     * @return	The result of the conversion.
     */
    static constexpr uintptr_t encodeRoundDown(uintptr_t x) {
        return x >> alignmentBits;
    }

    /**
     * Bytes to internal size.
     *
     * Converts a size from bytes to the internal representation, while rounding up.
     *
     * @param 	x The size (in bytes) to be converted to internal units.
     * @return	The result of the conversion.
     */
    static constexpr uintptr_t encodeRoundUp(uintptr_t x) {
        return (x + (uintptr_t(1) << alignmentBits) - 1) >> alignmentBits;
    }

    /**
     * Internal size to bytes.
     *
     * Converts a size from the internal representation to bytes.
     *
     * @param 	x The size (in internal units) to be converted to bytes.
     * @return	The result of the conversion.
     */
    static constexpr uintptr_t decode(uintptr_t x) {
        return x << alignmentBits;
    }

    /**
     * Heap block handle.
     *
     * Enables access to a (free or used) heap block, with added
     * functionality available only for the HeapHost.
     *
     * @see	Heap for the details.
     */
    struct Block: Base::Block
    {
        using Base::Block::Block;

        inline Block(const typename Base::Block& o): Base::Block(o) {}

        /// Static block header size.
        static constexpr uintptr_t headerSize = (useChecksum ? 3 : 2) * sizeof(SizeType);

        /// The offset of the previous field from the start of the payload.
        static constexpr int prevFieldIdx = -2;

        /// The offset of the field used for storing checksum of the other fields, if checksumming is enabled.
        static constexpr int checksumFieldIdx = -3;

        /// Has (physically) previous block? (Or is it the first one?)
        really_inline bool hasPrev() const {
            return this->ptr[prevFieldIdx];
        }

        /// Get (physically) previous block.
        really_inline Heap::Block getPrev() const {
            return {(char*)this->ptr - decode(this->ptr[prevFieldIdx])};
        }

        /// Set (physically) previous block.
        really_inline void setPrev(const Heap::Block &prev) const
        {
            const uintptr_t diff = (char*)this->ptr - (char*)prev.ptr;
            const auto encDiff = encodeRoundDown(diff);

            assertThat(decode(encDiff) == diff, "Heap corruption: bad block offset in setPrev");

            this->ptr[prevFieldIdx] = encDiff;
        }

        /// Is this an unused block?
        really_inline bool isFree() const {
            return this->ptr[Base::Block::nextFieldIdx] & Base::sizeMsb;
        }

        /// Set whether the block is free or used.
        really_inline void setFree(bool free) const
        {
            this->ptr[Base::Block::nextFieldIdx] = free
                ? (this->ptr[Base::Block::nextFieldIdx] | Base::sizeMsb)
                : (this->ptr[Base::Block::nextFieldIdx] & ~Base::sizeMsb);
        }

        /// Has next (physical) block? (Or is it the last one?)
        really_inline bool hasNext(const Heap::Block &end) const {
            return this->getNext().ptr != end.ptr;
        }

        /// Set (physically) next block. NOTE: It affects the size!
        really_inline void setNext(const Heap::Block &next) const
        {
            const uintptr_t diff = (char*)next.ptr - (char*)this->ptr;
            const auto encDiff = encodeRoundDown(diff);

            assertThat(decode(encDiff) == diff, "Heap corruption: bad block offset in setNext");

            this->setSize(encDiff);
        }

        /// Get (physically) next block.
        inline Heap::Block getNext() const {
            return {(char*)this->ptr + decode(this->getSize())};
        }

        /// Set the size of the payload of this block.
        really_inline void setSize(SizeType size) const
        {
            this->ptr[Base::Block::nextFieldIdx] =
                    (this->ptr[Base::Block::nextFieldIdx] & Base::sizeMsb)
                    | (size & ~Base::sizeMsb);
        }

        /// Helper that shrinks this block and returns the leftover.
        really_inline Heap::Block split(uintptr_t size, bool isLeftoverFree) const
        {
            Heap::Block oldNext = getNext();
            setSize(size);

            Heap::Block leftover(getNext());
            leftover.setPrev(*this);
            leftover.setNext(oldNext);
            leftover.setFree(isLeftoverFree);
            return leftover;
        }

        /**
         * Helper that merges the other block into this.
         *
         * After the operation this block has a length so that,
         * its end is the same as the end of the other.
         * @see	Heap for the details.
         */
        really_inline void merge(const Heap::Block &other) const
        {
            setNext(other.getNext());
            other.setPrev(*this);

            other.updateChecksum();
            updateChecksum();
        }

        /// Update the prev size field of the next block, if present.
        really_inline void updateNext(const Heap::Block &end) const
        {
            if(hasNext(end))
            {
                getNext().setPrev(*this);
                getNext().updateChecksum();
            }
        }

        static really_inline SizeType getLow(SizeType v) {
            return v & ~(SizeType(-1) << (sizeof(SizeType) * 8 / 2));
        }

        static really_inline SizeType getHigh(SizeType v) {
            return v >> (sizeof(SizeType) * 8 / 2);
        }

        static really_inline SizeType hash(SizeType v, SizeType k)
        {
            static constexpr auto sizeBits = 8 * sizeof(SizeType);
            static constexpr auto sizeLog = ilog2(sizeBits);

            const auto f = (v + 5) * (k + 31);
            const auto top = f >> (sizeBits - sizeLog);
            const auto rot = (f >> top) | (f << (sizeBits - top));

            return getLow(rot) ^ getHigh(rot);
        }

        /// Helper that calculates a one (double) round Feistel mix of thr prev field using the next field as key.
        really_inline SizeType calculateChecksum() const
        {
            const auto al = getLow(this->ptr[prevFieldIdx]);
            const auto ah = getHigh(this->ptr[prevFieldIdx]);
            const auto bl = getLow(this->ptr[Base::Block::nextFieldIdx]);
            const auto bh = getHigh(this->ptr[Base::Block::nextFieldIdx]);
            const auto oh = ah ^ hash(al, bl);
            const auto ol = al ^ hash(oh, bh);
            return (oh << (sizeof(SizeType) * 8 / 2)) + ol;
        }

        /// Helper to update the checksum field.
        really_inline void updateChecksum() const
        {
            if(useChecksum)
            {
                this->ptr[checksumFieldIdx] = this->calculateChecksum();
            }
        }

        /// Helper to check the validity of the checksum field.
        really_inline bool checkChecksum() const {
            return useChecksum ? (this->ptr[checksumFieldIdx] == this->calculateChecksum()) : true;
        }
    };

    Block end = 0; //!< Tricky one, points over the end, never dereferenced (ie. no setter or getter methods called on it).

    static constexpr uintptr_t maxBlockSize = decode((Base::sizeMsb - 1));
    static constexpr uintptr_t minEncodedBlockSize = encodeRoundUp(Policy::freeHeaderSize + Block::headerSize);

    /**
     * Round down to alignment.
     *
     * Used internally to enforce alignment of various addresses and sizes.
     *
     * @param	x The address/size to be aligned.
     * @return	The largest value that is:
     * 				1. Not greater than _x_ and
     * 				2. Satisfies the alignment criterion.
     */
    really_inline static constexpr uintptr_t alignDown(uintptr_t x) {
        return x & (uintptr_t(-1) << alignmentBits);
    }

    /**
     * Round up to alignment.
     *
     * Used internally to enforce alignment of various addresses and sizes.
     *
     * @param	x The address/size to be aligned.
     * @return	The smallest value that is:
     * 				1. Not smaller than _x_ and
     * 				2. Satisfies the alignment criterion.
     */
    really_inline static constexpr uintptr_t alignUp(uintptr_t x) {
        return alignDown(x + (uintptr_t(1) << alignmentBits) - 1);
    }

    really_inline  bool canSplit(Block block, uintptr_t size) {
        return minEncodedBlockSize + size <= block.getSize();
    }

public:
    struct Stat
    {
        size_t longestFree;
        size_t totalFree;
        size_t nUsed;
        size_t totalUsed;
    };

    /**
     * Create an uninitialized heap, must be set up before use with the **init** method.
     */
    inline Heap() = default;

    /**
     * Initialize the heap.
     *
     * Initializes the internal data structures of the heap on the given block of memory.
     *
     * @param	space The pointer to the start of the heap space.
     * @param	size The size of the heap space.
     */
    inline Heap(void* start, uintptr_t size) {
        init(start, size);
    }

    inline void init(void* start, uintptr_t size)
    {
        assertThat(decode(minEncodedBlockSize) < size, "Heap too big for format (SizeType can not represent size of heap space)");

        const auto firstBlockPtr = (char*)alignUp((uintptr_t)((char*)start + Block::headerSize));
        const auto downAlignedSize = alignDown((uintptr_t)((char*)start + size - firstBlockPtr));

        assertThat(downAlignedSize <= maxBlockSize, "Heap too big for format (SizeType can not represent size of heap space)");

        end.ptr = (SizeType*)(firstBlockPtr + downAlignedSize);

        const Block first(firstBlockPtr);
        first.setFree(true);
        first.setNext(end);
        first.setPrev(first);
        first.updateChecksum();

        Policy::init(first);

        info() << "Heap created at: " << start << " - " << (void*)(((char*)start) + size) << "\n";
    }

    /**
     * Allocate memory.
     *
     * Allocates the specified amount of memory.
     * The allocated region is aligned to the specified bits (given as alignmentBits template argument).
     *
     * @param	size The amount (in bytes) to be allocated.
     * @param	hot Prefer lower addresses if true higher if false
     * @return	A pointer to the start of the allocated region or NULL on failure.
     */
    inline void* alloc(uintptr_t sizeParam, bool hot = false)
    {
        if(sizeParam > maxBlockSize)
        {
            warn() << "alloc(): Too large block requested !\n";
            return 0;
        }

        const uintptr_t size = encodeRoundUp(max(sizeParam, Policy::freeHeaderSize) + Block::headerSize);

        const Block block(this->Policy::findAndRemove(size, hot));

        if(block.ptr == 0)
        {
            warn() << "alloc(): Can not allocate " << decode(size) << " bytes\n";
            return nullptr;
        }
        else
        {
            assertThat(block.checkChecksum(), "Heap corruption: free block with invalid checksum fetched from storage");
            assertThat(size <= block.getSize(), "Internal error");
            assertThat(block.isFree(), "Internal error");

            if(hot)
            {
                if(canSplit(block, size))
                {
                    const auto leftover(block.split(size, true));
                    leftover.updateNext(end);
                    leftover.updateChecksum();
                    this->Policy::add(leftover);
                }
            }
            else
            {
                const auto splitPoint = block.getSize() - size;
                if(minEncodedBlockSize <= splitPoint)
                {
                    const auto leftover(block.split(splitPoint, false));
                    leftover.updateNext(end);
                    leftover.updateChecksum();

                    block.updateChecksum();
                    this->Policy::add(block);

                    dbg() << "alloc(" << sizeParam << "): " << block.ptr << "\n";
                    return leftover.ptr;
                }
            }

            dbg() << "alloc(" << sizeParam << "): " << block.ptr << "\n";

            block.setFree(false);
            block.updateChecksum();

            assertThat(block.getSize() >= size, "Internal error: output size less than requested");
            return block.ptr;
        }


    }

    /**
     * Release used memory.
     *
     * Places back the given block into the free stores.
     *
     * @param	r The pointer to the block that is to be freed.
     * 			It has to be a pointer returned by the method alloc, without any offset!
     */
    inline void free(void* r)
    {
        assertThat(r, "free(): Invalid argument\n");

        const Block block(r);
        assertThat(block.checkChecksum(), "Heap corruption: free called on block with invalid checksum");
        assertThat(!block.isFree(), "Heap corruption: free called on block that is already free");

        dbg() << "free(" <<  r << "): " << decode(block.getSize()) << " freed\n";

        bool prevFree = block.hasPrev() && block.getPrev().isFree();
        bool nextFree = block.hasNext(end) && block.getNext().isFree();

        if(prevFree)
        {
            const auto prev(block.getPrev());
            assertThat(prev.checkChecksum(), "Heap corruption: previous block has invalid checksum when freeing");

            uintptr_t oldSize = prev.getSize();

            if(nextFree)
            {
                const auto next(block.getNext());
                assertThat(next.checkChecksum(), "Heap corruption: next block has invalid checksum when freeing");

                this->Policy::remove(next);

                prev.merge(next);
                prev.updateNext(end);
            }
            else
            {
                if(block.hasNext(end))
                {
                    assertThat(block.getNext().checkChecksum(), "Heap corruption: next block has invalid checksum when freeing");
                }

                prev.merge(block);
                prev.updateNext(end);
            }

            this->Policy::update(oldSize, prev);
        }
        else
        {
            block.setFree(true);

            if(nextFree)
            {
                const Block next(block.getNext());
                assertThat(next.checkChecksum(), "Heap corruption: next block has invalid checksum when freeing");

                this->Policy::remove(next);

                block.merge(next);
                block.updateNext(end);
            }
            else
            {
                block.updateChecksum();
            }

            this->Policy::add(block);
        }
    }

    /**
     * Resizes an allocation block.
     *
     * If smaller size is requested it splits the given block and frees up the end of it, if
     * the resulting block is big enough. If a larger size is requested and if the next block
     * is free and big enough then it is split and its first half is merged with the given
     * allocation.
     *
     * NOTE: It is not the same as stdlibc's _realloc_, as it never moves the data.
     *
     * @param	ptr The pointer to the block whose allocation size is to modified.
     * 			It has to be a pointer returned by the method alloc, without any offset!
     * @param 	newSizeParam The requested new size of the block.
     * @return	The new size of the block. Note that depending on the actual arguments
     * 			and the configuration and state of the allocator it can be more or less
     * 			than to the _newSizeParam_ parameter, however it is never smaller than
     * 			the smaller of the current size and the requested size.
     */
    inline uintptr_t resize(void* ptr, uintptr_t newSizeParam)
    {
        assertThat(ptr, "resize(): Null argument\n");

        const Block block(ptr);
        assertThat(block.checkChecksum(), "Heap corruption: resize called on a block with invalid checksum");
        assertThat(!block.isFree(), "Heap corruption: resize called on free block");

        dbg() << "resize(" <<  ptr << "): " << (decode(block.getSize()) - Block::headerSize) << " -> " << newSizeParam;

        uintptr_t requestedSize = encodeRoundUp(max(newSizeParam, Policy::freeHeaderSize) + Block::headerSize);

        if(requestedSize < block.getSize())
        {
            if(canSplit(block, requestedSize))
            {
                const auto leftover(block.split(requestedSize, true));
                block.updateChecksum();

                if(leftover.hasNext(end))
                {
                    const auto next(leftover.getNext());
                    assertThat(next.checkChecksum(), "Heap corruption: next block has invalid checksum when resizing");

                    if(next.isFree())
                    {
                        this->Policy::remove(next);
                        leftover.merge(next);
                    }
                    else
                    {
                        leftover.updateChecksum();
                    }

                    leftover.updateNext(end);
                }
                else
                {
                    leftover.updateChecksum();
                }

                this->Policy::add(leftover);
            }
        }
        else if(block.getSize() < requestedSize)
        {
            if(block.hasNext(end))
            {
                const auto next(block.getNext());
                assertThat(next.checkChecksum(), "Heap corruption: next block has invalid checksum when resizing");

                if(next.isFree() && requestedSize <= next.getSize() + block.getSize())
                {
                    const auto newSliceSize = requestedSize - block.getSize();

                    this->Policy::remove(next);

                    if(canSplit(next, newSliceSize))
                    {
                        const auto oldNextNext(next.getNext());
                        block.setSize(requestedSize);
                        block.updateChecksum();

                        const auto newNext(block.getNext());
                        newNext.setNext(oldNextNext);
                        newNext.setFree(true);
                        newNext.setPrev(block);
                        newNext.updateChecksum();

                        newNext.updateNext(end);

                        this->Policy::add(newNext);
                    }
                    else
                    {
                        block.merge(next);
                        block.updateNext(end);
                    }
                }
            }
        }

        return decode(block.getSize()) - Block::headerSize;
    }

    /**
     * Decrease size of a block by moving its starting address higher and leaving the end
     * of the allocation range at its current address.
     *
     * The block modified to start at a higher address, and its size is decreased accordingly.
     * The newly created free block is merged with the previous one if it is also free or
     * it gets put back into storage if the previous block is used. This also affects the
     * minimal size that can be cut off.
     *
     * @param	ptr The pointer to the block whose allocation size is to modified.
     * 			It has to be a pointer returned by the method alloc, without any offset!
     * @param 	offset is the request number of bytes to move the block start address.
     * @return	The new starting address of the block. Note that depending on the actual
     *          arguments and the configuration and state of the allocator it can either
     *          be the original starting address or the original address plus the
     *          requested offset rounded down to the nearest aligned address.
     */
    inline void* dropFront(void* ptr, uintptr_t offset)
    {
        assertThat(ptr, "dropFront(): Null argument\n");
        dbg() << "dropFront(" <<  ptr << ", " << offset << ") ";

        const Block block(ptr);
        assertThat(block.checkChecksum(), "Heap corruption: dropFront called on a block that has invalid checksum");
        assertThat(!block.isFree(), "Heap corruption: dropFront called on a free block");

        const auto maxOffset = block.getSize() - minEncodedBlockSize;

        if(uintptr_t splitOffset = min(maxOffset, encodeRoundDown(offset)))
        {
            if(block.hasPrev() && block.getPrev().isFree())
            {
                const auto next(block.getNext());
                const auto prev(block.getPrev());
                assertThat(prev.checkChecksum(), "Heap corruption: previous block has invalid checksum in dropFront");

                const auto oldSize = prev.getSize();
                prev.setSize(prev.getSize() + splitOffset);
                prev.updateChecksum();
                this->Policy::update(oldSize, prev);

                const auto newBlock(prev.getNext());
                newBlock.setFree(false);
                newBlock.setNext(next);
                newBlock.setPrev(prev);
                newBlock.updateChecksum();
                newBlock.updateNext(end);

                return newBlock.ptr;
            }
            else if(minEncodedBlockSize <= splitOffset)
            {
                const auto newBlock(block.split(splitOffset, false));
                block.setFree(true);
                block.updateChecksum();
                this->Policy::add(block);

                newBlock.updateChecksum();
                newBlock.updateNext(end);
                return newBlock.ptr;
            }
        }

        return block.ptr;
    }

    /** @cond */
    inline Stat getStats(void *start)
    {
        Block block((char*)alignUp((uintptr_t)((char*)start + Block::headerSize)));
        Stat ret{0, 0, 0, 0};

        bool prevFree = false;
        while(true)
        {
            assertThat(block.checkChecksum(), "Heap corruption: invalid checksum found during getStats");

            const auto len = decode(block.getSize()) - Block::headerSize;

            if(block.isFree())
            {
                if(ret.longestFree < len)
                {
                    ret.longestFree = len;
                }

                ret.totalFree += len;
                assertThat(!prevFree, "Heap corruption: unmerged free blocks found during getStats");
                prevFree = true;
            }
            else
            {
                ret.nUsed++;
                ret.totalUsed += len;
                prevFree = false;
            }

            if(block.hasNext(end))
            {
                block = block.getNext();
            }
            else
            {
                break;
            }
        }

        return ret;
    }
    /** @endcond */
};

}

#endif /* HEAP_H_ */
