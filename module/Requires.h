/*
 * Requires.h
 *
 *  Created on: 2017.05.21.
 *      Author: tooma
 */

#ifndef REQUIRES_H_
#define REQUIRES_H_

#include "ModuleInterface.h"

namespace pet {

template<class... Xs>
class Requires {
    protected:
        constexpr static auto nDeps = sizeof...(Xs);
        constexpr static ModuleInterface* deps[nDeps] = {&Xs::instance...};
};

template<class... Xs>
constexpr ModuleInterface* Requires<Xs...>::deps[];

}

#endif /* REQUIRES_H_ */
