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

#ifndef DELEGATE_H_
#define DELEGATE_H_

#include "meta/Utility.h"
#include "meta/Resettable.h"

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
    /**
     * The type of a to a free-standing function whose signature is the same as the delegate's
     */
    using TPtr = Retval (*)(Args...);

    /**
     * The type of a pointer to the trampoline function used internally to call the actual target.
     */
    using FPtr = Retval (*)(uintptr_t, Args...);

    /**
     * Pointer to the automatically generated trampoline function used to pass control to the target.
     */
    FPtr f;

    /**
     * Single-pointer sized data storage used by the trampoline in a target type dependent way.
     */
    uintptr_t data;

    /**
     * Constructor for the free-standing function target case, the data element is not used,
     * thus it is not initialized.
     */
    inline constexpr Delegate(FPtr f): f(f) {}

    /**
     * Constructor for the member function target case, the data element contains
     * a pointer to instance the member is to be called on.
     */
    inline constexpr Delegate(FPtr f, uintptr_t data): data(data), f(f) {}

    /**
     * Constructor for the lambda target case, the data element contains a copy of the lambda object.
     */
    template<class C>
    inline Delegate(FPtr f, C&& c): f(f)
    {
        static_assert(sizeof(C) <= sizeof(uintptr_t));
        new(&this->data, NewOperatorDisambiguator()) C(pet::move(c));
    }

    /**
     * Trampoline implementation for the free-standing function target case.
     */
    template<TPtr StaticFunctionPointer>
    struct StaticCallHelper
    {
        static inline auto call(uintptr_t data, Args... args) {
            return StaticFunctionPointer(args...);
        }
    };

    /**
     * Trampoline implementation for the member function case.
     */
    template<class MemberPointerType, MemberPointerType PointerToMember, class EnclosingType>
    struct MemberCallHelper
    {
        static inline auto call(uintptr_t data, Args... args) {
            return (reinterpret_cast<EnclosingType*>(data)->*PointerToMember)(args...);
        }
    };

    /**
     * Trampoline implementation for the lambda target case.
     */
    template<class LambdaType>
    struct LambdaCallHelper
    {
        static inline auto call(uintptr_t data, Args... args) {
            return reinterpret_cast<LambdaType*>(&data)->operator()(args...);
        }
    };

public:
    /**
     * Constructor method for a free-standing function target.
     */
    template<TPtr StaticFunctionPointer>
    static inline constexpr Delegate create() {
        return Delegate(&StaticCallHelper<StaticFunctionPointer>::call);
    }

    /**
     * Constructor method for a mebmer function target.
     */
    template<class MemberPointerType, MemberPointerType PointerToMember, class EnclosingType>
    static inline constexpr Delegate create(EnclosingType* instance) {
        return Delegate(&MemberCallHelper<MemberPointerType, PointerToMember, EnclosingType>::call, reinterpret_cast<uintptr_t>(instance));
    }

    /**
     * Implicit constructor for a lambda target.
     */
    template<class C>
    inline Delegate(C &&c): Delegate(&LambdaCallHelper<C>::call, pet::move(c)) {}

    /**
     * The invocation operator, that transfers control to the target.
     */
    inline Retval operator()(Args... args) {
        return f(this->data, args...);
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
 * Delegate builder method with automatic type deduction for a free-standing function (requires C++17).
 */
template<auto StaticFunctionPointer>
static inline constexpr auto delegate()
{
    using T = Delegate<typename detail::template StripStaticSignature<decltype(StaticFunctionPointer)>::Type>;
    return T::template create<StaticFunctionPointer>();
}

/**
 * Delegate builder method with automatic type deduction for a member function (requires C++17).
 */
template<auto Member, class Type>
static inline constexpr auto delegate(Type* instance)
{
    using T = Delegate<typename detail::template StripMemberSignature<decltype(Member)>::Type>;
    return T::template create<decltype(Member), Member, Type>(instance);
}

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
