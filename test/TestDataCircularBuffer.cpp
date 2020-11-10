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

#include "data/CircularBuffer.h"

using namespace pet;

TEST_GROUP(CircularWindow) {};

TEST(CircularWindow, Sane) {
    CircularBuffer<int, 3> w;

    w.access()[0] = 3;
    w.access()[1] = 4;
    w.access()[2] = 5;

    CHECK(w.front() == 3);
    w.replace(6);
    CHECK(w.front() == 4);
    w.replace(7);
    CHECK(w.front() == 5);
    w.replace(8);
};

TEST(CircularWindow, Wrap) {
    CircularBuffer<int, 3> w;

    w.replace(1);
    w.replace(2);
    w.replace(3);

    CHECK(w.front() == 1);
    w.replace(4);

    CHECK(w.front() == 2);
    w.replace(5);

    CHECK(w.front() == 3);
    w.replace(6);

    CHECK(w.front() == 4);
    w.replace(7);
};

TEST(CircularWindow, Iterator) {
    CircularBuffer<int, 3> w;

    w.replace(1);
    w.replace(2);
    w.replace(3);

    CHECK(w.front() == 1);
    w.replace(4);

    CHECK(w.front() == 2);
    w.replace(5);

    CHECK(w.front() == 3);
    w.replace(6);

    CHECK(w.front() == 4);
    w.replace(7);
};
