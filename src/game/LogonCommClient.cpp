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

#include "StdAfx.h"

typedef struct
{
	uint16 opcode;
	uint16 size;
}logonpacket;

LogonCommClientSocket::LogonCommClientSocket(SOCKET fd) : Socket(fd, 32768, 65536)
{
	// do nothing
	last_ping = time(NULL);
	remaining = opcode = 0;
	_id=0;
	gotpong = true;
	pingseq = 0;
	latency = 0;
}

void LogonCommClientSocket::OnRead()
{
	while(true)
	{
		if(!remaining)
		{
			if(GetReadBufferSize() < 4)
				return;	 // no header

			// read header
			Read(2, (uint8*)&opcode);
			Read(2, (uint8*)&remaining);
		}

		// do we have a full packet?
		if(GetReadBufferSize() < remaining)
			return;

		// create the buffer
		WorldPacket buff(opcode, remaining);
		buff.resize(remaining);
		Read(remaining, (uint8*)buff.contents());

		// handle the packet
		HandlePacket(buff);
		
		remaining = 0;
		opcode = 0;
	}
}

void LogonCommClientSocket::HandlePacket(WorldPacket & recvData)
{
	switch(recvData.GetOpcode())
	{
	case RSMSG_REALM_REGISTERED:
		HandleRegister(recvData);
		break;

	case RSMSG_SESSION_RESULT:
		HandleSessionInfo(recvData);
		break;

	case RSMSG_PONG:
		HandlePong(recvData);
		break;

	default:
		printf("Got unknwon packet from logoncomm: %u\n", recvData.GetOpcode());
		break;
	}
}

void LogonCommClientSocket::HandleRegister(WorldPacket & recvData)
{
	uint32 realmlid;
	uint32 error;
	string realmname;
	recvData >> error >> realmlid >> realmname;

	sLog.outColor(TNORMAL, "\n		   >> realm `%s` registered under id ", realmname.c_str());
	sLog.outColor(TGREEN, "%u", realmlid);
	
	LogonCommHandler::getSingleton().AdditionAck(_id, realmlid);
}

void LogonCommClientSocket::HandleSessionInfo(WorldPacket & recvData)
{
	uint32 request_id;
	recvData >> request_id;

	Mutex & m = sLogonCommHandler.GetPendingLock();
	m.Acquire();

	// find the socket with this request
	WorldSocket * sock = sLogonCommHandler.GetSocketByRequest(request_id);
	if(sock == 0 || sock->Authed)	   // Expired/Client disconnected
	{
		m.Release();
		return;
	}

	// extract sessionkey / account information (done by WS)
	sock->Authed = true;
	sLogonCommHandler.RemoveUnauthedSocket(request_id);
	sock->InformationRetreiveCallback(recvData, request_id);
	m.Release();
}

void LogonCommClientSocket::HandlePong(WorldPacket & recvData)
{
	if(latency)
		sLog.outDebug(">> logonserver latency: %ums", getMSTime() - pingtime);
	latency = getMSTime() - pingtime;
	gotpong = true;
}

void LogonCommClientSocket::SendPing()
{
	pingseq++;
	pingtime = getMSTime();
	WorldPacket data(RCMSG_PING, 4);
	data << pingseq;
	SendPacket(&data);

	last_ping = time(NULL);
	gotpong = false;
}

void LogonCommClientSocket::SendPacket(WorldPacket * data)
{
	logonpacket header;
	bool rv;

	BurstBegin();
	if((4 + data->size() + GetWriteBufferSize()) >= 32768)
	{
		BurstEnd();
		Disconnect();
		return;
	}

	header.opcode = data->GetOpcode();
	header.size   = data->size();

	rv = BurstSend((const uint8*)&header, 4);

	if(data->size() > 0 && rv)
		rv = BurstSend((const uint8*)data->contents(), data->size());

	if(rv) BurstPush();
	BurstEnd();
}

void LogonCommClientSocket::OnDisconnect()
{
	if(_id != 0)
		sLogonCommHandler.ConnectionDropped(_id);	
}

LogonCommClientSocket::~LogonCommClientSocket()
{

}
