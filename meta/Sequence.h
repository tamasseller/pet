/*
 * Sequence.h
 *
 *      Author: tamas.seller
 */

#ifndef SEQUENCE_H_
#define SEQUENCE_H_

namespace pet {

template<int... T>
class Sequence {};

namespace detail {

template<int... rest> struct SequenceMaker;

template<int min, int... rest> struct SequenceMaker<min, rest...> {
  using type = typename SequenceMaker<min - 1, min, rest...>::type;
};

template<int... rest> struct SequenceMaker<0, rest...> {
  using type = Sequence<0, rest...>;
};

template<> struct SequenceMaker<-1> {
        using type = Sequence<>;
};


template<int shift>
struct SequenceShifter {
        template<int... idx>
        static Sequence<(idx + shift)...> x(const Sequence<idx...>&) {return Sequence<(idx + shift)...>();}
};

}

template<int min, int max>
using sequence = decltype(detail::SequenceShifter<min>::x(typename detail::SequenceMaker<max - min - 1>::type()));

}

#endif /* SEQUENCE_H_ */
