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

    template<size_t N>
    bool isUnique(void* (&ptrs)[N])
    {
        for(int n = 1; n <= N; n++)
        {
            for(int i = 0; i<n; i++)
            {
                if(ptrs[i] == ptrs[n])
                    return false;
            }
        }

        return true;
    }
};

TEST(Buddy, Abuse)
{
    CHECK(!uut.init(mem, mem));
    CHECK(!uut.init(mem, mem - size));
    CHECK(!uut.init(mem, mem + 1));

    CHECK(uut.init(mem, mem + size));
    CHECK(!uut.allocate(10000000));
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

    void* ptrs[14];

    for(int i = 0; i < 14; i++) {
        ptrs[i] = uut.allocate(1);
        CHECK(ptrs[i]);
    }

    CHECK(isUnique(ptrs));

    for(int n = 1; n <= 14; n++)
    {
        CHECK(!uut.allocate(1));

        for(int i = 0; i < n; i++)
            uut.free(ptrs[i]);

        for(int i = 0; i < n; i++) {
            ptrs[i] = uut.allocate(1);
            CHECK(ptrs[i]);
        }

        CHECK(isUnique(ptrs));
    }
}

TEST(Buddy, AllBiggerSteppingShuffle)
{
    CHECK(uut.init(mem, mem + size));

    void* ptrs[7];

    for(int i = 0; i < 7; i++) {
        ptrs[i] = uut.allocate(8);
        CHECK(ptrs[i]);
    }

    CHECK(isUnique(ptrs));

    for(int n = 1; n <= 7; n++)
    {
        CHECK(!uut.allocate(8));

        for(int i = 0; i < n; i++)
            uut.free(ptrs[i]);

        for(int i = 0; i < n; i++) {
            ptrs[i] = uut.allocate(8);
            CHECK(ptrs[i]);
        }

        CHECK(isUnique(ptrs));
    }
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
