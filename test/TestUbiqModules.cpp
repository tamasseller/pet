/*
 * TestModules.cpp
 *
 *      Author: tamas.seller
 */

#include "1test/Test.h"
#include "1test/Mock.h"

#include "module/Module.h"
#include "module/Modules.h"

using namespace pet;

#define SERVICE(s)                                          \
class s: public Service<s> {};

#define MODULE(n, r, p)                                     \
struct n: public Module<n, r, p> {                          \
    virtual void init() {                                   \
        instance.installModule();                           \
        MOCK(modules)::CALL(n);                             \
    }                                                       \
}

SERVICE(S1);
MODULE(M1, Requires<S1>, Provides<S1>);

TEST(Modules) {
    MOCK(modules)::EXPECT(M1);
    Modules::initAll();
}

