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

#ifndef STACK_H_
#define STACK_H_

#include "ubiquitous/Error.h"

#undef new

namespace mm {

/**
 * Dynamic, framed stack like storage (immutable part).
 *
 * Elements can be allocated and released at the end of the storage,
 * uses a framed internal storage, when the current frame gets full
 * during an allocation, it allocates a new one from the specified
 * Allocator. If the current frame gets empty releases it to the
 * underlying Allocator.
 *
 * The main purpose of this storage is to support conversion of
 * naturally recursive algorithms into iterative ones, where the
 * first-in-last-out behavior is necessary.
 *
 * Provides read-only iterators to access the stored data, and some
 * convenience methods, to simplify loop exit conditions on readout.
 *
 * @note	This is a base class intended to used internally only.
 * 			It lacks mutators as these would require a memory
 * 			management policy that is incorporated only in the
 * 			next level.
 */
template<class Element, unsigned int frameSize>
class ImmutableDynamicStack {
protected:
	/** A storage frame */
	struct Frame {
		Frame *prev;				//!< Previous frame of the stack.
		Element data[frameSize];	//!< Stored elements of this frame.
		Frame();					//!< No previous frame constructor.
	};

	int pos = 0;					//!< The index of the current element on the current frame.
	Frame *currFrame = 0;			//!< The current frame.
public:
	/**
	 * Is there more data?
	 *
	 * Convenience method for loop exit conditions.
	 *
	 * @return	True if there is more data to be popped.
	 */
	inline bool hasMore() const;

	/**
	 * The current data element.
	 *
	 * Accesses the current top of stack.
	 *
	 * @return	The current top of stack or NULL if empty.
	 */
	inline Element* current() const;

	/** Empty stack constructor */
	ImmutableDynamicStack() = default;

	/**
	 * Disable copy constructor
	 *
	 * It is disabled because the copy operation can trigger an
	 * out of memory event that needs to be reported and as
	 * exceptions are out of question the error has to be returned.
	 *
	 * @note	Use the copyFrom method instead.
	 */
	ImmutableDynamicStack(const ImmutableDynamicStack&) = delete;

	/**
	 * First-in-last-out iterator.
	 *
	 * Traverses the elements in the same order as if they were
	 * popped one after the other.
	 *
 	 * @note 	Modifying the stack while iterating through it with an iterator results in undefined behavior.
	 * 			This condition is not checked, because there is no zero-overhead way to do it, so avoiding
	 * 			this usage is entirely up to the user.
	 *
	 * @warning	Modifying the list while iterating through it with an iterator results in undefined behavior.
	 * 			This condition is not checked, because there is no zero-overhead way to do it, so avoiding
	 * 			this usage is entirely up to the user.
	 */
	class Iterator {
	public:
		/**
		 * Current element.
		 *
		 * Gets the iteration state.
		 *
		 * @return The current element the iterator stands at or NULL if over the end.
		 */
		inline Element* current() const;

		/**
		 * Progress one step.
		 *
		 * Steps the iterator towards to bottom of the stack or does nothind if already finished.
		 */
		inline void step();
	private:
		friend ImmutableDynamicStack;
		inline Iterator(Frame *n, int p);

		Frame *curr;
		int pos;
	};

	/**
	 * Get a FILO iterator.
	 *
	 * Obtain an Iterator that is initialized at the current top of stack.
	 *
	 * @return	An iterator standing at TOS.
	 */
	inline Iterator iterator() const;

	/**
	 * First-in-first-out iterator.
	 *
	 * Traverses the elements in the order they were pushed.
	 *
	 * @note 	Modifying the stack while iterating through it with an iterator results in undefined behavior.
  	 *			This condition is not checked, because there is no zero-overhead way to do it, so avoiding
  	 *			this usage is entirely up to the user.
  	 *
	 * @warning	Modifying the list while iterating through it with an iterator results in undefined behavior.
	 * 			This condition is not checked, because there is no zero-overhead way to do it, so avoiding
	 * 			this usage is entirely up to the user.
	 */
	class BottomUpIterator {
	public:
		/** @copydoc Iterator::current() */
		inline Element* current() const;

		/**
		 * @copybrief Iterator::current()
		 *
		 * Steps the iterator towards to top of the stack or does nothing if already finished.
		 *
		 * @param	parent The stack that it iterates over. It is required as it has to search over
		 * 			from the end to find the next frame, because frames are linked only in
		 * 			the other direction.
		 */
		inline void step(const ImmutableDynamicStack &parent);
	private:
		friend ImmutableDynamicStack;
		inline BottomUpIterator(Frame *n);

