/*******************************************************************************
 *
 * Copyright (c) 2022 Tamás Seller. All rights reserved.
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
