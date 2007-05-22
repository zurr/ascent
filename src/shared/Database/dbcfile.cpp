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

#include "dbcfile.h"
#include <stdio.h>

DBCFile::DBCFile()
{
	
}

bool DBCFile::open(const char*fn)
{
	FILE*pf=fopen(fn,"rb");
	if(!pf)return false;
	char header[4];

	fread(header,4,1,pf); // Number of records
	assert(header[0]=='W' && header[1]=='D' && header[2]=='B' && header[3] == 'C');
	fread(&recordCount,4,1,pf); // Number of records
	fread(&fieldCount,4,1,pf); // Number of fields
	fread(&recordSize,4,1,pf); // Size of a record
	fread(&stringSize,4,1,pf); // String size
	
	data = new unsigned char[recordSize*recordCount+stringSize];
	stringTable = data + recordSize*recordCount;
	fread(data,recordSize*recordCount+stringSize,1,pf);
	fclose(pf);
	return true;
}

DBCFile::~DBCFile()
{
	delete [] data;
}

DBCFile::Record DBCFile::getRecord(size_t id)
{
	assert(data);
	return Record(*this, data + id*recordSize);
}

DBCFile::Iterator DBCFile::begin()
{
	assert(data);
	return Iterator(*this, data);
}
DBCFile::Iterator DBCFile::end()
{
	assert(data);
	return Iterator(*this, stringTable);
}

