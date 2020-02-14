/*******************************************************************************
 *
 * Copyright (c) 2020 Seller Tamás. All rights reserved.
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

#ifndef RESETTABLE_H_
#define RESETTABLE_H_

#include <stddef.h>

struct NewOperatorDisambiguator{};
inline void* operator new(size_t _, void* ret, const NewOperatorDisambiguator&) { return ret; }

namespace pet
{

template<class Child>
class Resettable
{
protected:
    inline void reset() {
        new(static_cast<Child*>(this), NewOperatorDisambiguator()) Child();
    }
};

}

#endif /* RESETTABLE_H_ */
