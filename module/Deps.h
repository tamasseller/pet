/*
 * Deps.h
 *
 *      Author: tamas.seller
 */

#ifndef DEPS_H_
#define DEPS_H_

#include "Service.h"

namespace pet {

template<class... Xs>
class Requires {
    protected:
        constexpr static auto nDeps = sizeof...(Xs);
        constexpr static Tag* deps[nDeps] = {&Xs::instance...};
};

template<class... Xs>
constexpr Tag* Requires<Xs...>::deps[];


template<class... Xs>
class Provides {
    protected:
        constexpr static auto nServices = sizeof...(Xs);
        constexpr static Tag* services[nServices] = {&Xs::instance...};
};

template<class... Xs>
constexpr Tag* Provides<Xs...>::services[];

}

#endif /* DEPS_H_ */