		inline Frame *findNext(Frame *start, Frame *which);
		Frame *curr;
		unsigned int pos;
	};

	/**
	 * Get a FIFO iterator.
	 *
	 * Obtain an Iterator that is initialized at the current top of stack.
	 *
	 * @return	An iterator standing at TOS.
	 */
	inline BottomUpIterator bottomUpiterator() const;
};

/**
 * Dynamic, framed stack like, mutable storage.
 *
 * @copydetails mm::ImmutableDynamicStack
 *
 * @note	This is the class that is supposed to be used by the end user.
 */
template<class Element, class Allocator, unsigned int frameSize>
class DynamicStack: public ImmutableDynamicStack<Element, frameSize> {
	using Base = ImmutableDynamicStack<Element, frameSize>;
	using Frame = typename Base::Frame;
	inline void cleanup();
public:
	/** @copydoc DyanmicStackBase::DyanmicStackBase() */
	DynamicStack() = default;

	/**
	 * Standard move constructor.
	 *
	 * Takes over the content of the parameter, leaving it empty.
	 *
	 * @param	old The stack whose data is to be claimed.
	 *
	 * @see		Same as moveFrom().
	 */
	inline DynamicStack(DynamicStack&& old);

	/**
	 * Standard destructor.
	 *
	 * Frees up all of the frames. Using a pointer returned by any of the
	 * methods or the iterators is an error after the stack has been destroyed.
	 *
	 * @see	Same as clear().
	 */
	inline ~DynamicStack();

	/**
	 * Copy another.
	 *
	 * Makes a copy of all the data contined in the other stack.
	 * It can fail as it needs to allocate new memory for the copies.
	 *
	 * @param	other The stack whose content is to be copied.
	 * @return	The new top of stack or NULL on allocation error.
	 */
	inline ubiq::FailPointer<Element> copyFrom(const DynamicStack& other);

	/**
	 * Destroy current content and take over another.
	 *
	 * Frees up all currently used frames and moves the others content,
	 * leaves the other empty.
	 *
	 * @param	other The stack object to be taken over.
	 */
	inline void moveFrom(DynamicStack& other);

	/**
	 * Zero-copy push.
	 *
	 * To implement zero copy behavior one needs to acquire an element on
	 * the stack, and fill the slot through the returned pointer.
	 *
	 * @return	The new slot acquired or NULL on allocation failure.
	 */
	inline ubiq::FailPointer<Element> acquire();

	/**
	 * Zero-copy pop.
	 *
	 * To implement zero copy behavior one needs to access the top of stack
	 * through the current method and then call the release method.
	 *
	 * @return	Returns true if there are more data to be popped after the release.
	 */
	inline bool release();

