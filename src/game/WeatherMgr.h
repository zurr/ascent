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

#ifndef __WEATHERMGR_H
#define __WEATHERMGR_H

#define WEATHER_DENSITY_UPDATE 0.05f

class WeatherInfo;
class WeatherMgr;

void BuildWeatherPacket(WorldPacket * data, uint32 Effect, float Density);

class WeatherMgr :  public Singleton < WeatherMgr >
{
public:
	WeatherMgr();
	~WeatherMgr();

	void LoadFromDB();
	void SendWeather(Player *plr);

private:
	std::map<uint32, WeatherInfo*> m_zoneWeathers;
};

class WeatherInfo : public EventableObject
{
	friend class WeatherMgr;
public:
	WeatherInfo();
	~WeatherInfo();

	void Update();	
	void SendUpdate();
	void SendUpdate(Player *plr);

protected:
	void _GenerateWeather();

	uint32 m_zoneId;

	uint32 m_totalTime;
	uint32 m_currentTime;

	float m_maxDensity;
	float m_currentDensity;

	uint32 m_currentEffect;
	bool m_increase;
	std::map<uint32, uint32> m_effectValues;
};

#define sWeatherMgr WeatherMgr::getSingleton()

#endif
