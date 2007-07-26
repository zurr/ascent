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

#ifndef __DBC_H
#define __DBC_H

#include "../Common.h"

enum DBCFmat
{
	F_STRING = 0,
	F_INT = 1,
	F_FLOAT = 2,
	F_NADA = 3
};

class DBC {
	int rows, cols, dblength,weird2; // Weird2 = most probably line length
	unsigned int* tbl;
	char* db,name[MAX_PATH];
	bool loaded;
	DBCFmat *format;
public:
	DBC();
	void Load(const char *filename);
	void CSV(char *filename, bool info = false);
	void GuessFormat();
	DBCFmat GuessFormat(int row, int col);
	void FormatCSV(const char *filename, bool info = false);
	void Lookup(char* out, int row, int col,char isstr=0,bool onlystr=false);
	void LookupFormat(char* out, int row, int col);
	void RowToStruct(void* out, int row);
	bool IsLoaded() { return loaded; }
	void* __fastcall GetRow(unsigned const int index) { return (void *)&tbl[index*cols]; }
	char* __fastcall LookupString(unsigned const int offset) { return db+offset; }
	int GetRows() { return rows; }
	int GetCols() { return cols; }
	int GetDBSize() { return dblength; }
	~DBC();
};
			
#endif
