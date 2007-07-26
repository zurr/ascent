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

#if !defined (CONFIG_H)
#define CONFIG_H
#include "Common.h"

using namespace std;
struct ConfigSetting
{
	string AsString;
	bool AsBool;
	int AsInt;
	float AsFloat;
};

typedef map<uint32, ConfigSetting> ConfigBlock;

class SERVER_DECL ConfigFile
{
public:
	ConfigFile();
	~ConfigFile();

	bool SetSource(const char *file, bool ignorecase = true);
	ConfigSetting * GetSetting(const char * Block, const char * Setting);

	bool GetString(const char * block, const char* name, std::string *value);
	std::string GetStringDefault(const char * block, const char* name, const char* def);
	std::string GetStringVA(const char * block, const char* def, const char * name, ...);
	bool GetString(const char * block, char * buffer, const char * name, const char * def, uint32 len);

	bool GetBool(const char * block, const char* name, bool *value);
	bool GetBoolDefault(const char * block, const char* name, const bool def);

	bool GetInt(const char * block, const char* name, int *value);
	int GetIntDefault(const char * block, const char* name, const int def);
	int GetIntVA(const char * block, int def, const char* name, ...);

	bool GetFloat(const char * block, const char* name, float *value);
	float GetFloatDefault(const char * block, const char* name, const float def);
	float GetFloatVA(const char * block, float def, const char* name, ...);

private:
	map<uint32, ConfigBlock> m_settings;
};


class SERVER_DECL ConfigMgr
{
	// Mainly used for WS, others will probably only have one.
public:
	ConfigFile MainConfig;
	ConfigFile RealmConfig;
	ConfigFile ClusterConfig;
};

extern SERVER_DECL ConfigMgr Config;

#endif
