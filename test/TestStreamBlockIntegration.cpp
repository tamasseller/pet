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
#include "stream/VirtualStream.h"
#include "data/Fifo.h"

#include "CppUTest/TestHarness.h"

#include <string.h>

#include <list>

using namespace pet;

TEST_GROUP(BlockStreamIntegration) {
	struct TestBlockDevice {
		static constexpr const uint32_t numberOfBlocks = 16;
    	static constexpr const uint32_t blockSize = 16;
    	char data[numberOfBlocks*blockSize];
    	unsigned int dataLength;
	};

	struct InpUut;
	struct OutpUut;
	struct IoUut;

    struct TestDataAccessor {
    	TestBlockDevice* device;
    	unsigned int blockIdx;

    	bool isLastBlock() const {
    		return blockIdx == device->dataLength / TestBlockDevice::blockSize;
    	}

    	uint32_t readLength() const {
    		if(isLastBlock())
    			return device->dataLength % TestBlockDevice::blockSize;
    		else
    			return TestBlockDevice::blockSize;
    	}

    	inline pet::GenericError nextBlock(ReadLength* actLen, char* &buff, uint16_t length) const {
    		buff = &device->data[blockIdx*TestBlockDevice::blockSize];
    		actLen->length = readLength();
    		return 0;
    	}

    	inline pet::GenericError nextBlock(WriteLength* actLen, char* &buff, uint16_t length) const {
    		buff = &device->data[blockIdx*TestBlockDevice::blockSize];
    		actLen->length = TestBlockDevice::blockSize;
    		return 0;
    	}

    	inline pet::GenericError blockDone(ReadLength* actLen, char* &buffer, uint32_t &offset) {
    		if(offset == TestBlockDevice::blockSize) {
    			actLen->length = 0;
    			buffer = 0;
    			offset = 0;
    			blockIdx++;
    		} else
    			actLen->length = readLength();

    		return 0;
    	}

    	inline pet::GenericError blockDone(WriteLength* actLen, char* &buffer, uint32_t &offset) {
    		if(isLastBlock() && offset > readLength())
    			device->dataLength = blockIdx*TestBlockDevice::blockSize + offset;

    		if(offset == TestBlockDevice::blockSize) {
    			actLen->length = 0;
    			buffer = 0;
    			offset = 0;
    			blockIdx++;
    		}

    		return 0;
    	}

    public:
    	inline TestDataAccessor(TestBlockDevice* device): device(device), blockIdx(0) {}
    };

    typedef VirtualizedStream<TestDataAccessor> Uut;

    TestBlockDevice dut;
    Uut inpUut = Uut(&dut);
    Uut outpUut = Uut(&dut);
    Uut ioUut = Uut(&dut);

    void flush(VirtualStream *where = 0) {
    	if(!where) where = &outpUut;
    	CHECK(!where->flush().failed());
    }

    void write(const char* what, VirtualStream *where = 0) {
    	if(!where) where = &outpUut;
        GenericError r = where->write(what, strlen(what));
        CHECK(!r.failed() && r == strlen(what));
    }

    void read(const char* what, VirtualStream *where = 0) {
    	if(!where) where = &inpUut;
        char temp[256];
        GenericError r = where->read(temp, strlen(what));
        CHECK(!r.failed() && r == strlen(what));
        CHECK(memcmp(temp, what, strlen(what)) == 0);
    }
};

TEST(BlockStreamIntegration, ItemAccessSane)
{
    int x = 123;
    CHECK(inpUut.read(x).failed());

    CHECK(!outpUut.write(x).failed());
    CHECK(!outpUut.flush().failed());

    x = 345;
    CHECK(!inpUut.read(x).failed());

    CHECK(x == 123);
}

TEST(BlockStreamIntegration, FastForwardAccess)
{
    int x = 123;
    CHECK(!outpUut.write(x).failed());
    x = 234;
    CHECK(!outpUut.write(x).failed());
    CHECK(!outpUut.flush().failed());

    x = 345;
    CHECK(!inpUut.read(x).failed());
    CHECK(x == 123);
    x = 345;
    CHECK(!inpUut.read(x).failed());
    CHECK(x == 234);
}


TEST(BlockStreamIntegration, BlockAccessSane)
{
    const char *str = "foobar";
    char temp[strlen(str)];
    GenericError r;

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
    write("88888888", &ioUut);
    flush(&ioUut);

    read("8888");

    write("aaaaaaaaaa", &ioUut);
    flush(&ioUut);

    read("8888aaaaaaaaa");

    write("cc", &ioUut);
    flush(&ioUut);

    read("acc");
}

TEST(BlockStreamIntegration, MultipleBlock)
{
    write("0123456789abcdef0123456789abcd");
    flush();
    read("0123456789abcdef0123456789abcd", &ioUut);
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
