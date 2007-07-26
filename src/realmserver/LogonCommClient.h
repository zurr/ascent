/****************************************************************************
 *
 * General Object Type File
 * Copyright (c) 2007 Team Ascent
 *
 * This work is licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 3.0
 * License. To view a copy of this license, visit
 * http://creativecommons.org/licenses/by-nc-sa/3.0/ or send a letter to Creative Commons,
 * 543 Howard Street, 5th Floor, San Francisco, California, 94105, USA.
 *
 * EXCEPT TO THE EXTENT REQUIRED BY APPLICABLE LAW, IN NO EVENT WILL LICENSOR BE LIABLE TO YOU
 * ON ANY LEGAL THEORY FOR ANY SPECIAL, INCIDENTAL, CONSEQUENTIAL, PUNITIVE OR EXEMPLARY DAMAGES
 * ARISING OUT OF THIS LICENSE OR THE USE OF THE WORK, EVEN IF LICENSOR HAS BEEN ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGES.
 *
 */

#ifndef _R_LOGON_COMM_CLIENT_H
#define _R_LOGON_COMM_CLIENT_H

#include "../logonserver/LogonOpcodes.h"
#include <RC4Engine.h>

class LogonCommClientSocket : public Socket
{
	uint32 remaining;
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
	void HandleRequestAccountMapping(WorldPacket & recvData);
	void UpdateAccountCount(uint32 account_id, uint8 add);

	void OnDisconnect();
	void CompressAndSend(ByteBuffer & uncompressed);
	uint32 last_ping;
	uint32 last_pong;

	uint32 pingtime;
	uint32 latency;
	uint32 _id;
	uint32 authenticated;
	bool use_crypto;
	set<uint32> realm_ids;
};

typedef void (LogonCommClientSocket::*logonpacket_handler)(WorldPacket&);

#endif

