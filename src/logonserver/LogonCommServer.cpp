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

#include "LogonStdAfx.h"

typedef struct
{
	uint16 opcode;
	uint16 size;
}logonpacket;

LogonCommServerSocket::LogonCommServerSocket(SOCKET fd) : Socket(fd, 4096, 4096)
{
	// do nothing
	last_ping = time(NULL);
	remaining = opcode = 0;
	sInfoCore.AddServerSocket(this);
	removed = false;
}

LogonCommServerSocket::~LogonCommServerSocket()
{

}

void LogonCommServerSocket::OnDisconnect()
{
	// if we're registered -> de-register
	if(!removed)
	{
		set<uint32>::iterator itr = server_ids.begin();
		for(; itr != server_ids.end(); ++itr)
			sInfoCore.RemoveRealm((*itr));

		sInfoCore.RemoveServerSocket(this);
	}
}

void LogonCommServerSocket::OnRead()
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

void LogonCommServerSocket::HandlePacket(WorldPacket & recvData)
{
	switch(recvData.GetOpcode())
	{
	case RCMSG_REGISTER_REALM:
		HandleRegister(recvData);
		break;

	case RCMSG_REQUEST_SESSION:
		HandleSessionRequest(recvData);
		break;

	case RCMSG_PING:
		HandlePing(recvData);
		break;

	case RCMSG_SQL_EXECUTE:
		HandleSQLExecute(recvData);
		break;

	default:
		printf("Got unknwon packet from logoncomm: %u\n", recvData.GetOpcode());
		break;
	}
}

void LogonCommServerSocket::HandleRegister(WorldPacket & recvData)
{
	Realm realm;

	recvData >> realm.Name >> realm.Address;
	recvData >> realm.Colour >> realm.Icon >> realm.TimeZone >> realm.Population;

	uint32 my_id = sInfoCore.GenerateRealmID();
	sLog.outString("Registering realm `%s` under ID %u.", realm.Name.c_str(), my_id);

	// Add to the main realm list
	sInfoCore.AddRealm(my_id, &realm);

	// Send back response packet.
	WorldPacket data(RSMSG_REALM_REGISTERED, 4);
	data << uint32(0);	  // Error
	data << my_id;		  // Realm ID
	data << realm.Name;
	SendPacket(&data);
	server_ids.insert(my_id);
}

void LogonCommServerSocket::HandleSessionRequest(WorldPacket & recvData)
{
	uint32 request_id;
	string account_name;
	recvData >> request_id;
	recvData >> account_name;

	// get sessionkey!
	uint32 error = 0;
	Account * acct = sAccountMgr.GetAccount(account_name);
	BigNumber * sessionkey = acct ? sInfoCore.GetSessionKey(acct->AccountId) : 0;
	
	if(sessionkey == 0 || acct == 0)
		error = 1;		  // Unauthorized user.

	// build response packet
	WorldPacket data(RSMSG_SESSION_RESULT, 150);
	data << request_id;
	data << error;
	if(!error)
	{
		// Append account information.
		data << acct->AccountId;
		data << acct->Username;
		data << acct->GMFlags;
		data << acct->AccountFlags;
		data.append(sessionkey->AsByteArray(), 40);
	}
	
	SendPacket(&data);

	printf("Sessionkey sent for account %s, err %u\n", account_name.c_str(), error);
}

void LogonCommServerSocket::HandlePing(WorldPacket & recvData)
{
	uint32 seq;
	recvData >> seq;

	WorldPacket data(RSMSG_PONG, 4);
	data << seq;
	SendPacket(&data);
	last_ping = time(NULL);
}

void LogonCommServerSocket::SendPacket(WorldPacket * data)
{
	BurstBegin();	   //  << Lock

	logonpacket header;
	header.opcode = data->GetOpcode();
	header.size   = data->size();

	BurstSend((uint8*)&header, 4);
	if(data->size() > 0)
		BurstSend(data->contents(), data->size());

	BurstPush();
	BurstEnd();		 //  >> Unlock
}

void LogonCommServerSocket::HandleSQLExecute(WorldPacket & recvData)
{
	uint8 key[20];
	string Query;
	recvData.read(key, 20);
	recvData >> Query;

	if(memcmp(key, LogonServer::getSingleton().sql_hash, 20))
	{
		sLog.outString("Invalid SQL execute attempted from %s, query was %s", this->GetRemoteIP().c_str(), Query.c_str());
		
		// Kill the socket off, we don't want to keep bad people around.
		Disconnect();

		// Might wanna add an IP ban after this.. meh :/
		return;
	}
	
	sLogonSQL->Execute(Query.c_str());
}
