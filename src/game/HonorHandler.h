/****************************************************************************
 *
 * Honor System
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
