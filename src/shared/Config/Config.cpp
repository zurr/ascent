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

#include "ConfigEnv.h"
ConfigMgr Config;

ConfigFile::ConfigFile()
{
}


ConfigFile::~ConfigFile()
{
	
}

unsigned const char lowermap[256] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,				/* 0-19 */
				20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39,				/* 20-39 */
				40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59,				/* 40-59 */
				60, 61, 62, 63, 64, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111,		/* 60-79 */
				112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 94, 95, 96, 97, 98, 99,		/* 80-99 */
				100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119,	/* 100-119 */
				120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139,	/* 120-139 */
				140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159,	/* 140-159 */
				160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179,	/* 160-179 */
				180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199,	/* 180-199 */
				200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219,	/* 200-219 */
				220, 221, 222, 223, 224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239,	/* 220-239 */
				240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255				/* 240-255 */
};

void remove_spaces(string& str)
{
	while(*str.begin() == ' ')
		str.erase(str.begin());
}

void remove_all_spaces(string& str)
{
	string::size_type off = str.find(" ");
	while(off != string::npos)
	{
		str.erase(off, 1);
		off = str.find(" ");
	}
}

bool is_comment(string& str, bool * in_multiline_quote)
{
	string stemp = str;
	remove_spaces(stemp);
	if(stemp.length() == 0)
		return false;

	if(stemp[0] == '/')
	{
		if(stemp.length() < 2)
			return false;

		if(stemp[1] == '*')
		{
			*in_multiline_quote = true;
			return true;
		}
		else if(stemp[2] == '/')
		{
			return true;
		}
	}

	if(stemp[0] == '#')
		return true;

	return false;
}

void apply_setting(string & str, ConfigSetting & setting)
{
	setting.AsString = str;
	setting.AsInt = atoi(str.c_str());
	setting.AsBool = (setting.AsInt > 0);

	/* check for verbal yes/no answers */
	if(str.length() > 1)
	{
		// this might be a yes/no?
		if(str.size() >= 3 && !strnicmp("yes", str.c_str(), 3))
		{
			setting.AsBool = true;
			setting.AsInt = 1;
		}
		else if(str.size() >= 2 && !strnicmp("no", str.c_str(), 2))
		{
			setting.AsBool = false;
			setting.AsInt = 0;
		}
	}
}

uint32 hash(const char * str)
{
	register int len = strlen(str);
	register uint32 ret = 0;
	register int i = 0;
	for(; i < len; ++i)
		ret += 5 * ret + lowermap[(unsigned char)str];
	return ret;
}

uint32 hash(string& str)
{
	return hash(str.c_str());
}