	/**
	 * Drop content.
	 *
	 * Releases all of the frames allocated, leaves the stack empty.
	 */
	inline void clear();
};

//////////////////////////////////////////////////////////////////////////

template<class Element, unsigned int frameSize>
inline ImmutableDynamicStack<Element, frameSize>::Frame::Frame():prev(0){}


template<class Element, unsigned int frameSize>
inline bool ImmutableDynamicStack<Element, frameSize>::hasMore() const
{
	if(!this->currFrame)
		return false;

	if(this->pos == 0)
		return this->currFrame->prev != 0;

	return true;
}

template<class Element, unsigned int frameSize>
inline Element* ImmutableDynamicStack<Element, frameSize>::current() const {
	if(currFrame)
		return currFrame->data+pos;

	return 0;
}

template<class Element, unsigned int frameSize>
inline Element* ImmutableDynamicStack<Element, frameSize>::Iterator::current() const {
	if(this->curr){
		return this->curr->data+this->pos;
	}

	return 0;
}

template<class Element, unsigned int frameSize>
inline void ImmutableDynamicStack<Element, frameSize>::Iterator::step() {
	if(!this->curr)
		return;
	if(pos)
		this->pos--;
	else{
		this->pos = frameSize-1;
		this->curr = this->curr->prev;
	}
}

template<class Element, unsigned int frameSize>
inline ImmutableDynamicStack<Element, frameSize>::Iterator::Iterator(Frame *n, int p): curr(n), pos(p){}

template<class Element, unsigned int frameSize>
inline typename ImmutableDynamicStack<Element, frameSize>::Iterator
ImmutableDynamicStack<Element, frameSize>::iterator() const{
	return Iterator(currFrame, pos);
}

template<class Element, unsigned int frameSize>
inline typename ImmutableDynamicStack<Element, frameSize>::Frame*
ImmutableDynamicStack<Element, frameSize>::BottomUpIterator::findNext(Frame* frame, Frame* needle)
{
	while(frame && frame->prev != needle)
		frame = frame->prev;

	return frame;
}

template<class Element, unsigned int frameSize>
inline Element* ImmutableDynamicStack<Element, frameSize>::BottomUpIterator::current() const
{
	if(this->curr){
		return this->curr->data+this->pos;
	}

	return 0;
}

template<class Element, unsigned int frameSize>
inline void ImmutableDynamicStack<Element, frameSize>::BottomUpIterator::step(const ImmutableDynamicStack &parent)
{
	const unsigned int limit = (curr == parent.currFrame)?parent.pos:(frameSize-1);

	if(this->pos < limit) {
		this->pos++;
	} else {
		this->pos = 0;
		this->curr = BottomUpIterator::findNext(parent.currFrame, this->curr);
	}
}

template<class Element, unsigned int frameSize>
inline ImmutableDynamicStack<Element, frameSize>::BottomUpIterator::BottomUpIterator(Frame *n): pos(0) {
	curr = BottomUpIterator::findNext(n, 0);
}

template<class Element, unsigned int frameSize>
inline typename ImmutableDynamicStack<Element, frameSize>::BottomUpIterator ImmutableDynamicStack<Element, frameSize>::bottomUpiterator() const
{
	return BottomUpIterator(currFrame);
}

template<class Element, class Allocator, unsigned int frameSize>
inline ubiq::FailPointer<Element> DynamicStack<Element, Allocator, frameSize>::acquire(){
	if((!Base::currFrame) || (Base::pos == (frameSize-1))){
		Frame *newFrame = (Frame *)Allocator::alloc(sizeof(Frame));

		if(!newFrame)
			return 0;

		newFrame->prev = Base::currFrame;
		Base::currFrame = newFrame;
		Base::pos = 0;
	}else
		Base::pos++;

	return Base::currFrame->data+Base::pos;
}

template<class Element, class Allocator, unsigned int frameSize>
inline bool DynamicStack<Element, Allocator, frameSize>::release()
{
	if(!Base::currFrame)
		return false;

	if(Base::pos == 0){
		Frame *oldFrame = Base::currFrame->prev;
		Allocator::free(Base::currFrame);
		Base::currFrame = oldFrame;
		Base::pos = frameSize-1;
		return oldFrame != 0;
	}else
		Base::pos--;

	return true;
}

template<class Element, class Allocator, unsigned int frameSize>
inline void DynamicStack<Element, Allocator, frameSize>::cleanup() {
	for(Frame *f = Base::currFrame; f;) {
		Frame* prev = f->prev;
		Allocator::free(f);
		f = prev;
	}
};

template<class Element, class Allocator, unsigned int frameSize>
inline void DynamicStack<Element, Allocator, frameSize>::clear(){
	cleanup();
	Base::pos = 0;
	Base::currFrame = 0;
}


template<class Element, class Allocator, unsigned int frameSize>
inline DynamicStack<Element, Allocator, frameSize>::~DynamicStack(){
	cleanup();
}

template<class Element, class Allocator, unsigned int frameSize>
inline ubiq::FailPointer<Element> DynamicStack<Element, Allocator, frameSize>::copyFrom(const DynamicStack& other) {
	cleanup();
	Base::pos = other.pos;
	Base::currFrame = other.currFrame;

	for(Frame **place = &this->Base::currFrame; *place;) {
		void *newArea = Allocator::alloc(sizeof(Frame));

		if(!newArea) {
			*place = 0;
			clear();
			return 0;
		}

		*place = new(newArea) Frame(**place);
		place = &((*place)->prev);
	}

	return Base::current();
}

template<class Element, class Allocator, unsigned int frameSize>
inline void DynamicStack<Element, Allocator, frameSize>::moveFrom(DynamicStack& other) {
	cleanup();
	Base::pos = other.pos;
	Base::currFrame = other.currFrame;

	other.pos = 0;
	other.currFrame = 0;
}

template<class Element, class Allocator, unsigned int frameSize>
inline DynamicStack<Element, Allocator, frameSize>::DynamicStack(DynamicStack&& old) {
	moveFrom(old);
}

}

#endif /* STACK_H_ */
