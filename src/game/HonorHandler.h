/****************************************************************************
 *
 * Honor System
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

#ifndef HONORHANDLER_H
#define HONORHANDLER_H

class HonorHandler
{
public:
	static int32 CalculateHonorPointsForKill(Player *pPlayer, Unit* pVictim);
	
	static void RecalculateHonorFields(Player *pPlayer);
	static void DailyFieldMove(Player *pPlayer);

	static void PerformStartupTasks();

	static void AddHonorPointsToPlayer(Player *pPlayer, uint32 uAmount);
	static void OnPlayerKilledUnit(Player *pPlayer, Unit* pVictim);
	static time_t GetNextUpdateTime();
	
};


#endif
