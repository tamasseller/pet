/*
 * TestMetaSequence.cpp
 *
 *      Author: tamas.seller
 */

#include "1test/Test.h"

#include "meta/Sequence.h"

using namespace pet;

TEST_GROUP(Sequence) {
    template<int... idx> void test(int min, int max, const Sequence<idx...>&) {
        int arr[] = {idx...};

        CHECK((sizeof(arr)/sizeof(arr[0])) == (max - min));

        for(int i = 0; i < max - min; i++)
            arr[i] = min + i;
    }
};

TEST(Sequence, Sanity) {
    test(0, 5, sequence<0, 5>());
    test(5, 10, sequence<5, 10>());
}

TEST(Sequence, Single) {
    test(10, 11, sequence<10, 11>());
}

TEST(Sequence, Empty) {
    test(11, 11, sequence<11, 11>());
}

