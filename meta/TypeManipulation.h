/*
 * TypeManipulation.h
 *
 *      Author: tamas.seller
 */

#ifndef TYPEMANIPULATION_H_
#define TYPEMANIPULATION_H_

namespace pet {

namespace detail {

template< class T > struct RreferenceRemover {typedef T type;};
template< class T > struct RreferenceRemover<T&> {typedef T type;};
template< class T > struct RreferenceRemover<T&&> {typedef T type;};


}

template<class T>
using removeReference = typename detail::RreferenceRemover<T>::type;

template<class T, class U> struct sameTypes { static constexpr bool value = false; };
template<class T> struct sameTypes<T, T> { static constexpr bool value = true; };

template<bool B, class T=void> struct enableIf {};
template<class T> struct enableIf<true, T> { typedef T type; };

/// TODO forwarding

}

#endif /* TYPEMANIPULATION_H_ */
