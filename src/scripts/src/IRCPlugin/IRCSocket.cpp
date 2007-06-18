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

#include "IRCSocket.h"

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
	uint32 ParameterCount;
	void(IRCSocket::*CommandHandler)();
};

IRCCommandHandler * getCommandHandlers()
{
	static IRCCommandHandler Handlers[] = {
		{ "JOIN", 3, &IRCSocket::HandleJoin },
		{ "433", 3, &IRCSocket::Handle433 },
		{ 0, 0, 0 },
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

	string numeric = line.substr(0, sp);
	
	/* find the command handler */
	IRCCommandHandler * ch = getCommandHandlers();
	for(; ch->Command != 0; ++ch)
	{
		if(strcmp(ch->Command, numeric.c_str()))
			break;
	}

	/* split into parameters */

}
