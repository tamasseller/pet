/*******************************************************************************
 *
 * Copyright (c) 2017 Tamás Seller. All rights reserved.
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

#ifndef FIFO_H_
#define FIFO_H_

#include <stdint.h>

#include "meta/Utility.h"

namespace pet {

/**
 * Lock-free circular buffer index manager.
 *
 * This class provides data placement management logic for First In
 * First Out buffers. It can be used by a **single reader** and a
 * **single writer** concurrently without any locking.
 * Locking is not required due to the fact that:
 *
 *  - a. the _reader index_ is only modified (owned) by the reader,
 *  - b. the _writer index_ is only modified (owned) by the writer,
 *  - c. both only modify their side atomically as seen by the other.
 *
 *  In cases when the _fifo_ is more than one element further from being empty
 *  or full there is no contention, both sides can progress without affecting
 *  the other in any way.
 *
 *  However, in the specific corner cases, when the _fifo_ is free/empty
 *  or one operation away from that, and both the reader and writer sides
 *  manipulate the _fifo_ at the same time the result is nondeterministic.
 *  But for the above mentioned reasons it can be easily seen that if there
 *  are concurrent modifications even in these corner cases, the state of the
 *  _fifo_ is kept consistent. In other words, any reading or writing will
 *  atomically fail or succeed.
 *
 *  Circular buffers, that represent their state in the form of reader and
 *  writer pointers (as opposed to start pointer and size) bear that property
 *  by nature, but the presented implementation also utilizes the available
 *  buffer fully, that is the last byte is not lost as it would be with the
 *  trivial naive read/write pointer implementation.
 *
 *  The problem of lost byte is rooted in the ambiguity between the full and
 *  empty states: if the reader and writer indices were simple array indices
 *  than these logical states would yield the same representation.
 *
 *  This slight inefficiency is mitigated by enabling the indices to take
 *  values not only between zero and the size of the array minus one, but
 *  instead the interval is doubled. Whenever the data elements are required
 *  to be addressed the reader/writer indices are modulo reduced by the size
 *  of the array. By enabling them to use the doubled intervals the empty
 *  and full states can easily be differentiated. The _fifo_ buffer is
 *  empty when the indices are equal, and it is full when the writer is
 *  ahead of the reader by exactly the size of the buffer.
 *
 *  This optimization is not important for sparing that single byte of memory
 *  from being wasted, but because it enables actual power of two sized
 *  buffers with a modulus that is also power of two (the same value as the
 *  size of the buffer).
 */
template<uint16_t size>
class FifoBase
{
    uint16_t readIdx = 0, writeIdx = 0;
public:
    /**
     * Obtain a readable block.
     *
     * The application can request a block of the FIFO buffer for
     * reading by utilizing this function. When done it needs to be
     * released with the _doneReading_ method.
     *
     * @param idx A reference to variable into which the index of the block is stored.
     * @return The number of data items available (or zero if none).
     */
    inline uint16_t nextReadableIdx(uint16_t &idx) const;

    /**
     * Obtain a writable block.
     *
     * The application can request a block of the FIFO buffer for
     * writing by utilizing this function. When done it needs to be
     * released with the _doneWriting_ method.
     *
     * @param idx A reference to variable into which the index of the block is stored.
     * @return The number of data items available (or zero if none).
     */
    inline uint16_t nextWritableIdx(uint16_t &idx) const;

    /**
     * Release a readable block.
     *
     * The user can release the block previously acquired by the nextWritable method as
     * fully or partially read in case of the whole available data was not used.
     *
     * @param length The number of bytes of data consumed by the user.
     */
    inline void doneReading(uint16_t length);

    /**
     * Release a written block.
     *
     * The user can release the block previously acquired by the nextWritable method as
     * fully or partially written in case of the whole available space was not used.
     *
     * @param length The number of bytes of data produced by the user.
     */
    inline void doneWriting(uint16_t length);

    /**
     * Is there data to be read?
     *
     * Checks whether the FIFO is empty.
     *
     * @return True if there is no data to be read.
     */
    inline bool isEmpty() const;

    /**
     * Is there free space to be written?
     *
     * Checks whether the FIFO is full.
     *
     * @return True if there is no more free space that can be written to.
     */
    inline bool isFull() const;

    /**
     * An (almost) STL style iterator that can be used to iterate over the contents of the FIFO.
     */
    class Iterator
    {
        /**
         * The extended index (as used internally) of the current element.
         */
        uint16_t idx;


