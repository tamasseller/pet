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

#ifndef MOCK_H_
#define MOCK_H_

#include <stdint.h>

#include "TestRunner.h"

#include "Macro.h"

#ifndef EXPECT_POOL_SIZE

/**
 * Conservative default value for small systems (512 words -> 2kB ram)
 *
 * This is intended to be overridden by compiler CLI macro definition.
 */
#define EXPECT_POOL_SIZE    512
#endif

namespace pet {

/**
 * Function that calculates the java String hash of a C string.
 */
constexpr uintptr_t javaHash(const char* str = "") {
    return *str ? (*str + 31u * javaHash(str + 1)) : 1;
}

/**
 * Function that calculates the java String hash over a range of bytes.
 */
constexpr uintptr_t javaHash(const char* data, uintptr_t length) {
    return length ? (*data + 31 * javaHash(data + 1, length - 1)) : 1;
}

/**
 * Circular buffer of mock expectations.
 *
 * The hashes of the expected values are stored, both for the method
 * calls and also the expected values, thus on mismatch only the
 * existence of the error can be identified but not the cause of the
 * problem.
 */
template<uint16_t size>
class ExpectPool {
        template<uintptr_t hash>
        friend class Mock;

        static_assert(size <= 32767, "Pool size too big (max 32767)");

        template<class>
        friend class TestBase;

        /**
         * Data storage area
         */
        static uintptr_t pool[EXPECT_POOL_SIZE];

        /**
         * Reader index into the data, used by _call_.
         */
        static uint16_t readIdx;

        /**
         * Reader index into the data, used by _expect_.
         */
        static uint16_t writeIdx;

        /**
         * Write a single expectation hash.
         *
         * @return False if there is no free space to store the value.
         */
        inline static bool expect(uintptr_t value);

        /**
         * Read and check the next element.
         *
         * @return False if the next element is not matching or if there is no element to read.
         */
        inline static bool call(uintptr_t value);

        /**
         * Resets the buffer.
         */
        inline static void clear();

public:
        /**
         * Triggers a test failure if the buffer is not empty.
         */
        inline static void check();
};

/**
 * Facade incorporating the EXPECT_POOL_SIZE macro that can be set from compiler CLI.
 */
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
    if (writeIdx != (readIdx + size) % (2 * size))
    {
        pool[writeIdx % size] = value;
        writeIdx = static_cast<uint16_t>((writeIdx + 1) % (2 * size));
        return true;
    }
    else
    {
        return false;
    }
}

template<uint16_t size>
inline bool ExpectPool<size>::call(uintptr_t value)
{
    if (readIdx != writeIdx)
    {
        bool ret = pool[readIdx % size] == value;
        readIdx = static_cast<uint16_t>((readIdx + 1) % (2 * size));
        return ret;
    }
    else
    {
        return false;
    }
}

template<uint16_t size>
inline void ExpectPool<size>::check()
{
    if(readIdx != writeIdx)
    {
        TestRunner::failTest("--- Finalization ---", "Mock - unmet expectations");
    }
}

