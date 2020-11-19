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

#ifndef MODULE_H_
#define MODULE_H_

#include "ubiquitous/Registry.h"
#include "ModuleInterface.h"
#include "Requires.h"

namespace pet {

template<class Child, class Reqs = Requires<>>
class Module: public Registry<ModuleInterface>::StaticElement<Child>, public Reqs {
    protected:
        inline void installModule() {}

        inline virtual unsigned int requires(ModuleInterface* const * &reqs) {
            reqs = Reqs::deps;
            return Reqs::nDeps;
        }

        template<class...> friend class Requires;
        using Registry<ModuleInterface>::StaticElement<Child>::instance;
    public:
        inline virtual ~Module() {}
};

}

#endif /* MODULE_H_ */
