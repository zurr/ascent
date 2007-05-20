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

class SERVER_DECL ConfigFile
{
public:
    ConfigFile();
    ~ConfigFile();

    bool SetSource(const char *file, bool ignorecase = true);

    bool GetString(const char* name, std::string *value);
    std::string GetStringDefault(const char* name, const char* def);
    std::string GetStringVA(const char* def, const char * name, ...);

    bool GetBool(const char* name, bool *value);
    bool GetBoolDefault(const char* name, const bool def = false);

    bool GetInt(const char* name, int *value);
    int GetIntDefault(const char* name, const int def);
    int GetIntVA(int def, const char* name, ...);

    bool GetFloat(const char* name, float *value);
    float GetFloatDefault(const char* name, const float def);
    float GetFloatVA(float def, const char* name, ...);


private:
    DOTCONFDocument *mConf;
};


class SERVER_DECL ConfigMgr
{
    // Mainly used for WS, others will probably only have one.
public:
    ConfigFile MainConfig;
    ConfigFile RealmConfig;
};

extern SERVER_DECL ConfigMgr Config;

#endif
