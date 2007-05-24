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

#if !defined(FIELD_H)
#define FIELD_H

class Field
{
public:

	inline void SetValue(char* value) { mValue = value; }

	inline const char *GetString() { return mValue; }
	inline float GetFloat() { return mValue ? static_cast<float>(atof(mValue)) : 0; }
	inline bool GetBool() { return mValue ? atoi(mValue) > 0 : false; }
	inline uint8 GetUInt8() { return mValue ? static_cast<uint8>(atol(mValue)) : 0; }
	inline int8 GetInt8() { return mValue ? static_cast<int8>(atol(mValue)) : 0; }
	inline uint16 GetUInt16() { return mValue ? static_cast<uint16>(atol(mValue)) : 0; }
	inline uint32 GetUInt32() { return mValue ? static_cast<uint32>(atol(mValue)) : 0; }
	inline uint32 GetInt32() { return mValue ? static_cast<int32>(atol(mValue)) : 0; }
	uint64 GetUInt64() 
	{
		if(mValue)
		{
			uint64 value;
			sscanf(mValue,I64FMTD,&value);
			return value;
		}
		else
			return 0;
	}

private:
		char *mValue;
};

#endif
