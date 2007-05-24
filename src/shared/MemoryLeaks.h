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

#ifndef WOWSERVER_MEMORY_H
#define WOWSERVER_MEMORY_H

#include "Common.h"
#include "Singleton.h"

#if COMPILER == COMPILER_MICROSOFT
#  define _CRTDBG_MAP_ALLOC
#  include <stdlib.h>
#  include <crtdbg.h>
#endif

struct MemoryManager : public Singleton < MemoryManager > {
	MemoryManager();
};

#endif

