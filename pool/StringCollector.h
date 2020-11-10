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

#ifndef STRINGSTORAGE_H
#define STRINGSTORAGE_H

#include <stddef.h>

namespace pet {

/**
 * Appendable multiple string storage.
 *
 * This storage provider is capable of storing multiple strings packed together into frames.
 * Useful for storing many strings highly varying in size.
 * Also provides spare configurable amount of spare space for each string, that could be used
 * for storing a header along with the strings (like HString for example).
 *
 * The storage can be modeled like this:
 *
 *  - There are multiple string objects in the storage.
 *  - Only the last one is available for modification, the others live as long as the container.
 *  - The last (active) one can be:
 *  	- Appended to -> store additional characters at the end,
 *  	- Dropped -> release the already acquired storage area,
 *  	- Peeked -> read back the already stored content,
 *  	- Commited -> make it fixed, successive calls will not alter its content or size.
 *
 *  Any string in the storage can be at most as big as a frame (minus the optional spare area),
 *  but multiple shorter strings are grouped together in a single frame. Only ever allocates
 *  whole frames at once.
 *
 * @tparam	frameSize The size of the frames in sizeof(char).
 * @tparam	spareSize The size of spare area per string in sizeof(char).
 * @tparam	Allocator The allocator used for the allocation of frames.
 *
 * @note	It is mainly supposed to be the lower layer of the Registry container, but probably
 * 			could be used for something else by itself too.
 */
template<unsigned int frameSize, unsigned int spareSize, class Allocator>
class StringCollector {
	static_assert(frameSize > spareSize + 1, "frameSize too small");
protected:
	class Frame {
		friend StringCollector;
		Frame* prev;
		char* end;
		char data[frameSize];

		Frame(Frame* f): prev(f), end(data){}

		void* operator new(size_t s, void* self){
			return self;
		}
	};

	Frame *currFrame;
	char *start;

	bool ensure(unsigned int size){					// TODO	ensure correct alignment.
		if(currFrame->end-start+size > frameSize)
			return false;

		if((currFrame->end-currFrame->data)+size > frameSize){
			Frame* newFrame;
			void* space = Allocator::alloc((unsigned int)sizeof(Frame));

			if(!space)
				return false;

			newFrame = new(space) Frame(currFrame);
			int size = currFrame->end-start;
			for(char *in = start, *out = newFrame->data; in<currFrame->end;)
				*out++ = *in++;

			currFrame->end = start;
			currFrame = newFrame;
			start = newFrame->data;
			newFrame->end = start+size;
		}
		return true;
	}
public:
	StringCollector(): currFrame(0), start(0) {}

	/**
	 *	Write a character.
	 *
	 *	Appends a character to the currently active string.
	 *
	 *	@param	c The character to be appended.
 	 *	@return	True on success, false if the active string can not be enlarged any more.
	 */
	inline bool put(char c)
	{
		if(!currFrame) {
			void* space = Allocator::alloc((unsigned int)sizeof(Frame));

			if(!space)
				return false;

			currFrame = new(space) Frame(0);
			start = currFrame->data;
		}

		if(currFrame->end == start) {
			ensure(spareSize+1);	// always true
			currFrame->end += spareSize;
		} else {
			if(!ensure(1))
				return false;
		}

		*currFrame->end++ = c;
		return true;
	}

	/**
	 * Write a null-terminated string.
	 *
	 * Convenience method that iterates over a zero-terminated character string and
	 * calls the put(char) on each character respectively.
	 *
	 * @param	str The character to be appended.
 	 * @return	True on success, false if the active string can not be enlarged any more.
	 */
	inline bool put(const char *str){
		while(*str)
			if(!put(*str++))
				return false;

		return true;
	}

	/**
	 * Drop content of the active string.
	 *
	 * Resets the active string to empty state, releases the resources acquired
	 * for the current string if there is any.
	 */
	inline void discard(){
		if(start == currFrame->data){
			Frame *prev = currFrame->prev;
			if(prev){
				Allocator::free(currFrame);
				currFrame = prev;
			}
			start = currFrame->end;
		}else
			currFrame->end = start;
	}

	/**
	 * Commits the active string.
	 *
	 * Makes the active string final.
	 *
	 * @return	A pointer to the start of the block of memory where the string is stored.
	 *
	 * @note	If the spareArea parameter is nonzero, the pointer returned points to the address
	 * 			that is that many sizeof(chars) before the string (that is the spare area)
	 */
	inline void* terminate(){
		put('\0');
		char* ret = start;
		start = currFrame->end;

		if(ret == currFrame->data){
			Frame* prev = currFrame->prev;
			if(prev)
				Allocator::shrink(prev, sizeof(Frame)-(prev->data+frameSize-prev->end));
		}

		return ret;
	}

	/**
	 * Peek the active string.
	 *
	 * Enables access to the stored content, without terminating the string.
	 *
	 * @return	A pointer to the start of the block of memory where the string is stored.
	 *
	 * @note	If the spareArea parameter is nonzero, the pointer returned points to the address
	 * 			that is that many sizeof(chars) before the string (that is the spare area)
	 */
	inline void* peek(){
		put('\0');
		currFrame->end--;
		return start;
	}

	/**
	 * Frees all the allocated frames.
	 *
	 * The blocks returned by terminate should only be used as long as the container is not destroyed,
	 * because it exhibits an arena like behavior and releases all the resources claimed.
	 */
	~StringCollector(){
		for(Frame* f = currFrame; f;){
			Frame* prev = f->prev;
			Allocator::free(f);
			f = prev;
		}
	}
};

}

#endif
