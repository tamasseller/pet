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

#include "stream/VirtualizedStream.h"
#include "data/Fifo.h"

#include "CppUTest/TestHarness.h"

#include <string.h>

#include <list>

using namespace pet;

TEST_GROUP(StreamVirtual) {
    typedef VirtualizedIOStream<StaticFifo<16>> Uut1;
    typedef VirtualizedIOStream<StaticFifo<32>> Uut2;

    Uut1 uut1;
    Uut2 uut2;

    void sanityCheck(IndirectIOStream &uut) {
        int x = 123;
        CHECK(uut.read(x).failed());

        CHECK(!uut.write(x).failed());
        CHECK(!uut.flush().failed());

        x = 345;
        CHECK(!uut.read(x).failed());

        CHECK(x == 123);

        const char *str = "foobar";
        char temp[strlen(str)];
        GenericError r;

        r = uut.read(temp, sizeof(temp));
        CHECK(!r.failed() && r == 0);

        for(int i = 0; i < 12; i++) {
            GenericError r = uut.write(str, strlen(str));
            CHECK(!r.failed() && r == strlen(str));

            CHECK(!uut.flush().failed());

            char temp[256];
            r = uut.read(temp, strlen(str));
            CHECK(!r.failed() && r == strlen(str));
            CHECK(memcmp(temp, str, strlen(str)) == 0);
        }
    }
};


TEST(StreamVirtual, Sanity)
{
	sanityCheck(uut1);
	sanityCheck(uut2);
}
