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

#ifndef _DATABASE_H
#define _DATABASE_H

#include <string>

using namespace std;
class QueryResult;
class DatabaseThread;

enum DatabaseType
{
	DATABASE_TYPE_NONE	  = 0,
	DATABASE_TYPE_MYSQL	 = 1,
	DATABASE_TYPE_PGSQL	 = 2,
	DATABASE_TYPE_ORACLE10  = 3,
	DATABASE_TYPE_TOTAL	 = 4,
};

class Database
{
	friend class DatabaseThread;
public:
	Database(DatabaseType type) : mType(type) {}
	virtual ~Database() {}

	virtual bool Initialize(const char* Hostname, unsigned int port,
		const char* Username, const char* Password, const char* DatabaseName,
		uint32 ConnectionCount, uint32 BufferSize) = 0;

	virtual void Shutdown() = 0;

	virtual QueryResult* Query(const char* QueryString, ...) = 0;
	virtual bool WaitExecute(const char* QueryString, ...) = 0;
	virtual bool Execute(const char* QueryString, ...) = 0;
	virtual string EscapeString(string Escape) = 0;

	virtual void CheckConnections() = 0;

	inline DatabaseType GetType() { return mType; }

protected:
	DatabaseType mType;
};

class QueryResult
{
public:
	QueryResult(uint32 FieldCount, uint32 RowCount, uint32 Type);
	virtual ~QueryResult();

	virtual bool NextRow() = 0;

	inline Field* Fetch() { return mCurrentRow; }
	inline uint32 GetFieldCount() const { return mFieldCount; }
	inline uint32 GetRowCount() const { return mRowCount; }

protected:

	Field *mCurrentRow;
	uint32 mFieldCount;
	uint32 mRowCount;
	uint32 mType;

};

Database* CreateDatabaseInterface(DatabaseType type);
void DestroyDatabaseInterface(Database * ptr);

#endif
