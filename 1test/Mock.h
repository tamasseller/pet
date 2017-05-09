/*
 * Mock.h
 *
 *      Author: tamas.seller
 */

#ifndef MOCK_H_
#define MOCK_H_

#include <stdint.h>

#define EXPECT_POOL_SIZE    64

constexpr uintptr_t javaHash(const char* str = "") {
    return *str ? (*str + 31 * javaHash(str + 1)) : 1;
}

constexpr uintptr_t javaHash(const char* data, uintptr_t length) {
    return length ? (*data + 31 * javaHash(data + 1, length - 1)) : 1;
}


template <uint16_t size>
class ExpectPool {
    template <uintptr_t hash>
    friend class Mock;

    static uintptr_t pool[EXPECT_POOL_SIZE];
    static uint16_t writeIdx, readIdx;

    inline static bool expect(uintptr_t value);
    inline static bool add(uintptr_t value);
    inline static bool check();
    inline static void clear();
};

template <uint16_t size>
uintptr_t ExpectPool<size>::pool[EXPECT_POOL_SIZE];

template <uint16_t size>
uint16_t ExpectPool<size>::readIdx = 0;

template <uint16_t size>
uint16_t ExpectPool<size>::writeIdx = 0;

template <uint16_t size>
inline bool ExpectPool<size>::expect(uintptr_t value)
{
    if(readIdx < size) {
        pool[readIdx++] = value;
        return true;
    } else
        return false;
}

template <uint16_t size>
inline bool ExpectPool<size>::add(uintptr_t value)
{
    if(writeIdx < readIdx) {
        return pool[writeIdx++] == value;
    } else
        return false;
}

template <uint16_t size>
inline bool ExpectPool<size>::check()
{
    bool ret = writeIdx == readIdx;
    clear();
    return ret;
}

template <uint16_t size>
inline void ExpectPool<size>::clear()
{
    writeIdx = readIdx = 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////

template<bool (*output)(uintptr_t value)>
class MockDecorator {
        bool ok;

        template <uintptr_t hash>
        friend class Mock;

        inline MockDecorator(const bool &ok): ok(ok) {}
        inline MockDecorator(const MockDecorator& other): ok(other.ok) {}
    public:
        inline operator bool() {
            return ok;
        }

        template<class T>
        inline MockDecorator withParam(T param) {
            if(!ok)
                return false;
            return output(param);
        }

        template<class T>
        inline MockDecorator withParam(T* param) {
            if(!ok)
                return false;
            return output(reinterpret_cast<uintptr_t>(param));
        }

        inline MockDecorator withParam(const char* param) {
            if(!ok)
                return false;
            return output(javaHash(param));
        }

        inline MockDecorator withParam(char* param) {
            if(!ok)
                return false;
            return output(javaHash(param));
        }

        inline MockDecorator withParam(void* param, uintptr_t length) {
            if(!ok)
                return false;
            return output(javaHash(static_cast<const char*>(param), length));
        }
};

template <uintptr_t hash>
class Mock {
        typedef ExpectPool<EXPECT_POOL_SIZE> Pool;
    public:
        typedef MockDecorator<&Pool::expect> ExpectDecorator;
        typedef MockDecorator<&Pool::add> CallDecorator;

        inline static ExpectDecorator expect(const char* str);
        inline static CallDecorator call(const char* str);
        inline static bool check();
        inline static void clear();
};

template <uintptr_t hash>
inline typename Mock<hash>::ExpectDecorator Mock<hash>::expect(const char* str)
{
    if(!Pool::expect(hash))
        return false;

    if(!Pool::expect(javaHash(str)))
        return false;

    return true;
}

template <uintptr_t hash>
inline typename Mock<hash>::CallDecorator Mock<hash>::call(const char* str)
{
    if(!Pool::add(hash))
        return false;

    if(!Pool::add(javaHash(str)))
        return false;

    return true;
}

template <uintptr_t hash>
inline bool Mock<hash>::check()
{
    return Pool::check();
}

template <uintptr_t hash>
inline void Mock<hash>::clear()
{
    return Pool::clear();
}


#define MOCK(name) Mock<javaHash(#name)>

#endif /* MOCK_H_ */
