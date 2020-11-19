/*******************************************************************************
 *
 * Copyright (c) 2020 Tamás Seller. All rights reserved.
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
#include "1test/Mock.h"

#include "ubiquitous/Delegate.h"
#include "managed/RefCnt.h"

#include "MockAllocator.h"

TEST_GROUP(RefCntDelegateIntegration)
{
    struct TestEntity: pet::RefCnt<TestEntity, Allocator>
    {
        inline TestEntity() = default;
        inline TestEntity(const TestEntity&) = delete;
        inline TestEntity(TestEntity&&) = delete;

        inline ~TestEntity()
        {
            MOCK(TestEntity)::CALL(Dtor).withParam(this);
        }

        inline void target()
        {
            MOCK(TestEntity)::CALL(Target);
        }

        inline void* tell()
        {
            return this;
        }

        inline void learn(void* v)
        {
            MOCK(TestEntity)::CALL(Learn).withParam(v);
        }

        using VoidPtrParamCallback = pet::Delegate<void(void*)>;
        using ServiceCall = pet::Delegate<void(VoidPtrParamCallback)>;

        ServiceCall service()
        {
            return pet::delegate([self{self()}](VoidPtrParamCallback cb){
                cb(self->tell());
            });
        }

        using NoParamCallback = pet::Delegate<void(void)>;
        NoParamCallback registeredCallback;

        using RegistrationCall = pet::Delegate<void(NoParamCallback)>;
        inline RegistrationCall registration()
        {
            return pet::delegate([self{self()}](NoParamCallback cb){
                self->registeredCallback = std::move(cb);
            });
        }

        inline void trigger() {
            registeredCallback();
        }

        using UnRegistration = pet::Delegate<void()>;
        inline UnRegistration unRegistration()
        {
            return pet::delegate([self{self()}](){
                self->registeredCallback.clear();
            });
        }
    };
};

TEST(RefCntDelegateIntegration, CtorDtorSanity)
{
    pet::Delegate<void()> d;
    void* exp;

    {
        auto ptr = TestEntity::make();
        exp = ptr.get();
        d = pet::delegate([ptr](){ptr->target();});
    }

    MOCK(TestEntity)::EXPECT(Target);
    d();

    MOCK(TestEntity)::EXPECT(Dtor).withParam(exp);
}

TEST(RefCntDelegateIntegration, BackAndForthCall)
{
    TestEntity::ServiceCall srv;
    TestEntity::VoidPtrParamCallback cli;
    void *cliAddr, *srvAddr;

    {
        auto server = TestEntity::make();
        srvAddr = server.get();
        srv = server->service();
    }

    {
        auto client = TestEntity::make();
        cliAddr = client.get();
        cli = [self{client}](void* val){self->learn(val);};
    }

    MOCK(TestEntity)::EXPECT(Learn).withParam(srvAddr);
    MOCK(TestEntity)::EXPECT(Dtor).withParam(cliAddr);
    MOCK(TestEntity)::EXPECT(Dtor).withParam(srvAddr);

    srv(pet::move(cli));
}

TEST(RefCntDelegateIntegration, RegisteredCallbackUnregisterDestroy)
{
    TestEntity::Ptr<TestEntity> server;
    void *cliAddr, *srvAddr;

    {
        server = TestEntity::make();
        srvAddr = server.get();

        auto client = TestEntity::make();
        cliAddr = client.get();

        (server->registration())([self{client}](){self->target();});

        MOCK(TestEntity)::EXPECT(Target);
        server->trigger();
    }

    MOCK(TestEntity)::EXPECT(Target);
    server->trigger();

    MOCK(TestEntity)::EXPECT(Dtor).withParam(cliAddr);
    (server->unRegistration())();

    MOCK(TestEntity)::EXPECT(Dtor).withParam(srvAddr);
}

TEST(RefCntDelegateIntegration, RegisteredCallbackCascadeDestroy)
{
    TestEntity::Ptr<TestEntity> server;
    void *cliAddr, *srvAddr;

    {
        server = TestEntity::make();
        srvAddr = server.get();

        auto client = TestEntity::make();
        cliAddr = client.get();

        (server->registration())([self{client}](){self->target();});

        MOCK(TestEntity)::EXPECT(Target);
        server->trigger();
    }

    MOCK(TestEntity)::EXPECT(Target);
    server->trigger();

    MOCK(TestEntity)::EXPECT(Dtor).withParam(srvAddr);
    MOCK(TestEntity)::EXPECT(Dtor).withParam(cliAddr);
    server = nullptr;
}
