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

#ifndef _THREADING_GUARD_H
#define _THREADING_GUARD_H

#include "Mutex.h"

/************************************************************************/
/* Guard class, unlocks mutex on destroy								*/
/************************************************************************/
// crossplatform :)

#ifdef _ALLOW_GUARD_CLASS_USE

class SERVER_DECL Guard
{
public:
	Guard(Mutex& mutex) : target(mutex)
	{
		target.Acquire();
	}

	~Guard()
	{
		target.Release();
	}

protected:
	Mutex& target;
};

#endif

#endif

