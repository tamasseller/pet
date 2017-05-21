/*
 * TestModules.cpp
 *
 *      Author: tamas.seller
 */

#include "1test/Test.h"
#include "1test/Mock.h"

#include "module/Module.h"
#include "module/Modules.h"

#define MODULE(n, ...)                                     	\
struct n: public pet::Module<n, pet::Requires<__VA_ARGS__>> {                        \
    virtual void init() {                                   \
        instance.installModule();                           \
        MOCK(modules)::CALL(n);                             \
    }                                                       \
}

MODULE(M1);
MODULE(M2);
MODULE(M3);
MODULE(M4, M1, M2);
MODULE(M5, M2, M3);
MODULE(M6, M1, M3);
MODULE(M7, M5);

TEST(Modules) {
    MOCK(modules)::EXPECT(M2);
    MOCK(modules)::EXPECT(M3);
    MOCK(modules)::EXPECT(M5);
    MOCK(modules)::EXPECT(M7);
    MOCK(modules)::EXPECT(M1);
    MOCK(modules)::EXPECT(M6);
    MOCK(modules)::EXPECT(M4);
    CHECK(pet::Modules::init());
}

