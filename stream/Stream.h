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

#ifndef STREAM_H_
#define STREAM_H_

#include "ubiquitous/Error.h"

#include <stdint.h>
#include <string.h>

template<class Child>
class Stream;

class StreamBackend;

class StreamState {
	template<class Child>
	friend class Stream;

	friend StreamBackend;

    char* buffer = 0;
    uint16_t idx = 0;
    uint16_t readableLength = 0;
    uint16_t writableLength = 0;
    bool isDirty;

    inline uint16_t availableBytesForWriting();
    inline bool fullyWritten();

    inline uint16_t availableBytesForReading();
    inline bool fullyRead();

    inline bool isValid();
    inline char* accessAndAdvance(uint16_t length);
};

template<class Child>
class Stream: public StreamState {
    public:
        inline pet::GenericError read(void* buffer, uint32_t length);
        inline pet::GenericError write(const void* buffer, uint32_t length);
        inline pet::GenericError flush();

        template<class T>
        inline pet::GenericError read(T& item);

        template<class T>
        inline pet::GenericError write(const T& item);
};

class StreamBackend {
protected:
	template<class T, T StreamState::* member>
	static T& accessState(StreamState* subject) {
		return subject->*member;
	}
};

//////////////////////////////////////////////////////////////////////

inline uint16_t StreamState::availableBytesForWriting()
{
	return writableLength - idx;
}

inline bool StreamState::fullyWritten()
{
	return idx >= writableLength;
}

inline uint16_t StreamState::availableBytesForReading()
{
	return readableLength - idx;
}

inline bool StreamState::fullyRead()
{
	return idx >= readableLength;
}

inline bool StreamState::isValid()
{
	return buffer;
}

inline char* StreamState::accessAndAdvance(uint16_t length)
{
	char* ret = buffer + idx;
	idx += length;
	return ret;
}

template<class Child>
inline pet::GenericError Stream<Child>::read(void* output, uint32_t toBeRead)
{
	StreamState* state = static_cast<StreamState*>(this);
	Child* self = static_cast<Child*>(this);
    uint32_t done = 0;

    while(done < toBeRead) {
        const uint32_t yetToBeRead = toBeRead - done;

        if(!state->isValid() || state->fullyRead()) {
            if(state->isValid()) {
                pet::GenericError r = self->blockDone(state);

                if(r.failed())
                    return r.rethrow();
            }

            if(!state->isValid()) {
            	pet::GenericError r = self->nextBlock(state, yetToBeRead, false);

				if(r.failed())
					return r.rethrow();
            }

			if(state->fullyRead())
				return done;
        }

        const uint32_t available = state->availableBytesForReading();

        uint32_t currentBurstSize = (yetToBeRead <= available) ? yetToBeRead : available;

        memcpy((char*)output + done, state->accessAndAdvance(currentBurstSize), currentBurstSize);
        done += currentBurstSize;
    }

    if(state->fullyRead() && self->isValid()) {
		pet::GenericError r = self->blockDone(state);

		if(r.failed())
			return r.rethrow();
    }

    return done;
}

template<class Child>
template<class T>
inline pet::GenericError Stream<Child>::read(T& item)
{
	StreamState* state = static_cast<StreamState*>(this);
	Child* self = static_cast<Child*>(this);
    static constexpr const uint32_t size = sizeof(T);

    if(self->availableBytesForReading() >= size) {
        memcpy(&item, state->accessAndAdvance(size), size);
        return size;
    } else {
        pet::GenericError r = read(&item, size);

        if(r.failed())
            return r.rethrow();
        else if(r != size)
            return pet::GenericError::readError();
        else
            return (int)r;
    }
}

template<class Child>
inline pet::GenericError Stream<Child>::write(const void* input, uint32_t toBeWritten)
{
	StreamState* state = static_cast<StreamState*>(this);
	Child* self = static_cast<Child*>(this);
    uint32_t done = 0;

    while(done < toBeWritten) {
        const uint32_t yetToBeWritten = toBeWritten - done;

    	if(!state->isValid() || state->fullyWritten()) {
            if(state->isValid()) {
                pet::GenericError r = self->blockDone(state);

                if(r.failed())
                    return r.rethrow();
            }

            if(!state->isValid()) {
				pet::GenericError r = self->nextBlock( state, yetToBeWritten, true);

				if(r.failed())
					return r.rethrow();
            }

            if(state->fullyWritten())
            	return done;
        }

        const uint32_t available = state->availableBytesForWriting();

        uint32_t currentBurstSize = (yetToBeWritten <= available) ? yetToBeWritten : available;

        memcpy(state->accessAndAdvance(currentBurstSize), (char*)input + done, currentBurstSize);
        done += currentBurstSize;
    }

    return done;
}

template<class Child>
template<class T>
inline pet::GenericError Stream<Child>::write(const T& item)
{
	StreamState* state = static_cast<StreamState*>(this);
	Child* self = static_cast<Child*>(this);

    static constexpr const uint32_t size = sizeof(T);

    if(state->availableBytesForWriting() >= size) {
        memcpy(state->accessAndAdvance(size), &item, size);
        return size;
    } else {
        pet::GenericError r = write(&item, size);

        if(r.failed())
            return r.rethrow();
        else if(r != size)
            return pet::GenericError::writeError();
        else
            return (int)r;
    }
}

template<class Child>
inline pet::GenericError Stream<Child>::flush()
{
	StreamState* state = static_cast<StreamState*>(this);
	Child* self = static_cast<Child*>(this);

    pet::GenericError r = self->blockDone(state);

    if(r.failed())
        return r.rethrow();

    return 0;
}

#endif /* STREAM_H_ */
