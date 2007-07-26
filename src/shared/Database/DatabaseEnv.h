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
