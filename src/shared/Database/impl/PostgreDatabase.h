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

#ifndef _POSTGREDATABASE_H
#define _POSTGREDATABASE_H

#include <string>
#include "../Database.h"
#include <postgre/libpq-fe.h>

using namespace std;
class PostgreQueryResult;
class PostgreDatabaseThread;

class PostgreDatabase : public Database
{
public:
	PostgreDatabase();
	~PostgreDatabase();

	bool Initialize(const char* Hostname, unsigned int port,
		const char* Username, const char* Password, const char* DatabaseName,
		uint32 ConnectionCount, uint32 BufferSize);

	void Shutdown();

	QueryResult* Query(const char* QueryString, ...);
	bool WaitExecute(const char* QueryString, ...);
	bool Execute(const char* QueryString, ...);

	void CheckConnections();

	/** Burlex TODO: Implement these :P
	 */
	string EscapeString(const char * QueryString, ...) {}

protected:

	bool Connect();

	bool Connect(uint32 ConnectionIndex);
	bool Disconnect(uint32 ConnectionIndex);
	PGresult * SendQuery(uint32 ConnectionIndex, const char* Sql, bool Self = false);

	uint32 GetConnection();

	PGconn ** Connections;
	char ** QueryBuffer;

	Mutex DelayedQueryBufferMutex;
	char * DelayedQueryBuffer;

	bool * InUseMarkers;

	int32 mConnectionCount;

	// For reconnecting a broken connection
	string mConnectionString;

	uint32 mPort;

	uint32 mNextPing;

	Mutex mSearchMutex;
	PostgreDatabaseThread* mQueryThread;
};

class PostgreQueryResult : public QueryResult
{
public:
	PostgreQueryResult(PGresult * res, uint32 FieldCount, uint32 RowCount);
	~PostgreQueryResult();

	bool NextRow();
	void Destroy();

protected:
	Field *mCurrentRow;
	uint32 mRow;

	PGresult * mResult;
};

#endif