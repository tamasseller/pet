/*******************************************************************************
 *
 * Copyright (c) 2020 Tamás Seller. All rights reserved.
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

#ifndef ATOMICCOMMON_H_
#define ATOMICCOMMON_H_

namespace pet {

namespace IntelArchCommon {

template<class Value, class CasHolder>
class Atomic
{
    volatile Value data;
public:
    inline Atomic(): data(0) {}

    inline Atomic(Value value) {
        data = value;
    }

    inline operator Value() {
        return data;
    }

    template<class Op, class... Args>
    really_inline Value operator()(Op&& op, Args... args)
    {
        Value old, result;

        do
        {
            old = this->data;

            if(!op(old, result, args...))
            {
                break;
            }

        } while(unlikely(!CasHolder::cas(&this->data, old, result)));

        return old;
    }

    really_inline bool compareAndSwap(Value expectedValue, Value newValue) {
        return CasHolder::cas(&this->data, expectedValue, newValue);
    }

    really_inline Value swap(Value newValue)
    {
        Value ret;

        do
        {
            ret = this->data;
        }
        while(unlikely(!CasHolder::cas(&this->data, ret, newValue)));

        return ret;
    }
};

}

}

#endif /* ATOMICCOMMON_H_ */