        /**
         * Initializing constructor used only internally to produce begin and end iterators.
         */
        inline Iterator(uint16_t idx): idx(idx) {}
        friend FifoBase;
    public:
        /**
         * Default constructor that creates an invalid iterator.
         */
        inline Iterator() = default;

        /**
         * Copy constructor, that works as expected.
         */
        inline Iterator(const Iterator&) = default;

        /**
         * Copy assignment, that works as expected.
         */
        inline Iterator& operator=(const Iterator&) = default;

        /**
         * Target access iterator that returns the real index of the element to be accesed.
         */
        inline auto operator*() const { return idx % size; }
        inline auto operator==(const Iterator& o) const { return idx == o.idx; }
        inline auto operator!=(const Iterator& o) const { return idx != o.idx; }
        inline Iterator operator++() { return idx = (idx + 1) % (2 * size); }
        inline Iterator operator++(int)
        {
            const auto ret = idx;
            ++*this;
            return ret;
        }
    };

    /**
     * Get an iterator pointing to the start of readable data.
     *
     * @note Iterators are not guaranteed to be validated after mutations.
     */
    inline Iterator begin() const {
        return {readIdx};
    }

    /**
     * Get an iterator pointing to the end of readable data.
     *
     * @note Iterators are not guaranteed to be validated after mutations.
     */
    inline Iterator end() const {
        return {writeIdx};
    }

    /**
     * Set the internal read index according the iterator provided.
     *
     * @note This method can be used to implement conditional consumption
     *       of the stored data.
     */
    inline void commitRead(const Iterator& it) {
        readIdx = it.idx;
    }
};

template<uint16_t size>
inline bool FifoBase<size>::isFull() const
{
    /*
     * The writer is ahead of the reader by the
     * size of the buffer, the FIFO is full.
     */
    return writeIdx == (readIdx + size) % (2 * size);
}

template<uint16_t size>
inline bool FifoBase<size>::isEmpty() const
{
    /*
     * If the reader and writer are at the same index, the FIFO is empty.
     */
    return writeIdx == readIdx;
}


template<uint16_t size>
inline uint16_t FifoBase<size>::nextReadableIdx(uint16_t &idx) const
{
    if(isEmpty())
        return 0;

    /*
     * The actual index in the buffer.
     */
    const uint32_t realIdx = (readIdx % size);

    /*
     * The actual space left until the end of buffer,
     * if the reader is at the end, the whole buffer.
     */
    const uint32_t space = size - realIdx;

    /*
     * Pointer to the start of the data.
     */
    idx = realIdx;

    /*
     * Number of data items before the writer pointer.
     */
    const uint32_t nData = (writeIdx - readIdx) % (2 * size);

    /*
     * Amount of data before the end of buffer or the
     * writer pointer, whichever is encountered first.
     */
    return  (space > nData) ? nData : space;
}

template<uint16_t size>
inline uint16_t FifoBase<size>::nextWritableIdx(uint16_t &idx) const
{
    /*
     * The writer is ahead of the reader by the
     * size of the buffer, the FIFO is full.
     */
    if(writeIdx == (readIdx + size) % (2 * size))
        return 0;
    /*
     * The actual index in the buffer.
     */
    const uint32_t realIdx = writeIdx % size;

    /*
     * The actual space left until the end of buffer,
     * if the writer is at the end, the whole buffer.
     */
    const uint32_t space = size - realIdx;

    /*
     * Pointer to the start of the data.
     */
    idx = realIdx;

    /*
     * Number of data items before the reader pointer.
     */
    const uint32_t nData = size - (2 * size + writeIdx - readIdx) % size;

    /*
     * Amount of data before the end of buffer or the
     * reader pointer, whichever is encountered first.
     */
    return  (space > nData) ? nData : space;
}

template<uint16_t size>
inline void FifoBase<size>::doneReading(uint16_t length)
{
    readIdx = (readIdx + length) % (2 * size);
}

template<uint16_t size>
inline void FifoBase<size>::doneWriting(uint16_t length)
{
    writeIdx = (writeIdx + length) % (2 * size);
}

/**
 * Type injection helper.
 */
template<uint32_t size, class DataType, class Child>
class TypedFifoBase: protected FifoBase<size>
{

