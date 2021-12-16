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

#ifndef DELEGATE_H_
#define DELEGATE_H_

#include "meta/Utility.h"
#include "meta/Resettable.h"
#include "meta/TypeManipulation.h"

#include <stdint.h>

namespace pet
{

/**
 * Generalized function pointer-like structure, capable of calling members of objects.
 *
 * Similar in basic function to the std::function template class of the STL, but get
 * around using dynamic memory allocations by restricting the size of the closure to
 * a single pointer, which enables it represent calls to member functions but it can
 * also 'contain' a lambda object whose size is not greater than a single pointer's.
 */
template<class C> class Delegate;

template<class Retval, class... Args>
class Delegate<Retval(Args...)>
{
    struct VTable
    {
        Retval (*const call)(uintptr_t*, Args...);
        void (*const relocate)(uintptr_t*, uintptr_t*);
        void (*const destroy)(uintptr_t*);
    };

    /**
     * Pointer to the automatically generated trampoline function used to pass control to the target.
     */
    const VTable* f = nullptr;

    /**
     * Single-pointer sized data storage used by the trampoline in a target type dependent way.
     */
    uintptr_t data;

    /**
     * Constructor for the lambda target case, the data element contains a copy of the lambda object.
     */
    template<class C>
    inline Delegate(const VTable *f, C&& c): f(f)
    {
        static_assert(sizeof(C) <= sizeof(uintptr_t));
        new(&this->data, NewOperatorDisambiguator()) C(pet::move(c));
    }

    /**
     * Trampoline implementation for the lambda target case.
     */
    template<class LambdaType>
    struct LambdaCallHelper
    {
        static inline auto call(uintptr_t *data, Args... args) {
            return reinterpret_cast<LambdaType*>(data)->operator()(pet::forward<Args>(args)...);
        }

        static inline void move(uintptr_t *old, uintptr_t *current) {
            new(current, NewOperatorDisambiguator()) LambdaType(pet::move(*(reinterpret_cast<LambdaType*>(old))));
            reinterpret_cast<LambdaType*>(old)->~LambdaType();
        }

        static inline void destroy(uintptr_t *data) {
            reinterpret_cast<LambdaType*>(data)->~LambdaType();
        }

        static constexpr const inline auto vtable = VTable{&call, &move, &destroy};
    };

public:
    /**
     * Potentially dangerous default constructor that leaves the data field uninitialized.
     */
    inline Delegate() = default;

    /**
     * Copying is forbidden in order to relax requirements on the type of the captured object.
     */
    inline Delegate(const Delegate&) = delete;
    inline Delegate& operator =(const Delegate&) = delete;

    /**
     * Moving is allowed and constructor is forwarded accordingly to the captured object, thus
     * it is required to be movable.
     */
    inline Delegate(Delegate&& o): f(o.f)
    {
    	if(f)
    		f->relocate(&o.data, &data);

        o.f = nullptr;
    }

    inline Delegate& operator =(Delegate&& o)
    {
        if(f)
            f->destroy(&data);

        f = o.f;

        if(f)
        	f->relocate(&o.data, &data);

        o.f = nullptr;
        return *this;
    }

    /**
     * Implicit constructor for a lambda target.
     */
    template<class C>
    inline Delegate(C &&c): Delegate(&LambdaCallHelper<removeReference<C>>::vtable, pet::move(c)) {}

    /**
     * The invocation operator, that transfers control to the target.
     */
    template<class... InvokeArgs>
    inline Retval operator()(InvokeArgs&&... args) {
        return (f->call)(&this->data, pet::forward<InvokeArgs>(args)...);
    }

    inline void clear()
    {
    	/*
    	 * Copy content in order to allow the destructor to free an object that happens to
    	 * contain the delegate itself without the risk of heap corruption.
    	 */
        if(auto temp = f)
        {
        	f = nullptr;
            (temp->destroy)(&this->data);
        }
    }

    inline operator bool() {
		return f;
    }

    /**
     * Destructor that take care of delegating the destructor call to the captured data.
     */
    inline ~Delegate()
    {
        if(f)
            (f->destroy)(&this->data);
    }
};

namespace detail
{
    /*
     * Meta helper to extract signature from a pointer-to-member-function.
     */
    template<class Ms> struct StripMemberSignature;
    template<class Class, class Retval, class... Args>
    struct StripMemberSignature<Retval (Class::*)(Args...)> {
        using Type = Retval(Args...);
    };

    template<class Class, class Retval, class... Args>
    struct StripMemberSignature<Retval (Class::*)(Args...) const> {
        using Type = Retval(Args...);
    };

    /*
     * Meta helper to extract signature of a function from a pointer to it.
     */
    template<class Ss> struct StripStaticSignature;
    template<class Retval, class... Args>
    struct StripStaticSignature<Retval (*)(Args...)> {
        using Type = Retval(Args...);
    };
}

#if __cplusplus >= 201703L

/**
 * Delegate builder method with automatic type deduction for a lambda target (requires C++17).
 */
template<class C>
static inline auto delegate(C &&c)
{
    using T = Delegate<typename detail::template StripMemberSignature<decltype(&C::operator())>::Type>;
    return T(pet::move(c));
}

#endif

}


#endif /* UBIQUITOUS_DELEGATE_H_ */
