/*
Copyright (c) 2019, Yasuhiro Hasegawa
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

  1. Redistributions of source code must retain the above copyright notice,
     this list of conditions and the following disclaimer.
  2. Redistributions in binary form must reproduce the above copyright notice,
     this list of conditions and the following disclaimer in the documentation
     and/or other materials provided with the distribution.
  3. The name of the author may not be used to endorse or promote products derived
     from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
OF SUCH DAMAGE.
*/


//
// CourierRaw.h
//

#ifndef COURIER_COURIER_RAW_H_
#define COURIER_COURIER_RAW_H_

#include <QtCore>

#include "../courier/Block.h"

#define COURIER_ERROR() { logger.fatal("ERROR %s %d %s", __FILE__, __LINE__, __FUNCTION__); throw Courier::CourierError(__FILE__, __LINE__, __FUNCTION__); }

namespace Courier {

class CourierError {
public:
	const char *file;
	const int   line;
	const char *func;

	CourierError(const char *file_, const int line_, const char *func_) : file(file_), line(line_), func(func_) {}
};

using BLOCK         = Block;
using BYTE          = quint8;
using BOOLEAN       = bool;
using CARDINAL      = quint16;
using LONG_CARDINAL = quint32;
using STRING        = QString;
using UNSPECIFIED   = quint16;
using UNSPECIFIED2  = quint32;
using UNSPECIFIED3  = quint64;

// Suppose T implements Serializable
template <typename T>
struct SEQUENCE {
public:
	static const int MAX_SIZE = 65535;
	const int maxSize;
	SEQUENCE(int maxSize_ = MAX_SIZE) : maxSize(maxSize_) {
		if (0 <= maxSize && maxSize <= MAX_SIZE) {
			data = new (std::nothrow) T [maxSize];
			if (data == nullptr) {
				logger.error("Failed to allocate memory.  maxSize = %d  MAX_SIZE = %d", maxSize, MAX_SIZE);
				COURIER_ERROR();
			}
		} else {
			logger.error("Overflow  maxSize = %d  MAX_SIZE = %d", maxSize, MAX_SIZE);
			COURIER_ERROR();
		}
	}
	~SEQUENCE() {
		delete[] data;
	}

	int getSize() {
		return size;
	}
	void clear() {
		size = 0;
	}

	void append(const T& newValue) {
		if (size < maxSize) {
			data[size++] = newValue;
		} else {
			logger.error("Unexpected overflow  size = %d  maxSize = %d", size, maxSize);
			COURIER_ERROR();
		}
	}
	T& operator[](int i) {
		if (0 <= i && i < maxSize) {
			// OK
		} else {
			logger.error("Unexpected overflow  i = %d  maxSize = %d", i, maxSize);
			COURIER_ERROR();
		}
		return data[i];
	}
	const T& operator[](int i) const {
		if (0 <= i && i < maxSize) {
			// OK
		} else {
			logger.error("Unexpected overflow  i = %d  maxSize = %d", i, maxSize);
			COURIER_ERROR();
		}
		return data[i];
	}

private:
	int size = 0;
	T*  data;
};

template <typename T>
struct ARRAY {
	static const int MAX_SIZE = 65535;
	const int maxSize;

	ARRAY(int maxSize_) : maxSize(maxSize_) {
		if (0 <= maxSize && maxSize <= MAX_SIZE) {
			data = new (std::nothrow) T [maxSize];
			if (data == nullptr) {
				logger.error("Failed to allocate memory.  maxSize = %d  MAX_SIZE = %d", maxSize, MAX_SIZE);
				COURIER_ERROR();
			}
		} else {
			logger.error("Overflow  maxSize = %d  MAX_SIZE = %d", maxSize, MAX_SIZE);
			COURIER_ERROR();
		}
	}
	~ARRAY() {
		delete[] data;
	}

	int getSize() {
		return size;
	}
	void clear() {
		size = 0;
	}

	void append(const T& newValue) {
		if (size < maxSize) {
			data[size++] = newValue;
		} else {
			logger.error("Unexpected overflow  size = %d  maxSize = %d", size, maxSize);
			COURIER_ERROR();
		}
	}
	T& operator[](int i) {
		if (0 <= i && i < maxSize) {
			// OK
		} else {
			logger.error("Unexpected overflow  i = %d  maxSize = %d", i, maxSize);
			COURIER_ERROR();
		}
		return data[i];
	}
	const T& operator[](int i) const {
		if (0 <= i && i < maxSize) {
			// OK
		} else {
			logger.error("Unexpected overflow  i = %d  maxSize = %d", i, maxSize);
			COURIER_ERROR();
		}
		return data[i];
	}

private:
	int size = 0;
	T*  data;
};


}

#endif /* COURIER_COURIER_H_ */
