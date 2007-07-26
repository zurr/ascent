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

//
// NameTables.h
//

#ifndef __NAMETABLES_H
#define __NAMETABLES_H

struct NameTableEntry
{
	uint32 id;
	const char *name;
};

inline const char* LookupName(uint32 id, NameTableEntry *table)
{
	for(uint32 i = 0; table[i].name != 0; i++)
	{
		if (table[i].id == id)
			return table[i].name;
	}

	return "UNKNOWN";
}

extern NameTableEntry g_worldOpcodeNames[];
extern NameTableEntry g_logonOpcodeNames[];
extern NameTableEntry g_pluginOpcodeNames[];

#endif
