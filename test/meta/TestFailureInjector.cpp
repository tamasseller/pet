/*
 * TestFailureInjector.cpp
 *
 *      Author: tamas.seller
 */

#include "1test/Test.h"
#include "1test/FailureInjector.h"

using namespace pet;

TEST_GROUP(FailureInjector) {
};


TEST(FailureInjector, Sanity) {
    FailureInjector::shouldSimulateError();
}

TEST(FailureInjector, Multiple) {
    FailureInjector::shouldSimulateError();
    FailureInjector::shouldSimulateError();
    FailureInjector::shouldSimulateError();
}

TEST(FailureInjector, Check) {
    CHECK(FailureInjector::shouldSimulateError());
}

TEST(FailureInjector, CheckAlways) {
    FailureInjector::shouldSimulateError();
    CHECK_ALWAYS(false);
}
