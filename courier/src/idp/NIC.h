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
// NIC.h
//

#ifndef NIC_H__
#define NIC_H__

#include <linux/if_ether.h>

#include <QtCore>

class NIC {
public:
	// packet type of Xerox IDP
	static const quint16 ETH_P_IDP = 0x0600;

	NIC() : name(0), protocol(0), fd(0), address(0) {}

	const char* getName() {
		return name;
	}
	quint16 getProtocol() {
		return protocol;
	}
	quint64 getAddress() {
		return address;
	}

	void attach(const char* name, const quint16 protocol);
	void detach();

	// discard already received packet
	void discardRecievedPacket();
	void discardOnePacket();
	int  select(quint32 timeout, int& opErrno);

	// returns return code of send and recv. no error checking
	int transmit(quint8* data, quint32 dataLen, int& opErrno);
	int receive (quint8* data, quint32 dataLen, int& opErrno);

private:
	const char* name;
	quint16     protocol;
	int         fd;
	quint64     address;
};

#endif
