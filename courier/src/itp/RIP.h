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
// RIPs.h
//


#ifndef ITP_RIP_H__
#define ITP_RIP_H__

#include "../itp/IDP.h"

namespace ITP {
class RIP {
public:
	enum class Operation : quint16 {
        REQUEST  = 1,
        RESPONSE = 2,
	};

	class Tupple {
	public:
		IDP::Network  network;
		IDP::HopCount hopCount;

		Tupple(IDP::Network  network_, IDP::HopCount hopCount_) : network(network_), hopCount(hopCount_) {}
		Tupple() : network((IDP::Network)0), hopCount((IDP::HopCount)0) {}

	    void serialize  (NetData& netData);
	    void deserialize(NetData& netData);
	};

    void serialize  (NetData& netData);
    void deserialize(NetData& netData);

    RIP() : operation((Operation)0) {}

	Operation     operation;
	QList<Tupple> tupples;
};
}

QString toString(const ITP::RIP::Operation value);
QString toString(const ITP::RIP::Tupple& value);
QString toString(const ITP::RIP& value);

#endif
