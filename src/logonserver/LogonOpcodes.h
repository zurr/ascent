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

#ifndef __LOGONOPCODES_H
#define __LOGONOPCODES_H

enum RealmListOpcodes
{
	// Initialization of server/client connection...
	RCMSG_REGISTER_REALM			= 1,
	RSMSG_REALM_REGISTERED		  = 2,

	// Upon client connect (for WS)
	RCMSG_REQUEST_SESSION		   = 3,
	RSMSG_SESSION_RESULT			= 4,

	// Ping/Pong
	RCMSG_PING					  = 5,
	RSMSG_PONG					  = 6,

	// SQL Query Execute
	RCMSG_SQL_EXECUTE				= 7,
	RCMSG_RELOAD_ACCOUNTS			= 8,
};

#endif
