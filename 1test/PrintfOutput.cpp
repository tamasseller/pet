#include "PrintfOutput.h"

#include <iostream>
#include <iomanip>

using namespace pet;

PrintfOutput PrintfOutput::instance;

void PrintfOutput::reportProgress()
{
	std::cout << ((++nDots % 64 == 0) ? ".\n" : ".") << std::flush;
}

void PrintfOutput::reportTestFailure(bool isSynthetic, const char* testName, const char* sourceInfo, const char *failureSourceInfo, const char *text)
{
	std::cout << std::endl << (isSynthetic ? "Synthetic test based on '" : "Test '") << testName << "' (" << sourceInfo <<  ")" << std::endl
			<< std::endl << "    failed at " << failureSourceInfo
			<< ((text) ? ": " : "") << ((text) ? text : "") << std::endl << std::endl;
}

void PrintfOutput::reportFinal(uint32_t normal, uint32_t failure, uint32_t synthetic)
{
	std::cout << std::endl << std::endl;

    if(failure)
    {
        if(synthetic)
        {
        	std::cout << "ERROR: " << failure << " of " << normal << " regular and " << synthetic << " synthetic tests failed !";
        }
        else
        {
        	std::cout << "ERROR: " << failure << " of " << normal << " tests failed !";
        }
    }
    else
    {
    	if(normal)
    	{
			if(synthetic)
			{
				std::cout << "OK: all " << normal << " regular and " << synthetic << " synthetic tests have been ran successfully.";
			}
			else
			{
				std::cout << "OK: all " << normal << " tests have been ran successfully.";
			}
    	}
    	else
    	{
    		std::cout << "No tests registered to run !";
    	}
    }

	std::cout << std::endl << std::endl;
}
