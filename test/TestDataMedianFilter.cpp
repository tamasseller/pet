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

#include "data/MedianFilter.h"

using namespace pet;

#include <list>
#include <vector>
#include <algorithm>
#include <array>
#include <stdlib.h>

TEST_GROUP(MedianFilter) {};

TEST(MedianFilter, Fill) {
    MedianFilter<int, 7> uut;

    uut.enterValue(0);
    CHECK(uut.getOutput() == 0);

    uut.enterValue(1);
    uut.enterValue(2);
    CHECK(uut.getOutput() == 1);

    uut.enterValue(3);
    uut.enterValue(4);
    CHECK(uut.getOutput() == 2);

    uut.enterValue(5);
    uut.enterValue(6);
    CHECK(uut.getOutput() == 3);

    uut.enterValue(5);
    uut.enterValue(4);
    CHECK(uut.getOutput() == 4);

    uut.enterValue(3);
    uut.enterValue(2);
    CHECK(uut.getOutput() == 4);

    uut.enterValue(1);
    uut.enterValue(0);
    CHECK(uut.getOutput() == 3);
}

TEST(MedianFilter, MedianChange) {
    MedianFilter<int, 3> uut;

    uut.enterValue(0);

    for(int i=0; i<3; i++) {
        uut.enterValue(-1);
        uut.enterValue(1);
    }

    uut.enterValue(2);
    CHECK(uut.getOutput() == 1);
}

TEST(MedianFilter, Stress) {
    std::list<int> window;
    MedianFilter<int, 11> uut;

    srand(1);

    for(int i = 0; i<11; i++) {
        int x = rand();
        uut.enterValue(x);
        window.push_back(x);
    }

    for(int i = 0; i<10000; i++) {
        std::vector<int> sorted(window.begin(), window.end());
        std::sort(sorted.begin(), sorted.end());
        CHECK(uut.getOutput() == sorted[sorted.size()/2]);

        int x = rand();
        uut.enterValue(x);
        window.pop_front();
        window.push_back(x);
    }
}


TEST(MedianFilter, FailureScenario1) {
    std::list<int> window;
    MedianFilter<int, 7> uut;

    for(auto x :std::array<int, 8>{2, 8, 5, 7, 4, 3, 1, 6})
        uut.enterValue(x);

    CHECK(uut.getOutput() == 5);
}
