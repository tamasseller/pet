#include "TestInterface.h"

using namespace pet;

static auto wcMatcherRecursionLimit = 5;

static bool wildcardMatcherWorker(const char* pattern, const char* input, int recLimit)
{
	const char *p = pattern;
	const char *str = input;

	while(true)
	{
		switch(*p)
		{
		case '?':
			if(!*str)
				return false;

			p++;
			str++;
			break;
		case '*':
			if(!recLimit)
				return false;

			do {
				p++;
			} while(*p == '*');

			do
			{
				if(wildcardMatcherWorker(p, str, recLimit - 1))
					return true;
			}
			while(*str++);

			return false;

		default:
			if(*p != *str)
				return false;

			if(*p++ == '\0')
				return true;

			str++;
		}
	}
}

bool TestInterface::matches(const char* filter) {
	return wildcardMatcherWorker(filter, getName(), wcMatcherRecursionLimit);
}
