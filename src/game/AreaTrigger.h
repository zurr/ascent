/****************************************************************************
 *
 * Areatrigger Handling
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
