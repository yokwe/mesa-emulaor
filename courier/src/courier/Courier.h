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
// Courier.h
//

#ifndef COURIER_COURIER_H_
#define COURIER_COURIER_H_

#include <QtCore>

#include "../courier/Block.h"

#define COURIER_FATAL_ERROR() { logger.fatal("FATAL ERROR %s %d %s", __FILE__, __LINE__, __FUNCTION__); throw Courier::CourierFatalError(__FILE__, __LINE__, __FUNCTION__); }

namespace Courier {

class CourierError {
};

class CourierFatalError : public CourierError {
public:
	const char *file;
	const int   line;
	const char *func;

	CourierFatalError(const char *file_, const int line_, const char *func_) : file(file_), line(line_), func(func_) {}
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

QString toString(const Block&   value);
QString toString(const quint8   value);
QString toString(const bool     value);
QString toString(const QString& value);
QString toString(const quint16  value);
QString toString(const quint32  value);
QString toString(const quint64  value);

// Write whole value to block
void serialize(Block& block, const Block&   value);
void serialize(Block& block, const quint8   value);
void serialize(Block& block, const bool     value);
void serialize(Block& block, const QString& value);
void serialize(Block& block, const quint16  value);
void serialize(Block& block, const quint32  value);
void serialize(Block& block, const quint64  value);

// Read rest of block
void deserialize(Block& block, Block&    value);
void deserialize(Block& block, quint8&   value);
void deserialize(Block& block, bool&     value);
void deserialize(Block& block, QString&  value);
void deserialize(Block& block, quint16&  value);
void deserialize(Block& block, quint32&  value);
void deserialize(Block& block, quint64&  value);

class Enum {
private:
	int         ordinal;
	const char* name;

protected:
	static constexpr const int   NULL_ORDINAL = -1;
	static constexpr const char* NULL_NAME    = "#NULL#";;

	class Pair {
	public:
		const int   ordinal;
		const char* name;
		Pair(int ordinal_, const char* name_) : ordinal(ordinal_),     name(name_) {}
		Pair(const Pair& that)                : ordinal(that.ordinal), name(that.name) {}
	};

	static void initPairList(QList<Pair>& pairList, std::initializer_list<Enum::Pair> initList);
	static const Pair& findPair(const QList<Pair>& pairList, const int ordinal);
	static const Pair& findPair(const QList<Pair>& pairList, const char* name);

	Enum(const Pair& pair) : ordinal(pair.ordinal), name(pair.name) {}
	Enum(const Enum& that) : ordinal(that.ordinal), name(that.name) {}
	Enum()                 : ordinal(NULL_ORDINAL), name(NULL_NAME) {}

	void deserialize(const QList<Pair>& pairList, Block& block) {
		quint16 ordinal_;
		block.deserialize16(ordinal_);
		Pair pair = findPair(pairList, ordinal_);

		ordinal = pair.ordinal;
		name    = pair.name;
	}

public:
	Enum& operator= (const Enum& that) {
		ordinal = that.ordinal;
		name    = that.name;
		return *this;
	}
	operator int() const {
		return ordinal;
	}
	operator const char*() const {
		return name;
	}
	QString toString() {
		return QString(name);
	}
	void serialize(BLOCK& block);
	bool isNull() {
		return ordinal == NULL_ORDINAL;
	}
};

template <typename T, int N = 65535>
struct SEQUENCE {
public:
	using ELEMENT = T;
	static const int MAX_SIZE = 65535;
	const int maxSize;
	SEQUENCE() : maxSize(N) {
		initialize();
	}
	~SEQUENCE() {
		delete[] data;
	}

	SEQUENCE(std::initializer_list<T> initList) : maxSize(N) {
		initialize();
		setSize(initList.size());
		int j = 0;
		for(auto i = initList.begin(); i != initList.end(); i++) {
			data[j++] = *i;
		}
	}

	// Copy constructor
	SEQUENCE(const SEQUENCE& that) : maxSize(that.maxSize) {
		initialize();
		size = that.size;
		for(int i = 0; i < size; i++) {
			data[i] = that.data[i];
		}
	}
	SEQUENCE& operator=(const SEQUENCE& that) {
		if (maxSize != that.maxSize) {
			logger.error("Unexpected this.maxSize = %d  that.maxSize = %d", this->maxSize, that.maxSize);
			COURIER_FATAL_ERROR();
		}
		size = that.size;
		for(int i = 0; i < size; i++) {
			data[i] = that.data[i];
		}

		return *this;
	}

