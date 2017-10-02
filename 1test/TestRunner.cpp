/*******************************************************************************
 *
 * Copyright (c) 2016, 2017 Seller Tamás. All rights reserved.
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

LinkedList<TestPlugin> TestRunner::plugins;
TestInterface* TestRunner::currentTest;
TestOutput* TestRunner::output;
bool TestRunner::isSynthetic;


int TestRunner::runAllTests(TestOutput* output)
{
	TestRunner::output = output;
	volatile unsigned int run = 0, failed = 0, synthetic = 0;

	for(auto it = Registry<TestInterface>::iterator(); it.current(); it.step()) {
	    isSynthetic = false;
	    FailureInjector::reset();
	    while(true) {
	        currentTest = it.current();

	        if(setjmp(jmpBuff))
	            failed++;
	        else {
	        	for(auto pluginIt = plugins.iterator(); pluginIt.current(); pluginIt.step())
	        		pluginIt.current()->beforeTest();

	        	currentTest->runTest();

	        }
        	for(auto pluginIt = plugins.iterator(); pluginIt.current(); pluginIt.step())
        		pluginIt.current()->afterTest();


            output->reportProgress();

            if(!isSynthetic) {
                FailureInjector::firstRunDone();
            }

	        if(!FailureInjector::hasMore())
	            break;

	        FailureInjector::step();

	        synthetic++;
	        isSynthetic = true;
	    }

        run++;
	}

	currentTest = 0;
	TestRunner::output = 0;

    output->reportFinal(run, failed, synthetic);
    return failed ? -1 : 0;
}

bool TestRunner::installPlugin(TestPlugin* plugin) {
	return plugins.add(plugin);
}


void TestRunner::failTestAlways(const char* sourceInfo, const char* text)
{
    output->reportTestFailure(isSynthetic, currentTest->getName(), currentTest->getSourceInfo(), sourceInfo, text);
    longjmp(jmpBuff,1);
}

void TestRunner::failTest(const char* sourceInfo, const char* text)
{
    if(!isSynthetic)
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
