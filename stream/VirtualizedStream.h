/*
 * VirtualizedStream.h
 *
 *  Created on: 2017.03.03.
 *      Author: tooma
 */

#ifndef VIRTUALIZEDSTREAM_H_
#define VIRTUALIZEDSTREAM_H_

#include "IndirectStream.h"

namespace pet {

template<class Backend>
class Renamer: public Backend {};

template<class Backend>
class VirtualizedInputStream: public IndirectInputStream, public Renamer<Backend> {
	virtual pet::GenericError nextReadable(char* &buff, uint32_t length) {
		return Renamer<Backend>::nextReadable(buff, length);
	}

	virtual pet::GenericError doneReading(char* &buff, uint32_t &offset) {
		return Renamer<Backend>::doneReading(buff, offset);
	}
};

template<class Backend>
class VirtualizedOutputStream: public IndirectOutputStream, public Renamer<Backend> {
	virtual pet::GenericError nextWritable(char* &buff, uint32_t length) {
		return Renamer<Backend>::nextWritable(buff, length);
	}

	virtual pet::GenericError doneWriting(char* &buff, uint32_t &offset) {
		return Renamer<Backend>::doneWriting(buff, offset);
	}
};

template<class Backend>
class VirtualizedIOStream: public IndirectIOStream, public Renamer<Backend> {
	virtual pet::GenericError nextReadable(char* &buff, uint32_t length) {
		return Renamer<Backend>::nextReadable(buff, length);
	}

	virtual pet::GenericError doneReading(char* &buff, uint32_t &offset) {
		return Renamer<Backend>::doneReading(buff, offset);
	}

	virtual pet::GenericError nextWritable(char* &buff, uint32_t length) {
		return Renamer<Backend>::nextWritable(buff, length);
	}

	virtual pet::GenericError doneWriting(char* &buff, uint32_t &offset) {
		return Renamer<Backend>::doneWriting(buff, offset);
	}
};

}

#endif /* VIRTUALIZEDSTREAM_H_ */
