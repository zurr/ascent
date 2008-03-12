/*
 * Ascent MMORPG Server
 * Copyright (C) 2005-2007 Ascent Team <http://www.ascentemu.com/>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "StdAfx.h"
#define SWIMMING_TOLERANCE_LEVEL -0.08f
#define MOVEMENT_PACKET_TIME_DELAY 500

#ifdef WIN32

#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")
#define DELTA_EPOCH_IN_USEC  11644473600000000ULL

uint32 TimeStamp()
{
	//return timeGetTime();

	FILETIME ft;
	uint64 t;
	GetSystemTimeAsFileTime(&ft);

	t = (uint64)ft.dwHighDateTime << 32;
	t |= ft.dwLowDateTime;
	t /= 10;
	t -= DELTA_EPOCH_IN_USEC;

	return uint32(((t / 1000000L) * 1000) + ((t % 1000000L) / 1000));
}

ASCENT_INLINE uint32 mTimeStamp()
{
	return timeGetTime();
}

#else

uint32 TimeStamp()
{
	struct timeval tp;
	gettimeofday(&tp, NULL);
	return (tp.tv_sec * 1000) + (tp.tv_usec / 1000);
}

ASCENT_INLINE uint32 mTimeStamp()
{
	struct timeval tp;
	gettimeofday(&tp, NULL);
	return (tp.tv_sec * 1000) + (tp.tv_usec / 1000);
}

#endif

void WorldSession::HandleMoveWorldportAckOpcode( WorldPacket & recv_data )
{
	GetPlayer()->SetPlayerStatus(NONE);
	if(_player->IsInWorld())
	{
		// get outta here
		return;
	}
	sLog.outDebug( "WORLD: got MSG_MOVE_WORLDPORT_ACK." );
	
	if(_player->m_CurrentTransporter && _player->GetMapId() != _player->m_CurrentTransporter->GetMapId())
	{
		/* wow, our pc must really suck. */
		Transporter * pTrans = _player->m_CurrentTransporter;
		float c_tposx = pTrans->GetPositionX() + _player->m_TransporterX;
		float c_tposy = pTrans->GetPositionY() + _player->m_TransporterY;
		float c_tposz = pTrans->GetPositionZ() + _player->m_TransporterZ;

		WorldPacket dataw(SMSG_NEW_WORLD, 20);
		dataw << pTrans->GetMapId() << c_tposx << c_tposy << c_tposz << _player->GetOrientation();
		SendPacket(&dataw);
	}
	else
	{
		_player->m_TeleportState = 2;
		_player->AddToWorld();
	}
}

void WorldSession::HandleMoveTeleportAckOpcode( WorldPacket & recv_data )
{
	uint64 guid;
	recv_data >> guid;
	if(guid == _player->GetGUID())
	{
		if(sWorld.antihack_teleport && !(HasGMPermissions() && sWorld.no_antihack_on_gm) && _player->GetPlayerStatus() != TRANSFER_PENDING)
		{
			/* we're obviously cheating */
			sCheatLog.writefromsession(this, "Used teleport hack, disconnecting.");
			Disconnect();
			return;
		}

		if(sWorld.antihack_teleport && !(HasGMPermissions() && sWorld.no_antihack_on_gm) && _player->m_position.Distance2DSq(_player->m_sentTeleportPosition) > 625.0f)	/* 25.0f*25.0f */
		{
			/* cheating.... :( */
			sCheatLog.writefromsession(this, "Used teleport hack {2}, disconnecting.");
			Disconnect();
			return;
		}

		sLog.outDebug( "WORLD: got MSG_MOVE_TELEPORT_ACK." );
		GetPlayer()->SetPlayerStatus(NONE);
		GetPlayer()->SetMovement(MOVE_UNROOT,5);
		_player->ResetHeartbeatCoords();

		if(GetPlayer()->GetSummon() != NULL)		// move pet too
			GetPlayer()->GetSummon()->SetPosition((GetPlayer()->GetPositionX() + 2), (GetPlayer()->GetPositionY() + 2), GetPlayer()->GetPositionZ(), float(M_PI));
		if(_player->m_sentTeleportPosition.x != 999999.0f)
		{
			_player->m_position = _player->m_sentTeleportPosition;
			_player->m_sentTeleportPosition.ChangeCoords(999999.0f,999999.0f,999999.0f);
		}
	}

}

