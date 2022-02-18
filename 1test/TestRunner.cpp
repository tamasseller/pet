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

#include "ubiquitous/Registry.h"

#include "TestRunner.h"
#include "Mock.h"
#include "FailureInjector.h"

#include "setjmp.h" // Intentionally not <setjmp.h> !

namespace pet {

static jmp_buf jmpBuff;

LinkedPtrList<TestPlugin*> TestRunner::plugins;
TestInterface* TestRunner::currentTest;
TestOutput* TestRunner::output;
bool TestRunner::synthetic;
bool TestRunner::failing;

TestRunner::Result TestRunner::runTest(TestInterface* test)
{
    Result ret;

    synthetic = false;
    failing = false;
    FailureInjector::reset();
    
    while(true) 
    {
        currentTest = test;

        bool failed = false;
        if(setjmp(jmpBuff))
        {
            ret.failed++;
            failing = false;
            failed = true;
        }
        else
        {
            for(auto pluginIt = plugins.iterator(); pluginIt.current(); pluginIt.step())
            {
                pluginIt.current()->beforeTest();
            }

            currentTest->runTest();
        }

        for(auto pluginIt = plugins.iterator(); pluginIt.current(); pluginIt.step())
        {
            pluginIt.current()->afterTest(failed);
        }

        output->reportProgress(currentTest->getName(), synthetic);

        if (!synthetic)
        {
            FailureInjector::firstRunDone();
        }

        if(!FailureInjector::hasMore())
        {
            break;
        }

        currentTest->reset();

        FailureInjector::step();

        if(!synthetic)
        {
            if(failed)
                break;

            synthetic = true;
        }

        ret.synthetic++;
    }

    return ret;
}

int TestRunner::runAllTests(TestOutput* output, const char* filter)
{
    TestRunner::output = output;
    volatile unsigned int run = 0, failed = 0, synthetic = 0;

    for(auto it = Registry<TestInterface>::iterator(); it.current(); it.step())
    {
        if(!filter || it.current()->matches(filter))
        {
            auto ret = runTest(it.current());
            failed = failed + ret.failed;
            synthetic = synthetic + ret.synthetic;
            run = run + 1;
        }
    }

    currentTest = nullptr;
    TestRunner::output = nullptr;

    output->reportFinal(run, failed, synthetic);
    return failed ? -1 : 0;
}

int TestRunner::getTestCount(const char* filter)
{
    int ret = 0;

    for(auto it = Registry<TestInterface>::iterator(); it.current(); it.step())
    {
        if(!filter || it.current()->matches(filter))
        {
            ret++;
        }
    }

    return ret;
}

bool TestRunner::installPlugin(TestPlugin* plugin)  {
    return plugins.add(plugin);
}

void TestRunner::failTestAlways(const char* sourceInfo, const char* text)
{
    if(!failing)
    {
        failing = true;
        output->reportTestFailure(FailureInjector::getRerunOrdinal(), currentTest->getName(), currentTest->getSourceInfo(), sourceInfo, text);
        longjmp(jmpBuff, 1);
    }
}

void TestRunner::failTest(const char* sourceInfo, const char* text)
{
    if(!synthetic)
        failTestAlways(sourceInfo, text);
}

void TestRunner::checkExpectationAlways(bool condition, const char* sourceInfo, const char* text)
{
    if(!condition)
        failTestAlways(sourceInfo, text);
}

void TestRunner::checkExpectation(bool condition, const char* sourceInfo, const char* text)
{
    if(!condition)
        failTest(sourceInfo, text);
}

}
