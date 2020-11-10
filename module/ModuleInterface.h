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

#ifndef MODULEINTERFACE_H_
#define MODULEINTERFACE_H_

namespace pet {

class ModuleInterface {
    bool visited = false;
	bool locked = false;

    friend class LinkedPtrList<ModuleInterface*>;
    ModuleInterface *next = nullptr;

    friend class Modules;
    virtual void init() = 0;
    virtual unsigned int requires(ModuleInterface* const * &) = 0;
public:
    inline virtual ~ModuleInterface() {}
};

}

#endif /* MODULEINTERFACE_H_ */
