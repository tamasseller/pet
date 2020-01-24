/*
 * StateChange.h
 *
 *  Created on: 2020. jan. 24.
 *      Author: tooma
 */

#ifndef PET_META_STATECHANGE_H_
#define PET_META_STATECHANGE_H_


template<class T1, class T2>
struct Pair {
    template<class Current> struct Applier;
};

template<class T1, class T2>
template<class N1, class N2>
struct Pair<T1, T2>::Applier<Pair<N1, N2>> {
    static void apply() {
        T1::template Applier<N1>::apply();
        T2::template Applier<N2>::apply();
    }
};


template<class... Params> struct Pairs;

template<class P, class... Params>
struct Pairs<P, Params...> {
    using Type = Pair<P, typename Pairs<Params...>::Type>;
};

struct Stopper {
    template<class Current> struct Applier;
};

template<>
struct Stopper::Applier<Stopper> {
    static void apply() {}
};

template<>
struct Pairs<> {
    using Type = Stopper;
};

template<class... Params> struct Compound
{
    using Value = typename Pairs<Params...>::Type;
};

#define STATE(stateVar)                \
template<class> struct stateVar;

#define VALUE(stateVar, value)                                                      \
class value;                                                                        \
template<> struct stateVar<value> {                                                 \
    template<class> struct Applier;                                                 \
};                                                                                  \
                                                                                    \
template<> template<>                                                               \
struct stateVar<value>::Applier<stateVar<value>> { static void apply() {} };        \
                                                                                    \
template<> template<class otherValue>                                               \
struct stateVar<value>::Applier<stateVar<otherValue>> {                             \
    static void apply();                                                            \
};                                                                                  \
                                                                                    \
template<> template<class otherValue>                                               \
void stateVar<value>::Applier<stateVar<otherValue>>::apply()

template<class Current, class New>
static void apply()
{
    New::template Applier<Current>::apply();
}

#endif /* PET_META_STATECHANGE_H_ */
