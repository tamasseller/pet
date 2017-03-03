/*
 * Stream.h
 *
 *      Author: tamas.seller
 */

#ifndef STREAM_H_
#define STREAM_H_

#include "ubiquitous/Error.h"

#include <stdint.h>
#include <string.h>

class StreamBase {
protected:
    char* buffer = 0;
    uint32_t length = 0, idx = 0;
    inline uint32_t remaining() const;

};

template<class Child>
class InputStream: protected StreamBase {
    public:
        inline pet::GenericError read(void* buffer, uint32_t length);

        template<class T>
        inline pet::GenericError read(T& item);
};


template<class Child>
class OutputStream: protected StreamBase {
    public:
        inline pet::GenericError write(const void* buffer, uint32_t length);

        template<class T>
        inline pet::GenericError write(const T& item);

        inline pet::GenericError flush();
};

//////////////////////////////////////////////////////////////////////

template<class Child>
inline pet::GenericError InputStream<Child>::read(void* output, uint32_t toBeRead)
{
    uint32_t done = 0;

    while(done < toBeRead) {
        if(idx == length) {
            idx = 0;

            if(buffer) {
                pet::GenericError r = ((Child*)this)->doneReading(buffer, length);

                if(r.failed())
                    return r.rethrow();

            }

            pet::GenericError r = ((Child*)this)->nextReadable(buffer);

            if(r.failed())
                return r.rethrow();

            if(!(length = r))
                return done;
        }

        const uint32_t available = length - idx;

        uint32_t currentBurstSize = (toBeRead <= available) ? toBeRead : available;

        memcpy((char*)output + done, buffer + idx, currentBurstSize);

        idx += currentBurstSize;
        done += currentBurstSize;
    }

    return done;
}

template<class Child>
template<class T>
inline pet::GenericError InputStream<Child>::read(T& item)
{
    static constexpr const uint32_t size = sizeof(T);

    if(length - idx > size) {
        memcpy(&item, buffer + idx, size);
        idx += size;
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
inline pet::GenericError OutputStream<Child>::write(const void* input, uint32_t toBeWritten)
{
    uint32_t done = 0;

    while(done < toBeWritten) {
        if(idx == length || !buffer) {
            idx = 0;

            if(buffer) {
                pet::GenericError r = ((Child*)this)->doneWriting(buffer, length);

                if(r.failed())
                    return r.rethrow();
            }

            pet::GenericError r = ((Child*)this)->nextWritable(buffer);

            if(r.failed())
                return r.rethrow();

            if(!(length = r))
                return done;
        }

        const uint32_t available = length - idx;

        uint32_t currentBurstSize = (toBeWritten <= available) ? toBeWritten : available;

        memcpy(buffer + idx, (char*)input + done, currentBurstSize);

        idx += currentBurstSize;
        done += currentBurstSize;
    }

    return done;
}

template<class Child>
template<class T>
inline pet::GenericError OutputStream<Child>::write(const T& item)
{
    static constexpr const uint32_t size = sizeof(T);

    if(length - idx > size) {
        memcpy(buffer + idx, &item, size);
        idx += size;
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
inline pet::GenericError OutputStream<Child>::flush()
{
    pet::GenericError r = ((Child*)this)->doneWriting(buffer, idx);

    buffer = 0;

    if(r.failed())
        return r.rethrow();

    return 0;
}

#endif /* STREAM_H_ */
