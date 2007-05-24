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

#if !defined(DATABASEENV_H)
#define DATABASEENV_H

/************************************************************************/
/* Database Support Setup											   */
/************************************************************************/
// Define the databases that you would like the server to be compiled with here.

#define DATABASE_SUPPORT_MYSQL
//#define DATABASE_SUPPORT_PGSQL
//#define DATABASE_SUPPORT_ORACLE10

//! Other libs we depend on.
#include "Common.h"
#include "Singleton.h"
#include "Log.h"
#include "../game/CThreads.h"

//! Our own includes.
#include <mysql/mysql.h>
#include "Database/DBCStores.h"
#include "Database/Field.h"
#include "Database/Database.h"

// Implementations

/************************************************************************/
/* MySQL																*/
/************************************************************************/
#ifdef DATABASE_SUPPORT_MYSQL
#include "Database/impl/MySQLDatabase.h"
#endif

/************************************************************************/
/* PostgreSQL														   */
/************************************************************************/
#ifdef DATABASE_SUPPORT_PGSQL
#include "Database/impl/PostgreDatabase.h"
#endif

/************************************************************************/
/* Oracle 10g														   */
/************************************************************************/
#ifdef DATABASE_SUPPORT_ORACLE10
//#include "Database/impl/Oracle10.h"
#endif

#endif
