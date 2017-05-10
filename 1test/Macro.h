/*
 * Macro.h
 *
 *      Author: tamas.seller
 */

#ifndef MACRO_H_
#define MACRO_H_

#define INTERNAL_STRINGIFY_2(x)                 #x
#define INTERNAL_STRINGIFY(x)                   INTERNAL_STRINGIFY_2(x)
#define INTERNAL_AT()                           __FILE__ ":" INTERNAL_STRINGIFY(__LINE__)

#define ARG_NUMBER_TAGGER(F, _1, _2, _3, real, ...) F##real
#define VAR_ARG_MACRO(F, ...) ARG_NUMBER_TAGGER(F, ##__VA_ARGS__, 3, 2, 1, 0)(__VA_ARGS__)

#endif /* MACRO_H_ */
