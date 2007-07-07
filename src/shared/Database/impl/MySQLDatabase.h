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

#ifndef _MYSQLDATABASE_H
#define _MYSQLDATABASE_H

#include <string>
#include "../Database.h"
#include "../../Threading/Queue.h"

using namespace std;
class MySQLQueryResult;
class MySQLDatabaseThread;

typedef struct{
	MYSQL*	con;
volatile	bool	busy;
}MysqlCon;

class MySQLDatabase : public Database,public CThread
{
	friend class MySQLDatabaseThread;
public:
	MySQLDatabase();
	~MySQLDatabase();

	bool Initialize(const char* Hostname, unsigned int port,
		const char* Username, const char* Password, const char* DatabaseName,
		uint32 ConnectionCount, uint32 BufferSize);
	void run();
	void Shutdown();

	QueryResult* Query(const char* QueryString, ...);
	bool WaitExecute(const char* QueryString, ...);//Wait For Request Completion
	bool Execute(const char* QueryString, ...);

	void CheckConnections();
	bool ThreadRunning;

	inline string GetHostName() { return mHostname; }
	inline string GetDatabaseName() { return mDatabaseName; }
	inline uint32 GetQueueSize() { return queries_queue.get_size(); }

	string EscapeString(string Escape);

protected:

	bool Connect(MysqlCon * con);
	bool Disconnect(MysqlCon * con);

	bool HandleError(MysqlCon*, uint32 ErrorNumber);

	bool SendQuery(MysqlCon *con, const char* Sql, bool Self = false);

////////////////////////////////
	FQueue<char*> queries_queue;
	Mutex	lock;
	MysqlCon * Connections;
	MysqlCon * GetFreeConnection();
	uint32 _counter;
///////////////////////////////
	
	int32 mConnectionCount;
   
	// For reconnecting a broken connection
	string mHostname;
	string mUsername;
	string mPassword;
	string mDatabaseName;
	uint32 mPort;

	//uint32 mNextPing;

};

class MySQLQueryResult : public QueryResult
{
public:
	MySQLQueryResult(MYSQL_RES* res, uint32 FieldCount, uint32 RowCount);
	~MySQLQueryResult();

	bool NextRow();
	void Destroy();

protected:
	MYSQL_RES* mResult;
};

#endif