template<uint16_t size>
inline void ExpectPool<size>::clear()
{
    readIdx = writeIdx = 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Helper class that is used to chain call event and parameter expectations or actual calls.
 *
 * An instance of it is returned by the _expect_ and _call_ methods of the _Mock_ class,
 * and also its own members, to allow decorator style call chaining.
 *
 * It also passes along error information. If there is an error the later elements of the
 * chain are not executed, the destructor triggers the test failure.
 */
class MockDecorator {
        /**
         * Error string to be passed along the chain.
         *
         * Null means no error.
         */
        const char* error = nullptr;

        /**
         * Source information to be passed with error.
         *
         * Valid only if error is non-null.
         */
        const char* srcInfo;

        /**
         * Worker method to be used by the withParam methods.
         *
         * This indirection allows for using the Mock decorator for both the
         * _expect_ and _call_ functionality. This value should either be
         * set to _MockExpectPool::call_ or _MockExpectPool::expect_, both of
         * which exhibit the same semantics of accepting a single integer and
         * returning true on success.
         */
        typedef bool (*Output)(uintptr_t value);
        Output output;

        /**
         * Function used for reporting failure.
         *
         * Can be set to TestRunner::failTest to only fail on non-synthetic
         * tests or to TestRunner::failTestAlways to always report failures.
         */
        void (*fail)(const char* sourceInfo, const char* text);

        template<uintptr_t hash> friend class Mock;

        inline MockDecorator(Output output, const char* srcInfo, decltype(fail) fail) : error(nullptr), srcInfo(srcInfo), output(output), fail(fail) {}
        inline MockDecorator(const MockDecorator& other) = default;

        /**
         * Move constructor that takes over the error reference.
         *
         * This is required so that only the last destructor will report the error.
         */
        inline MockDecorator(MockDecorator&& other) : error(other.error), srcInfo(other.srcInfo), output(other.output), fail(other.fail) {
            other.error = nullptr;
        }

    public:
        /**
         * Add an expected/actual integral parameter to the chain initiated by the _Mock::call_ or _Mock::expect_.
         *
         * @return Returns a copy of the current object, for further chaining of expectations/calls.
         */
        template<class T>
        inline MockDecorator withParam(T param)
        {
            if (!error && !output(param))
            {
                error = "Mock - parameter error";
            }

            return *this;
        }

        /**
         * Add an expected/actual pointer parameter to the chain initiated by the _Mock::call_ or _Mock::expect_.
         *
         * @return Returns a copy of the current object, for further chaining of expectations/calls.
         *
         * @NOTE The pointer is checked not the content.
         */
        template<class T>
        inline MockDecorator withParam(T* param)
        {
            if (!error && !output(reinterpret_cast<uintptr_t>(param)))
            {
                error = "Mock - pointer parameter error";
            }

            return *this;
        }

        /**
         * Add an expected/actual proper c-string parameter to the chain initiated by the _Mock::call_ or _Mock::expect_.
         *
         * @return Returns a copy of the current object, for further chaining of expectations/calls.
         *
         * @NOTE The contents of the string are checked not the pointer itself.
         */
        inline MockDecorator withStringParam(const char* param)
        {
            if (!error && !output(javaHash(param)))
            {
                error = "Mock - string parameter error";
            }

            return *this;
        }

        /**
         * Add an expected/actual (almost) c-string parameter to the chain initiated by the _Mock::call_ or _Mock::expect_.
         *
         * @return Returns a copy of the current object, for further chaining of expectations/calls.
         *
         * @NOTE The contents of the string are checked not the pointer itself.
         */
        inline MockDecorator withStringParam(char* param)
        {
            if (!error && !output(javaHash(param)))
            {
                error = "Mock - string parameter error";
            }

            return *this;
        }

        /**
         * Add an expected/actual parameter to the chain initiated by the _Mock::call_ or _Mock::expect_.
         *
         * @return Returns a copy of the current object, for further chaining of expectations/calls.
         */
        inline MockDecorator withParam(const void* param, uintptr_t length)
        {
            if (!error && !output(javaHash(static_cast<const char*>(param), length)))
            {
                error = "Mock - data block parameter error";
            }

            return *this;
        }

        /**
         * Chain terminator.
         *
         * Triggers test failure if there is an error set in the chain.
         */
        inline ~MockDecorator()
        {
            if (error)
            {
                (*fail)(srcInfo, error);
            }
        }
};

/**
 * Mock system front-end class for a single filter category.
 *
 * This class is used to selectively enable and disable the mock category
 * and to initiate chains of an expected or actual call and its parameters.
 *
 * The _hash_ parameter identifies the category by the hash of its name.
 *
 * Chained decorator calls in the form:
 *
 *      Mock<categoryHash>::expect("fnName", "srcLine").withParam(xyz)...
 *      Mock<categoryHash>::call("fnName", "srcLine").withParam(xyz)...
 *
 * Are translated into the expectation buffer in the following way:
 *
 *  - For each chain, the category hash is the first value to be put/checked.
 *  - It is followed by the hash of the functions name.
 *  - Then one hash value for each parameter.
 *
 * Mock category filtering is achieved by using a no-op output method for the
 * returned decorator, thus no modifications are made to the buffer, but other
 * than that, everything is executed as for a non disabled categories.
 */
template<uintptr_t hash>
class Mock {
        /**
         * Static pointer to the current test if the current mock category is disabled.
         */
        static TestInterface* disabledForTest;

        typedef ExpectPool<EXPECT_POOL_SIZE> Pool;

        /**
         * Output provider used for disabled categories.
         */
        inline static bool nopOutput(uintptr_t value);

        public:

        /**
         * Start an expectation chain.
         *
         * Writes the category hash followed by the hash of _str_ into the buffer.
         * The returned decorator can then be used for writing the expected parameters.
         *
         * Returns a _MockDecorator_ carrying the error if there is any.
         */
        inline static MockDecorator expect(const char* str, const char* srcInfo);

        /**
         * Start an actual call chain.
         *
         * Reads two values from the buffer, the first is check against the category hash,
         * and the second is expected to be the hash of _str_. If any of these fails an
         * error is triggered.
         *
         * The returned decorator can then be used for checking the expected parameters.
         *
         * Returns a _MockDecorator_ carrying the error if there is any.
         */
        inline static MockDecorator call(const char* str, const char* srcInfo, void (*fail)(const char* sourceInfo, const char* text) = &pet::TestRunner::failTest);

        /**
         * Enable this mock category.
         *
         * Flag that this mock category is to be checked for the rest of the current test.
         *
         * @NOTE A new test always has all categories enabled, thus it
         *       is not required to call this during test tear-down.
         */
        inline static void enable();

        /**
         * Disable this mock category.
         *
         * Flag that this mock category is to be filtered for the rest of the current test.
         *
         * @NOTE This only affects the current test, a new test always has all categories enabled.
         */
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
    /*
     * Use no-op output method for disabled test,
     * no modification are made to the buffer.
     */
    if(disabledForTest == TestRunner::getCurrentTest())
        return MockDecorator(&nopOutput, srcInfo, pet::TestRunner::failTestAlways);

    /*
     * Decorator _withParam_ methods shall use the
     * _ExpectPool::expect_ method for writing the hashes.
     */
    MockDecorator ret(&MockExpectPool::expect, srcInfo, pet::TestRunner::failTestAlways);

    /*
     * Write the category hash first.
     */
    if (!Pool::expect(hash))
        ret.error = "Mock - pool is full";

    /*
     * Then write the function name hash.
     */
    if (!Pool::expect(javaHash(str)))
        ret.error = "Mock - pool is full";

    return ret;
}

template<uintptr_t hash>
inline MockDecorator Mock<hash>::call(const char* str, const char* srcInfo, void (*fail)(const char* sourceInfo, const char* text))
{
    /*
     * Use no-op output method for disabled test,
     * no actual accesses are made to the buffer.
     */
    if(disabledForTest == TestRunner::getCurrentTest())
        return MockDecorator(&nopOutput, srcInfo, fail);

    /*
     * Decorator _withParam_ methods shall use the
     * _ExpectPool::call_ method for checking the hashes.
     */
    MockDecorator ret(&MockExpectPool::call, srcInfo, fail);

    /*
     * Check the category hash first.
     */
    if (!Pool::call(hash))
        ret.error = "Mock - unexpected call";

    /*
     * Then if category is OK, check the function name hash.
     */
    else if (!Pool::call(javaHash(str)))
        ret.error = "Mock - unexpected call";

    return ret;
}

template<uintptr_t hash>
inline void Mock<hash>::enable() {
    /*
     * Set the static filter test pointer to null
     * so that it won't match any valid test.
     */
    disabledForTest = nullptr;
}

template<uintptr_t hash>
inline void Mock<hash>::disable() {
    /*
     * Set the static filter test pointer to the current test
     * so that _Mock::expect_ and _Mock::call_ will see that
     * they have nothing to do with this test.
     */
    disabledForTest = TestRunner::getCurrentTest();
}

}

/// Mock category class access facade (see _Mock_ for details).
#define MOCK(name) pet::Mock<pet::javaHash(#name)>

/// Mock _expect_ method facade for source location injection (see _Mock::expect_ for details).
#define EXPECT(name) expect(#name, INTERNAL_AT())

/// Mock _call_ method facade for source location injection (see _Mock::expect_ for details).
#define CALL(name) call(#name, INTERNAL_AT(), &pet::TestRunner::failTest)

/// Mock _call_ method facade for source location injection (see _Mock::expect_ for details).
#define CALL_ALWAYS(name) call(#name, INTERNAL_AT(), &pet::TestRunner::failTestAlways)

/// Mock _expect_ method facade for source location injection (see _Mock::expect_ for details).
#define EXPECTNC(name) expect(name, INTERNAL_AT())

/// Mock _call_ method facade for source location injection (see _Mock::expect_ for details).
#define CALLNC(name) call(name, INTERNAL_AT(), pet::TestRunner::failTest)

#endif /* MOCK_H_ */
