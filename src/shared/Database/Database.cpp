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