bool ConfigFile::SetSource(const char *file, bool ignorecase)
{
	/* wipe any existing settings. */
	m_settings.clear();

	/* open the file */
	if(file != 0)
	{
		FILE * f = fopen(file, "r");
		int length;
		if(!f)
		{
			sLog.outError("Could not open %s.", file);
			return false;
		}

		/* get the length of the file */
		fseek(f, 0, SEEK_END);
		length = ftell(f);
		fseek(f, 0, SEEK_SET);

		/* read the file into one large buffer */
		string buffer;
		buffer.reserve(length);
		if(length != fread((void*)buffer.data(), 1, length, f))
		{
			sLog.outError("Could not read file %s.", file);
			return false;
		}

		/* close the file, it is no longer needed */
		fclose(f);

		/* let's parse it. */
		string line;
		string::size_type end;
		string::size_type offset;
		bool in_multiline_comment = false;
		bool in_multiline_quote = false;
		bool in_multiline_setting = false;
		bool in_block = false;
		string current_setting = "";
		string current_variable = "";
		string current_block = "";
		ConfigBlock current_block_map;
		ConfigSetting current_setting_struct;

		for(;;)
		{
			/* grab a line. */
			end = buffer.find("\n");
			if(end == string::npos)
				break;

			line = buffer.substr(0, end);
			buffer.erase(0, end);
			goto parse;

parse:
			/* are we a comment? */
			if(!in_multiline_comment && is_comment(line, &in_multiline_comment))
			{
				/* our line is a comment. */
				if(!in_multiline_comment)
				{
					/* the entire line is a comment, skip it. */
					continue;
				}
			}

			/* handle our cases */
			if(in_multiline_comment)
			{
				// we need to find a "*/".
				offset = line.find("*/", 0);
				
				/* skip this entire line, eh? */
				if(offset == string::npos)
					continue;

				/* remove up to the end of the comment block. */
				line.erase(0, offset + 2);
				in_multiline_comment = false;
			}
		
			if(in_block)
			{
				/* handle settings across multiple lines */
				if(in_multiline_quote)
				{
					/* attempt to find the end of the quote block. */
					offset = line.find("\"");

					if(offset == string::npos)
					{
						/* append the whole line to the quote. */
						current_setting += line;
						current_setting += "\n";
						continue;
					}
	                
					/* only append part of the line to the setting. */
					current_setting.append(line.c_str(), offset+1);
					line.erase(0, offset + 1);
				
					/* append the setting to the config block. */
					if(current_block == "" || current_variable == "")
					{
						sLog.outError("Quote without variable.");
						return false;
					}

					/* apply the setting */
					apply_setting(current_setting, current_setting_struct);

					/* the setting is done, append it to the current block. */
                    current_block_map[hash(current_variable)] = current_setting_struct;

					/* no longer doing this setting, or in a quote. */
					current_setting = "";
					current_variable = "";
                    in_multiline_quote = false;					
				}

				/* remove any leading spaces */
				remove_spaces(line);

				/* our target is a *setting*. look for an '=' sign, this is our seperator. */
                offset = line.find("=");
				if(offset != string::npos)
				{
					ASSERT(current_variable == "");
					current_variable = line.substr(0, offset+1);

					/* remove any spaces from the end of the setting */
					remove_all_spaces(current_variable);

					/* remove the directive *and* the = from the line */
					line.erase(0, offset + 1);
				}

				/* look for the opening quote. this signifies the start of a setting. */
				offset = line.find("\"");
				if(offset != string::npos)
				{
					ASSERT(current_setting == "");
					ASSERT(current_variable == "");

					/* try and find the ending quote */
					end = line.find("\"", offset + 1);
					if(end != string::npos)
					{
						/* the closing quote is on the same line, oh goody. */
						current_setting = line.substr(offset+1, offset-end-1);

						/* erase up to the end */
						line.erase(0, end + 1);
						
						/* apply the setting */
						apply_setting(current_setting, current_setting_struct);

						/* the setting is done, append it to the current block. */
						current_block_map[hash(current_variable)] = current_setting_struct;

						/* no longer doing this setting, or in a quote. */
						current_setting = "";
						current_variable = "";
						in_multiline_quote = false;		
					}
					else
					{
						/* the closing quote is not on the same line. means we'll try and find it on
						   the next. */
						current_setting.append(line.c_str(), offset);

						/* skip to the next line. (after setting our condition first, of course :P */
						in_multiline_quote = true;
						continue;
					}
				}

				/* are we at the end of the block yet? */
				if(line.find(">") != string::npos)
				{
					// freeeee!
					in_block = false;
					
					/* assign this block to the main "big" map. */
					m_settings[hash(current_block)] = current_block_map;

					/* erase all data for this so it doesn't seep through */
					current_block_map.clear();
					current_setting = "";
					current_variable = "";
					current_block = "";
				}
			}
			else
			{
				/* we're not in a block. look for the start of one. */
				offset = line.find("<");

				if(offset != string::npos)
				{
					in_block = true;

					/* whee, a block! let's cut the string and re-parse. */
					line.erase(0, offset + 1);

					/* find the name of the block first, though. */
					offset = line.find(" ");
					if(offset != string::npos)
					{
						current_block = line.substr(0, offset);
						line.erase(0, offset + 1);
					}
					else
					{
						sLog.outError("Block without name.");
						return false;
					}

					/* skip back */
					goto parse;
				}
			}
		}

		/* handle any errors */
		if(in_block)
		{
			sLog.outError("Unterminated block.");
			return false;
		}

		if(in_multiline_comment)
		{
			sLog.outError("Unterminated comment.");
			return false;
		}

		if(in_multiline_quote)
		{
			sLog.outError("Unterminated quote.");
			return false;
		}

		/* we're all good :) */
		return true;
	}

	return false;
}

