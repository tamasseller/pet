/*******************************************************************************
 *
 * Copyright (c) 2021 Tamás Seller. All rights reserved.
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

#include "PrintfOutput.h"

#include <iostream>
#include <iomanip>

using namespace pet;

PrintfOutput PrintfOutput::instance;

void PrintfOutput::reportProgress()
{
	std::cout << ((++nDots % 64 == 0) ? ".\n" : ".") << std::flush;
}

void PrintfOutput::reportTestFailure(uint32_t rerunIdx, const char* testName, const char* sourceInfo, const char *failureSourceInfo, const char *text)
{
	std::cout << std::endl;

	if(rerunIdx)
	{
		std::cout << "Synthetic test #" << rerunIdx << " based on '";
	}
	else
	{
		std::cout << "Test '";
	}


	std::cout << testName << "' (" << sourceInfo <<  ")" << std::endl << std::endl;
	std::cout << "    failed at " << failureSourceInfo;

	if(text)
	{
		std::cout << ": " << text;
	}

	std::cout << std::endl << std::endl;
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
