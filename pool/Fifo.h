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

#ifndef FIFO_IN_POOL_H_
#define FIFO_IN_POOL_H_

#include "ubiquitous/Error.h"

#include <stddef.h>

namespace pet {

/**
 * Dynamic, framed FIFO object.
 *
 * Uses frames of predefined size to store incoming data.
 * As data is read out the frames that become unused are freed.
 *
 * @tparam	Element The (copy constructible) type of the stored data.
 * @tparam	Allocator The allocator to be used for dynamic memory operations.
 * @tparam 	frameSize the number of Elements to be put on a single frame.
 */
template<class Element, class Allocator, unsigned int frameSize>
class DynamicFifo {
private:
	struct Frame {
		Frame *next = nullptr;
		Element data[frameSize];
		inline void* operator new( size_t sz, void* here ) {return here;}
	};

	struct Position {
		int pos = 0;
		Frame *frame = nullptr;

		inline bool isValid();
		inline bool release();
		inline bool acquire();
		inline operator Element*();
		inline bool operator ==(const Position&);
	};

	Position write, read;
	bool canRead = false;
	inline void freeFrames();
public:
	/**
	 * Write to the FIFO.
	 *
	 * Writes (copies) an element to the back of the FIFO.
	 *
	 * @param	elem The element to be written.
	 * @return	True on success, false on memory allocation error.
	 */
	inline bool writeOne(const Element& elem);

	/**
	 * Read from the FIFO.
	 *
	 * Reads an element from the front of the FIFO.
	 *
	 * @param	elem A reference through which the read value is returned.
	 * @return	True on success, false if there is nothing to read.
	 */
	inline bool readOne(Element& elem);

	/**
	 * Empty out.
	 *
	 * Discards the content of the FIFO.
	 */
	inline void clear();

	inline ~DynamicFifo();
};

//////////////////////////////////////////////////////////////////////////////////////////

template<class Element, class Allocator, unsigned int frameSize>
inline bool DynamicFifo<Element, Allocator, frameSize>::Position::isValid() {
	return frame != 0;
}

template<class Element, class Allocator, unsigned int frameSize>
inline DynamicFifo<Element, Allocator, frameSize>::Position::operator Element*() {
	return frame->data + pos;
}

template<class Element, class Allocator, unsigned int frameSize>
inline bool DynamicFifo<Element, Allocator, frameSize>::Position::operator ==(const Position& other) {
	return (frame == other.frame) && (pos == other.pos);
}

template<class Element, class Allocator, unsigned int frameSize>
inline bool DynamicFifo<Element, Allocator, frameSize>::Position::release()
{
	if(frame) {
		if(pos == frameSize - 1) {
			Frame* oldFrame = frame;

			frame = frame->next;
			pos = 0;

			Allocator::free(oldFrame);
			return true;
		} else
			pos++;
	}
	return false;
}

template<class Element, class Allocator, unsigned int frameSize>
inline bool DynamicFifo<Element, Allocator, frameSize>::Position::acquire()
{
	if(!frame || (pos == frameSize - 1)) {
		void *place = Allocator::alloc((unsigned int) sizeof(Frame));

		if(!place)
			return false;

		Frame* newFrame = new(place) Frame;

		if(frame)
			frame->next = newFrame;

		frame = newFrame;
		pos = 0;
	} else
		pos++;

	return true;
}

template<class Element, class Allocator, unsigned int frameSize>
inline bool DynamicFifo<Element, Allocator, frameSize>::writeOne(const Element& element)
{
	if(!write.acquire())
		return false;

	*(Element*)write = element;

	if(!read.isValid())
		read = write;

	canRead = true;
	return true;
}

template<class Element, class Allocator, unsigned int frameSize>
inline bool DynamicFifo<Element, Allocator, frameSize>::readOne(Element& ret)
{
	if(!read.isValid() || !canRead)
		return false;

	if(read == write)
		canRead = false;

	ret = *(Element*)read;

	if(read.release() && !canRead)
		write.frame = 0;

	return true;
}

template<class Element, class Allocator, unsigned int frameSize>
inline void DynamicFifo<Element, Allocator, frameSize>::freeFrames()
{
	Frame *f = read.frame;
	while(f) {
		Frame *next = f->next;
		Allocator::free(f);
		f = next;
	}
}

template<class Element, class Allocator, unsigned int frameSize>
inline void DynamicFifo<Element, Allocator, frameSize>::clear()
{
	freeFrames();
	read.frame = 0;
	read.pos = 0;
	write.frame = 0;
	write.pos = 0;
}

template<class Element, class Allocator, unsigned int frameSize>
inline DynamicFifo<Element, Allocator, frameSize>::~DynamicFifo()
{
	freeFrames();
}

}

#endif /* FIFO_IN_POOL_H_ */
