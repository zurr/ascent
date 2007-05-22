/****************************************************************************
 *
 * Chat/Command System
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

#include "StdAfx.h"

extern std::string LogFileName;
extern bool bLogChat;

void WorldSession::HandleMessagechatOpcode( WorldPacket & recv_data )
{
    CHECK_PACKET_SIZE(recv_data, 9);
    WorldPacket *data;
    if(!_player->IsInWorld())
        return;

    uint32 type;
    uint32 lang;

    recv_data >> type;
    recv_data >> lang;

    std::stringstream irctext;
    irctext.rdbuf()->str("");
    std::string msg;
    msg.reserve(256);
    switch(type)
    {
    case CHAT_MSG_EMOTE:
        {
            recv_data >> msg;

            if(sWorld.getReqGmClient() == false)
                if (sChatHandler.ParseCommands(msg.c_str(), this) > 0)
                    break;

            if(GetPlayer()->m_modlanguage >=0)
                data = sChatHandler.FillMessageData( CHAT_MSG_EMOTE, GetPlayer()->m_modlanguage,  msg.c_str(), _player->GetGUID(), _player->bGMTagOn ? 3 : 0 );
            else    
                data = sChatHandler.FillMessageData( CHAT_MSG_EMOTE, CanUseCommand('c') ? LANG_UNIVERSAL : lang,  msg.c_str(), _player->GetGUID(), _player->bGMTagOn ? 3 : 0 );
            GetPlayer()->SendMessageToSet( data, true );

            sLog.outString("[emote] %s: %s", _player->GetName(), msg.c_str());
            delete data;

        }break;
    case CHAT_MSG_SAY:
        {
            recv_data >> msg;
            if(sWorld.getReqGmClient() == false)
                if (sChatHandler.ParseCommands(msg.c_str(), this) > 0)
                    break;

            if(GetPlayer()->m_modlanguage >=0)
                data = sChatHandler.FillMessageData( CHAT_MSG_SAY, GetPlayer()->m_modlanguage,  msg.c_str(), _player->GetGUID(), _player->bGMTagOn ? 3 : 0 );
            else 
                data = sChatHandler.FillMessageData( CHAT_MSG_SAY, CanUseCommand('c') ? LANG_UNIVERSAL : lang,  msg.c_str(), _player->GetGUID(), _player->bGMTagOn ? 3 : 0 );

            GetPlayer()->SendMessageToSet( data, true );
            sLog.outString("[say] %s: %s", _player->GetName(), msg.c_str());
            delete data;
        } break;
    case CHAT_MSG_PARTY:
    case CHAT_MSG_RAID:
    case CHAT_MSG_RAIDLEADER:
        {
            recv_data >> msg;

            if(sWorld.getReqGmClient() == false)
                if (sChatHandler.ParseCommands(msg.c_str(), this) > 0)
                    break;
            
            Group *pGroup = _player->GetGroup();
            if(pGroup == NULL) break;
            
            if(GetPlayer()->m_modlanguage >=0)
                data=sChatHandler.FillMessageData( type, GetPlayer()->m_modlanguage,  msg.c_str(), _player->GetGUID(), _player->bGMTagOn ? 3 : 0 );
            else
                data=sChatHandler.FillMessageData( type, CanUseCommand('c') ? LANG_UNIVERSAL : lang, msg.c_str(), _player->GetGUID(), _player->bGMTagOn ? 3 : 0);
            if(type == CHAT_MSG_PARTY && pGroup->GetGroupType() == GROUP_TYPE_RAID)
            {
                // only send to that subgroup
                SubGroup *sgr = _player->GetSubGroup();
                if(sgr)
                {
                    for(GroupMembersSet::iterator itr = sgr->GetGroupMembersBegin(); itr != sgr->GetGroupMembersEnd(); ++itr)
                    {
                        (*itr)->GetSession()->SendPacket(data);
                    }
                }
            }
            else
            {
                pGroup->SendPacketToAll(data);
            }
            sLog.outString("[party] %s: %s", _player->GetName(), msg.c_str());
            delete data;
        } break;
    case CHAT_MSG_GUILD:
        {
            recv_data >> msg;
            if(sWorld.getReqGmClient() == false)
                if (sChatHandler.ParseCommands(msg.c_str(), this) > 0)
                {
                    break;
                }

            if (GetPlayer()->IsInGuild())
            {
                Guild *pGuild = objmgr.GetGuild( GetPlayer()->GetGuildId() );
                if(pGuild)
                {
                    pGuild->BroadCastToGuild(this, msg);
                }
            }
            //sLog.outString("[guild] %s: %s", _player->GetName(), msg.c_str());
        } break;
    case CHAT_MSG_OFFICER:
        {
            recv_data >> msg;

            if(sWorld.getReqGmClient() == false)
                if (sChatHandler.ParseCommands(msg.c_str(), this) > 0)
                    break;

            if (GetPlayer()->IsInGuild())
            {
                Guild *pGuild = objmgr.GetGuild( GetPlayer()->GetGuildId() );
                if(pGuild)
                    pGuild->OfficerChannelChat(this, msg);
            }
            sLog.outString("[officer] %s: %s", _player->GetName(), msg.c_str());
        } break;
    case CHAT_MSG_YELL:
        {
            recv_data >> msg;

            if(sWorld.getReqGmClient() == false)
                if (sChatHandler.ParseCommands(msg.c_str(), this) > 0)
                    break;

            if(GetPlayer()->m_modlanguage >=0)
                data = sChatHandler.FillMessageData( CHAT_MSG_YELL, GetPlayer()->m_modlanguage,  msg.c_str(), _player->GetGUID(), _player->bGMTagOn ? 3 : 0 );
            else
                data = sChatHandler.FillMessageData( CHAT_MSG_YELL, CanUseCommand('c') ? LANG_UNIVERSAL : lang,  msg.c_str(), _player->GetGUID(), _player->bGMTagOn ? 3 : 0 );

            SendPacket(data);
            sWorld.SendZoneMessage(data, GetPlayer()->GetZoneId(), this);
            delete data;
            sLog.outString("[yell] %s: %s", _player->GetName(), msg.c_str());            
        } break;
    case CHAT_MSG_WHISPER:
        {
            std::string to = "",tmp;
            recv_data >> to >> msg;
         
            Player *player = objmgr.GetPlayer(to.c_str(), false);
            if(!player)
            {
                tmp = "Player \"";
                tmp += to.c_str();
                tmp += "\" is not online.";
                data = sChatHandler.FillSystemMessageData(  tmp.c_str() );
                SendPacket(data);              
                delete data;
                break;
            }

            // Check that the player isn't a gm with his status on
            if(!_player->GetSession()->GetPermissionCount() && player->bGMTagOn && player->gmTargets.count(_player) == 0)
            {
                // Build automated reply
                string Reply = "I am currently occupied, please submit your problem through the GM Ticket System.";
                data = sChatHandler.FillMessageData( CHAT_MSG_WHISPER, LANG_UNIVERSAL, Reply.c_str(), player->GetGUID(), 3);
                SendPacket(data);
                delete data;
                break;
            }

            if( sSocialMgr.HasIgnore(player, GetPlayer()) )
                return;

            if(GetPlayer()->m_modlanguage >=0)
                data = sChatHandler.FillMessageData( CHAT_MSG_WHISPER, GetPlayer()->m_modlanguage,  msg.c_str(), _player->GetGUID(), _player->bGMTagOn ? 3 : 0 );
            else
                data = sChatHandler.FillMessageData( CHAT_MSG_WHISPER, CanUseCommand('c') ? LANG_UNIVERSAL : lang,  msg.c_str(), _player->GetGUID(), _player->bGMTagOn ? 3 : 0 );

            player->GetSession()->SendPacket(data);
            delete data;
            //Sent the to Users id as the channel, this should be fine as it's not used for wisper
          
            data = sChatHandler.FillMessageData(CHAT_MSG_WHISPER_INFORM, LANG_UNIVERSAL,msg.c_str(), player->GetGUID(), player->bGMTagOn ? 3 : 0  );
            SendPacket(data);
            delete data;

            if(player->HasFlag(PLAYER_FLAGS, 0x02))
            {
                // Has AFK flag, autorespond.
                data = sChatHandler.FillMessageData(CHAT_MSG_AFK, LANG_UNIVERSAL,  player->m_afk_reason.c_str(),player->GetGUID(), _player->bGMTagOn ? 3 : 0);
                SendPacket(data);
                delete data;
            }
            else if(player->HasFlag(PLAYER_FLAGS, 0x04))
            {
                // Has AFK flag, autorespond.
                data = sChatHandler.FillMessageData(CHAT_MSG_DND, LANG_UNIVERSAL, player->m_afk_reason.c_str(),player->GetGUID(), _player->bGMTagOn ? 3 : 0);
                SendPacket(data);
                delete data;
            }

            sLog.outString("[whisper] %s to %s: %s", _player->GetName(), to.c_str(), msg.c_str());
        } break;
    case CHAT_MSG_CHANNEL:
        {
            std::string channel = "";
            recv_data >> channel;
            recv_data >> msg;
         

            if(strcmp(channel.c_str(), sWorld.getGmClientChannel().c_str()) == 0)
                if (sChatHandler.ParseCommands(msg.c_str(), this) > 0)
                    break;

            Channel *chn = channelmgr.GetChannel(channel.c_str(),GetPlayer()); 
            if(chn) 
                chn->Say(GetPlayer(),msg.c_str());

            sLog.outString("[%s] %s: %s", channel.c_str(), _player->GetName(), msg.c_str());

        } break;
    case CHAT_MSG_AFK:
        {
            std::string reason;
            recv_data >> reason;
            GetPlayer()->SetAFKReason(reason);
            /* WorldPacket *data, WorldSession* session, uint32 type, uint32 language, const char *channelName, const char *message*/
            if(GetPlayer()->HasFlag(PLAYER_FLAGS, 0x02))
                GetPlayer()->RemoveFlag(PLAYER_FLAGS, 0x02);
            else
            {
                GetPlayer()->SetFlag(PLAYER_FLAGS, 0x02);
                data = sChatHandler.FillMessageData(CHAT_MSG_AFK, LANG_UNIVERSAL, reason.c_str(),_player->GetGUID());
                GetPlayer()->SendMessageToSet(data, false);
                delete data;
            }            
        } break;
    case CHAT_MSG_DND:
        {
            std::string reason;
            recv_data >> reason;
            GetPlayer()->SetAFKReason(reason);
            if(GetPlayer()->HasFlag(PLAYER_FLAGS, 0x04))
                GetPlayer()->RemoveFlag(PLAYER_FLAGS, 0x04);
            else
            {
                GetPlayer()->SetFlag(PLAYER_FLAGS, 0x04);
                data = sChatHandler.FillMessageData(CHAT_MSG_DND, LANG_UNIVERSAL, reason.c_str(),_player->GetGUID());
                GetPlayer()->SendMessageToSet(data, false);
                delete data;
            }          
        } break;
    default:
        sLog.outError("CHAT: unknown msg type %u, lang: %u", type, lang);
    }
}

