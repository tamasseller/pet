#ifndef _TESTRUNNEREXPERIMENTAL_H_
#define _TESTRUNNEREXPERIMENTAL_H_

#include "TestRunner.h"

namespace pet {

class TestRunner::Experimental 
{
    static void timerHandler(int);
    static int runTestSubset(int timeLimitSec);
    
public:
    static int runTestsInParallel(int timeLimitSec = 30);
};

}

#endif /* _TESTRUNNEREXPERIMENTAL_H_ */
