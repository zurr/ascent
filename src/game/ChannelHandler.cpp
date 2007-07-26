/****************************************************************************
 *
 * Channel Handlers
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

#include "StdAfx.h"

initialiseSingleton( ChannelMgr );

void WorldSession::HandleChannelJoin(WorldPacket& recvPacket)
{
	CHECK_PACKET_SIZE(recvPacket, 1);
	string channelname,pass;
	uint32 code = 0;
	uint8 crap;

	recvPacket >> code >> crap;
	recvPacket >> channelname;
	recvPacket >> pass;
	if(channelmgr.GetJoinChannel(channelname.c_str(),GetPlayer())->Join(GetPlayer(),pass.c_str()))
	{
		WorldPacket data(SMSG_CHANNEL_NOTIFY, 100);
		data << uint8(2) << channelname;
		data << uint64(code);
		SendPacket(&data);
	}
}

void WorldSession::HandleChannelLeave(WorldPacket& recvPacket)
{
	CHECK_PACKET_SIZE(recvPacket, 1);
  //  if(GetPlayer()->GetTaxiState()) return;	 // dont allow join while on taxi
	//if we leave game etc we should leave this channel, we should not join -- yes,but not leave
	string channelname;
	uint32 code = 0;

	if(client_build >= 6337)
		recvPacket >> code;

	recvPacket >> channelname;

	if(!channelname.length())
		return;

	if(channelmgr.LeftChannel(channelname.c_str(),_player))
	{
		WorldPacket data(SMSG_CHANNEL_NOTIFY, 100);
		data << uint8(0x03) << channelname;
		if(client_build >= 6337)
			data << uint64(code);
		SendPacket(&data);
	}
}

void WorldSession::HandleChannelList(WorldPacket& recvPacket)
{
	CHECK_PACKET_SIZE(recvPacket, 1);
	string channelname;
	recvPacket >> channelname;
	Channel *chn = channelmgr.GetChannel(channelname.c_str(),GetPlayer()); if(chn) chn->List(GetPlayer());
}

void WorldSession::HandleChannelPassword(WorldPacket& recvPacket)
{
	CHECK_PACKET_SIZE(recvPacket, 1);
	string channelname,pass;
	recvPacket >> channelname;
	recvPacket >> pass;
	Channel *chn = channelmgr.GetChannel(channelname.c_str(),GetPlayer()); if(chn) chn->Password(GetPlayer(),pass.c_str());
}

void WorldSession::HandleChannelSetOwner(WorldPacket& recvPacket)
{
	CHECK_PACKET_SIZE(recvPacket, 1);
	string channelname,newp;
	recvPacket >> channelname;
	recvPacket >> newp;
	Channel *chn = channelmgr.GetChannel(channelname.c_str(),GetPlayer());
	if(chn) chn->SetOwner(GetPlayer(),newp.c_str());
}

void WorldSession::HandleChannelOwner(WorldPacket& recvPacket)
{
	CHECK_PACKET_SIZE(recvPacket, 1);
	string channelname;
	recvPacket >> channelname;
	Channel *chn = channelmgr.GetChannel(channelname.c_str(),GetPlayer()); if(chn) chn->GetOwner(GetPlayer());
}

void WorldSession::HandleChannelModerator(WorldPacket& recvPacket)
{
	CHECK_PACKET_SIZE(recvPacket, 1);
	string channelname,otp;
	recvPacket >> channelname;
	recvPacket >> otp;
	Channel *chn = channelmgr.GetChannel(channelname.c_str(),GetPlayer()); if(chn) chn->SetModerator(GetPlayer(),otp.c_str());
}

void WorldSession::HandleChannelUnmoderator(WorldPacket& recvPacket)
{
	CHECK_PACKET_SIZE(recvPacket, 1);
	string channelname,otp;
	recvPacket >> channelname;
	recvPacket >> otp;
	Channel *chn = channelmgr.GetChannel(channelname.c_str(),GetPlayer()); if(chn) chn->UnsetModerator(GetPlayer(),otp.c_str());
}

void WorldSession::HandleChannelMute(WorldPacket& recvPacket)
{
	CHECK_PACKET_SIZE(recvPacket, 1);
	string channelname,otp;
	recvPacket >> channelname;
	recvPacket >> otp;
	Channel *chn = channelmgr.GetChannel(channelname.c_str(),GetPlayer()); if(chn) chn->SetMute(GetPlayer(),otp.c_str());
}

void WorldSession::HandleChannelUnmute(WorldPacket& recvPacket)
{
	CHECK_PACKET_SIZE(recvPacket, 1);
	string channelname,otp;
	recvPacket >> channelname;
	recvPacket >> otp;
	Channel *chn = channelmgr.GetChannel(channelname.c_str(),GetPlayer()); if(chn) chn->UnsetMute(GetPlayer(),otp.c_str());
}

void WorldSession::HandleChannelInvite(WorldPacket& recvPacket)
{
	CHECK_PACKET_SIZE(recvPacket, 1);
	string channelname,otp;
	recvPacket >> channelname;
	recvPacket >> otp;
	Channel *chn = channelmgr.GetChannel(channelname.c_str(),GetPlayer()); if(chn) chn->Invite(GetPlayer(),otp.c_str());
}
void WorldSession::HandleChannelKick(WorldPacket& recvPacket)
{
	CHECK_PACKET_SIZE(recvPacket, 1);
	string channelname,otp;
	recvPacket >> channelname;
	recvPacket >> otp;
	Channel *chn = channelmgr.GetChannel(channelname.c_str(),GetPlayer()); if(chn) chn->Kick(GetPlayer(),otp.c_str());
}

void WorldSession::HandleChannelBan(WorldPacket& recvPacket)
{
	CHECK_PACKET_SIZE(recvPacket, 1);
	string channelname,otp;
	recvPacket >> channelname;
	recvPacket >> otp;
	Channel *chn = channelmgr.GetChannel(channelname.c_str(),GetPlayer()); if(chn) chn->Ban(GetPlayer(),otp.c_str());
}

void WorldSession::HandleChannelUnban(WorldPacket& recvPacket)
{
	CHECK_PACKET_SIZE(recvPacket, 1);
	string channelname,otp;
	recvPacket >> channelname;
	recvPacket >> otp;
	Channel *chn = channelmgr.GetChannel(channelname.c_str(),GetPlayer()); if(chn) chn->UnBan(GetPlayer(),otp.c_str());
}

void WorldSession::HandleChannelAnnounce(WorldPacket& recvPacket)
{
	CHECK_PACKET_SIZE(recvPacket, 1);
	string channelname;
	recvPacket >> channelname;
	Channel *chn = channelmgr.GetChannel(channelname.c_str(),GetPlayer()); if(chn) chn->Announce(GetPlayer());
}

void WorldSession::HandleChannelModerate(WorldPacket& recvPacket)
{
	CHECK_PACKET_SIZE(recvPacket, 1);
	string channelname;
	recvPacket >> channelname;
	Channel *chn = channelmgr.GetChannel(channelname.c_str(),GetPlayer()); if(chn) chn->Moderate(GetPlayer());
}
