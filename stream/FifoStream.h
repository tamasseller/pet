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
#include "stream/VirtualizedStream.h"

#include "ubiquitous/Error.h"

namespace pet {

template<class Base>
class ReadableFifoStreamBackend: private StreamBackend {
	template<class T>
	friend class Stream;

	template<class T>
	friend class VirtualizedStream;

	/**
	 * Obtain a readable block, compatibly with the StreamBackend concept.
	 */
	pet::GenericError nextBlock(StreamState* state, uint16_t lengthHint, bool growAsNeeded)
	{
		if(growAsNeeded)
			return pet::GenericError::writeError();

		Base* base = static_cast<Base*>(this);

		if(base->isEmpty())
			return 0;

		return accessState<uint16_t, &StreamState::readableLength>(state) = base->access(
				accessState<char*, &StreamState::buffer>(state),
				base->getRealReadedIdx(),
				(lengthHint > base->getOccupied()) ? base->getOccupied() : lengthHint);
	}

	/**
	 * Release a readable block, compatibly with the StreamBackend concept.
	 */
	pet::GenericError blockDone(StreamState* state) {
		Base* base = static_cast<Base*>(this);

		uint16_t &idx = accessState<uint16_t, &StreamState::idx>(state);

		base->Base::doneReading(idx);
		accessState<uint16_t, &StreamState::readableLength>(state) = 0;
		accessState<char*, &StreamState::buffer>(state) = 0;
		idx = 0;
		return 0;
	}
};

template<class Base>
class WritableFifoStreamBackend: private StreamBackend {
	template<class T>
	friend class Stream;

	template<class T>
	friend class VirtualizedStream;

	/**
	 * Obtain a writable block, compatibly with the StreamBackend concept.
	 */
	pet::GenericError nextBlock(StreamState* state, uint16_t lengthHint, bool growAsNeeded)
	{
		if(!growAsNeeded)
			return pet::GenericError::readError();

		Base* base = static_cast<Base*>(this);

		if(base->isFull())
			return 0;

		return accessState<uint16_t, &StreamState::writableLength>(state) = base->access(
				accessState<char*, &StreamState::buffer>(state),
				base->getRealWriterIdx(),
				(lengthHint > base->getFree()) ? base->getFree() : lengthHint);
	}

	/**
	 * Release a writable block, compatibly with the StreamBackend concept.
	 */
	pet::GenericError blockDone(StreamState* state) {
		Base* base = static_cast<Base*>(this);

		uint16_t &idx = accessState<uint16_t, &StreamState::idx>(state);

		base->Base::doneWriting(idx);
		accessState<uint16_t, &StreamState::writableLength>(state) = 0;
		accessState<char*, &StreamState::buffer>(state) = 0;
		idx = 0;
		return 0;
	}
};

template<class Base>
class WritableFifoStream:
		public WritableFifoStreamBackend<Base>,
		public Stream<WritableFifoStream<Base>> {};

template<class Base>
class ReadableFifoStream:
		public ReadableFifoStreamBackend<Base>,
		public Stream<ReadableFifoStream<Base>> {};

template<class Base>
using VirtualReadableFifoStream = VirtualizedStream<ReadableFifoStreamBackend<Base>>;

template<class Base>
using VirtualWritableFifoStream = VirtualizedStream<WritableFifoStreamBackend<Base>>;

template<class Base, bool virtualize> struct RBaseSelector;

template<class Base> struct RBaseSelector<Base, true> {
	typedef VirtualReadableFifoStream<Base> ReadableStream;
};

template<class Base> struct RBaseSelector<Base, false> {
	typedef ReadableFifoStream<Base> ReadableStream;
};

template<class Base, bool virtualize> struct WBaseSelector;

template<class Base> struct WBaseSelector<Base, true> {
	typedef VirtualWritableFifoStream<Base> WritableStream;
};

template<class Base> struct WBaseSelector<Base, false> {
	typedef WritableFifoStream<Base> WritableStream;
};

template<uint16_t size, bool virtualizeWriteable = false, bool virtualizeReadable = false>
class StaticFifoStream:
		public StaticFifo<size>,
		public WBaseSelector<StaticFifoStream<size>, virtualizeWriteable>::WritableStream,
		public RBaseSelector<StaticFifoStream<size>, virtualizeReadable>::ReadableStream {};

template<uint16_t size, bool virtualizeWriteable = false, bool virtualizeReadable = false>
class IndirectFifoStream:
		public IndirectFifo<size>,
		public WBaseSelector<IndirectFifo<size>, virtualizeWriteable>::WritableStream,
		public RBaseSelector<IndirectFifo<size>, virtualizeReadable>::ReadableStream {};
}


#endif /* FIFOSTREAM_H_ */