void WorldSession::HandleTextEmoteOpcode( WorldPacket & recv_data )
{
    CHECK_PACKET_SIZE(recv_data, 16);
    if(!_player->IsInWorld())
        return;

    uint64 guid;
    uint32
        text_emote,
        unk,
        namelen =1;
    const char* name =" ";

    recv_data >> text_emote;
    recv_data >> unk;
    recv_data >> guid;

    Unit * pUnit = _player->GetMapMgr()->GetUnit(guid);
    if(pUnit)
    {
        if(pUnit->IsPlayer())
        {
            name = static_cast<Player*>(pUnit)->GetName();
            namelen = strlen(name) + 1;
        }
        else if(pUnit->GetTypeId() == TYPEID_UNIT)
        {
            Creature * p = static_cast<Creature*>(pUnit);
            if(p->GetCreatureName())
            {
                name = p->GetCreatureName()->Name.c_str();
                namelen = strlen(name) + 1;
            }
            else
            {
                name = 0;
                namelen = 0;
            }
        }
    }

    emoteentry *em = sEmoteStore.LookupEntry(text_emote);
    if(em)
    {
        WorldPacket data(SMSG_EMOTE, 28 + namelen);

        data << (uint32)em->textid;
        data << (uint64)GetPlayer()->GetGUID();
        GetPlayer()->SendMessageToSet(&data, true); //If player receives his own emote, his animation stops.

        data.Initialize(SMSG_TEXT_EMOTE);
        data << (uint64)GetPlayer()->GetGUID();
        data << (uint32)text_emote;
        if( namelen > 1 )   data << (uint32)0x00;
        else                data << (uint32)0xFF; //When nothing/self selected, no name is displayed. (default text)
        data << (uint32)namelen;
        if( namelen > 1 )   data.append(name, namelen);
        else                data << (uint8)0x00;

        GetPlayer()->SendMessageToSet(&data, true);
    }
}

