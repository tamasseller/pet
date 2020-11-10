/*******************************************************************************
 *
 * Copyright (c) 2016, 2017 Tamás Seller. All rights reserved.
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

#include "data/PrioQueue.h"

using namespace pet;

TEST_GROUP(PrioQueue) {};

TEST(PrioQueue, Fill) {
    PrioQueue<int, 7> uut;

    CHECK(uut.insert(10));
    CHECK(uut.extreme() == 10);

    CHECK(uut.insert(15));
    CHECK(uut.extreme() == 10);

    CHECK(uut.insert(5));
    CHECK(uut.extreme() == 5);

    CHECK(uut.insert(20));
    CHECK(uut.extreme() == 5);

    CHECK(uut.insert(0));
    CHECK(uut.extreme() == 0);

    CHECK(uut.remove(&uut.extreme()));
    CHECK(uut.extreme() == 5);

    CHECK(uut.remove(&uut.extreme()));
    CHECK(uut.extreme() == 10);

    CHECK(uut.remove(&uut.extreme()));
    CHECK(uut.extreme() == 15);

    CHECK(uut.remove(&uut.extreme()));
    CHECK(uut.extreme() == 20);
}

TEST(PrioQueue, Abuse) {
    PrioQueue<int, 3> uut;

    CHECK(uut.isEmpty());

    CHECK(uut.insert(3));

    CHECK(!uut.isEmpty());

    CHECK(uut.insert(1));
    CHECK(uut.insert(2));

    CHECK(!uut.isEmpty());

    CHECK(uut.extreme() == 1);

    CHECK(!uut.insert(0));
    CHECK(uut.extreme() == 1);

    CHECK(!uut.isEmpty());

    CHECK(!uut.insert(0));
    CHECK(uut.extreme() == 1);

    CHECK(uut.remove(&uut.extreme()));
    CHECK(uut.extreme() == 2);

    CHECK(uut.remove(&uut.extreme()));
    CHECK(uut.extreme() == 3);

    CHECK(!uut.isEmpty());

    CHECK(uut.remove(&uut.extreme()));

    CHECK(uut.isEmpty());

    CHECK(!uut.remove(&uut.extreme()));
}

TEST(PrioQueue, Update) {
    PrioQueue<int, 3> uut;

    CHECK(!uut.update(&uut.extreme() + 0));
    CHECK(!uut.update(&uut.extreme() + 3));

    CHECK(uut.insert(0));
    CHECK(uut.insert(1));
    CHECK(uut.insert(2));

    CHECK(!uut.update(&uut.extreme() + 3));

    (&uut.extreme())[0] = 3;
    CHECK(uut.update(&uut.extreme() + 0));

    CHECK(uut.extreme() == 1);
    CHECK(uut.remove(&uut.extreme()));
    CHECK(uut.extreme() == 2);
    CHECK(uut.remove(&uut.extreme()));
    CHECK(uut.extreme() == 3);
    CHECK(uut.remove(&uut.extreme()));
    CHECK(!uut.remove(&uut.extreme()));
}

TEST_GROUP(PrioQueueWindow) {};

TEST(PrioQueueWindow, SmallContains) {
    PrioQueueWindow<int, 3> w;

    for(int i = 0; i < 3; i++)
        CHECK(w.contains(w.access() + i));

    for(int i = 3; i < 10; i++)
        CHECK(!w.contains(w.access() + i));

    for(int i = -12; i < -10; i++)
        CHECK(!w.contains(w.access() + i));
}

TEST(PrioQueueWindow, SmallNoop) {
    PrioQueueWindow<int, 3> w;

    w.access()[0] = 1;
    w.access()[1] = 2;
    w.access()[2] = 3;

    CHECK(w.extreme() == 1);

    w.replace(w.access() + 0, 0);
    CHECK(w.extreme() == 0);

    w.replace(w.access() + 1, 20);
    CHECK(w.extreme() == 0);

    w.replace(w.access() + 2, 40);
    CHECK(w.extreme() == 0);
}

TEST(PrioQueueWindow, SmallUp) {
    PrioQueueWindow<int, 3> w;

    w.access()[0] = 10;
    w.access()[1] = 20;
    w.access()[2] = 30;

    w.replace(w.access() + 1, 5);
    CHECK(w.extreme() == 5);

    w.replace(w.access() + 2, 0);
    CHECK(w.extreme() == 0);
}

TEST(PrioQueueWindow, SmallDown) {
    PrioQueueWindow<int, 3> w;

    w.access()[0] = 1;
    w.access()[1] = 3;
    w.access()[2] = 5;

    w.replace(w.access() + 0, 7);
    CHECK(w.extreme() == 3);

    w.replace(w.access() + 0, 9);
    CHECK(w.extreme() == 5);

    w.replace(w.access() + 0, 8);
    CHECK(w.extreme() == 7);
}

TEST(PrioQueueWindow, BigUp) {
    PrioQueueWindow<int, 15> w;

    for(int i=0; i < sizeof(w.access())/sizeof(int); i++)
        w.access()[i] = 2000 - i * 10;

    for(int i = sizeof(w.access()) / sizeof(int) - 1; i; i--) {
        w.replace(w.access() + i, i * 10);
        CHECK(w.extreme() == i * 10);
    }
}

TEST(PrioQueueWindow, BigDownRoot) {
    PrioQueueWindow<int, 42> w;

    for(int i=0; i < sizeof(w.access())/sizeof(int); i++)
        w.access()[i] = i * 10;

    CHECK(w.extreme() == 0);

    w.replace(w.access() + 0, 1000);
    CHECK(w.extreme() == 10);

    w.replace(w.access() + 0, 15);
    CHECK(w.extreme() == 15);

    w.replace(w.access() + 0, 25);
    CHECK(w.extreme() == 20);

    w.replace(w.access() + 0, 70);
    CHECK(w.extreme() == 25);
}

TEST(PrioQueueWindow, BigDownNonRoot) {
    PrioQueueWindow<int, 123> w;

    for(int i=0; i < sizeof(w.access())/sizeof(int); i++)
        w.access()[i] = i * 10;

    for(int i=1; i < sizeof(w.access())/sizeof(int); i++) {
        w.replace(w.access() + i, 2000 - i * 10);
        CHECK(w.extreme() == 0);
    }
}
