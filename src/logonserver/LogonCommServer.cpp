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

	use_crypto = false;
	authenticated = 0;
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

			if(use_crypto)
			{
				// decrypt the packet
				recvCrypto.Process((unsigned char*)&opcode, (unsigned char*)&opcode, 2);
				recvCrypto.Process((unsigned char*)&remaining, (unsigned char*)&remaining, 2);
			}

			/* reverse byte order */
			remaining = ntohs(remaining);
		}

		// do we have a full packet?
		if(GetReadBufferSize() < remaining)
			return;

		// create the buffer
		WorldPacket buff(opcode, remaining);
		buff.resize(remaining);
		Read(remaining, (uint8*)buff.contents());

		if(use_crypto)
			recvCrypto.Process((unsigned char*)buff.contents(), (unsigned char*)buff.contents(), remaining);

		// handle the packet
		HandlePacket(buff);

		remaining = 0;
		opcode = 0;
	}
}

void LogonCommServerSocket::HandlePacket(WorldPacket & recvData)
{
	if(authenticated == 0 && recvData.GetOpcode() != RCMSG_AUTH_CHALLENGE)
	{
		// invalid
		Disconnect();
		return;
	}

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

	case RCMSG_RELOAD_ACCOUNTS:
		HandleReloadAccounts(recvData);
		break;

	case RCMSG_AUTH_CHALLENGE:
		HandleAuthChallenge(recvData);
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
}

void LogonCommServerSocket::HandlePing(WorldPacket & recvData)
{
	WorldPacket data(RSMSG_PONG, 4);
	SendPacket(&data);
	last_ping = time(NULL);
}

void LogonCommServerSocket::SendPacket(WorldPacket * data)
{
	BurstBegin();	   //  << Lock

	logonpacket header;
	header.opcode = data->GetOpcode();
	header.size   = ntohs(data->size());

	if(use_crypto)
		sendCrypto.Process((unsigned char*)&header, (unsigned char*)&header, 4);

	BurstSend((uint8*)&header, 4);

	if(data->size() > 0)
	{
		if(use_crypto)
			sendCrypto.Process((unsigned char*)data->contents(), (unsigned char*)data->contents(), data->size());

		BurstSend(data->contents(), data->size());
	}

	BurstPush();
	BurstEnd();		 //  >> Unlock
}

void LogonCommServerSocket::HandleSQLExecute(WorldPacket & recvData)
{
	string Query;
	recvData >> Query;
	sLogonSQL->Execute(Query.c_str());
}

void LogonCommServerSocket::HandleReloadAccounts(WorldPacket & recvData)
{
	sAccountMgr.ReloadAccounts(true);
}

void LogonCommServerSocket::HandleAuthChallenge(WorldPacket & recvData)
{
	unsigned char key[20];
	uint32 result = 1;
	recvData.read(key, 20);

	// check if we have the correct password
	if(memcmp(key, LogonServer::getSingleton().sql_hash, 20))
		result = 0;

	sLog.outString("Authentication request from %s, result %s.", inet_ntoa(GetRemoteAddress()), result ? "OK" : "FAIL");

	printf("Key: ");
	for(int i = 0; i < 20; ++i)
		printf("%.2X", key[i]);
	printf("\n");

	recvCrypto.Setup(key, 20);
	sendCrypto.Setup(key, 20);	

	/* packets are encrypted from now on */
	use_crypto = true;

	/* send the response packet */
	WorldPacket data(RSMSG_AUTH_RESPONSE, 1);
	data << result;
	SendPacket(&data);

	/* set our general var */
	authenticated = result;
}