void _HandleBreathing(MovementInfo &movement_info, Player * _player, WorldSession * pSession)
{

	// no water breathing is required
	if( !sWorld.BreathingEnabled || _player->FlyCheat || _player->m_bUnlimitedBreath || !_player->isAlive() || _player->GodModeCheat )
	{
		// player is flagged as in water
		if( _player->m_UnderwaterState & UNDERWATERSTATE_SWIMMING  )
		{
			_player->m_UnderwaterState &= ~UNDERWATERSTATE_SWIMMING;
			if( _player->FlyCheat )
			{
				if(_player->m_lastMoveType != 2)
				{
					_player->m_lastMoveType = 2; // flying
					_player->ResetHeartbeatCoords();
				}
			}
			else
			{
				if(_player->m_lastMoveType != 1)
				{
					_player->m_lastMoveType = 1; // swimming
					_player->ResetHeartbeatCoords();
				}
			}
		}

		// player is flagged as under water
		if( _player->m_UnderwaterState & UNDERWATERSTATE_UNDERWATER )
		{
			_player->m_UnderwaterState &= ~UNDERWATERSTATE_UNDERWATER;
			WorldPacket data(SMSG_START_MIRROR_TIMER, 20);
			data << uint32(1) << _player->m_UnderwaterTime << _player->m_UnderwaterMaxTime << int32(-1) << uint32(0);
			pSession->SendPacket(&data);
		}

		// player is above water level
		if( pSession->m_bIsWLevelSet )
		{
			if( ( movement_info.z + _player->m_noseLevel ) > pSession->m_wLevel )
			{
				// remove druid aquatic form on land
				if( _player->getClass() == DRUID )
					_player->RemoveAura( 1066 );

				// unset swim session water level
				pSession->m_bIsWLevelSet = false;
			}
		}

		return;
	}

	//player is swiming and not flagged as in the water
	if( movement_info.flags & MOVEFLAG_SWIMMING && !( _player->m_UnderwaterState & UNDERWATERSTATE_SWIMMING ) )
	{
		// dismount if mounted
		if( _player->m_MountSpellId )
			_player->RemoveAura( _player->m_MountSpellId );

		// get water level only if it was not set before
		if( !pSession->m_bIsWLevelSet )
		{
			// water level is somewhere below the nose of the character when entering water
			pSession->m_wLevel = movement_info.z + _player->m_noseLevel * 0.95f;
			pSession->m_bIsWLevelSet = true;
		}

		if( _player->FlyCheat )
		{
			if( _player->m_lastMoveType != 2 )
			{
				_player->m_lastMoveType = 2; // flying
				_player->ResetHeartbeatCoords();
			}
		}
		else
		{
			if( _player->m_lastMoveType != 1 )
			{
				_player->m_lastMoveType = 1; // swimming
				_player->ResetHeartbeatCoords();
			}
		}
		_player->m_UnderwaterState |= UNDERWATERSTATE_SWIMMING;
	}

	// player is not swimming and is not stationary and is flagged as in the water
	if( !( movement_info.flags & MOVEFLAG_SWIMMING ) && !( movement_info.flags & MOVEFLAG_MOVE_STOP ) && _player->m_UnderwaterState & UNDERWATERSTATE_SWIMMING )
	{
		// player is above water level
		if( ( movement_info.z + _player->m_noseLevel ) > pSession->m_wLevel )
		{
			// remove druid aquatic form on land
			if( _player->getClass() == DRUID )
				_player->RemoveAura( 1066 );

			// unset swim session water level
			pSession->m_bIsWLevelSet = false;

			if( movement_info.flags & MOVEFLAG_AIR_SWIMMING )
			{
				if( _player->FlyCheat )
				{
					if( _player->m_lastMoveType != 2 )
					{
						_player->m_lastMoveType = 2; // flying
						_player->ResetHeartbeatCoords();
					}
				}
			}
			else
			{
				if( _player->m_lastMoveType != 0 )
				{
					_player->m_lastMoveType = 0; // walk or run on land
					_player->ResetHeartbeatCoords();
				}
			}
			_player->m_UnderwaterState &= ~UNDERWATERSTATE_SWIMMING;
		}
	}

	// player is flagged as in the water and is not flagged as under the water
	if( _player->m_UnderwaterState & UNDERWATERSTATE_SWIMMING && !( _player->m_UnderwaterState & UNDERWATERSTATE_UNDERWATER ) )
	{
		//the player is in the water and has gone under water, requires breath bar.
		if( ( movement_info.z + _player->m_noseLevel ) < pSession->m_wLevel )
		{
			_player->m_UnderwaterState |= UNDERWATERSTATE_UNDERWATER;
			WorldPacket data(SMSG_START_MIRROR_TIMER, 20);
			data << uint32(1) << _player->m_UnderwaterTime << _player->m_UnderwaterMaxTime << int32(-1) << uint32(0);
			pSession->SendPacket(&data);
		}
	}

	// player is flagged as in the water and is flagged as under the water
	if( _player->m_UnderwaterState & UNDERWATERSTATE_SWIMMING && _player->m_UnderwaterState & UNDERWATERSTATE_UNDERWATER )
	{
		//the player is in the water but their face is above water, no breath bar neeeded.
		if( ( movement_info.z + _player->m_noseLevel ) > pSession->m_wLevel )
		{
			_player->m_UnderwaterState &= ~UNDERWATERSTATE_UNDERWATER;
			WorldPacket data(SMSG_START_MIRROR_TIMER, 20);
			data << uint32(1) << _player->m_UnderwaterTime << _player->m_UnderwaterMaxTime << int32(10) << uint32(0);
			pSession->SendPacket(&data);
		}
	}

	// player is flagged as not in the water and is flagged as under the water
	if( !( _player->m_UnderwaterState & UNDERWATERSTATE_SWIMMING ) && _player->m_UnderwaterState & UNDERWATERSTATE_UNDERWATER )
	{
		//the player is out of the water, no breath bar neeeded.
		if( ( movement_info.z + _player->m_noseLevel ) > pSession->m_wLevel )
		{
			_player->m_UnderwaterState &= ~UNDERWATERSTATE_UNDERWATER;
			WorldPacket data(SMSG_START_MIRROR_TIMER, 20);
			data << uint32(1) << _player->m_UnderwaterTime << _player->m_UnderwaterMaxTime << int32(10) << uint32(0);
			pSession->SendPacket(&data);
		}
	}

}

