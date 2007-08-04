/****************************************************************************
 *
 * Weather System
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

#include "StdAfx.h"

initialiseSingleton( WeatherMgr );

WeatherMgr::WeatherMgr()
{
}

WeatherMgr::~WeatherMgr()
{
	std::map<uint32, WeatherInfo*>::iterator itr;
	for(itr = m_zoneWeathers.begin(); itr != m_zoneWeathers.end(); itr++)
	{
		delete itr->second;
	}

	m_zoneWeathers.clear();
}



void WeatherMgr::LoadFromDB()
{
	sLog.outString("  Loading Weather...");
	QueryResult *result = WorldDatabase.Query( "SELECT * FROM weather" );

	if( !result )
		return;

	do
	{
		Field *fields = result->Fetch();
		WeatherInfo *wi = new WeatherInfo;
		wi->m_zoneId = fields[0].GetUInt32();
		wi->m_effectValues[0] = fields[1].GetUInt32();
		wi->m_effectValues[1] = fields[2].GetUInt32();
		wi->m_effectValues[2] = fields[3].GetUInt32();
		m_zoneWeathers[wi->m_zoneId] = wi;

		wi->_GenerateWeather();
	} while( result->NextRow() );

	delete result;
}

void WeatherMgr::SendWeather(Player *plr)
{
	std::map<uint32, WeatherInfo*>::iterator itr;
	itr = m_zoneWeathers.find(plr->GetZoneId());

	if (itr == m_zoneWeathers.end())
	{
		WorldPacket data(SMSG_WEATHER, 13);
		BuildWeatherPacket(&data, 0, 0);
		plr->GetSession()->SendPacket( &data );
		plr->m_lastSeenWeather = 0;
	}
	else
	{
		itr->second->SendUpdate(plr);
	}
}

void WeatherMgr::BuildWeatherPacket(WorldPacket * data, uint32 Effect, float Density)
{
    data->Initialize(SMSG_WEATHER);
    /// effect 0 is no special weather
    if(Effect == 0)
    {
        *data << uint32 (0);        // effect
        *data << float  (0.0f);     // Density
        *data << uint32 (0);        // Sound id
        *data << uint8  (0);		// unknown
        return;
    }
    /// all other weather effects
    *data << Effect;
    *data << Density;
    *data << _GetWeatherSoundEffect(Effect,Density);
    *data << uint8(0);
}

uint32 WeatherMgr::_GetWeatherSoundEffect(uint32 Effect, float Density)
{
    switch(Effect)
    {
    case 1:                                             //rain
        if(Density<0.33333334f)
            return WEATHER_RAINLIGHT;
        else if(Density  <0.6666667f)
            return WEATHER_RAINMEDIUM;
        else
            return WEATHER_RAINHEAVY;
        break;
    case 2:                                             //snow
        if(Density<0.33333334f)
            return WEATHER_SNOWLIGHT;
        else if(Density<0.6666667f)
            return WEATHER_SNOWMEDIUM;
        else
            return WEATHER_SNOWHEAVY;
        break;
    case 3:                                             //storm
        if(Density<0.33333334f)
            return WEATHER_SANDSTORMLIGHT;
        else if(Density<0.6666667f)
            return WEATHER_SANDSTORMMEDIUM;
        else
            return WEATHER_SANDSTORMHEAVY;
        break;
    case 0:                                             //fine
    default:
        return WEATHER_NOSOUND;
        break;
    }
}

WeatherInfo::WeatherInfo()
{
	m_currentDensity = 0;
	m_currentEffect = 0;
	m_currentTime = 0;
	m_maxDensity = 0;
	m_totalTime = 0;
	m_zoneId = 0;
	m_increase = true;
}

WeatherInfo::~WeatherInfo()
{

}

void WeatherInfo::_GenerateWeather()
{
	m_currentTime = 0;
	m_currentDensity = 0;
	m_currentEffect = 0;

	uint32 rv = sRand.randInt(100);

	std::map<uint32, uint32>::iterator itr;

	for(itr = m_effectValues.begin(); itr != m_effectValues.end(); itr++)
	{
		if (rv <= itr->second)
		{
			m_currentEffect = itr->first;
			break;
		}
		else
		{
			rv -= itr->second;
		}
	}

	m_maxDensity = sRand.rand(2); //0 - 2
	m_totalTime = (sRand.randInt(11) + 5)*1000*60;
	m_increase = true;

	SendUpdate();

	sEventMgr.AddEvent(this, &WeatherInfo::Update, EVENT_WEATHER_UPDATE, 
		(uint32)(m_totalTime/ceil(m_maxDensity/WEATHER_DENSITY_UPDATE)*2), 0);
}

void WeatherInfo::Update()
{
	if (m_increase)
	{
		m_currentDensity += WEATHER_DENSITY_UPDATE;
		if (m_currentDensity >= m_maxDensity)
		{
			m_currentDensity = m_maxDensity;
			m_increase = false;
		}
	}
	else
	{
		m_currentDensity -= WEATHER_DENSITY_UPDATE;
		if (m_currentDensity <= 0)
		{
			m_currentDensity = 0;
			sEventMgr.RemoveEvents(this, EVENT_WEATHER_UPDATE);
			_GenerateWeather();
			return;
		}
	}
	SendUpdate();
}

void WeatherInfo::SendUpdate()
{
	WorldPacket data(SMSG_WEATHER, 13);
	sWeatherMgr.BuildWeatherPacket(&data, m_currentEffect, m_currentDensity);
	sWorld.SendZoneMessage(&data, m_zoneId, 0);
}

void WeatherInfo::SendUpdate(Player *plr)
{
	if(plr->m_lastSeenWeather == m_currentEffect)
		return;

	plr->m_lastSeenWeather = m_currentEffect;

	WorldPacket data(SMSG_WEATHER, 13);
	sWeatherMgr.BuildWeatherPacket(&data, m_currentEffect, m_currentDensity);
	plr->GetSession()->SendPacket( &data );
}
