/*
 * FailureInjector.h
 *
 *      Author: tamas.seller
 */

#ifndef FAILUREINJECTOR_H_
#define FAILUREINJECTOR_H_

#include "Registry.h"

namespace pet {

class FailureInjector {
    static int max, counter, failAt;
protected:
    friend class TestRunner;
    static void reset() {
        max = counter = failAt = 0;
    }

    static void firstRunDone() {
        max = counter;
    }

    static bool hasMore() {
        return failAt < max;
    }

    static bool step() {
        counter = 0;
        failAt++;
        return true;
    }
public:
    static bool shouldSimulateError() {
        counter++;
        return counter == failAt;
    }
};

}

#endif /* FAILUREINJECTOR_H_ */
