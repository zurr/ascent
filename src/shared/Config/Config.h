/****************************************************************************
 *
 * General Object Type File
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
