/*
 * TestStreamStream.cpp
 *
 *      Author: tamas.seller
 */

#include "stream/Stream.h"
#include "data/Fifo.h"

#include "CppUTest/TestHarness.h"

#include <string.h>

using namespace pet;

TEST_GROUP(FifoStreamIntegration) {
    class Uut: public StaticFifo<16>, public InputStream<Uut>, public OutputStream<Uut> {};
    Uut uut;

    void flush() {

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

    r = uut.write(str, strlen(str));
    CHECK(!r.failed() && r == strlen(str));

    CHECK(!uut.flush().failed());

    r = uut.read(temp, sizeof(temp));
    CHECK(!r.failed() && r == sizeof(temp));

    CHECK(memcmp(temp, str, strlen(str)) == 0);
}

TEST(FifoStreamIntegration, Segmented)
{
    write("foobar");
    CHECK(!uut.flush().failed());
    read("foo");
    read("bar");
}

TEST(FifoStreamIntegration, FunnyFlushing)
{
    write("88888888");
    CHECK(!uut.flush().failed());

    read("8888");

    write("aaaaaaaaaa");
    CHECK(!uut.flush().failed());

    read("8888aaaaaaaaa");

    write("cc");
    CHECK(!uut.flush().failed());

    read("acc");
}
