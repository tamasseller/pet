/*******************************************************************************
 *
 * Copyright (c) 2016 Tamás Seller. All rights reserved.
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

#include "pool/StringCollector.h"

#include "1test/Test.h"

#include <string.h>

#include "MockAllocator.h"

using namespace pet;

TEST_GROUP(StringStorageInstrumented) {
	template<unsigned int ss>
	struct InstrumentedTestStringStorage: public StringCollector<8+2*ss, ss, FailableAllocator> {
		void playStory() {
			if(!this->put('f'))
				return;

			if(!this->put("o"))
				return;

			if(!this->put('o'))
				return;

			CHECK(strcmp((char*)this->peek()+ss, "foo") == 0);
			CHECK(strcmp((char*)this->terminate()+ss, "foo") == 0);

			if(!this->put("bar"))
				return;

			CHECK(strcmp((char*)this->peek()+ss, "bar") == 0);
			this->discard();
			CHECK(strcmp((char*)this->peek()+ss, "") == 0);

			void* f = (void*)this->currFrame;

			this->put("123");

			char *s1 = (char*)this->peek()+ss;
			CHECK(strcmp(s1, "123") == 0);

			this->put('4');

			char *s2 = (char*)this->peek()+ss;
			CHECK(strcmp(s2, "1234") == 0);
			CHECK(s1 != s2);
			CHECK(f != this->currFrame);

			this->discard();
			s2 = (char*)this->peek()+ss;
			CHECK(strcmp(s2, "") == 0);
			CHECK(s1 == s2);
			CHECK(f == this->currFrame);

			this->put("y");
			this->put("xc");
			CHECK(strcmp((char*)this->terminate()+ss, "yxc") == 0);

			CHECK(f == this->currFrame);
			this->discard();
			CHECK(f == this->currFrame);
			this->discard();
			CHECK(f == this->currFrame);

			this->put('_');
			CHECK(f != this->currFrame);
			this->discard();
			CHECK(f == this->currFrame);
		}
	};

	TEST_TEARDOWN(){
		CHECK(Allocator::allFreed());
	}
};

TEST(StringStorageInstrumented, WithoutSpare) {
	InstrumentedTestStringStorage<0>().playStory();
}

TEST(StringStorageInstrumented, WithSpare) {
	InstrumentedTestStringStorage<3>().playStory();
}

TEST_GROUP(StringStorageFreeTest) {
	typedef StringCollector<8, 2, Allocator> FreeTestStringStorage;

	void putAsdQwe(FreeTestStringStorage &t){
		CHECK(t.put('a'));
		CHECK(t.put('s'));
		CHECK(t.put('d'));
		CHECK(t.put("qwe"));
	}

	TEST_TEARDOWN(){
		CHECK(Allocator::allFreed());
	}
};

TEST(StringStorageFreeTest, Sanity) {
	FreeTestStringStorage t;
	putAsdQwe(t);
}

TEST(StringStorageFreeTest, AfterTerminate) {
	FreeTestStringStorage t;
	putAsdQwe(t);
	t.terminate();
}

TEST(StringStorageFreeTest, AfterDiscard) {
	FreeTestStringStorage t;
	putAsdQwe(t);
	t.discard();
}

TEST(StringStorageFreeTest, AfterDoubleTerminate) {
	FreeTestStringStorage t;
	putAsdQwe(t);
	t.terminate();
	putAsdQwe(t);
	t.terminate();
}

TEST(StringStorageFreeTest, AfterTerminateDiscard) {
	FreeTestStringStorage t;
	putAsdQwe(t);
	t.terminate();
	putAsdQwe(t);
	t.discard();
}

TEST(StringStorageFreeTest, AfterTerminateDiscardTerminate) {
	FreeTestStringStorage t;
	putAsdQwe(t);
	t.terminate();
	putAsdQwe(t);
	t.discard();
	putAsdQwe(t);
	t.terminate();
	putAsdQwe(t);
}

TEST(StringStorageFreeTest, AfterTooLongDiscarded) {
	FreeTestStringStorage t;
	putAsdQwe(t);
	t.terminate();
	putAsdQwe(t);
	t.discard();
	putAsdQwe(t);
	t.terminate();
	putAsdQwe(t);

	CHECK(!t.put("too long"));
	t.discard();
	putAsdQwe(t);
}
