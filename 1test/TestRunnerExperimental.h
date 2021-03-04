#ifndef _TESTRUNNEREXPERIMENTAL_H_
#define _TESTRUNNEREXPERIMENTAL_H_

#include "TestRunner.h"

namespace pet {

class TestRunner::Experimental 
{
    static void timerHandler(int);
    static int runTestSubset();
    
public:
    static int runTestsInParallel();
};

}

#endif /* _TESTRUNNEREXPERIMENTAL_H_ */
