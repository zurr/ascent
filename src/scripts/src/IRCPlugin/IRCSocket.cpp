/****************************************************************************
 *
 * IRC Plugin for Antrix
 * Copyright (c) 2007 Burlex, Antrix Team
 *
 * This file may be distributed under the terms of the Q Public License
 * as defined by Trolltech ASA of Norway and appearing in the file
 * COPYING included in the packaging of this file.
 *
 * This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 * WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 */

#include "IRCPlugin.h"

IRCSocket::IRCSocket(SOCKET fd) : Socket(fd, 65536, 65536)
{

}

IRCSocket::~IRCSocket()
{

}

void IRCSocket::SendLine(const char * format, ...)
{
	char buf[514];
	va_list ap;
	va_start(ap, format);
	BurstBegin();
	int len = vsnprintf(buf, 514, format, ap);
	BurstSend((const uint8*)buf, len);
	BurstSend((const uint8*)"\r\n", 2);
	BurstEnd();
	va_end(ap);
}

void IRCSocket::OnRead()
{
	m_recvQLock.Acquire();
	recvq.append((char*)GetReadBuffer(0), GetReadBufferSize());
	m_recvQLock.Release();
	RemoveReadBufferBytes(GetReadBufferSize(), false);
}

bool IRCSocket::BufferReady()
{
	m_recvQLock.Acquire();
	bool ret = (recvq.rfind("\n") != string::npos);
	m_recvQLock.Release();
	return ret;
}

string IRCSocket::GetLine()
{
	m_recvQLock.Acquire();

	/* strip any \r/\n's from the front */
	string::iterator t = recvq.begin();
	while (t != recvq.end() && (*t == '\r' || *t == '\n'))
	{
		recvq.erase(t);
		t = recvq.begin();
	}

	for(; t != recvq.end(); ++t)
	{
		if(*t == '\n')
		{
			string ret = string(recvq.begin(), t);
			recvq.erase(recvq.begin(), t + 1);
			m_recvQLock.Release();
			return ret;
		}
	}
	
	m_recvQLock.Release();
	return "";
}

struct IRCCommandHandler
{
	const char * Command;
	void(IRCSocket::*CommandHandler)();
};

IRCCommandHandler * getCommandHandlers()
{
	static IRCCommandHandler Handlers[] = {
		{ "JOIN", &IRCSocket::HandleJoin },
		{ "433", &IRCSocket::Handle433 },
		{ "001", &IRCSocket::Handle001 },
		{ 0, 0 },
	};

	return Handlers;
}

void IRCSocket::PollRecvQ()
{
	if(!BufferReady())
		return;

	string line = GetLine();
	string::size_type sp = line.find(" ");

	/* find the first instance of " " */
	if(sp == string::npos)
		return;

	/* skip the leading : */
	string source = line.substr(1, sp);

	/* find the next space */
	string::size_type sp2 = line.find(" ", sp+1);

	/* grab the numeric */
	string numeric = line.substr(sp+1, sp2-sp-1);
	
	/* find the command handler */
	IRCCommandHandler * ch = getCommandHandlers();
	for(; ch->Command != 0; ++ch)
	{
		if(strcmp(ch->Command, numeric.c_str()))
			break;
	}

	printf("Numeric: %s\n", numeric.c_str());
	/* split into parameters */

	/* blah, execute it */
	if(ch->Command != 0)
		(this->*(ch->CommandHandler))();
}

void IRCSocket::Handle433()
{
	/* invalid nickname, try sending the alt */
	SendLine("NICK %s", Thread->AltNickName.c_str());
}

void IRCSocket::HandleJoin()
{
	/* joined channel */
}

void IRCSocket::HandlePrivMsg()
{
	/* message on channel */
}

void IRCSocket::Handle001()
{
	/* we're fully connected now */
	Thread->State = STATE_FULLCONNECTED;
}