	// Move constructor
	SEQUENCE(SEQUENCE&& that) : maxSize(that.maxSize) {
		if (maxSize != that.maxSize) {
			logger.error("Unexpected this.maxSize = %d  that.maxSize = %d", this->maxSize, that.maxSize);
			COURIER_FATAL_ERROR();
		}
		size = that.size;
		data = that.data;
		that.data = nullptr;
	}
	SEQUENCE& operator=(SEQUENCE&& that) {
		size = that.size;
		data = that.data;
		that.data = nullptr;

		return *this;
	}

	quint16 getSize() const {
		return (quint16)size;
	}
	void setSize(int newValue) {
		if (0 <= newValue && newValue <= maxSize) {
			size = newValue;
		} else {
			logger.error("Unexpected overflow  newValue = %d  maxSize = %d", newValue, maxSize);
			COURIER_FATAL_ERROR();
		}
	}

	T* begin() {
		return data;
	}
	T* end() {
		return data + size;
	}

	T& operator[](int i) {
		if (0 <= i && i < size) {
			// OK
		} else {
			logger.error("Unexpected overflow  i = %d  size = %d", i, size);
			COURIER_FATAL_ERROR();
		}
		return data[i];
	}
	const T& operator[](int i) const {
		if (0 <= i && i < size) {
			// OK
		} else {
			logger.error("Unexpected overflow  i = %d  size = %d", i, size);
			COURIER_FATAL_ERROR();
		}
		return data[i];
	}

private:
	int size;
	T*  data;

	void initialize() {
		if (0 <= maxSize && maxSize <= MAX_SIZE) {
			data = new (std::nothrow) T [maxSize];
			if (data == nullptr) {
				logger.error("Failed to allocate memory.  maxSize = %d  MAX_SIZE = %d", maxSize, MAX_SIZE);
				COURIER_FATAL_ERROR();
			}
		} else {
			logger.error("Overflow  maxSize = %d  MAX_SIZE = %d", maxSize, MAX_SIZE);
			COURIER_FATAL_ERROR();
		}
		size = 0;
	}
};


template <typename T, int N>
struct ARRAY {
	using ELEMENT = T;
	static const int MAX_SIZE = 65535;
	const int maxSize;

	ARRAY() : maxSize(N) {
		initialize();
	}
	~ARRAY() {
		delete[] data;
	}

	ARRAY(std::initializer_list<T> initList) : maxSize(N) {
		initialize();
		int j = 0;
		for(auto i = initList.begin(); i != initList.end(); i++) {
			data[j++] = *i;
		}
	}

	// Copy constructor
	ARRAY(const ARRAY& that) : maxSize(that.maxSize) {
		initialize();
		for(int i = 0; i < maxSize; i++) {
			this->data[i] = that.data[i];
		}
	}
	ARRAY& operator=(const ARRAY& that) {
		if (maxSize != that.maxSize) {
			logger.error("Unexpected this.maxSize = %d  that.maxSize = %d", this->maxSize, that.maxSize);
			COURIER_FATAL_ERROR();
		}
		for(int i = 0; i < maxSize; i++) {
			data[i] = that.data[i];
		}

		return *this;
	}

	// Move constructor
	ARRAY(ARRAY&& that) : maxSize(that.maxSize) {
		data = that.data;
		that.data = nullptr;
	}
	ARRAY& operator=(ARRAY&& that) {
		if (maxSize != that.maxSize) {
			logger.error("Unexpected this.maxSize = %d  that.maxSize = %d", this->maxSize, that.maxSize);
			COURIER_FATAL_ERROR();
		}
		data = that.data;
		that.data = nullptr;

		return *this;
	}

	T* begin() {
		return data;
	}
	T* end() {
		return data + maxSize;
	}

	T& operator[](int i) {
		if (0 <= i && i < maxSize) {
			// OK
		} else {
			logger.error("Unexpected overflow  i = %d  maxSize = %d", i, maxSize);
			COURIER_FATAL_ERROR();
		}
		return data[i];
	}
	const T& operator[](int i) const {
		if (0 <= i && i < maxSize) {
			// OK
		} else {
			logger.error("Unexpected overflow  i = %d  maxSize = %d", i, maxSize);
			COURIER_FATAL_ERROR();
		}
		return data[i];
	}

private:
	T*      data;

	void initialize() {
		if (0 <= maxSize && maxSize <= MAX_SIZE) {
			data = new (std::nothrow) T [maxSize];
			if (data == nullptr) {
				logger.error("Failed to allocate memory.  maxSize = %d  MAX_SIZE = %d", maxSize, MAX_SIZE);
				COURIER_FATAL_ERROR();
			}
		} else {
			logger.error("Overflow  maxSize = %d  MAX_SIZE = %d", maxSize, MAX_SIZE);
			COURIER_FATAL_ERROR();
		}
	}
};

}

#endif /* COURIER_COURIER_H_ */
