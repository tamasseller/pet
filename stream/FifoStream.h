/*
 * FifoStream.h
 *
 *  Created on: 2017.03.05.
 *      Author: tooma
 */

#ifndef FIFOSTREAM_H_
#define FIFOSTREAM_H_

#include "data/Fifo.h"
#include "stream/Stream.h"

#include "ubiquitous/Error.h"

namespace pet {

template<class Base>
class FifoStreamifier: public Base {
public:
	/**
	 * Obtain a readable block, compatibly with the StreamBackend concept.
	 *
	 * @param buff A reference to the pointer into which the address of the block is stored.
	 * @param reqLen The length of the block of the intended read operation (hint).
	 * @return The amount of data available for reading in bytes (or zero if none).
	 */
	inline pet::GenericError nextBlock(ReadLength* actLen, char* &buff, uint16_t reqLen) const;

	/**
	 * Obtain a writable block, compatibly with the StreamBackend concept.
	 *
	 * @param buff A reference to the pointer into which the address of the block is stored.
	 * @param reqLen The length of the block of the intended read operation (hint).
	 * @return The amount of space available for writing in bytes (or zero if none).
	 */
	inline pet::GenericError nextBlock(WriteLength* actLen, char* &buff, uint16_t reqLen) const;

	/**
	 * Release a readable block, compatibly with the StreamBackend concept.
	 *
	 * @param buff A pointer to the block acquired via the nextReadable call, gets reset to zero.
	 * @param offset The number of bytes used up, gets reset to zero.
	 * @return Zero on success, the error code otherwise.
	 */
	inline pet::GenericError blockDone(ReadLength* actLen, char* &buff, uint32_t &offset);

	/**
	 * Release a written block, compatibly with the StreamBackend concept.
	 *
	 * @param buff A pointer to the block acquired via the nextWritable call, gets reset to null.
	 * @param offset The number of bytes used up, gets reset to zero.
	 * @return Zero on success, the errorcode otherwise.
	 */
	inline pet::GenericError blockDone(WriteLength* actLen, char* &buff, uint32_t &offset);

};

template<class Base>
inline pet::GenericError FifoStreamifier<Base>::nextBlock(ReadLength* actLen, char* &buff, uint16_t length) const
{
	if(this->isEmpty())
		return 0;

	return actLen->length = this->access(buff,
			this->getRealReadedIdx(),
			(length > this->getOccupied()) ? this->getOccupied() : length);
}

template<class Base>
inline pet::GenericError FifoStreamifier<Base>::nextBlock(WriteLength* actLen, char* &buff, uint16_t length) const
{
	if(this->isFull())
		return 0;

	return actLen->length = this->access(buff,
			this->getRealWriterIdx(),
			(length > this->getFree()) ? this->getFree() : length);
}

template<class Base>
inline pet::GenericError FifoStreamifier<Base>::blockDone(ReadLength* actLen, char* &buffer, uint32_t &offset) {
	this->Base::doneReading((uint16_t)offset);
	offset = 0;
	buffer = 0;
	actLen->length = 0;
	return 0;
}

template<class Base>
inline pet::GenericError FifoStreamifier<Base>::blockDone(WriteLength* actLen, char* &buffer, uint32_t &offset) {
	this->Base::doneWriting((uint16_t)offset);
	offset = 0;
	buffer = 0;
	actLen->length = 0;
	return 0;
}

template<uint16_t size>
class StaticFifoStream:
		public FifoStreamifier<StaticFifo<size>>,
		public InputStream<StaticFifoStream<size>>,
		public OutputStream<StaticFifoStream<size>> {};

template<uint16_t size>
class IndirectFifoStream:
		public FifoStreamifier<IndirectFifo<size>>,
		public InputStream<IndirectFifoStream<size>>,
		public OutputStream<IndirectFifoStream<size>> {};
}


#endif /* FIFOSTREAM_H_ */
