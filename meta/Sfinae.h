/*
 * Sfinae.h
 *
 *  Created on: 2018.01.08.
 *      Author: tooma
 */

#ifndef SFINAE_H_
#define SFINAE_H_

template<bool b, class Type = void> struct EnableIf;
template<class T> struct EnableIf<true, T> { typedef T Type;};

#endif /* SFINAE_H_ */
