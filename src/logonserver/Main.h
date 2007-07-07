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

#include "../shared/Singleton.h"

extern bool mrunning;
class AuthSocket;
extern set<AuthSocket*> _authSockets;
extern Mutex _authSocketLock;

class LogonServer;
class LogonServer : public Singleton< LogonServer >
{
public:
	void CheckForDeadSockets();
	void Run(int argc, char ** argv);
	void Stop();
	uint32 max_build;
	uint32 min_build;
	uint8 sql_hash[20];

private:
	bool m_stopEvent;
};
