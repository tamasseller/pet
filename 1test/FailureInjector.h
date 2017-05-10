/*
 * FailureInjector.h
 *
 *      Author: tamas.seller
 */

#ifndef FAILUREINJECTOR_H_
#define FAILUREINJECTOR_H_

namespace pet {

template<class Child>
class StaticFailureSource {
    protected:
    static Child instance;

    inline bool shouldSimulateError() {
        return false;
    }
};

template<class Child>
Child StaticFailureSource<Child>::instance;

}

#define CHECK_ALWAYS(x) CHECK(x)

#endif /* FAILUREINJECTOR_H_ */
