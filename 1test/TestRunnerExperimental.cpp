#include <sys/sysinfo.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/time.h>

#include <signal.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>

#include <set>
#include <atomic>
#include <sstream>

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

	inline virtual void reportTestFailure(bool isSynthetic, const char* testName, const char* sourceInfo, const char *failureSourceInfo, const char *text) 
    {
        std::stringstream ss;

        ss << std::endl << (isSynthetic ? "Synthetic test based on '" : "Test '") 
            << testName << "' (" << sourceInfo <<  ")" << std::endl << std::endl;

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

inline int TestRunner::Experimental::runTestSubset(int timeLimitSec)
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

            reqIdx = SharedState::instance->next++;
        }
	}

    return 0;
}

int TestRunner::Experimental::runTestsInParallel(int timeLimitSec)
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
            return runTestSubset(timeLimitSec);
    }

    int ret = 0;
    while(children.size())
    {
        int status;
        auto pid = waitpid(-1, &status, 0);

        if(pid <= 0)
            return -3;

        if(!WIFEXITED(status) || WEXITSTATUS(status) != 0)
            ret = -4;

        if(children.erase(pid) != 1)
            return -5;
    }
    
    SharedState::instance->reportFinal(
        SharedState::instance->run, 
        SharedState::instance->failed, 
        SharedState::instance->synthetic
    );
    return ret;
}

}
