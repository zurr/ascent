/****************************************************************************
 *
 * General Object Type File
 * Copyright (c) 2007 Antrix Team
 *
 * This file may be distributed under the terms of the Q Public License
 * as defined by Trolltech ASA of Norway and appearing in the file
 * COPYING included in the packaging of this file.
 *
 * This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 * WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 */

#ifndef __LOGON_COMM_CLIENT_H
#define __LOGON_COMM_CLIENT_H

#include "../logonserver/LogonOpcodes.h"
#include <RC4Engine.h>

class LogonCommClientSocket : public Socket
{
	uint16 remaining;
	uint16 opcode;
	RC4Engine _sendCrypto;
	RC4Engine _recvCrypto;
public:
	LogonCommClientSocket(SOCKET fd);
	~LogonCommClientSocket();
	
	void OnRead();
	void SendPacket(WorldPacket * data);
	void HandlePacket(WorldPacket & recvData);
	void SendPing();
	void SendChallenge();
	void HandleAuthResponse(WorldPacket & recvData);

	void HandleRegister(WorldPacket & recvData);
	void HandlePong(WorldPacket & recvData);
	void HandleSessionInfo(WorldPacket & recvData);

	void OnDisconnect();
	uint32 last_ping;

	uint32 pingtime;
	uint32 latency;
	uint32 pingseq;
	uint32 _id;
	bool gotpong;
	uint32 authenticated;
	uint32 seed;
	bool use_crypto;
};

void SimpleCrypt(int len, char * buffer, uint32 key);

#endif

