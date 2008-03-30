/*
 * Ascent MMORPG Server
 * Copyright (C) 2005-2007 Ascent Team <http://www.ascentemu.com/>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


#include "DatabaseEnv.h"
#include "../CrashHandler.h"
#include "../NGLog.h"

SQLCallbackBase::~SQLCallbackBase()
{

}

Database::Database() : CThread()
{
	_counter=0;
	Connections = NULL;
	mConnectionCount = -1;   // Not connected.
	ThreadRunning = true;
}

Database::~Database()
{
	
}

void Database::_Initialize()
{
	// Spawn Database thread
	ThreadPool.ExecuteTask(this);

	// launch the query thread
	qt = new QueryThread(this);
	ThreadPool.ExecuteTask(qt);
}

DatabaseConnection * Database::GetFreeConnection()
{
	uint32 i = 0;
	for(;;)
	{
		DatabaseConnection * con = Connections[ ((i++) % mConnectionCount) ];
		if(con->Busy.AttemptAcquire())
			return con;
	}

	// shouldn't be reached
	return NULL;
}


QueryResult * Database::Query(const char* QueryString, ...)
{	
	char sql[16384];
	va_list vlist;
	va_start(vlist, QueryString);
	vsnprintf(sql, 16384, QueryString, vlist);
	va_end(vlist);

	// Send the query
	QueryResult * qResult = NULL;
	DatabaseConnection * con = GetFreeConnection();

	if(_SendQuery(con, sql, false))
		qResult = _StoreQueryResult( con );
	
	con->Busy.Release();
	return qResult;
}

QueryResult * Database::QueryNA(const char* QueryString)
{	
	// Send the query
	QueryResult * qResult = NULL;
	DatabaseConnection * con = GetFreeConnection();

	if( _SendQuery( con, QueryString, false ) )
		qResult = _StoreQueryResult( con );

	con->Busy.Release();
	return qResult;
}

QueryResult * Database::FQuery(const char * QueryString, DatabaseConnection * con)
{	
	// Send the query
	QueryResult * qResult = NULL;
	if( _SendQuery( con, QueryString, false ) )
		qResult = _StoreQueryResult( con );

	return qResult;
}

void Database::FWaitExecute(const char * QueryString, DatabaseConnection * con)
{	
	// Send the query
	_SendQuery( con, QueryString, false );
}

void QueryBuffer::AddQuery(const char * format, ...)
{
	char query[16384];
	va_list vlist;
	va_start(vlist, format);
	vsnprintf(query, 16384, format, vlist);
	va_end(vlist);

	size_t len = strlen(query);
	char * pBuffer = new char[len+1];
	memcpy(pBuffer, query, len + 1);

	queries.push_back(pBuffer);
}

void QueryBuffer::AddQueryNA( const char * str )
{
	size_t len = strlen(str);
	char * pBuffer = new char[len+1];
	memcpy(pBuffer, str, len + 1);

	queries.push_back(pBuffer);
}

void QueryBuffer::AddQueryStr(const string& str)
{
	size_t len = str.size();
	char * pBuffer = new char[len+1];
	memcpy(pBuffer, str.c_str(), len + 1);

	queries.push_back(pBuffer);
}

void Database::PerformQueryBuffer(QueryBuffer * b, DatabaseConnection * ccon)
{
	if(!b->queries.size())
		return;

    DatabaseConnection * con = ccon;
	if( ccon == NULL )
		con = GetFreeConnection();
	
	_BeginTransaction(con);

	for(vector<char*>::iterator itr = b->queries.begin(); itr != b->queries.end(); ++itr)
	{
		_SendQuery(con, *itr, false);
		delete[] (*itr);
	}

	_EndTransaction(con);

	if( ccon == NULL )
		con->Busy.Release();
}

bool Database::Execute(const char* QueryString, ...)
{
	char query[16384];

	va_list vlist;
	va_start(vlist, QueryString);
	vsnprintf(query, 16384, QueryString, vlist);
	va_end(vlist);

	if(!ThreadRunning)
		return WaitExecuteNA(query);

	size_t len = strlen(query);
	char * pBuffer = new char[len+1];
	memcpy(pBuffer, query, len + 1);

	queries_queue.push(pBuffer);
	return true;
}

bool Database::ExecuteNA(const char* QueryString)
{
	if(!ThreadRunning)
		return WaitExecuteNA(QueryString);

	size_t len = strlen(QueryString);
	char * pBuffer = new char[len+1];
	memcpy(pBuffer, QueryString, len + 1);

	queries_queue.push(pBuffer);
	return true;
}

//this will wait for completion
bool Database::WaitExecute(const char* QueryString, ...)
{
	char sql[16384];
	va_list vlist;
	va_start(vlist, QueryString);
	vsnprintf(sql, 16384, QueryString, vlist);
	va_end(vlist);

	DatabaseConnection * con = GetFreeConnection();
	bool Result = _SendQuery(con, sql, false);
	con->Busy.Release();
	return Result;
}

bool Database::WaitExecuteNA(const char* QueryString)
{
	DatabaseConnection * con = GetFreeConnection();
	bool Result = _SendQuery(con, QueryString, false);
	con->Busy.Release();
	return Result;
}

bool Database::run()
{
	SetThreadName("Database Execute Thread");
	SetThreadState(THREADSTATE_BUSY);
	ThreadRunning = true;
	char * query = queries_queue.pop();
	DatabaseConnection * con = GetFreeConnection();
	while(query)
	{
		_SendQuery( con, query, false );
		delete[] query;
		if(ThreadState == THREADSTATE_TERMINATE)
			break;

		query = queries_queue.pop();
	}

	con->Busy.Release();

	if(queries_queue.get_size() > 0)
	{
		// execute all the remaining queries
		query = queries_queue.pop_nowait();
		while(query)
		{
			DatabaseConnection * con = GetFreeConnection();
			_SendQuery( con, query, false );
			con->Busy.Release();
			delete[] query;
			query=queries_queue.pop_nowait();
		}
	}

	ThreadRunning = false;
	return false;
}

void AsyncQuery::AddQuery(const char * formatstring, ...)
{
	AsyncQueryResult res;
	size_t nSize = 0;
	char buff[1000];
	memset(buff, 0, sizeof(buff));
	va_list args;
	va_start(args, formatstring);
	nSize = vsnprintf_s( buff, sizeof(buff), _TRUNCATE, formatstring, args);
	res.result = NULL;
	res.query = new char[nSize+1];
	res.query[nSize] = 0;
	memcpy_s(res.query, nSize, buff, nSize);
	queries.push_back(res);
}

void AsyncQuery::Perform()
{
	DatabaseConnection * conn = db->GetFreeConnection();
	for(vector<AsyncQueryResult>::iterator itr = queries.begin(); itr != queries.end(); ++itr)
		itr->result = db->FQuery(itr->query, conn);

	conn->Busy.Release();
	func->run(queries);

	delete this;
}

AsyncQuery::~AsyncQuery()
{
	delete func;
	for(vector<AsyncQueryResult>::iterator itr = queries.begin(); itr != queries.end(); ++itr)
	{
		if(itr->result)
			delete itr->result;

		delete[] itr->query;
	}
}

void Database::EndThreads()
{
	SetThreadState(THREADSTATE_TERMINATE);
	while(ThreadRunning || qt)
	{
		if(query_buffer.get_size() == 0)
			query_buffer.GetCond().Broadcast();
	
		if(queries_queue.get_size() == 0)
			queries_queue.GetCond().Broadcast();

		Sleep(100);
		if(!ThreadRunning)
			break;
			
		Sleep(1000);
	}
}

bool QueryThread::run( )
{
	db->thread_proc_query( );
	return true;
}

QueryThread::~QueryThread()
{
	db->qt = NULL;
}

void Database::thread_proc_query()
{
	QueryBuffer * q;
	DatabaseConnection * con = GetFreeConnection();

	q = query_buffer.pop( );
	while( q != NULL )
	{
		PerformQueryBuffer( q, con );
		delete q;

		if( ThreadState == THREADSTATE_TERMINATE )
			break;

		q = query_buffer.pop( );
	}

	con->Busy.Release();

	// kill any queries
	q = query_buffer.pop_nowait( );
	while( q != NULL )
	{
		PerformQueryBuffer( q, NULL );
		delete q;

		q = query_buffer.pop_nowait( );
	}
}

bool Database::QueueAsyncQuery(AsyncQuery * query)
{
	for(vector<AsyncQueryResult>::iterator itr = query->queries.begin(); itr != query->queries.end(); ++itr)
	{
		if ( itr->query == NULL )
		{
			return false;
		}
	}
	query->db = this;

	query->Perform();
	return true;
}

void Database::AddQueryBuffer(QueryBuffer * b)
{
	if( qt != NULL )
		query_buffer.push( b );
	else
	{
		PerformQueryBuffer( b, NULL );
		delete b;
	}
}

void Database::FreeQueryResult(QueryResult * p)
{
	delete p;
}
