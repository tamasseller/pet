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
#include "data/Fifo.h"

#include "CppUTest/TestHarness.h"

#include <string.h>

#include <list>

using namespace pet;

TEST_GROUP(BlockStreamIntegration) {
    class Uut: public InputStream<Uut>, public OutputStream<Uut> {
    	friend InputStream<Uut>;
    	friend OutputStream<Uut>;
    	static constexpr const uint32_t blockSize = 16;
    	std::list<char*> data;
    	unsigned int lastSize;
    	char* unfinished = 0;

        inline pet::GenericError nextReadable(char* &buff, uint32_t length) {
        	if(data.empty())
        		return 0;

        	buff = data.front();

    		return (buff == unfinished) ? lastSize : blockSize;
        }

    	inline pet::GenericError doneReading(char* &buff, uint32_t &offset) {
    		if(offset == blockSize) {
           		data.pop_front();
           		unfinished = data.back();
        		delete[] buff;
        		buff = 0;
        		offset = 0;
        		return 0;
    		}

    		return (buff == unfinished) ? lastSize : blockSize;
    	}

    	inline pet::GenericError nextWritable(char* &buff, uint32_t length) {
        	buff = new char[blockSize];
        	return blockSize;
    	}

    	inline pet::GenericError doneWriting(char* &buff, uint32_t &offset) {
    		if(data.back() != buff) {
        		data.push_back(buff);
        		unfinished = data.back();
    		}

    		if(offset == blockSize) {
    			unfinished = 0;
    			lastSize = 0;
        		buff = 0;
        		offset = 0;
        		return 0;
    		} else {
    			CHECK(lastSize <= offset);
    			lastSize = offset;
    		}

    		return blockSize;
    	}
    };

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

    Uut uut;
};

TEST(BlockStreamIntegration, ItemAccessSane)
{
    int x = 123;
    CHECK(uut.read(x).failed());

    CHECK(!uut.write(x).failed());
    CHECK(!uut.flush().failed());

    x = 345;
    CHECK(!uut.read(x).failed());

    CHECK(x == 123);
}

TEST(BlockStreamIntegration, BlockAccessSane)
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

TEST(BlockStreamIntegration, Segmented)
{
    write("foobar");
    flush();
    read("foo");
    read("bar");
}

TEST(BlockStreamIntegration, FunnyFlushing)
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

TEST(BlockStreamIntegration, MultipleBlock)
{
    write("0123456789abcdef0123456789abcd");
    flush();
    read("0123456789abcdef0123456789abcd");
}

TEST(BlockStreamIntegration, MultipleBlockSegmentedRead)
{
    write("0123456789abcdef0123456789abcd");
    flush();
    read("0123456789");
    read("abcdef0123456789abcd");
}

TEST(BlockStreamIntegration, MultipleBlockSegmentedWrite)
{
	write("0123456789");
    write("abcdef0123456789abcd");
    flush();
    read("0123456789abcdef0123456789abcd");
}
