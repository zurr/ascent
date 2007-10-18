/*
 * Ascent MMORPG Server
 * Copyright (C) 2005-2007 Ascent Team <http://www.ascentemu.com/>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

/* Arena and Honor Point Calculation System
 *    Copyright (c) 2007 Burlex
 */

#include "StdAfx.h"
#ifdef WIN32
static HANDLE m_abortEvent = INVALID_HANDLE_VALUE;
#endif

DayWatcherThread::DayWatcherThread()
{
	m_running = true;
}

DayWatcherThread::~DayWatcherThread()
{

}

void DayWatcherThread::terminate()
{
	m_running = false;
#ifdef WIN32
	SetEvent(m_abortEvent);
#endif
}

void DayWatcherThread::dupe_tm_pointer(tm * returnvalue, tm * mypointer)
{
	memcpy(mypointer, returnvalue, sizeof(tm));
}

void DayWatcherThread::update_settings()
{
	CharacterDatabase.Execute("REPLACE INTO server_settings VALUES(\"last_honor_update_time\", %u)", last_honor_time);
	CharacterDatabase.Execute("REPLACE INTO server_settings VALUES(\"last_arena_update_time\", %u)", last_arena_time);
}

void DayWatcherThread::load_settings()
{
	string honor_timeout = Config.MainConfig.GetStringDefault("Periods", "HonorUpdate", "daily");
	string arena_timeout = Config.MainConfig.GetStringDefault("Periods", "ArenaUpdate", "weekly");

	honor_period = get_timeout_from_string(honor_timeout.c_str(), DAILY);
	arena_period = get_timeout_from_string(arena_timeout.c_str(), WEEKLY);

	QueryResult * result = CharacterDatabase.Query("SELECT setting_value FROM server_settings WHERE setting_id = \"last_honor_update_time\"");
	if(result)
	{
		last_honor_time = result->Fetch()[0].GetUInt32();
		delete result;
	}
	else
		last_honor_time = 0;

	result = CharacterDatabase.Query("SELECT setting_value FROM server_settings WHERE setting_id = \"last_arena_update_time\"");
	if(result)
	{
		last_arena_time = result->Fetch()[0].GetUInt32();
		delete result;
	}
	else
		last_arena_time = 0;
}

void DayWatcherThread::set_tm_pointers()
{
	dupe_tm_pointer(localtime(&last_arena_time), &local_last_arena_time);
	dupe_tm_pointer(localtime(&last_honor_time), &local_last_honor_time);
}

uint32 DayWatcherThread::get_timeout_from_string(const char * string, uint32 def)
{
	if(!stricmp(string, "weekly"))
		return WEEKLY;
	else if(!stricmp(string, "monthly"))
		return MONTHLY;
	else if(!stricmp(string, "daily"))
		return DAILY;
	else if(!stricmp(string, "hourly"))
		return HOURLY;
	else
		return def;
}

bool DayWatcherThread::has_timeout_expired(tm * now_time, tm * last_time, uint32 timeoutval)
{
	switch(timeoutval)
	{
	case WEEKLY:
		{
			if( (now_time->tm_mon != last_time->tm_mon) )
				return true;
            
			return ( (now_time->tm_mday / 7) != (last_time->tm_mday / 7) );
		}
		
	case MONTHLY:
		return (now_time->tm_mon != last_time->tm_mon);

	case HOURLY:
		return ((now_time->tm_hour != last_time->tm_hour) || (now_time->tm_mday != last_time->tm_mday) || (now_time->tm_mon != last_time->tm_mon));

	case DAILY:
		return ((now_time->tm_mday != last_time->tm_mday) || (now_time->tm_mday != last_time->tm_mday));
	}
	return false;
}

bool DayWatcherThread::run()
{
	Log.Notice("DayWatcherThread", "Started.");
	currenttime = time(NULL);
	dupe_tm_pointer(localtime(&currenttime), &local_currenttime);
	load_settings();
	set_tm_pointers();
	m_busy = false;
#ifdef WIN32
	m_abortEvent = CreateEvent(NULL, NULL, FALSE, NULL);
#endif
	
	while(ThreadState != THREADSTATE_TERMINATE)
	{
		m_busy=true;
		currenttime = UNIXTIME;
		dupe_tm_pointer(localtime(&currenttime), &local_currenttime);
		if(has_timeout_expired(&local_currenttime, &local_last_honor_time, honor_period))
			update_honor();

		if(has_timeout_expired(&local_currenttime, &local_last_arena_time, arena_period))
			update_arena();
        
		if(m_dirty)
			update_settings();

		m_busy=false;
		if(ThreadState == THREADSTATE_TERMINATE)
			break;

#ifdef WIN32
		WaitForSingleObject(m_abortEvent, 120000);
#else
		Sleep(120000);
#endif
		if(!m_running)
			break;
	}
#ifdef WIN32
	CloseHandle(m_abortEvent);
#endif
	return true;
}

