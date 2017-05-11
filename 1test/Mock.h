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

#ifndef MOCK_H_
#define MOCK_H_

#include <stdint.h>

#include "TestRunner.h"

#include "Macro.h"

#ifndef EXPECT_POOL_SIZE
#define EXPECT_POOL_SIZE    512
#endif

namespace pet {

constexpr uintptr_t javaHash(const char* str = "") {
    return *str ? (*str + 31 * javaHash(str + 1)) : 1;
}

constexpr uintptr_t javaHash(const char* data, uintptr_t length) {
    return length ? (*data + 31 * javaHash(data + 1, length - 1)) : 1;
}

template<uint16_t size>
class ExpectPool {
        template<uintptr_t hash>
        friend class Mock;

        template<class >
        friend class TestBase;

        static uintptr_t pool[EXPECT_POOL_SIZE];
        static uint16_t readIdx, writeIdx;

        inline static bool expect(uintptr_t value);
        inline static bool call(uintptr_t value);

        inline static void check();
        inline static void clear();
};

typedef ExpectPool<EXPECT_POOL_SIZE> MockExpectPool;

template<uint16_t size>
uintptr_t ExpectPool<size>::pool[EXPECT_POOL_SIZE];

template<uint16_t size>
uint16_t ExpectPool<size>::writeIdx = 0;

template<uint16_t size>
uint16_t ExpectPool<size>::readIdx = 0;

template<uint16_t size>
inline bool ExpectPool<size>::expect(uintptr_t value)
{
    if (writeIdx < size) {
        pool[writeIdx++] = value;
        return true;
    } else
        return false;
}

template<uint16_t size>
inline bool ExpectPool<size>::call(uintptr_t value)
{
    if (readIdx < writeIdx) {
        return pool[readIdx++] == value;
    } else
        return false;
}

template<uint16_t size>
inline void ExpectPool<size>::check()
{
    if(readIdx != writeIdx)
        TestRunner::failTest("--- Finalization ---", "Mock - unmet expectations");
}

template<uint16_t size>
inline void ExpectPool<size>::clear()
{
    readIdx = writeIdx = 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////

class MockDecorator {
        const char* error = nullptr;
        const char* srcInfo;

        typedef bool (*Output)(uintptr_t value);
        Output output;

        template<uintptr_t hash>
        friend class Mock;

        inline MockDecorator(Output output, const char* srcInfo) : error(nullptr), srcInfo(srcInfo), output(output) {}
        inline MockDecorator(const MockDecorator& other) = default;
        inline MockDecorator(MockDecorator&& other) : error(other.error), srcInfo(other.srcInfo), output(other.output) {
            other.error = nullptr;
        }

    public:
        inline operator bool() {
            return error != nullptr;
        }

        template<class T>
        inline MockDecorator withParam(T param) {
            if (!error && !output(param))
                error = "Mock - parameter error";

            return *this;
        }

        template<class T>
        inline MockDecorator withParam(T* param) {
            if (!error && !output(reinterpret_cast<uintptr_t>(param)))
                error = "Mock - pointer parameter error";

            return *this;
        }

        inline MockDecorator withStringParam(const char* param) {
            if (!error && !output(javaHash(param)))
                error = "Mock - string parameter error";

            return *this;
        }

        inline MockDecorator withStringParam(char* param) {
            if (!error && !output(javaHash(param)))
                error = "Mock - string parameter error";

            return *this;
        }

        inline MockDecorator withParam(void* param, uintptr_t length) {
            if (!error && !output(javaHash(static_cast<const char*>(param), length)))
                error = "Mock - data block parameter error";

            return *this;
        }

        inline ~MockDecorator() {
            if (error)
                TestRunner::failTest(srcInfo, error);
        }
};

template<uintptr_t hash>
class Mock {
        static TestInterface* disabledForTest;
        typedef ExpectPool<EXPECT_POOL_SIZE> Pool;
        inline static bool nopOutput(uintptr_t value);
        public:

        inline static MockDecorator expect(const char* str, const char* srcInfo);
        inline static MockDecorator call(const char* str, const char* srcInfo);

        inline static void enable();
        inline static void disable();
};

template<uintptr_t hash>
TestInterface* Mock<hash>::disabledForTest = nullptr;


template<uintptr_t hash>
inline bool  Mock<hash>::nopOutput(uintptr_t value)
{
    return true;
}

template<uintptr_t hash>
inline MockDecorator Mock<hash>::expect(const char* str, const char* srcInfo)
{
    if(disabledForTest == TestRunner::getCurrentTest())
        return MockDecorator(&nopOutput, srcInfo);

    MockDecorator ret(&MockExpectPool::expect, srcInfo);

    if (!Pool::expect(hash))
        ret.error = "Mock - pool is full";

    if (!Pool::expect(javaHash(str)))
        ret.error = "Mock - pool is full";

    return ret;
}

template<uintptr_t hash>
inline MockDecorator Mock<hash>::call(const char* str, const char* srcInfo)
{
    if(disabledForTest == TestRunner::getCurrentTest())
        return MockDecorator(&nopOutput, srcInfo);

    MockDecorator ret(&MockExpectPool::call, srcInfo);

    if (!Pool::call(hash))
        ret.error = "Mock - unexpected call";

    else if (!Pool::call(javaHash(str)))
        ret.error = "Mock - unexpected call";

    return ret;
}

template<uintptr_t hash>
inline void Mock<hash>::enable() {
    disabledForTest = nullptr;
}

template<uintptr_t hash>
inline void Mock<hash>::disable() {
    disabledForTest = TestRunner::getCurrentTest();
}

}

#define MOCK(name) pet::Mock<pet::javaHash(#name)>
#define EXPECT(name) expect(#name, INTERNAL_AT())
#define CALL(name) call(#name, INTERNAL_AT())

#endif /* MOCK_H_ */