void WorldSession::HandleMovementOpcodes( WorldPacket & recv_data )
{
	if(!_player->IsInWorld() || _player->m_uint32Values[UNIT_FIELD_CHARMEDBY] || _player->GetPlayerStatus() == TRANSFER_PENDING || _player->GetTaxiState())
		return;

	// spell cancel on movement, for now only fishing is added
	Object * t_go = _player->m_SummonedObject;
	if (t_go)
	{
		if (t_go->GetEntry() == GO_FISHING_BOBBER)
			((GameObject*)t_go)->EndFishing(GetPlayer(),true);
	}

	/************************************************************************/
	/* Make sure the packet is the correct size range.                      */
	/************************************************************************/
	if(recv_data.size() > 80) { Disconnect(); return; }

	/************************************************************************/
	/* Read Movement Data Packet                                            */
	/************************************************************************/
	movement_info.init(recv_data);

	/************************************************************************/
	/* Update player movement state                                         */
	/************************************************************************/
	if( recv_data.GetOpcode() == MSG_MOVE_STOP 
		|| recv_data.GetOpcode() == MSG_MOVE_STOP_STRAFE 
		|| recv_data.GetOpcode() == MSG_MOVE_STOP_TURN 
		)
		_player->m_isMoving = false;
	else
		_player->m_isMoving = true;

	/************************************************************************/
	/* Remove Emote State                                                   */
	/************************************************************************/
	if(_player->m_uint32Values[UNIT_NPC_EMOTESTATE])
		_player->SetUInt32Value(UNIT_NPC_EMOTESTATE,0);

	/************************************************************************/
	/* Make sure the co-ordinates are valid.                                */
	/************************************************************************/
	if( !((movement_info.y >= _minY) && (movement_info.y <= _maxY)) || !((movement_info.x >= _minX) && (movement_info.x <= _maxX)) )
	{
		Disconnect();
		return;
	}

	/************************************************************************/
	/* Dump movement flags - Wheee!                                         */
	/************************************************************************/
#if 0
	printf("=========================================================\n");
	printf("Full movement flags: 0x%.8X\n", movement_info.flags);
	uint32 z, b;
	for(z = 1, b = 1; b < 32;)
	{
		if(movement_info.flags & z)
			printf("   Bit %u (0x%.8X or %u) is set!\n", b, z, z);

		z <<= 1;
		b+=1;
	}
	printf("=========================================================\n");
#endif

	/************************************************************************/
	/* Orientation dumping                                                  */
	/************************************************************************/
#if 0
	printf("Packet: 0x%03X (%s)\n", recv_data.GetOpcode(), LookupName( recv_data.GetOpcode(), g_worldOpcodeNames ) );
	printf("Orientation: %.10f\n", movement_info.orientation);
#endif

	/************************************************************************/
	/* Anti-Hack Checks                                                     */
	/************************************************************************/
	if( !(HasGMPermissions() && sWorld.no_antihack_on_gm) && !_player->m_uint32Values[UNIT_FIELD_CHARM])
	{
		/************************************************************************/
		/* Anti-Teleport                                                        */
		/************************************************************************/
		if(sWorld.antihack_teleport && _player->m_position.Distance2DSq(movement_info.x, movement_info.y) > 5625.0f
			&& _player->m_runSpeed < 50.0f)
		{
			sCheatLog.writefromsession(this, "Used teleport hack {3}, speed was %f", _player->m_runSpeed);
			Disconnect();
			return;
		}
	}

	/************************************************************************/
	/* Calculate the timestamp of the packet we have to send out            */
	/************************************************************************/
	size_t pos = (size_t)m_MoverWoWGuid.GetNewGuidLen() + 1;
	uint32 mstime = mTimeStamp();
	int32 move_time;
	if(m_clientTimeDelay == 0)
		m_clientTimeDelay = mstime - movement_info.time;

	/************************************************************************/
	/* Copy into the output buffer.                                         */
	/************************************************************************/
	if(_player->m_inRangePlayers.size())
	{
		move_time = (movement_info.time - (mstime - m_clientTimeDelay)) + MOVEMENT_PACKET_TIME_DELAY + mstime;
		memcpy(&movement_packet[pos], recv_data.contents(), recv_data.size());
		movement_packet[pos+5]=0;

		/************************************************************************/
		/* Distribute to all inrange players.                                   */
		/************************************************************************/
		for(set<Player*>::iterator itr = _player->m_inRangePlayers.begin(); itr != _player->m_inRangePlayers.end(); ++itr)
		{
#ifdef USING_BIG_ENDIAN
			*(uint32*)&movement_packet[pos+5] = swap32(move_time + (*itr)->GetSession()->m_moveDelayTime);
#else
			*(uint32*)&movement_packet[pos+5] = uint32(move_time + (*itr)->GetSession()->m_moveDelayTime);
#endif
#if defined(ENABLE_COMPRESSED_MOVEMENT) && defined(ENABLE_COMPRESSED_MOVEMENT_FOR_PLAYERS)
			if( _player->GetPositionNC().Distance2DSq((*itr)->GetPosition()) >= World::m_movementCompressThreshold )
				(*itr)->AppendMovementData( recv_data.GetOpcode(), uint16(recv_data.size() + pos), movement_packet );
			else
				(*itr)->GetSession()->OutPacket(recv_data.GetOpcode(), uint16(recv_data.size() + pos), movement_packet);
#else
			(*itr)->GetSession()->OutPacket(recv_data.GetOpcode(), uint16(recv_data.size() + pos), movement_packet);			
#endif
		}
	}

	/************************************************************************/
	/* Falling damage checks                                                */
	/************************************************************************/

	if( movement_info.flags & MOVEFLAG_REDIRECTED && !( movement_info.flags & MOVEFLAG_FALLING ) )
	{
		//_player->blinked = true; //disabled cause i dont see how moveflag redirected is related to blink since its triggered even without blink
	}
	else
	{
		if( _player->blinked )
		{
			_player->blinked = false;
			_player->m_fallDisabledUntil = UNIXTIME + 5;
			_player->ResetHeartbeatCoords();
		}
		else
		{
			if( movement_info.flags & MOVEFLAG_FALLING ) // Falling
			{
				if( _player->m_fallTime < movement_info.FallTime )
					_player->m_fallTime = movement_info.FallTime;
			}
			else //once we done falling lets do some damage
			{
				if( _player->m_fallTime > 800 && _player->isAlive() && !_player->GodModeCheat )
				{
					//Check if we aren't falling in water
					if( !( movement_info.flags & MOVEFLAG_SWIMMING ) )
					{
						if( !_player->bFeatherFall && UNIXTIME > _player->m_fallDisabledUntil )
						{
							//10% dmg per sec after first 3 seconds
							//it rL a*t*t
							double coeff = 0.000000075 * ( _player->m_fallTime * _player->m_fallTime - _player->m_fallTime );
							
							if( coeff > 0.0 )
							{
								uint32 damage = (uint32)( _player->GetUInt32Value( UNIT_FIELD_MAXHEALTH ) * coeff );

								if( _player->bSafeFall )
									damage = float2int32( float( damage ) * 0.83f );

								if( damage > _player->GetUInt32Value( UNIT_FIELD_MAXHEALTH ) ) // Can only deal 100% damage.
									damage = _player->GetUInt32Value( UNIT_FIELD_MAXHEALTH );

								_player->SendEnvironmentalDamageLog( _player->GetGUID(), DAMAGE_FALL, damage );
								_player->DealDamage( _player, damage, 0, 0, 0 );
							}
						}
					}
					_player->m_fallTime = 0;
				}
				else
				{
					//player is dead, cheating with god mode or didn't fall for long enough, no need to keep increasing falltime
					_player->m_fallTime = 0;
				}
			}
		}
	}

	/************************************************************************/
	/* Transporter Setup                                                    */
	/************************************************************************/
	if(!_player->m_lockTransportVariables)
	{
		if(_player->m_TransporterGUID && !movement_info.transGuid)
		{
			/* we left the transporter we were on */
			if(_player->m_CurrentTransporter)
			{
				_player->m_CurrentTransporter->RemovePlayer(_player);
				_player->m_CurrentTransporter = NULL;
			}

			_player->m_TransporterGUID = 0;
			_player->ResetHeartbeatCoords();
		}
		else if(movement_info.transGuid)
		{
			if(!_player->m_TransporterGUID)
			{
				/* just walked into a transport */
				if(_player->IsMounted())
					_player->RemoveAura(_player->m_MountSpellId);

				_player->m_CurrentTransporter = objmgr.GetTransporter(GUID_LOPART(movement_info.transGuid));
				if(_player->m_CurrentTransporter)
					_player->m_CurrentTransporter->AddPlayer(_player);

				/* set variables */
				_player->m_TransporterGUID = movement_info.transGuid;
				_player->m_TransporterUnk = movement_info.transUnk;
				_player->m_TransporterX = movement_info.transX;
				_player->m_TransporterY = movement_info.transY;
				_player->m_TransporterZ = movement_info.transZ;
			}
			else
			{
				/* no changes */
				_player->m_TransporterUnk = movement_info.transUnk;
				_player->m_TransporterX = movement_info.transX;
				_player->m_TransporterY = movement_info.transY;
				_player->m_TransporterZ = movement_info.transZ;
			}
		}
		/*float x = movement_info.x - movement_info.transX;
		float y = movement_info.y - movement_info.transY;
		float z = movement_info.z - movement_info.transZ;
		Transporter* trans = _player->m_CurrentTransporter;
		if(trans) sChatHandler.SystemMessageToPlr(_player, "Client t pos: %f %f\nServer t pos: %f %f   Diff: %f %f", x,y, trans->GetPositionX(), trans->GetPositionY(), trans->CalcDistance(x,y,z), trans->CalcDistance(movement_info.x, movement_info.y, movement_info.z));*/
	}

	/************************************************************************/
	/* Anti-Speed Hack Checks                                               */
	/************************************************************************/

	if( sWorld.antihack_speed )
	{
		float speed;

		if( !_player->flying_aura )
		{
			switch( _player->m_lastMoveType )
			{
			case 1:
				speed = _player->m_swimSpeed;
				break;
			case 2:
				speed = _player->m_flySpeed;
				break;
			default:
				speed = _player->m_runSpeed;
				break;
			}
		}
		else
		{
			speed = _player->m_flySpeed;
		}

		if( _player->_heartbeatEnable && !_player->bFeatherFall && ( !_player->blinked/* || _player->m_redirectCount > 5*/ ) &&
			!_player->m_uint32Values[UNIT_FIELD_CHARM] && _player->m_TransporterGUID == 0 && !( movement_info.flags & MOVEFLAG_FULL_FALLING_MASK ) )
		{
			if( ( sWorld.no_antihack_on_gm && !HasGMPermissions() ) || !sWorld.no_antihack_on_gm )
			{
				if( _player->_lastHeartbeatV >= speed )
				{
					float delta_x = movement_info.x - _player->_lastHeartbeatX;
					float delta_y = movement_info.y - _player->_lastHeartbeatY;

					float distance_xy_plane = delta_x * delta_x + delta_y * delta_y;
					float distance_delta = distance_xy_plane / speed;
					float latency = float( _latency ) * 0.01f;
					float speed_delta = ( speed * 0.25f ) + std::max( latency + ( latency * 0.04f ), 0.32f );

					if( distance_delta >= speed_delta )
					{
						switch ( _player->m_speedhackChances )
						{
						case 2:
							{
								sChatHandler.SystemMessage( this, "Speedhacker detected this is your first warning. Your account has been flagged for later processing by server administrators. You will be unrooted in 10 seconds." );
								sCheatLog.writefromsession( this, "Speedhacker first warning, deterministic data : D(%f) DD(%f) S(%f) SD(%f) L(%f)", distance_xy_plane, distance_delta, speed, speed_delta, ( (float)_player->GetSession()->GetLatency() / 100.0f ) );
								sLog.outDebug( "Speedhacker D(%f) DD(%f) S(%f) SD(%f) L(%f)", distance_xy_plane, distance_delta, speed, speed_delta, ( (float)_player->GetSession()->GetLatency() / 100.0f ) );
								_player->SetMovement( MOVE_ROOT, 1 );
								sEventMgr.AddEvent( _player, &Player::SetMovement, uint8( MOVE_UNROOT ), uint32(1), EVENT_DELETE_TIMER, 10000, 1, EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT );
								_player->ResetHeartbeatCoords();
								_player->m_speedhackChances--;
								break;
							}
						case 1:
							{
								sChatHandler.SystemMessage( this, "Speedhacker detected this is your second warning. Your account has been flagged for later processing by server administrators. You will be unrooted in 45 seconds." );
								sCheatLog.writefromsession( this, "Speedhacker second warning, deterministic data : D(%f) DD(%f) S(%f) SD(%f) L(%f)", distance_xy_plane, distance_delta, speed, speed_delta, ( (float)_player->GetSession()->GetLatency() / 100.0f ) );
								sLog.outDebug( "Speedhacker D(%f) DD(%f) S(%f) SD(%f) L(%f)", distance_xy_plane, distance_delta, speed, speed_delta, ( (float)_player->GetSession()->GetLatency() / 100.0f ) );
								_player->SetMovement( MOVE_ROOT, 1 );
								sEventMgr.AddEvent( _player, &Player::SetMovement, uint8( MOVE_UNROOT ), uint32(1), EVENT_DELETE_TIMER, 45000, 1, EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT );
								_player->ResetHeartbeatCoords();
								_player->m_speedhackChances--;
								break;
							}
						case 0:
							{
								sChatHandler.SystemMessage( this, "Speedhacker detected you were warned. Your account has been flagged for later processing by server administrators. You will now be removed from the server." );
								sCheatLog.writefromsession( this, "Speedhacker kicked, deterministic data : D(%f) DD(%f) S(%f) SD(%f) L(%f)", distance_xy_plane, distance_delta, speed, speed_delta, ( (float)_player->GetSession()->GetLatency() / 100.0f ) );
								sLog.outDebug( "Speedhacker D(%f) DD(%f) S(%f) SD(%f) L(%f)", distance_xy_plane, distance_delta, speed, speed_delta, ( (float)_player->GetSession()->GetLatency() / 100.0f ) );
								_player->m_KickDelay = 0;
								sEventMgr.AddEvent( _player, &Player::_Kick, EVENT_PLAYER_KICK, 15000, 1, 0 );
								_player->SetMovement( MOVE_ROOT, 1 );
								break;
							}
						}
					}
				}
			}
		}

		_player->_lastHeartbeatV = speed;
		_player->_lastHeartbeatX = movement_info.x;
		_player->_lastHeartbeatY = movement_info.y;
		_player->_lastHeartbeatZ = movement_info.z;
		_player->_lastHeartbeatO = movement_info.orientation;

	}

	/************************************************************************/
	/* Breathing System                                                     */
	/************************************************************************/
	_HandleBreathing(movement_info, _player, this);

	/************************************************************************/
	/* Remove Spells                                                        */
	/************************************************************************/
	_player->RemoveAurasByInterruptFlag(AURA_INTERRUPT_ON_MOVEMENT);

	/************************************************************************/
	/* Update our position in the server.                                   */
	/************************************************************************/
	if( _player->m_CurrentCharm )
		_player->m_CurrentCharm->SetPosition(movement_info.x, movement_info.y, movement_info.z, movement_info.orientation);
	else
	{
		if(!_player->m_CurrentTransporter) 
		{
			if( !_player->SetPosition(movement_info.x, movement_info.y, movement_info.z, movement_info.orientation) )
			{
				_player->SetUInt32Value(UNIT_FIELD_HEALTH, 0);
				_player->KillPlayer();
			}
		}
		else
		{
			_player->SetPosition(movement_info.x, movement_info.y, movement_info.z, 
				movement_info.orientation + movement_info.transO, false);
		}
	}	
}

