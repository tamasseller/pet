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

struct ReadLength {
	uint32_t length = 0;
};

struct WriteLength {
	uint32_t length = 0;
};

class CommonStreamState {
protected:
    char* buffer = 0;
    uint32_t idx = 0;
};

class InputStreamState:
	public CommonStreamState,
	public ReadLength {
	bool isDirty() const;
};

class OutputStreamState:
	public CommonStreamState,
	public WriteLength {
	bool isDirty() const;
	void setDirty(bool);
};

class IoStreamState:
	public CommonStreamState,
	public ReadLength,
	public WriteLength {
	bool isDirty() const;
	void setDirty(bool);
};

template<class Child, class State>
class InputStreamLogic {
    public:
        inline pet::GenericError read(void* buffer, uint32_t length);

        template<class T>
        inline pet::GenericError read(T& item);
};

template<class Child, class State>
class OutputStreamLogic {
    public:
        inline pet::GenericError write(const void* buffer, uint32_t length);

        template<class T>
        inline pet::GenericError write(const T& item);

        inline pet::GenericError flush();
};

template<class Child>
class OutputStream:
		protected OutputStreamState,
		public OutputStreamLogic<Child, OutputStreamState> {
	friend OutputStreamLogic<Child, OutputStreamState>;
};

template<class Child>
class InputStream:
		public InputStreamState,
		public InputStreamLogic<Child, InputStreamState> {
	friend InputStreamLogic<Child, InputStreamState>;
};

template<class Child>
class IoStream:
		protected IoStreamState,
		public InputStreamLogic<Child, IoStreamState>,
		public OutputStreamLogic<Child, IoStreamState> {
	friend InputStreamLogic<Child, IoStreamState>;
	friend OutputStreamLogic<Child, IoStreamState>;
};

//////////////////////////////////////////////////////////////////////

template<class Child, class State>
inline pet::GenericError InputStreamLogic<Child, State>::read(void* output, uint32_t toBeRead)
{
	Child* self = static_cast<Child*>(this);
    uint32_t done = 0;

    while(done < toBeRead) {
        if(self->State::idx == self->State::ReadLength::length || !self->State::buffer) {
            if(self->State::buffer) {
                pet::GenericError r = self->blockDone(
                		static_cast<ReadLength*>(self),
                		self->State::buffer,
                		self->State::idx);

                if(r.failed())
                    return r.rethrow();
            }

            if(!self->State::buffer) {
            	pet::GenericError r = self->nextBlock(
            			static_cast<ReadLength*>(self),
            			self->State::buffer,
            			toBeRead - done);

				if(r.failed())
					return r.rethrow();

            }

			if(self->State::idx == self->State::ReadLength::length)
				return done;
        }

        const uint32_t yetToBeRead = toBeRead - done;
        const uint32_t available = self->State::ReadLength::length - self->State::idx;

        uint32_t currentBurstSize = (yetToBeRead <= available) ? yetToBeRead : available;

        memcpy((char*)output + done, self->State::buffer + self->State::idx, currentBurstSize);

        self->State::idx += currentBurstSize;
        done += currentBurstSize;
    }

    if(self->State::idx == self->State::ReadLength::length && self->State::buffer) {
		pet::GenericError r = self->blockDone(
				static_cast<ReadLength*>(self),
				self->State::buffer,
				self->State::idx);

		if(r.failed())
			return r.rethrow();
    }

    return done;
}

template<class Child, class State>
template<class T>
inline pet::GenericError InputStreamLogic<Child, State>::read(T& item)
{
	Child* self = static_cast<Child*>(this);
    static constexpr const uint32_t size = sizeof(T);

    if(self->State::ReadLength::length - self->State::idx >= size) {
        memcpy(&item, self->State::buffer + self->State::idx, size);
        self->State::idx += size;
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

template<class Child, class State>
inline pet::GenericError OutputStreamLogic<Child, State>::write(const void* input, uint32_t toBeWritten)
{
	Child* self = static_cast<Child*>(this);
    uint32_t done = 0;

    while(done < toBeWritten) {
        if(self->State::idx == self->State::WriteLength::length || !self->State::buffer) {
            if(self->State::buffer) {
                pet::GenericError r = self->blockDone(
                		static_cast<WriteLength*>(self),
                		self->State::buffer,
                		self->State::idx);

                if(r.failed())
                    return r.rethrow();
            }

            if(!self->State::buffer) {
				pet::GenericError r = self->nextBlock(
						static_cast<WriteLength*>(self),
						self->State::buffer,
						toBeWritten - done);

				if(r.failed())
					return r.rethrow();
            }

            if(self->State::idx == self->State::WriteLength::length)
            	return done;
        }

        const uint32_t yetToBeWritten = toBeWritten - done;
        const uint32_t available = self->State::WriteLength::length - self->State::idx;

        uint32_t currentBurstSize = (yetToBeWritten <= available) ? yetToBeWritten : available;

        memcpy(self->State::buffer + self->State::idx, (char*)input + done, currentBurstSize);

        self->State::idx += currentBurstSize;
        done += currentBurstSize;
    }

    return done;
}

template<class Child, class State>
template<class T>
inline pet::GenericError OutputStreamLogic<Child, State>::write(const T& item)
{
	Child* self = static_cast<Child*>(this);
    static constexpr const uint32_t size = sizeof(T);

    if(self->State::WriteLength::length - self->State::idx >= size) {
        memcpy(self->State::buffer + self->State::idx, &item, size);
        self->State::idx += size;
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

template<class Child, class State>
inline pet::GenericError OutputStreamLogic<Child, State>::flush()
{
	Child* self = static_cast<Child*>(this);
    pet::GenericError r = self->blockDone(
    		static_cast<WriteLength*>(self),
    		self->State::buffer,
    		self->State::idx);

    if(r.failed())
        return r.rethrow();

    return 0;
}

#endif /* STREAM_H_ */
