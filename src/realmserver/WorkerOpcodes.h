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

#ifndef _WORKER_OPCODES_H
#define _WORKER_OPCODES_H

enum WorkerServerOpcodes
{
	ISMSG_AUTH_REQUEST					= 1,
	ICMSG_AUTH_REPLY					= 2,
	ISMSG_AUTH_RESULT					= 3,
	ICMSG_REGISTER_WORKER				= 4,
	ISMSG_REGISTER_RESULT				= 5,
	ISMSG_CREATE_INSTANCE				= 6,
	ISMSG_DESTROY_INSTANCE				= 7,
	ISMSG_PLAYER_LOGIN					= 8,
	ISMSG_PLAYER_INFO					= 9,
	ICMSG_PLAYER_INFO					= 10,
	ISMSG_WOW_PACKET					= 11,
	ICMSG_WOW_PACKET					= 12,
	ICMSG_WHISPER						= 13,
    ICMSG_CHAT							= 14,
	ISMSG_WHISPER						= 15,
	ISMSG_CHAT							= 16,
	ISMSG_PACKED_PLAYER_INFO			= 17,
	ICMSG_PLAYER_LOGIN_RESULT			= 18,
	ICMSG_PLAYER_LOGOUT					= 19,
	ISMSG_DESTROY_PLAYER_INFO			= 20,
	ICMSG_TELEPORT_REQUEST				= 21,
	ISMSG_TELEPORT_RESULT				= 22,
	ISMSG_PLAYER_CHANGED_SERVERS		= 23,
	ISMSG_PLAYER_CHANGE_INSTANCES		= 24,
	ISMSG_CREATE_PLAYER					= 25,
	ICMSG_PLAYER_CHANGE_SERVER_INFO		= 26,

	IMSG_NUM_TYPES,
};

#endif		// _WORKER_OPCODES_H