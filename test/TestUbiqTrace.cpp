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

#include "1test/Test.h"
#include "1test/Mock.h"

#include "ubiquitous/Trace.h"

const char *critMsg = "Critical\n";
const char *failMsg = "Failure\n";
const char *warnMsg = "Warning\n";
const char *infoMsg = "Information";

template<class Client>
struct TestBase 
{
	void testWrite() 
	{
		Client::crit << critMsg;
		Client::fail << failMsg;
		Client::warn << warnMsg;
		Client::info << infoMsg;
	}

	void testAssert() 
	{
		Client::assertThat(true);
		Client::assertThat(false);
		Client::assertThat(true, "foo");
		Client::assertThat(false, "bar");
		Client::template assertThat<pet::Level::Warning>(false, "foobar");
	}
};

struct A: pet::Trace<A>, TestBase<A> {};
struct B: pet::Trace<B>, TestBase<B> {};

namespace ns {
	struct C: pet::Trace<C>, TestBase<C> {};
}

TEST_GROUP(TraceTest)
{
	A a;
	B b;
	ns::C c;

	TEST_SETUP() {
		writerMocked = true;
	}

	TEST_TEARDOWN() {
		writerMocked = false;
	}
};

TEST(TraceTest, GeneralFilter)
{
	MOCK(MockWriter)::EXPECT("write<const char*>").withParam(critMsg);
	MOCK(MockWriter)::EXPECT("write<const char*>").withParam(failMsg);
	a.testWrite();
}

TEST(TraceTest, ClassFilter)
{
	MOCK(MockWriter)::EXPECT("write<const char*>").withParam(critMsg);
	MOCK(MockWriter)::EXPECT("write<const char*>").withParam(failMsg);
	MOCK(MockWriter)::EXPECT("write<const char*>").withParam(warnMsg);
	MOCK(MockWriter)::EXPECT("write<const char*>").withParam(infoMsg);
	b.testWrite();
}

TEST(TraceTest, NamespacedClassFilter) 
{
	MOCK(MockWriter)::EXPECT("write<const char*>").withParam(critMsg);
	MOCK(MockWriter)::EXPECT("write<const char*>").withParam(failMsg);
	MOCK(MockWriter)::EXPECT("write<const char*>").withParam(warnMsg);
	c.testWrite();
}

TEST(TraceTest, GeneralFilterAssert)
{
	MOCK(MockWriter)::EXPECT("write<const char*>").withParam("Assertation failed: ");
	MOCK(MockWriter)::EXPECT("write<const char*>").withParam("unspecified");
	MOCK(MockWriter)::EXPECT("write<const char*>").withParam((const char*)pet::endl);
	MOCK(MockWriter)::EXPECT("write<const char*>").withParam("Assertation failed: ");
	MOCK(MockWriter)::EXPECT("write<const char*>").withParam("bar");
	MOCK(MockWriter)::EXPECT("write<const char*>").withParam((const char*)pet::endl);
	a.testAssert();
}

TEST(TraceTest, ClassFilterAssert)
{
	MOCK(MockWriter)::EXPECT("write<const char*>").withParam("Assertation failed: ");
	MOCK(MockWriter)::EXPECT("write<const char*>").withParam("unspecified");
	MOCK(MockWriter)::EXPECT("write<const char*>").withParam((const char*)pet::endl);
	MOCK(MockWriter)::EXPECT("write<const char*>").withParam("Assertation failed: ");
	MOCK(MockWriter)::EXPECT("write<const char*>").withParam("bar");
	MOCK(MockWriter)::EXPECT("write<const char*>").withParam((const char*)pet::endl);
	MOCK(MockWriter)::EXPECT("write<const char*>").withParam("Assertation failed: ");
	MOCK(MockWriter)::EXPECT("write<const char*>").withParam("foobar");
	MOCK(MockWriter)::EXPECT("write<const char*>").withParam((const char*)pet::endl);
	b.testAssert();
}

TEST(TraceTest, NamespacedClassFilterAssert)
{
	MOCK(MockWriter)::EXPECT("write<const char*>").withParam("Assertation failed: ");
	MOCK(MockWriter)::EXPECT("write<const char*>").withParam("unspecified");
	MOCK(MockWriter)::EXPECT("write<const char*>").withParam((const char*)pet::endl);
	MOCK(MockWriter)::EXPECT("write<const char*>").withParam("Assertation failed: ");
	MOCK(MockWriter)::EXPECT("write<const char*>").withParam("bar");
	MOCK(MockWriter)::EXPECT("write<const char*>").withParam((const char*)pet::endl);
	MOCK(MockWriter)::EXPECT("write<const char*>").withParam("Assertation failed: ");
	MOCK(MockWriter)::EXPECT("write<const char*>").withParam("foobar");
	MOCK(MockWriter)::EXPECT("write<const char*>").withParam((const char*)pet::endl);
	c.testAssert();
}

TEST(TraceTest, Callthrough)
{
	MOCK(MockWriter)::EXPECT("write<const char*>").withParam(critMsg);
	A::crit << critMsg;
	MOCK(MockWriter)::EXPECT("write<void*>").withParam((void*)critMsg);
	A::crit << (void*)critMsg;
	MOCK(MockWriter)::EXPECT("write<short>").withParam(1);
	A::crit << (short)1;
	MOCK(MockWriter)::EXPECT("write<unsigned short>").withParam(1);
	A::crit << (unsigned short)1;
	MOCK(MockWriter)::EXPECT("write<int>").withParam(1);
	A::crit << (int)1;
	MOCK(MockWriter)::EXPECT("write<unsigned int>").withParam(1);
	A::crit << (unsigned int)1;
	MOCK(MockWriter)::EXPECT("write<long>").withParam(1);
	A::crit << (long)1;
	MOCK(MockWriter)::EXPECT("write<unsigned long>").withParam(1);
	A::crit << (unsigned long)1;
	MOCK(MockWriter)::EXPECT("write<float>").withParam(1.0);
	A::crit << (float)1;
	MOCK(MockWriter)::EXPECT("write<double>").withParam(1.0f);
	A::crit << (double)1;
	MOCK(MockWriter)::EXPECT("write<long double>").withParam(1.0l);
	A::crit << (long double)1;
}
