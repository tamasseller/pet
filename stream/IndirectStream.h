/*
 * IndirectStream.h
 *
 *  Created on: 2017.03.03.
 *      Author: tooma
 */

#ifndef INDIRECTSTREAM_H_
#define INDIRECTSTREAM_H_

#include "Stream.h"

namespace pet {

class IndirectInputStream: public InputStream<IndirectInputStream> {
	friend InputStream<IndirectInputStream>;
	virtual pet::GenericError nextReadable(char* &buff, uint32_t length) = 0;
	virtual pet::GenericError doneReading(char* &buff, uint32_t &offset) = 0;
public:

	inline virtual ~IndirectInputStream () {}
};

class IndirectOutputStream: public OutputStream<IndirectOutputStream> {
	friend OutputStream<IndirectOutputStream>;
	virtual pet::GenericError nextWritable(char* &buff, uint32_t length) = 0;
	virtual pet::GenericError doneWriting(char* &buff, uint32_t &offset) = 0;

public:
	inline virtual ~IndirectOutputStream () {}
};

class IndirectIOStream: public IndirectInputStream, public IndirectOutputStream {};

}

#endif /* INDIRECTSTREAM_H_ */
