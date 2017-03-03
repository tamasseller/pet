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

#include "ubiquitous/Error.h"

#include <stdint.h>
#include <limits.h>

namespace pet {

/**
 * Lock-free circular buffer index manager.
 *
 * This class provides data placement management logic for First In
 * First Out buffers. It can be used by a **single reader** and a
 * **single writer** concurrently without any locking.
 * Locking is not required due to the fact that:
 *
 *  - a. the _reader index_ is only modified by the reader,
 *  - b. the _writer index_ is only modified by the writer,
 *  - c. both only modify their side atomically as seen by the other.
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
 *  and full states can easily be differentiated. The FIFO buffer is
 *  empty when the indices are equal, and it is full when the writer is
 *  ahead of the reader by exactly the size of the buffer.
 */
template<class Child, uint16_t size>
class FifoBase{
	uint16_t readIdx = 0, writeIdx = 0;

	inline uint16_t getOccupied() const {
		return ((uint16_t)(writeIdx - readIdx) % (2 * size));
	};

	inline uint16_t getFree() const {
		return size - getOccupied();
	};

	inline uint16_t getRealReadedIdx() const {
		return readIdx % size;
	};

	inline uint16_t getRealWriterIdx() const {
		return writeIdx % size;
	};

	/**
	 * Buffer access helper.
	 *
	 * @param realIdx The actual index in the buffer.
	 * @param nItems Number data items intended and possible to access.
	 */
	inline pet::GenericError access(char* &buff, uint16_t realIdx, uint16_t nItems) const {
		/*
		 * The actual space left until the end of buffer,
		 * if the reader is at the end, the whole buffer.
		 */
		const uint16_t space = size - realIdx;

		buff = ((Child*)this)->getBuffer() + realIdx;

		/*
		 * Amount of data before the end of buffer or the
		 * writer pointer, whichever is encountered first.
		 */
		return  (space > nItems) ? nItems : space;
	}

public:
	/**
	 * Obtain a readable block, compatibly with the StreamBackend concept.
	 *
	 * The application can request a block of the FIFO buffer for
	 * reading by utilizing this function. When done it needs to be
	 * released with the _doneReading_ method.
	 *
	 * @param buff A reference to the pointer into which the address of the block is stored.
	 * @param length The length of the block of the intended read operation (hint).
	 * @return The amount of data available for reading in bytes (or zero if none).
	 */
	inline pet::GenericError nextReadable(char* &buff, uint16_t length = USHRT_MAX) const;

	/**
	 * Obtain a writable block, compatibly with the StreamBackend concept.
	 *
	 * The application can request a block of the FIFO buffer for
	 * writing by utilizing this function. When done it needs to be
	 * released with the _doneWriting_ method.
	 *
	 * @param buff A reference to the pointer into which the address of the block is stored.
	 * @param length The length of the block of the intended read operation (hint).
	 * @return The amount of space available for writing in bytes (or zero if none).
	 */
	inline pet::GenericError nextWritable(char* &buff, uint16_t length = USHRT_MAX) const;

	/**
	 * Release a readable block, compatible with the StreamBackend concept.
	 *
	 *
	 * @param buff A pointer to the block acquired via the nextReadable call, gets reset to zero.
	 * @param offset The number of bytes used up, gets reset to zero.
	 * @return Zero on success, the error code otherwise.
	 */
	inline pet::GenericError doneReading(char* &buff, uint32_t &offset);

	/**
	 * Release a readable block, simplified.
	 *
	 * The user can release the block previously acquired by the nextWritable method as
	 * fully or partially read in case of the whole available data was not used.
	 *
	 * @param offset The number of bytes used up, gets reset to zero.
	 */
	inline void doneReading(uint16_t offset);

	/**
	 * Release a written block, compatibly with the StreamBackend concept.
	 *
	 * @param buff A pointer to the block acquired via the nextWritable call, gets reset to null.
	 * @param offset The number of bytes used up, gets reset to zero.
	 * @return Zero on success, the errorcode otherwise.
	 */
	inline pet::GenericError doneWriting(char* &buff, uint32_t &offset);

	/**
	 * Release a written block, simplified.
	 *
	 * The user can release the block previously acquired by the nextWritable method as
	 * fully or partially written in case of the whole available space was not used.
	 *
	 * @param offset The number of bytes used up, gets reset to zero.
	 */
	inline void doneWriting(uint16_t offset);
};

template<class Child, uint16_t size>
inline pet::GenericError FifoBase<Child, size>::nextReadable(char* &buff, uint16_t length) const
{
	/*
	 * If the reader and writer are at the same index, the FIFO is empty.
	 */
	if(writeIdx == readIdx)
		return 0;

	return access(buff, getRealReadedIdx(), (length > getOccupied()) ? getOccupied() : length);
}

template<class Child, uint16_t size>
inline pet::GenericError FifoBase<Child, size>::nextWritable(char* &buff, uint16_t length) const
{
	/*
	 * The writer is ahead of the reader by the
	 * size of the buffer, the FIFO is full.
	 */
	if(writeIdx == (readIdx + size) % (2 * size))
		return 0;

	return access(buff, getRealWriterIdx(), (length > getFree()) ? getFree() : length);
}

template<class Child, uint16_t size>
inline pet::GenericError FifoBase<Child, size>::doneReading(char* &buffer, uint32_t &offset) {
	doneReading(offset);
	offset = 0;
	buffer = 0;
	return 0;
}

template<class Child, uint16_t size>
inline void FifoBase<Child, size>::doneReading(uint16_t offset) {
	readIdx = (readIdx + offset) % (2 * size);
}

template<class Child, uint16_t size>
inline pet::GenericError FifoBase<Child, size>::doneWriting(char* &buffer, uint32_t &offset) {
	doneWriting(offset);
	offset = 0;
	buffer = 0;
	return 0;
}

template<class Child, uint16_t size>
inline void FifoBase<Child, size>::doneWriting(uint16_t offset) {
	writeIdx = (writeIdx + offset) % (2 * size);
}

/**
 * FIFO with embedded storage.
 *
 * This FIFO buffer variant contains the buffer-space used for
 * storing the business data, mainly for convenience but also to
 * avoid storing an additional pointer to the space next to it.
 */
template<uint32_t size>
class StaticFifo: public FifoBase<StaticFifo<size>, size> {
	typedef FifoBase<StaticFifo, size> Base;
	friend Base;

	char buffer[size];

	inline char* getBuffer() {
		return buffer;
	}
};

/**
 * FIFO with externals storage.
 *
 * This FIFO buffer variant does not contain the buffer-space
 * used for storing the business data but only a pointer to it.
 * It is mainly targeted at solutions requiring the buffer-space
 * to be placed in a specific memory range (because of DMA access
 * for example).
 */
template<uint32_t size>
class IndirectFifo: public FifoBase<IndirectFifo<size>, size> {
	typedef FifoBase<IndirectFifo, size> Base;
	friend Base;

	char* buffer;

	inline char* getBuffer() {
		return buffer;
	}
public:
	/**
	 * Construct with buffer.
	 *
	 * The address of the external buffer needs to be specified here,
	 * right at construction time.
	 */
	inline IndirectFifo(char* buffer): buffer(buffer) {}
};


}
#endif /* FIFO_H_ */