ConfigSetting * ConfigFile::GetSetting(const char * Block, const char * Setting)
{
	uint32 block_hash = hash(Block);
	uint32 setting_hash = hash(Setting);

	/* find it in the big map */
	map<uint32, ConfigBlock>::iterator itr = m_settings.find(block_hash);
	if(itr != m_settings.end())
	{
		ConfigBlock::iterator it2 = itr->second.find(setting_hash);
		if(it2 != itr->second.end())
			return &(it2->second);

		return 0;
	}

	return 0;
}

bool ConfigFile::GetString(const char * block, const char* name, std::string *value)
{
	ConfigSetting * Setting = GetSetting(block, name);
	if(Setting == 0)
		return false;

	*value = Setting->AsString;
	return true;
}


std::string ConfigFile::GetStringDefault(const char * block, const char* name, const char* def)
{
	string ret;
	return GetString(block, name, &ret) ? ret : def;
}


bool ConfigFile::GetBool(const char * block, const char* name, bool *value)
{
	ConfigSetting * Setting = GetSetting(block, name);
	if(Setting == 0)
		return false;

	*value = Setting->AsBool;
	return true;
}


bool ConfigFile::GetBoolDefault(const char * block, const char* name, const bool def /* = false */)
{
	bool val;
	return GetBool(block, name, &val) ? val : def;
}

bool ConfigFile::GetInt(const char * block, const char* name, int *value)
{
	ConfigSetting * Setting = GetSetting(block, name);
	if(Setting == 0)
		return false;

	return Setting->AsInt;
}

bool ConfigFile::GetFloat(const char * block, const char* name, float *value)
{
	ConfigSetting * Setting = GetSetting(block, name);
	if(Setting == 0)
		return false;

	*value = Setting->AsFloat;
	return true;
}

int ConfigFile::GetIntDefault(const char * block, const char* name, const int def)
{
	int val;
	return GetInt(block, name, &val) ? val : def;
}

float ConfigFile::GetFloatDefault(const char * block, const char* name, const float def)
{
	float val;
	return (GetFloat(block, name, &val) ? val : def);
}

int ConfigFile::GetIntVA(const char * block, int def, const char* name, ...)
{
	va_list ap;
	va_start(ap, name);
	char str[150];
	vsprintf(str, name, ap);
	va_end(ap);
	int val;
	return GetInt(block, str, &val) ? val : def;
}

float ConfigFile::GetFloatVA(const char * block, float def, const char* name, ...)
{
	va_list ap;
	va_start(ap, name);
	char str[150];
	vsprintf(str, name, ap);
	va_end(ap);
	float val;
	return GetFloat(block, str, &val) ? val : def;
}

std::string ConfigFile::GetStringVA(const char * block, const char* def, const char * name, ...)
{
	va_list ap;
	va_start(ap, name);
	char str[150];
	vsprintf(str, name, ap);
	va_end(ap);

	return GetStringDefault(block, str, def);
}

bool ConfigFile::GetString(const char * block, char * buffer, const char * name, const char * def, uint32 len)
{
	string val = GetStringDefault(block, name, def);
	int blen = val.length();
	if(blen > len)
		blen = len;

	memcpy(buffer, val.c_str(), blen);
	buffer[blen] = 0;
    
	return true;
}

