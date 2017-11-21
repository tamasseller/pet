/*******************************************************************************
 *
 * Copyright (c) 2016, 2017 Seller Tamás. All rights reserved.
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

#ifndef UNION_H_
#define UNION_H_

#include "meta/TypePackInfo.h"

namespace pet {

template<class... T>
class Union {
    static_assert(TypePackInfo<T...>::allUnique, "All types of Union must be unique");
    static_assert(TypePackInfo<T...>::noReferences, "Refernces are not allowed in Unions.");
    static_assert(sizeof...(T) < 255, "No more than 254 can be used for a Union");

    char data[(TypePackInfo<T...>::size + sizeof(char) - 1) / sizeof(char)] alignas(TypePackInfo<T...>::alignment);
    char tag = -1;

    typedef void (*dtorLink)(void*, size_t);
    inline static void nopDtorLink(void*, size_t) {}

    template<dtorLink rest, class U, class...>
    struct DtorAccessor {
        U u;

        static inline void step(void* data, size_t n) {
            if(!n)
                reinterpret_cast<DtorAccessor*>(data)->~DtorAccessor();
            else
                rest(data, n - 1);
        }

        static constexpr dtorLink value = &step;
    };

    typedef void *(*baseLink)(void*, size_t);
    typedef const void *(*cbaseLink)(const void*, size_t);
    inline static void* nopBaseLink(void*, size_t) {return nullptr;}
    inline static const void* cnopBaseLink(const void*, size_t) {return nullptr;}

    template<class B, class D=void*, class BL=baseLink>
    struct BaseAccessors {
        template<BL rest, class U, class...>
        struct Accessor {
            static inline D step(D data, size_t n) {
                if(!n)
                    return static_cast<B*>(reinterpret_cast<U*>(const_cast<void*>(data)));
                else
                    return rest(data, n - 1);
            }

            static constexpr BL value = &step;
        };
    };

public:
    template<class R, class... Args>
    void initAs(Args&&... args) {
        static_assert(TypePackInfo<T...>::template Position<R>::value != -1, "Can not initialize as non-contained type.");
        this->~Union();
        tag = TypePackInfo<T...>::template Position<R>::value;
        new(data) R(args...);
    }

    template<class R>
    R &accessAs() {
        static_assert(TypePackInfo<T...>::template Position<R>::value != -1, "Can not get as non-contained type.");
        if(tag != TypePackInfo<T...>::template Position<R>::value)
            return *static_cast<R*>(nullptr);

        return *reinterpret_cast<R*>(data);
    }

    template<class R>
    const R &accessAs() const {
        static_assert(TypePackInfo<T...>::template Position<R>::value != -1, "Can not get as non-contained type.");
        if(tag != TypePackInfo<T...>::template Position<R>::value)
            return *static_cast<const R*>(nullptr);

        return *reinterpret_cast<const R*>(data);
    }

    template<class R>
    R &accessAsBase() {
        return *reinterpret_cast<R*>((ApplyToPack<baseLink, BaseAccessors<R>::template Accessor, nopBaseLink, T...>::value)(data, tag));
    }

    template<class R>
    const R &accessAsBase() const {
        return *reinterpret_cast<const R*>((ApplyToPack<cbaseLink, BaseAccessors<R, const void*, cbaseLink>::template Accessor, cnopBaseLink, T...>::value)(data, tag));
    }

    Union() = default;

    template<class R>
    Union(const R& r) {
        initAs<R>(r);
    }

    template<class R>
    Union(R&& r) {
        initAs<R>(r);
    }

    ~Union() {
        (ApplyToPack<dtorLink, DtorAccessor, nopDtorLink, T...>::value)(data, tag);
    }
};

}

#endif /* UNION_H_ */
