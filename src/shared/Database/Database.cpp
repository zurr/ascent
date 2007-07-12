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

#include "DatabaseEnv.h"

Database* CreateDatabaseInterface(DatabaseType type)
{
	switch((int)type)
	{

#ifdef DATABASE_SUPPORT_MYSQL

	case DATABASE_TYPE_MYSQL:
		return new MySQLDatabase;
		break;

#endif

#ifdef DATABASE_SUPPORT_PGSQL

	case DATABASE_TYPE_PGSQL:
		return new PostgreDatabase;
		break;

#endif

	}
	sLog.outError("Invalid database type specified. It has to be in the range of 1 to 3.");
	return 0;
}

void DestroyDatabaseInterface(Database * ptr)
{
	switch((int)ptr->GetType())
	{

#ifdef DATABASE_SUPPORT_MYSQL

	case DATABASE_TYPE_MYSQL:
		delete ((MySQLDatabase*)ptr);
		return;
		break;

#endif

#ifdef DATABASE_SUPPORT_PGSQL

	case DATABASE_TYPE_PGSQL:
		delete ((PostgreDatabase*)ptr);
		return;
		break;

#endif

	}
	sLog.outError("Invalid database type specified. It has to be in the range of 1 to 3.");
}


QueryResult::QueryResult(uint32 FieldCount, uint32 RowCount, uint32 Type)
{
	mCurrentRow = new Field[FieldCount];
	mRowCount = RowCount;
	mFieldCount = FieldCount;
	mType = Type;
}

QueryResult::~QueryResult()
{
	delete [] mCurrentRow;
	switch(mType)
	{

#ifdef DATABASE_SUPPORT_MYSQL

	case DATABASE_TYPE_MYSQL:
		((MySQLQueryResult*)this)->Destroy();
		break;

#endif

#ifdef DATABASE_SUPPORT_PGSQL

	case DATABASE_TYPE_PGSQL:
		((PostgreQueryResult*)this)->Destroy();
		break;

#endif

	default:
		assert(false);
		break;
	}
}