void WorldSession::HandleMoveTimeSkippedOpcode( WorldPacket & recv_data )
{
	
}

void WorldSession::HandleMoveNotActiveMoverOpcode( WorldPacket & recv_data )
{

}


void WorldSession::HandleSetActiveMoverOpcode( WorldPacket & recv_data )
{
	// set current movement object
	uint64 guid;
	recv_data >> guid;

	if(guid != m_MoverWoWGuid.GetOldGuid())
	{
		// generate wowguid
		if(guid != 0)
			m_MoverWoWGuid.Init(guid);
		else
			m_MoverWoWGuid.Init(_player->GetGUID());

		// set up to the movement packet
		movement_packet[0] = m_MoverWoWGuid.GetNewGuidMask();
		memcpy(&movement_packet[1], m_MoverWoWGuid.GetNewGuid(), m_MoverWoWGuid.GetNewGuidLen());
	}
}

void WorldSession::HandleMoveSplineCompleteOpcode(WorldPacket &recvPacket)
{

}

void WorldSession::HandleMountSpecialAnimOpcode(WorldPacket &recvdata)
{
	WorldPacket data(SMSG_MOUNTSPECIAL_ANIM,8);
	data << _player->GetGUID();
	_player->SendMessageToSet(&data, true);
}

void WorldSession::HandleWorldportOpcode(WorldPacket & recv_data)
{
	uint32 mapid;
	float x,y,z,o;
	recv_data >> mapid >> x >> y >> z >> o;
	
	if(!_player->IsInWorld())
		return;

	if(!HasGMPermissions())
	{
		SendNotification("You do not have permission to use this function.");
		return;
	}

	LocationVector vec(x,y,z,o);
	_player->SafeTeleport(mapid,0,vec);
}

