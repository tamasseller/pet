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

#ifndef POOL_H_
#define POOL_H_

#include "data/LinkedList.h"
#include "data/DoubleList.h"
#include "ubiquitous/Error.h"

namespace pet {

/**
 * Static pool of objects.
 */
template <typename Element, unsigned char size=16>
class Pool {
private:
	char pool[size][sizeof(Element)];
	unsigned char head;
	unsigned char tail;

public:
	Pool(){
		static_assert(size != 0xff, "Túl nagy pool");
		clear();
	}

	/**
	 * Get a free object.
	 *
	 * @return	The allocated item.
	 */
	pet::FailPointer<Element> acquire(){
		Element* ret = 0;
		if(!hasFree()){
			if(hasSpare())
				ret = (Element*)pool[tail++];
		} else {
			ret = (Element*)pool[head];
			head = *((unsigned char*)(ret));
		}

		return ret;
	}

	/**
	 * Put back a free item.
	 *
	 * @param object The item to be freed.
	 */
	void release(Element* object){
		int idx = object-(Element*)pool;
		*((unsigned char*)(object)) = head;
		head = idx;
	}

	/**
	 * Uninitialized slots available.
	 *
	 * @return	True if can initialize one.
	 */
	bool hasSpare(){
		return tail < size;
	}

	/**
	 * Freed up slots available.
	 *
	 * @return	True if can allocate from freed up.
	 */
	bool hasFree(){
		return ((unsigned char)~head) != 0u;
	}

	/**
	 * Usable slots of any kind available.
	 *
	 * @return	True if can allocate.
	 */
	bool isEmpty(){
		return !hasFree() && !hasSpare();
	}

	/**
	 * Resets the pool.
	 */
	void clear() {
		head = 0xff;
		tail = 0;
	}
};

/** @copydoc pet::Pool */
template <typename Element, class Allocator, unsigned int countPerFrame = 16>
class StaticPool: public Pool<Element, countPerFrame> {};

/**
 * Dynamic, framed pool.
 *
 * Enables the use of a pool type allocator in cases where the number of
 * elements required is not known exactly. It does so by dynamically allocating
 * and freeing multiple frames out of which it supplies the requests. It is only
 * beneficial to use this type of memory management if it is probable that
 * there is going to be only one frame most of the time.
 *
 * @tparam 	Element The type of the stored elements.
 * @tparam 	Allocator The allocator to be used for frame allocation.
 * @tparam	countPerFrame the number of items in a frame.
 *
 * @note	The guaranteed alignment is the smaller of sizeof(Element) and
 * 			the alignment provided by the Allocator
 */
template <typename Element, class Allocator, unsigned int countPerFrame = 16>
class DynamicPool {
private:
	struct Frame;
	struct ElementWrapper{
		Frame* frame;
		unsigned char element[sizeof(Element)];

		static ElementWrapper* fromPtr(Element* ptr){
			return (ElementWrapper*)((unsigned char*)ptr - sizeof(Frame*));
		}

		Element* toPtr() const{
			return (Element*)((unsigned char*)this + sizeof(Frame*));
		}
	};

	struct Frame{
		Pool<ElementWrapper, countPerFrame> pool;

		void* operator new(unsigned int s, void* self){
			return self;
		}

		Frame *next, *prev;
		unsigned short counter;
	};

protected:
	/** The first element in the frame list */
	pet::DoubleList<Frame> nonemptyFrames;
	pet::LinkedList<Frame> emptyFrames;

public:
	/**
	 * Get a free element object.
	 *
	 * Acquires an unused element object.
	 *
	 * @return 	The allocated object or NULL on failure.
	 */
	pet::FailPointer<Element> acquire()
	{
		Frame* current = nonemptyFrames.iterator().current();

		if(current && current->pool.isEmpty()) {
			nonemptyFrames.remove(current);
			emptyFrames.add(current);
		}

		current = nonemptyFrames.iterator().current();

		if(!current){
			void* newSpace = Allocator::alloc(sizeof(Frame));

			if(!newSpace)
				return 0;

			current = new(newSpace) Frame();
			current->counter = 1;
			nonemptyFrames.add(current);
		}else
			current->counter++;

		auto ret = current->pool.acquire();

		if(ret.failed())
			return 0;

		((ElementWrapper*)ret)->frame = current;

		return ((ElementWrapper*)ret)->toPtr();
	}

	/**
	 * Release an element object.
	 *
	 * Frees an unused element object.
	 *
	 * @param	object The element object to be freed.
	 */
	void release(Element* object){
		ElementWrapper *elem = ElementWrapper::fromPtr(object);
		Frame *frame = elem->frame;

		frame->pool.release(elem);

		if(!--frame->counter){
			nonemptyFrames.remove(frame);
			Allocator::free(frame);
		}else {
			emptyFrames.remove(frame);
			nonemptyFrames.add(frame);
		}
	}

	/**
	 * Release the frames.
	 *
	 * Frees the used memory resources.
	 *
	 * @note	Does not call the destructors.
	 */
	~DynamicPool(){
		for(auto it = nonemptyFrames.iterator(); it.current();){
			Frame* current = it.current();
			Allocator::free(current);
			it.step();
		}

		for(auto it = emptyFrames.iterator(); it.current();){
			Frame* current = it.current();
			Allocator::free(current);
			it.step();
		}
	}
};

}

#endif /* POOL_H_ */
