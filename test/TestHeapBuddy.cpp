/*******************************************************************************
 *
 * Copyright (c) 2020 Seller Tamás. All rights reserved.
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

#include "heap/Buddy.h"

#include "1test/Test.h"

TEST_GROUP(Buddy)
{
    static constexpr auto size = 16;
    uint32_t mem[size];

    pet::BuddyAllocator<2, 2> uut;

    bool isUnique(void** ptrs, size_t n)
    {
        for(int i = 1; i <= n; i++)
        {
            for(int j = 0; j<j; j++)
            {
                if(ptrs[i] == ptrs[j])
                    return false;
            }
        }

        return true;
    }

    template<size_t N>
    bool isUnique(void* (&ptrs)[N]) {
        return isUnique(ptrs, N);
    }

    void steppingShuffle(int size)
    {
        void* ptrs[1024], **p = ptrs;

        do {
            *p = uut.allocate(size);
        } while(*p++);

        CHECK(isUnique(ptrs, p - ptrs));

        for(int n = 1; n < p - ptrs; n++)
        {
            CHECK(!uut.allocate(size));

            for(int i = 0; i < n; i++)
                uut.free(ptrs[i]);

            for(int i = 0; i < n; i++) {
                ptrs[i] = uut.allocate(size);
                CHECK(ptrs[i]);
            }

            CHECK(isUnique(ptrs));
        }
    }
};

TEST(Buddy, Abuse)
{
    CHECK(!uut.init(mem, mem));
    CHECK(!uut.init(mem, mem - size));

    CHECK(uut.init(mem, mem + size));
    uut.free(nullptr);

    CHECK(!uut.allocate(10000000));

    CHECK(uut.allocate(1));
}

TEST(Buddy, Sanity)
{
    CHECK(uut.init(mem, mem + size));

    auto x = uut.allocate(1);
    CHECK(x);

    uut.free(x);
}

TEST(Buddy, AllSmallsSteppingShuffle)
{
    CHECK(uut.init(mem, mem + size));
    steppingShuffle(1);
}

TEST(Buddy, AllBiggerSteppingShuffle)
{
    CHECK(uut.init(mem, mem + size));
    steppingShuffle(8);
}


TEST(Buddy, PassBigger)
{
    for(int small = 4; small < 8; small <<= 1)
    {
        for(int big = 8; big <16; big <<= 1)
        {
            CHECK(uut.init(mem, mem + size));
            CHECK(uut.allocate(small));
            CHECK(uut.allocate(big));
            CHECK(uut.allocate(small));
            CHECK(uut.allocate(small));
        }
    }
}

TEST(Buddy, RandomStress)
{
    void* ptrs[1024];
    int idx = 0;
    int a = 1, b = 1;

    CHECK(uut.init(mem, mem + size));

    for(int round = 0; round < 1000; round++)
    {
        while(true)
        {
            auto size = a;
            size = (a + b) % 31;
            b = a;
            a = size;

            CHECK(idx < sizeof(ptrs)/sizeof(*ptrs));

            if(auto x = uut.allocate(size))
                ptrs[idx++] = x;
            else
                break;
        }

        CHECK(isUnique(ptrs, idx));

        const auto nFree = idx / 2;
        for(int i = 0; i < nFree; i++)
            uut.free(ptrs[--idx]);
    }
}

TEST(Buddy, Grow)
{
    CHECK(uut.init(mem, mem + size));
    auto x = uut.allocate(4);
    auto y = uut.allocate(4);
    auto z = uut.allocate(4);
    CHECK(!uut.adjust(x, 8));
    CHECK(!uut.adjust(y, 8));
    CHECK(uut.adjust(z, 8));

    uut.free(y);
    CHECK(uut.adjust(x, 8));
    CHECK(!uut.adjust(x, 32));

    uut.free(z);
    CHECK(uut.adjust(x, 32));

    uut.free(x);

    CHECK(uut.allocate(32));
    CHECK(uut.allocate(16));
    CHECK(uut.allocate(8));
    CHECK(!uut.allocate(1));
}

TEST(Buddy, Shrink)
{
    CHECK(uut.init(mem, mem + size));
    auto x = uut.allocate(32);
    auto y = uut.allocate(16);
    auto z = uut.allocate(8);
    CHECK(!uut.allocate(1));

    CHECK(uut.adjust(x, 4));

    auto t = uut.allocate(4);
    auto u = uut.allocate(8);
    auto v = uut.allocate(16);
    CHECK(!uut.allocate(1));

    uut.free(x);
    uut.free(y);
    uut.free(z);
    uut.free(t);
    uut.free(u);
    uut.free(v);

    CHECK(uut.allocate(32));
    CHECK(uut.allocate(16));
    CHECK(uut.allocate(8));
    CHECK(!uut.allocate(1));
}

TEST(Buddy, GrowAndShrink)
{
    CHECK(uut.init(mem, mem + size));
    auto x = uut.allocate(16);
    CHECK(uut.adjust(x, 32));

    auto y = uut.allocate(8);

    CHECK(uut.adjust(x, 16));

    auto z = uut.allocate(16);

    CHECK(!uut.adjust(x, 32));
    uut.free(z);

    CHECK(uut.adjust(x, 32));

    uut.free(x);
    uut.free(y);
    CHECK(uut.allocate(32));
    CHECK(uut.allocate(16));
    CHECK(uut.allocate(8));
    CHECK(!uut.allocate(1));
}
