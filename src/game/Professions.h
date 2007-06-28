/****************************************************************************
 *
 * Profession Defines
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

#ifndef _PROFFESIONS_H
#define _PROFFESIONS_H

uint32 GetGOReqSkill(GameObject * gameObjTarget);
void UseFishing(Player *player);
void AddItemFromDisenchant(ItemPrototype *proto,Player*owner);
void AddItemFromProspecting(ItemPrototype *proto,Player*owner);

#endif
