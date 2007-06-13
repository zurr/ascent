/****************************************************************************
 *
 * Areatrigger Handling
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


#ifndef __AREATRIGGERHANDLER_H
#define __AREATRIGGERHANDLER_H


enum AreaTriggerType
{
	ATTYPE_NULL		 = 0,
	ATTYPE_INSTANCE	 = 1,
	ATTYPE_QUESTTRIGGER = 2,
	ATTYPE_INN		  = 3,
	ATTYPE_TELEPORT	 = 4,
	ATTYPE_SPELL		= 5,
	ATTYPE_BATTLEGROUND = 6,
};

typedef struct AreaTrigger
{
	uint32 AreaTriggerID;
	uint8 Type;
	uint32 Mapid;
	uint32 PendingScreen;
	char * Name;
	float x;
	float y;
	float z;
	float o;
	uint32 required_honor_rank;
	uint32 required_level;
}AreaTrigger;



#endif
