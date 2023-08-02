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

#include <sys/sysinfo.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/uio.h>

#include <signal.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>

#include <set>
#include <atomic>
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <optional>

#include "TestRunnerExperimental.h"

namespace pet {

struct SharedState: TestOutput
{
    static inline struct SharedState* instance;

    std::atomic<int> run, next, failed, synthetic, dot;
    inline SharedState(): run(0), next(0), failed(0), synthetic(0), dot(0) {}

    inline virtual void reportProgress() 
    {
        if(++dot % 64 == 0)
            write(STDOUT_FILENO, ".\n", 2);
        else
            write(STDOUT_FILENO, ".", 1);
    }

    inline virtual void reportTestFailure(uint32_t rerunIdx, const char* testName, const char* sourceInfo, const char *failureSourceInfo, const char *text)
    {
        std::stringstream ss;

        ss << std::endl;

        if(rerunIdx)
        {
            ss << "Synthetic #" << rerunIdx << " test based on '";
        }
        else
        {
            ss << "Test '";
        }


        ss << testName << "' (" << sourceInfo <<  ")" << std::endl << std::endl;
        ss << "    failed at " << failureSourceInfo << ": " << text << std::endl << std::endl;

        auto str = ss.str();
        write(STDOUT_FILENO, str.c_str(), str.length());
    }

    inline virtual void reportFinal(uint32_t normal, uint32_t failure, uint32_t synthetic) 
    {
        std::stringstream ss;
        ss << std::endl;

        if(failure) 
        {
            ss << "ERROR (" << failure << " of " << normal << " regular ";

            if(synthetic)
                ss << "plus " << synthetic << " synthetic ";

            ss << "tests failed)";
        } 
        else 
        {
            if(normal) 
            {
                ss << "OK (all " << normal << " regular ";

                if(synthetic)
                    ss << "and " << synthetic << " synthetic ";

                ss << "tests have been ran successfully)";
            }
            else
            {
                ss << "No tests registered to run !";
            }
        }

        ss << std::endl << std::endl;
        auto str = ss.str();
        write(STDOUT_FILENO, str.c_str(), str.length());
    }

    virtual inline ~SharedState() {}
};

void TestRunner::Experimental::timerHandler(int signum)
{
    char buffer[1024];
    auto l = sprintf(buffer, "\n>>> Test is taking too long: '%s' at: %s <<<\n", 
        TestRunner::getCurrentTest()->getName(), 
        TestRunner::getCurrentTest()->getSourceInfo());

    char* p = buffer + l;

    for(auto i = 0; i < SharedState::instance->dot % 64; i++)
        *p++ = ' ';

    write(STDOUT_FILENO, buffer, p - buffer);
}

inline int TestRunner::Experimental::runTestSubset(const char* filter, int timeLimitSec)
{
    int idx = 0, reqIdx = SharedState::instance->next++;
    auto it = Registry<TestInterface>::iterator();

    struct sigaction sa;
    memset(&sa, 0, sizeof (sa));
    sa.sa_handler = &timerHandler;
    sigaction (SIGVTALRM, &sa, NULL);

    struct itimerval timer;
    memset(&timer, 0, sizeof(timer));
    timer.it_value.tv_sec = timeLimitSec;

    while(true)
    {
        if(idx < reqIdx)
        {
            it.step();
            if(!it.current())
                break;

            idx++;
        }
        else
        {
            assert(idx == reqIdx);

            if(!filter || it.current()->matches(filter))
            {
                setitimer(ITIMER_VIRTUAL, &timer, NULL);
                clock_t begin = clock();
                auto ret = runTest(it.current());
                double time_spent = (double)(clock() - begin) / CLOCKS_PER_SEC;

                SharedState::instance->failed += ret.failed;
                SharedState::instance->synthetic += ret.synthetic;
                SharedState::instance->run++;

                if(time_spent > timeLimitSec)
                {
                    char buffer[1024];
                    auto l = sprintf(buffer, "\n!!! Test took %.1fs: '%s' at: %s !!!\n",
                        time_spent,
                        TestRunner::getCurrentTest()->getName(),
                        TestRunner::getCurrentTest()->getSourceInfo());

                    char* p = buffer + l;

                    for(auto i = 0; i < SharedState::instance->dot % 64; i++)
                        *p++ = ' ';

                    write(STDOUT_FILENO, buffer, p - buffer);
                }
            }

            reqIdx = SharedState::instance->next++;
        }
    }

    return 0;
}

int TestRunner::Experimental::runTestsInParallel(int timeLimitSec, const char* filter)
{
    auto ptr = mmap(NULL, sizeof(SharedState), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    SharedState::instance = new(ptr) SharedState;
    TestRunner::output = SharedState::instance;

    auto nprocs = get_nprocs();
    assert(nprocs > 0);
    std::set<pid_t> children;

    for(int i = 0; i < nprocs + 1; i++)
    {
        auto pid = fork();

        if(pid < 0)
            return -2;
        else if(pid > 0)
            children.insert(pid);
        else
            return runTestSubset(filter, timeLimitSec);
    }

    int ret = 0;
    while(children.size())
    {
        int status;
        auto pid = waitpid(-1, &status, 0);

        if(pid <= 0)
            return -3;

        if(!WIFEXITED(status) || WEXITSTATUS(status) != 0)
        {
            std::cerr << std::endl << "Child " << pid << " exited abnormally!" << std::endl << std::endl;

            for(auto ch: children)
            {
                kill(ch, SIGTERM);
            }

            ret = -4;
            break;
        }

        if(children.erase(pid) != 1)
            return -5;
    }
    
    assert(SharedState::instance->run == pet::TestRunner::getTestCount(filter));

    SharedState::instance->reportFinal(
        SharedState::instance->run, 
        SharedState::instance->failed, 
        SharedState::instance->synthetic
    );

    return ret;
}

int TestRunner::Experimental::main(int argc, const char* argv[], TestOutput* output)
{
    const std::vector<std::string> args{argv, argv + argc};

    std::optional<bool> doParallel;
    std::optional<std::string> filter;
    bool show = false;

    for(auto it = args.cbegin(); it != args.cend();)
    {
        const auto name = *it++;

        if(name == "--show" || name == "-s")
        {
            show = true;
        }
        else if(name == "--no-parallel" || name == "-n")
        {
            doParallel = false;
        }
        else if(name == "--parallel" || name == "-p")
        {
            doParallel = true;
        }
        else if(name == "--filter" || name == "-f")
        {
            if(it == args.cend())
            {
                std::cerr << "Filter expression expected after '-f' option" << std::endl;
                return -1;
            }
            else
            {
                filter = *it++;
            }
        }
    }

    const char* const f = filter.has_value() ? filter->c_str() : nullptr;

    if(show)
    {
        int n = 0;
        for(auto it = Registry<TestInterface>::iterator(); it.current(); it.step())
        {
            const char* name = it->getName();
            const char nl = '\n';
            char buff[32];

            const struct iovec v[] =
            {
                    {.iov_base = const_cast<char*>(buff), .iov_len = (size_t)snprintf(buff, sizeof(buff), "%03d: ", ++n)},
                    {.iov_base = const_cast<char*>(name), .iov_len = strlen(name)},
                    {.iov_base = const_cast<char*>(&nl), 1},
            };

            writev(STDOUT_FILENO, v, sizeof(v)/sizeof(v[0]));
        }
    }

    if(doParallel.value_or(pet::TestRunner::getTestCount(f) > 50))
    {
        return pet::TestRunner::Experimental::runTestsInParallel(30, f);
    }
    else
    {
        return pet::TestRunner::runAllTests(output, f);
    }
}

}
