/*
 * TlsfStress.cpp
 *
 *  Created on: 2016.11.15.
 *      Author: tamas.seller
 */

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"

#include "Trace.h"

const char *critMsg = "Critical\n";
const char *failMsg = "Failure\n";
const char *warnMsg = "Warning\n";
const char *infoMsg = "Information";

template<class Client>
struct TestBase {
	void testWrite() {
		Client::crit << critMsg;
		Client::fail << failMsg;
		Client::warn << warnMsg;
		Client::info << infoMsg;
	}

	void testAssert() {
		Client::assert(true);
		Client::assert(false);
		Client::assert(true, "foo");
		Client::assert(false, "bar");
		Client::template assert<ubiq::Level::Warning>(false, "foobar");
	}
};

struct A: ubiq::Trace<A>, TestBase<A> {};
struct B: ubiq::Trace<B>, TestBase<B> {};
namespace ns {
struct C: ubiq::Trace<C>, TestBase<C> {};
}

TEST_GROUP(TraceTest) {
	A a;
	B b;
	ns::C c;

	TEST_TEARDOWN() {
		mock("MockWriter").checkExpectations();
		mock().clear();
	}
};

TEST(TraceTest, GeneralFilter) {
	mock("MockWriter").expectOneCall("write<const char*>").withStringParameter("val", critMsg);
	mock("MockWriter").expectOneCall("write<const char*>").withStringParameter("val", failMsg);
	a.testWrite();
}

TEST(TraceTest, ClassFilter) {
	mock("MockWriter").expectOneCall("write<const char*>").withStringParameter("val", critMsg);
	mock("MockWriter").expectOneCall("write<const char*>").withStringParameter("val", failMsg);
	mock("MockWriter").expectOneCall("write<const char*>").withStringParameter("val", warnMsg);
	mock("MockWriter").expectOneCall("write<const char*>").withStringParameter("val", infoMsg);
	b.testWrite();
}

TEST(TraceTest, NamespacedClassFilter) {
	mock("MockWriter").expectOneCall("write<const char*>").withStringParameter("val", critMsg);
	mock("MockWriter").expectOneCall("write<const char*>").withStringParameter("val", failMsg);
	mock("MockWriter").expectOneCall("write<const char*>").withStringParameter("val", warnMsg);
	c.testWrite();
}

TEST(TraceTest, GeneralFilterAssert) {
	mock("MockWriter").expectOneCall("write<const char*>").withStringParameter("val", "Assertation failed: ");
	mock("MockWriter").expectOneCall("write<const char*>").withStringParameter("val", "unspecified");
	mock("MockWriter").expectOneCall("write<const char*>").withStringParameter("val", "\n");
	mock("MockWriter").expectOneCall("write<const char*>").withStringParameter("val", "Assertation failed: ");
	mock("MockWriter").expectOneCall("write<const char*>").withStringParameter("val", "bar");
	mock("MockWriter").expectOneCall("write<const char*>").withStringParameter("val", "\n");
	a.testAssert();
}

TEST(TraceTest, ClassFilterAssert) {
	mock("MockWriter").expectOneCall("write<const char*>").withStringParameter("val", "Assertation failed: ");
	mock("MockWriter").expectOneCall("write<const char*>").withStringParameter("val", "unspecified");
	mock("MockWriter").expectOneCall("write<const char*>").withStringParameter("val", "\n");
	mock("MockWriter").expectOneCall("write<const char*>").withStringParameter("val", "Assertation failed: ");
	mock("MockWriter").expectOneCall("write<const char*>").withStringParameter("val", "bar");
	mock("MockWriter").expectOneCall("write<const char*>").withStringParameter("val", "\n");
	mock("MockWriter").expectOneCall("write<const char*>").withStringParameter("val", "Assertation failed: ");
	mock("MockWriter").expectOneCall("write<const char*>").withStringParameter("val", "foobar");
	mock("MockWriter").expectOneCall("write<const char*>").withStringParameter("val", "\n");
	b.testAssert();
}

TEST(TraceTest, NamespacedClassFilterAssert) {
	mock("MockWriter").expectOneCall("write<const char*>").withStringParameter("val", "Assertation failed: ");
	mock("MockWriter").expectOneCall("write<const char*>").withStringParameter("val", "unspecified");
	mock("MockWriter").expectOneCall("write<const char*>").withStringParameter("val", "\n");
	mock("MockWriter").expectOneCall("write<const char*>").withStringParameter("val", "Assertation failed: ");
	mock("MockWriter").expectOneCall("write<const char*>").withStringParameter("val", "bar");
	mock("MockWriter").expectOneCall("write<const char*>").withStringParameter("val", "\n");
	mock("MockWriter").expectOneCall("write<const char*>").withStringParameter("val", "Assertation failed: ");
	mock("MockWriter").expectOneCall("write<const char*>").withStringParameter("val", "foobar");
	mock("MockWriter").expectOneCall("write<const char*>").withStringParameter("val", "\n");
	c.testAssert();
}

TEST(TraceTest, Callthrough) {
	mock("MockWriter").expectOneCall("write<const char*>").withStringParameter("val", critMsg);
	A::crit << critMsg;
	mock("MockWriter").expectOneCall("write<void*>").withPointerParameter("val", (void*)critMsg);
	A::crit << (void*)critMsg;
	mock("MockWriter").expectOneCall("write<short>").withIntParameter("val", 1);
	A::crit << (short)1;
	mock("MockWriter").expectOneCall("write<unsigned short>").withIntParameter("val", 1);
	A::crit << (unsigned short)1;
	mock("MockWriter").expectOneCall("write<int>").withIntParameter("val", 1);
	A::crit << (int)1;
	mock("MockWriter").expectOneCall("write<unsigned int>").withIntParameter("val", 1);
	A::crit << (unsigned int)1;
	mock("MockWriter").expectOneCall("write<long>").withIntParameter("val", 1);
	A::crit << (long)1;
	mock("MockWriter").expectOneCall("write<unsigned long>").withIntParameter("val", 1);
	A::crit << (unsigned long)1;
	mock("MockWriter").expectOneCall("write<float>").withDoubleParameter("val", 1.0);
	A::crit << (float)1;
	mock("MockWriter").expectOneCall("write<double>").withDoubleParameter("val", 1.0f);
	A::crit << (double)1;
	mock("MockWriter").expectOneCall("write<long double>").withDoubleParameter("val", 1.0l);
	A::crit << (long double)1;
}