void WorldSession::HandleTeleportToUnitOpcode(WorldPacket & recv_data)
{
	string object_name;
	recv_data >> object_name;
/*
	if(!_player->IsInWorld())
		return;

	if(!HasGMPermissions())
	{
		SendNotification("You do not have permission to use this function.");
		return;
	}

	if( (target = _player->GetMapMgr()->GetUnit(_player->GetSelection())) == NULL )
		return;

	_player->SafeTeleport(_player->GetMapId(), _player->GetInstanceID(), target->GetPosition());*/
}

void WorldSession::HandleTeleportCheatOpcode(WorldPacket & recv_data)
{
	float x,y,z,o;
	LocationVector vec;

	if(!HasGMPermissions())
	{
		SendNotification("You do not have permission to use this function.");
		return;
	}

	recv_data >> x >> y >> z >> o;
	vec.ChangeCoords(x,y,z,o);
	_player->SafeTeleport(_player->GetMapId(),_player->GetInstanceID(),vec);
}

void MovementInfo::init(WorldPacket & data)
{
	transGuid = 0;
	unk13 = 0;
	data >> flags >> unk_230 >> time;
	data >> x >> y >> z >> orientation;

	if (flags & MOVEFLAG_TAXI)
	{
		data >> transGuid >> transX >> transY >> transZ >> transO >> transUnk;
	}
	if (flags & MOVEFLAG_SWIMMING)
	{
		data >> unk6;
	}
	if (flags & MOVEFLAG_FALLING || flags & MOVEFLAG_REDIRECTED)
	{
		data >> FallTime >> unk8 >> unk9 >> unk10;
	}
	if (flags & MOVEFLAG_SPLINE_MOVER)
	{
		data >> unk12;
	}

	data >> unklast;
	if(data.rpos() != data.wpos())
	{
		if(data.rpos() + 4 == data.wpos())
			data >> unk13;
		else
			sLog.outDebug("Extra bits of movement packet left");
	}
}

void MovementInfo::write(WorldPacket & data)
{
	data << flags << unk_230 << getMSTime();

	data << x << y << z << orientation;

	if (flags & MOVEFLAG_TAXI)
	{
		data << transGuid << transX << transY << transZ << transO << transUnk;
	}
	if (flags & MOVEFLAG_SWIMMING)
	{
		data << unk6;
	}
	if (flags & MOVEFLAG_FALLING)
	{
		data << FallTime << unk8 << unk9 << unk10;
	}
	if (flags & MOVEFLAG_SPLINE_MOVER)
	{
		data << unk12;
	}
	data << unklast;
	if(unk13)
		data << unk13;
}