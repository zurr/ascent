/****************************************************************************
 *
 * Channel System
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

class ChannelMgr :  public Singleton < ChannelMgr >
{
 
public:
	ChannelMgr()
	{
		
	}

	~ChannelMgr();

	Channel *GetJoinChannel(const char *name, Player *p)
	{ 
		ChannelList* cl=&Channels[0];
		if(seperatechannels)
			cl = &Channels[p->GetTeam()];

		ChannelList::iterator i=cl->find(string(name));
		if(i!=cl->end())
			return i->second;
		else 
		{
			Channel *nchan = new Channel(name,p);
			(*cl)[name]=nchan;
			return nchan;
		}

	}
	Channel *GetChannel(const char *name, Player *p)
	{ 
		ChannelList* cl=&Channels[0];
		if(seperatechannels)
			cl = &Channels[p->GetTeam()];

		ChannelList::iterator i=cl->find(string(name));
		if(i!=cl->end())
			return i->second;
		else 
		{
			WorldPacket data(100);
			data.SetOpcode (SMSG_CHANNEL_NOTIFY);
			data << (uint8)0x05 << string(name);
			p->GetSession()->SendPacket(&data);
			return NULL;
		}
	}
	bool LeftChannel(const char *name,Player*p)
	{ 
		ChannelList* cl=&Channels[0];
		if(seperatechannels)
			cl = &Channels[p->GetTeam()];

		ChannelList::iterator i=cl->find(string(name));
		if(i!=cl->end())
		{
			Channel *ch=i->second;

			ch->Leave(p);
			if(!ch->IsConstant()  && ch->GetNumPlayers() == 0 )
			{
				cl->erase(i);
				delete ch;
			}
			return true;
		}
		return false;
	}
	bool seperatechannels;
private:
	//team 0: aliance, team 1 horde
	typedef map<string,Channel *> ChannelList;
	ChannelList Channels[2];

};

#define channelmgr ChannelMgr::getSingleton()
