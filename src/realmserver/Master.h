#ifndef _R_MASTER_H
#define _R_MASTER_H

extern MySQLDatabase * Database_World;
extern MySQLDatabase * Database_Character;

#define WorldDatabase (*Database_World)
#define CharacterDatabase (*Database_Character)

#endif
