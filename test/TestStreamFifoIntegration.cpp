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

#include "stream/Stream.h"
#include "stream/FifoStream.h"

#include "CppUTest/TestHarness.h"

#include <string.h>

using namespace pet;

TEST_GROUP(FifoStreamIntegration) {
    typedef StaticFifoStream<16> Uut;
    Uut uut;

    void flush() {
    	CHECK(!uut.flush().failed());
    }

    void write(const char* what) {
        GenericError r = uut.write(what, strlen(what));
        CHECK(!r.failed() && r == strlen(what));
    }

    void read(const char* what) {
        char temp[256];
        GenericError r = uut.read(temp, strlen(what));
        CHECK(!r.failed() && r == strlen(what));
        CHECK(memcmp(temp, what, strlen(what)) == 0);
    }
};

TEST(FifoStreamIntegration, ItemAccessSane)
{
    int x = 123;
    CHECK(uut.read(x).failed());

    CHECK(!uut.write(x).failed());
    CHECK(!uut.flush().failed());

    x = 345;
    CHECK(!uut.read(x).failed());

    CHECK(x == 123);
}

TEST(FifoStreamIntegration, BlockAccessSane)
{
    const char *str = "foobar";
    char temp[strlen(str)];
    GenericError r;

    r = uut.read(temp, sizeof(temp));
    CHECK(!r.failed() && r == 0);

    for(int i = 0; i < 12; i++) {
    	write(str);
    	flush();
    	read(str);
    }
}

TEST(FifoStreamIntegration, Segmented)
{
    write("foobar");
    flush();
    read("foo");
    read("bar");
}

TEST(FifoStreamIntegration, Fill)
{
    write("sixteencharshere");

    GenericError r = uut.write("can't write", 12);
    CHECK(!r.failed() && r == 0);

    int neitherThisCanBeWritten = 1;
    CHECK(uut.write(neitherThisCanBeWritten).failed());

    flush();
    read("sixteen");
	read("chars");
	read("here");
}

TEST(FifoStreamIntegration, NotEnough)
{
	char temp[8];
    write("foobar");
    flush();
    GenericError r = uut.read(temp, 8);
    CHECK(!r.failed() && r == 6);
}

TEST(FifoStreamIntegration, TooMuch)
{
    write("foobar");
    write("foobar");

    GenericError r = uut.write("foobar", 6);
    CHECK(!r.failed() && r == 4);
}

TEST(FifoStreamIntegration, FunnyFlushing)
{
    write("88888888");
    flush();

    read("8888");

    write("aaaaaaaaaa");
    flush();

    read("8888aaaaaaaaa");

    write("cc");
    flush();

    read("acc");
}