void DayWatcherThread::update_arena()
{
	Log.Notice("DayWatcherThread", "Running Weekly Arena Point Maintenance...");
	QueryResult * result = CharacterDatabase.Query("SELECT guid, arenaPoints FROM characters");		/* this one is a little more intensive. */
	Player * plr;
	uint32 guid, arenapoints, orig_arenapoints;
	ArenaTeam * team;
	long double X, Y;
	if(result)
	{
		do
		{
			Field * f = result->Fetch();
			guid = f[0].GetUInt32();
			arenapoints = f[1].GetUInt32();
			orig_arenapoints = arenapoints;

			/* are we in any arena teams? */
			for(uint32 i = 0; i < 3; ++i)			// 3 arena team types
			{
				team = objmgr.GetArenaTeamByGuid(guid, i);
				if(team)
				{
					/* TODO: In the future we might want to do a check that says is the player active in this arena team.
					 * Private servers are kinda smaller so this probably isn't such a good idea.
					 * - Burlex */

					/* we're in an arena team of this type! */
					/* Source: http://www.wowwiki.com/Arena_point */
					X = (long double)team->m_stat_rating;
					if(X <= 510.0)	// "if X<=510"
						continue;		// no change
					else if(X > 510.0 && X <= 1500.0)		// "if 510 < X <= 1500"
					{
						Y = (0.38 * X) - 194.0;
					}
					else			// "if X > 1500"
					{
						// how nice of wowwiki. they prevent a divide by zero for us.
						//              1426.79
						//   ---------------------------
						//                 -0.00386405X
						//         1+918.836

						long double power = ((-0.00386405) * X);
						//if(power < 1.0)
						//	power = 1.0;

						long double divisor = pow(((long double)(918.836)), power);						
						divisor += 1.0;
						//if(divisor < 1.0)
						//	divisor = 1.0;

						Y = 1426.79 / divisor;
					}

					// 2v2 teams only earn 70% (Was 60% until 13th March 07) of the arena points, 3v3 teams get 80%, while 5v5 teams get 100% of the arena points.
					if(team->m_type == ARENA_TEAM_TYPE_2V2)
					{
						Y *= 0.70;
						Y *= sWorld.getRate(RATE_ARENAPOINTMULTIPLIER2X);
					}
					else if(team->m_type == ARENA_TEAM_TYPE_3V3)
					{
						Y *= 0.80;
						Y *= sWorld.getRate(RATE_ARENAPOINTMULTIPLIER3X);
					}
					else
					{
						Y *= sWorld.getRate(RATE_ARENAPOINTMULTIPLIER5X);
					}
					
					if(Y > 1.0)
						arenapoints += long2int32(double(ceil(Y)));
				}
			}

			if(orig_arenapoints != arenapoints)
			{
				plr = objmgr.GetPlayer(guid);
				if(plr)
				{
					plr->m_arenaPoints = arenapoints;
					
					/* update visible fields (must be done through an event because of no uint lock */
					sEventMgr.AddEvent(plr, &Player::RecalculateHonor, EVENT_PLAYER_UPDATE, 100, 1, 0);
	
					/* send a little message :> */
					sChatHandler.SystemMessage(plr->GetSession(), "Your arena points have been updated! Check your PvP tab!");
				}

				/* update in sql */
				CharacterDatabase.Execute("UPDATE characters SET arenaPoints = %u WHERE guid = %u", arenapoints, guid);
			}
		}while(result->NextRow());
		delete result;
	}

	//===========================================================================
	last_arena_time = UNIXTIME;
	dupe_tm_pointer(localtime(&last_arena_time), &local_last_arena_time);
	m_dirty = true;
}

void DayWatcherThread::update_honor()
{
	Log.Notice("DayWatcherThread", "Running Daily Honor Maintenance...");
	uint32 guid, killstoday, killsyesterday, honortoday, honoryesterday, honorpoints, points_to_add;
	Player * plr;
	QueryResult * result = CharacterDatabase.Query("SELECT guid, killsToday, killsYesterday, honorToday, honorYesterday, honorPoints, honorPointsToAdd FROM characters");
	
	if(result)
	{
		do 
		{
			Field * f = result->Fetch();
			guid = f[0].GetUInt32();
			killstoday = f[1].GetUInt32();
			killsyesterday = f[2].GetUInt32();
			honortoday = f[3].GetUInt32();
			honoryesterday = f[4].GetUInt32();
			honorpoints = f[5].GetUInt32();
			points_to_add = f[6].GetUInt32();

			/* add his "honor points to be added" */
			honorpoints += points_to_add;
			points_to_add = 0;

			/* update the yesterday fields */
			killsyesterday = killstoday;
			honoryesterday = honortoday;
			honortoday = killstoday = 0;

			/* are we online? */
			plr = objmgr.GetPlayer(guid);
			if(plr)
			{
				/* we're online, update our fields here too */
				plr->m_honorPoints = honorpoints;
				plr->m_honorPointsToAdd = points_to_add;
				plr->m_honorToday = honortoday;
				plr->m_honorYesterday = honoryesterday;
				plr->m_killsToday = killstoday;
				plr->m_killsYesterday = killsyesterday;

				/* update visible fields (must be done through an event because of no uint lock */
				sEventMgr.AddEvent(plr, &Player::RecalculateHonor, EVENT_PLAYER_UPDATE, 100, 1, 0);

				/* send a little message :> */
				sChatHandler.SystemMessage(plr->GetSession(), "Your honor points have been updated! Check your PvP tab!");
			}

			/* update in database */
			CharacterDatabase.Execute("UPDATE characters SET killsToday = %u, killsYesterday = %u, honorToday = %u, honorYesterday = %u, honorPoints = %u, honorPointsToAdd = %u WHERE guid = %u",
				killstoday, killsyesterday, honortoday, honoryesterday, honorpoints, points_to_add, guid);
		} while(result->NextRow());
		delete result;
	}

	//===========================================================================
	last_honor_time = UNIXTIME;
	dupe_tm_pointer(localtime(&last_honor_time), &local_last_honor_time);
	m_dirty = true;
}

