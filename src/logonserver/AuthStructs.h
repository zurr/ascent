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

#ifndef AUTHSTRUCTS_H
#define AUTHSTRUCTS_H

#if __GNUC__ && (GCC_MAJOR < 4 || GCC_MAJOR == 4 && GCC_MINOR < 1)
#pragma pack(1)
#else
#pragma pack(push,1)
#endif 

typedef struct {
	uint8   cmd;
	uint8   error;		  // 0x00
	uint16  size;		   // 0x0026
	uint8   gamename[4];	// 'WoW'
	uint8   version1;	   // 0x00
	uint8   version2;	   // 0x08 (0.8.0)
	uint8   version3;	   // 0x00
	uint16  build;		  // 3734
	uint8   platform[4];	// 'x86'
	uint8   os[4];		  // 'Win'
	uint8   country[4];	 // 'enUS'
	uint32  timezone_bias;  // -419
	uint32  ip;			 // client ip
	uint8   I_len;		  // length of account name
	uint8   I[50];		   // account name
} sAuthLogonChallenge_C;

typedef sAuthLogonChallenge_C sAuthReconnectChallenge_C;

typedef struct {
	uint8   cmd;			// 0x00 CMD_AUTH_LOGON_CHALLENGE
	uint8   error;		  // 0 - ok
	uint8   unk2;		   // 0x00
	uint8   B[32];
	uint8   g_len;		  // 0x01
	uint8   g[1];
	uint8   N_len;		  // 0x20
	uint8   N[32];
	uint8   s[32];
	uint8   unk3[16];
} sAuthLogonChallenge_S;

typedef struct {
	uint8   cmd; // 0x01
	uint8   A[32];
	uint8   M1[20];
	uint8   crc_hash[20];
	uint8   number_of_keys;
	uint8   unk;
} sAuthLogonProof_C;

typedef struct {
	uint16  unk1;
	uint32  unk2;
	uint8   unk3[4];
	uint16  unk4[20];	   // sha1(A,g,?)
}  sAuthLogonProofKey_C;

typedef struct {
	uint8   cmd;			// 0x01 CMD_AUTH_LOGON_PROOF
	uint8   error;
	uint8   M2[20];
	uint32  unk2;
	uint16  unk203;
} sAuthLogonProof_S;


#if __GNUC__ && (GCC_MAJOR < 4 || GCC_MAJOR == 4 && GCC_MINOR < 1)
#pragma pack()
#else
#pragma pack(pop)
#endif

enum CLIENT_VERSIONS
{
	CLIENT_2_0_3		= 6299,
	CLIENT_2_0_5		= 6320,
	CLIENT_2_0_6		= 6337,
	CLIENT_2_0_7		= 6383,
};

#endif

