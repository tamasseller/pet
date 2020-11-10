/*******************************************************************************
 *
 * Copyright (c) 2020 Tamás Seller. All rights reserved.
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

#include "platform/Atomic.h"

#include "1test/Test.h"
#include "1test/Mock.h"

#include "meta/Utility.h"

TEST_GROUP(Atomic) {
};

TEST(Atomic, SanityNonAtomicAccess)
{
    pet::Atomic<uintptr_t> a;

    a = 123;

    CHECK(a == 123);
}

TEST(Atomic, AtomicSet)
{
    pet::Atomic<uintptr_t> a(123);

    CHECK(123 == a([](uintptr_t o, uintptr_t &n){ n = 234; return true; }));

    CHECK(a == 234);
}

TEST(Atomic, AtomicSetDenied)
{
    pet::Atomic<uintptr_t> a(123);

    CHECK(123 == a([](uintptr_t o, uintptr_t &n){ n = 234; return false; }));

    CHECK(a == 123);
}

TEST(Atomic, AtomicSquare)
{
    pet::Atomic<uintptr_t> a(5);

    CHECK(5 == a([](uintptr_t o, uintptr_t &n){ n = o * o; return true; }));

    CHECK(a == 25);
}

TEST(Atomic, ImmobileCapture)
{
    struct Immobile {
        Immobile() { MOCK(Atomic)::CALL(ImmobileCaptureConstruct); }
        Immobile(const Immobile&) = delete;
        Immobile(Immobile&&) = delete;
        inline ~Immobile() { MOCK(Atomic)::CALL(ImmobileCaptureDestroy); }

        inline bool operator ()(uintptr_t o, uintptr_t &n)
        {
            n = o + 1;
            return true;
        }
    };

    MOCK(Atomic)::EXPECT(ImmobileCaptureConstruct);
    Immobile immo;

    pet::Atomic<uintptr_t> a(1);

    CHECK(1 == a(pet::move(immo)));

    CHECK(a == 2);

    MOCK(Atomic)::EXPECT(ImmobileCaptureDestroy);
}