    typedef FifoBase<size> Base;

public:
    /**
     * Obtain a readable block.
     *
     * Sets the provided pointer to the address of the block containing the data to be read.
     * See FifoBase::nextReadableIdx for details.
     *
     * @param buff A reference to the pointer into which the address of the block is stored.
     * @return The number of data items available (or zero if none).
     * @see FifoBase::nextReadableIdx
     */
    inline uint16_t nextReadable(DataType* &buff)
    {
        uint16_t idx, ret = this->nextReadableIdx(idx);
        buff = static_cast<Child*>(this)->getBuffer() + idx;
        return ret;
    }

    /**
     * Obtain a writable block.
     *
     * Sets the provided pointer to the address of the free block where the data is to be written.
     * See FifoBase::nextWritableIdx for details.
     *
     * @param buff A reference to the pointer into which the address of the block is stored.
     * @return The number of data items available (or zero if none).
     * @see FifoBase::nextWritableIdx
     */
    inline uint16_t nextWritable(DataType* &buff)
    {
        uint16_t idx = 0;
        uint16_t ret = this->nextWritableIdx(idx);
        buff = static_cast<Child*>(this)->getBuffer() + idx;
        return ret;
    }

    using Base::doneReading;
    using Base::doneWriting;
    using Base::isEmpty;
    using Base::isFull;

    using Base::Iterator;
    using Base::begin;
    using Base::end;
    using Base::commitRead;

    /**
     * Read single data element.
     *
     * Convenience method that wraps the next/done sequence of reading one element from the _fifo_.
     *
     * @param data The variable into which element is read.
     * @return True on success, false if already empty.
     */
    inline bool readOne(DataType &data)
    {
        DataType *from;

        if(!nextReadable(from))
            return false;

        data = pet::move(*from);
        doneReading(1);
        return true;
    }

    /**
     * Write single data element.
     *
     * Convenience method that wraps the next/done sequence of writing one element into the _fifo_.
     *
     * @param data The element to be stored.
     * @return True on success, false if already full.
     */
    inline bool writeOne(const DataType &data)
    {
        DataType *to;
        if(!nextWritable(to))
            return false;

        *to = data;
        doneWriting(1);
        return true;
    }

    /**
     * Write single data element.
     *
     * Convenience method that wraps the next/done sequence of writing one element into the _fifo_.
     *
     * @param data The element to be stored.
     * @return True on success, false if already full.
     */
    inline bool writeOne(DataType &&data)
    {
        DataType *to;
        if(!nextWritable(to))
            return false;

        *to = pet::move(data);
        doneWriting(1);
        return true;
    }

    DataType *access(uint16_t idx) {
        return static_cast<Child*>(this)->getBuffer() + idx;
    }

    inline const DataType *access(uint16_t idx) const {
        return static_cast<const Child*>(this)->getBuffer() + idx;
    }
};

/**
 * FIFO with embedded storage.
 *
 * This _fifo_ buffer variant contains the buffer-space used for
 * storing the business data, mainly for convenience but also to
 * avoid storing an additional pointer to the space next to it.
 *
 * @see This _fifo_ is based on the FifoBase lock-free index manager.
 */
template<uint32_t size, class DataType = char>
class StaticFifo: public TypedFifoBase<size, DataType, StaticFifo<size, DataType>> {
    typedef TypedFifoBase<size, DataType, StaticFifo> Base;
    friend Base;

    DataType buffer[size];

    inline DataType* getBuffer() {
        return buffer;
    }

    inline const DataType* getBuffer() const {
        return buffer;
    }
};

/**
 * FIFO with externals storage.
 *
 * This _fifo_ buffer variant does not contain the buffer-space
 * used for storing the business data but only a pointer to it.
 * It is mainly targeted at solutions requiring the buffer-space
 * to be placed in a specific memory range (because of DMA access
 * for example).
 *
 * @see This _fifo_ is based on the FifoBase lock-free index manager.
 */
template<uint32_t size, class DataType = char>
class IndirectFifo: public TypedFifoBase<size, DataType, IndirectFifo<size, DataType>> {
    typedef TypedFifoBase<size, DataType, IndirectFifo> Base;
    friend Base;

    DataType* buffer;

    inline DataType* getBuffer() {
        return buffer;
    }

    inline const DataType* getBuffer() const{
        return buffer;
    }
public:
    /**
     * Construct with buffer.
     *
     * The address of the external buffer needs to be specified here,
     * right at construction time.
     */
    inline IndirectFifo(DataType* buffer): buffer(buffer) {}
};


}
#endif /* FIFO_H_ */
