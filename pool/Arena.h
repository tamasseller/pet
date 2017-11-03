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

#ifndef ARENA_H_
#define ARENA_H_

#include "ubiquitous/Error.h"

namespace pet {
/**
 * Incremental allocator.
 *
 * Implements a memory management strategy where the only deallocation taking
 * place is during destruction. Groups together individual objects in frames
 * of bigger size in order to circumvent heap fragmentation.
 *
 * @tparam	Element The type of the objects to be managed.
 * @tparam	Allocator The allocator to be used for allocating and finally freeing the frames.
 * @tparam 	countPerFrame The number of items in a frame.
 */
template <typename Element, class Allocator, unsigned int countPerFrame = 16>
class Arena {
	struct Frame{
		Frame* prev;
		char elements[countPerFrame][sizeof(Element)];

		void* operator new(unsigned int s, void* self){
			return self;
		}

		Frame(Frame* p): prev(p){}
	};
	Frame *frameList=nullptr;
	unsigned int counter;
public:
	/**
	 * Get a free element object.
	 *
	 * Acquires an unused element object.
	 *
	 * @return 	The allocated object or NULL on failure.
	 */
	pet::FailPointer<Element> acquire(){
		if(!frameList || counter == countPerFrame){
			counter = 0;

			void *space = Allocator::alloc(sizeof(Frame));

			if(!space)
				return 0;

			frameList = new(space) Frame(frameList);
		}

		return (Element*)frameList->elements[counter++];
	}

	/**
	 * Do nothing.
	 *
	 * This is only required to fulfill the requirements of the pool concept.
	 */
	void release(Element*) {

	}

	/**
	 * Release the frames.
	 *
	 * Frees the used memory resources.
	 *
	 * @note	Does not call the destructors.
	 */
	~Arena(){
		for(Frame* f = frameList; f;){
			Frame* prev = f->prev;
			Allocator::free(f);
			f = prev;
		}
	}
};

}

#endif /* ARENA_H_ */
