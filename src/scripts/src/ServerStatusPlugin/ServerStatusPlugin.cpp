#include "StdAfx.h"
#include "../../../shared/svn_revision.h"

#ifdef WIN32
#pragma pack(push,8)
#include "PerfCounters.h"
#pragma pack(pop)
#include <Psapi.h>

#define SYSTEM_OBJECT_INDEX					2		// 'System' object
#define PROCESS_OBJECT_INDEX				230		// 'Process' object
#define PROCESSOR_OBJECT_INDEX				238		// 'Processor' object
#define TOTAL_PROCESSOR_TIME_COUNTER_INDEX	240		// '% Total processor time' counter (valid in WinNT under 'System' object)
#define PROCESSOR_TIME_COUNTER_INDEX		6		// '% processor time' counter (for Win2K/XP)

#pragma comment(lib, "advapi32")
#pragma comment(lib, "Psapi")
#endif

extern "C" SCRIPT_DECL uint32 _exp_get_version()
{
    return MAKE_SCRIPT_VERSION(SCRIPTLIB_VERSION_MAJOR, SCRIPTLIB_VERSION_MINOR);
}

#ifdef WIN32
bool m_bFirstTime = true;
LONGLONG		m_lnOldValue = 0;
LARGE_INTEGER	m_OldPerfTime100nSec;
uint32 number_of_cpus;
#endif

#ifdef WIN32
/* This is needed because windows is a piece of shit. ;) */
BOOL APIENTRY DllMain( HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved )
{
    return TRUE;
}
#endif

char Filename[MAX_PATH];
class StatDumper
{
public:
    void DumpStats();
};

StatDumper dumper;

extern "C" SCRIPT_DECL void _exp_script_register(ScriptMgr* mgr)
{
    //strcpy(Filename, Config.MainConfig.GetStringDefault("StatDumper.Filename", "stats.xml").c_str());
    //strcpy(Filename, "stats.xml");
	if(!Config.MainConfig.GetString("StatDumper", Filename, "Filename", "stats.xml", MAX_PATH))
		strcpy(Filename, "stats.xml");

#ifdef WIN32
memset(&m_OldPerfTime100nSec, 0, sizeof(m_OldPerfTime100nSec));
SYSTEM_INFO si;
GetSystemInfo(&si);
number_of_cpus = si.dwNumberOfProcessors;
#endif
    dumper.DumpStats();

	int t = Config.MainConfig.GetIntDefault("StatDumper", "Interval", 120000);
	TimedEvent * te = TimedEvent::Allocate(&dumper, new CallbackP0<StatDumper>(&dumper, &StatDumper::DumpStats), 1, t, 0);
    sWorld.event_AddEvent(te);
}

#ifdef WIN32
/*** GRR ***/
int __cdecl HandleCrash(PEXCEPTION_POINTERS pExceptPtrs)
{
    return 0;
}
#endif

void SetThreadName(const char* format, ...)
{

}

void GenerateUptimeString(char * Dest)
{
    uint32 seconds = sWorld.GetUptime();
    uint32 mins=0;
    uint32 hours=0;
    uint32 days=0;
    if(seconds >= 60)
    {
        mins = seconds / 60;
        if(mins)
        {
            seconds -= mins*60;
            if(mins >= 60)
            {
                hours = mins / 60;
                if(hours)
                {
                    mins -= hours*60;
                    if(hours >= 24)
                    {
                        days = hours/24;
                        if(days)
                            hours -= days*24;
                    }
                }
            }
        }
    }
    sprintf(Dest, "%d days, %d hours, %d minutes, %d seconds", days, hours, mins, seconds);
}

#ifdef WIN32

float GetCPUUsageWin32()
{
    CPerfCounters<LONGLONG> PerfCounters;
    DWORD dwObjectIndex = PROCESS_OBJECT_INDEX;
    DWORD dwCpuUsageIndex = PROCESSOR_TIME_COUNTER_INDEX;

    int				CpuUsage = 0;
    LONGLONG		lnNewValue = 0;
    PPERF_DATA_BLOCK pPerfData = NULL;
    LARGE_INTEGER	NewPerfTime100nSec = {0};

    lnNewValue = PerfCounters.GetCounterValueForProcessID(&pPerfData, dwObjectIndex, dwCpuUsageIndex, GetCurrentProcessId());
    NewPerfTime100nSec = pPerfData->PerfTime100nSec;

    if (m_bFirstTime)
    {
        m_bFirstTime = false;
        m_lnOldValue = lnNewValue;
        m_OldPerfTime100nSec = NewPerfTime100nSec;
        return 0;
    }

    LONGLONG lnValueDelta = lnNewValue - m_lnOldValue;
    double DeltaPerfTime100nSec = (double)NewPerfTime100nSec.QuadPart - (double)m_OldPerfTime100nSec.QuadPart;

    m_lnOldValue = lnNewValue;
    m_OldPerfTime100nSec = NewPerfTime100nSec;

    double a = (double)lnValueDelta / DeltaPerfTime100nSec;
    a /= double(number_of_cpus);
    return (a * 100);
}

float GetRAMUsageWin32()
{
    PROCESS_MEMORY_COUNTERS pmc;
    GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc));
    float ram = pmc.PagefileUsage;
    ram /= 1024.0f;
    ram /= 1024.0f;
    return ram;
}

#endif

float GetCPUUsage()
{
#ifdef WIN32
    return GetCPUUsageWin32();
#else
    return 0.0f;
#endif
}

float GetRAMUsage()
{
#ifdef WIN32
    return GetRAMUsageWin32();
#else
     return 0.0f;
#endif
}

void FillOnlineTime(uint32 Time, char * Dest)
{
    uint32 seconds = Time;
    uint32 mins=0;
    uint32 hours=0;
    uint32 days=0;
    if(seconds >= 60)
    {
        mins = seconds / 60;
        if(mins)
        {
            seconds -= mins*60;
            if(mins >= 60)
            {
                hours = mins / 60;
                if(hours)
                {
                    mins -= hours*60;
                    if(hours >= 24)
                    {
                        days = hours/24;
                        if(days)
                            hours -= days*24;
                    }
                }
            }
        }
    }
    sprintf(Dest, "%d hours, %d minutes, %d seconds", hours, mins, seconds);
}

void StatDumper::DumpStats()
{
    FILE * f = fopen(Filename, "w");
    // Dump Header
    fprintf(f, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
    fprintf(f, "<?xml-stylesheet type=\"text/xsl\" href=\"server_stats.xsl\"?>\n");
    fprintf(f, "<serverpage>\n");
    fprintf(f, "  <status>\n");
    std::deque<Player*> gms;
    {
        // Dump server information.
#ifdef WIN32
        fprintf(f, "    <platform>Antrix/Win32 v2.1.0-%u</platform>\n", g_getRevision());
#else
        fprintf(f, "    <platform>Antrix/Linux v2.1.0-%u</platform>\n", g_getRevision());
#endif

        char uptime[80];
        GenerateUptimeString(uptime);
        float AvgLat;
        uint32 GMCount;
        int gm = 0;
        int count = 0;
        int avg = 0;
        // lock players reader
        objmgr._playerslock.AcquireReadLock();

        HM_NAMESPACE::hash_map<uint32, Player*>::const_iterator itr;
        for (itr = objmgr._players.begin(); itr != objmgr._players.end(); itr++)
        {
            if(itr->second->GetSession() && itr->second->IsInWorld())
            {
                count++;
                avg += itr->second->GetSession()->GetLatency();
                if(itr->second->GetSession()->GetPermissionCount())
                {
                    gm++;
                    gms.push_back(itr->second);
                }
            }            
        }
        objmgr._playerslock.ReleaseReadLock();

        AvgLat = count ? (float)((float)avg / (float)count) : 0;
        GMCount = gm;

        fprintf(f, "    <uptime>%s</uptime>\n", uptime);
        fprintf(f, "    <oplayers>%u</oplayers>\n", sWorld.AlliancePlayers + sWorld.HordePlayers);
        fprintf(f, "    <cpu>%2.2f</cpu>\n", GetCPUUsage());
        fprintf(f, "    <qplayers>%u</qplayers>\n", sWorld.GetQueueCount());
        fprintf(f, "    <ram>%.3f</ram>\n", GetRAMUsage());
        fprintf(f, "    <avglat>%.3f</avglat>\n", AvgLat);
        fprintf(f, "    <threads>%u</threads>\n", sThreadMgr.GetThreadCount());
        time_t t = time(NULL);
        fprintf(f, "    <gmcount>%u</gmcount>\n", GMCount);
        fprintf(f, "    <lastupdate>%s</lastupdate>\n", asctime(localtime(&t)));
        fprintf(f, "    <alliance>%u</alliance>\n", sWorld.AlliancePlayers);
        fprintf(f, "    <horde>%u</horde>\n", sWorld.HordePlayers);
        fprintf(f, "    <acceptedconns>%u</acceptedconns>\n", sWorld.mAcceptedConnections);
        fprintf(f, "    <peakcount>%u</peakcount>\n", sWorld.PeakSessionCount);
		fprintf(f, "    <querysize>%u</querysize>\n", ((MySQLDatabase*)Database_Main)->GetQueueSize());
    }
    fprintf(f, "  </status>\n");
    Player * plr;
    uint32 t = time(NULL);
    char otime[100];
    {
        fprintf(f, "  <instances>\n");
        
        // need a big buffer..
        static char buf[50000];
        memset(buf, 0, 50000);

        // Dump Instance Information
        sWorldCreator.BuildXMLStats(buf);
        fprintf(f, buf);
        fprintf(f, "  </instances>\n");
    }
    {
        // GM Information
        fprintf(f, "  <gms>\n");
        while(!gms.empty())
        {
            plr = gms.front();
            gms.pop_front();

            FillOnlineTime(t - plr->OnlineTime, otime);
            fprintf(f, "    <gmplr>\n");
            fprintf(f, "      <name>%s</name>\n", plr->GetName());
            fprintf(f, "      <race>%u</race>\n", plr->getRace());
            fprintf(f, "      <class>%u</class>\n", plr->getClass());
            fprintf(f, "      <gender>%u</gender>\n", plr->getGender());
            fprintf(f, "      <pvprank>%u</pvprank>\n", plr->GetPVPRank());
            fprintf(f, "      <level>%u</level>\n", plr->GetUInt32Value(UNIT_FIELD_LEVEL));
            fprintf(f, "      <map>%u</map>\n", plr->GetMapId());
            fprintf(f, "      <areaid>%u</areaid>\n", plr->GetAreaID());
            fprintf(f, "      <ontime>%s</ontime>\n", otime);
            fprintf(f, "      <latency>%u</latency>\n", plr->GetSession()->GetLatency());
            fprintf(f, "      <permissions>%s</permissions>\n", plr->GetSession()->GetPermissions());
            fprintf(f, "    </gmplr>\n");
        }

        fprintf(f, "  </gms>\n");
    }

    {
    fprintf(f, "  <sessions>\n");
        // Dump Player Information
        objmgr._playerslock.AcquireReadLock();
        HM_NAMESPACE::hash_map<uint32, Player*>::const_iterator itr;

        for (itr = objmgr._players.begin(); itr != objmgr._players.end(); itr++)
        {
            plr = itr->second;
            if(itr->second->GetSession() && itr->second->IsInWorld())
            {
                FillOnlineTime(t - plr->OnlineTime, otime);

                fprintf(f, "    <plr>\n");
                fprintf(f, "      <name>%s</name>\n", plr->GetName());
                fprintf(f, "      <race>%u</race>\n", plr->getRace());
                fprintf(f, "      <class>%u</class>\n", plr->getClass());
				fprintf(f, "      <gender>%u</gender>\n", plr->getGender());
				fprintf(f, "      <pvprank>%u</pvprank>\n", plr->GetPVPRank());
                fprintf(f, "      <level>%u</level>\n", plr->GetUInt32Value(UNIT_FIELD_LEVEL));
                fprintf(f, "      <map>%u</map>\n", plr->GetMapId());
                fprintf(f, "      <areaid>%u</areaid>\n", plr->GetAreaID());
				//requested by Zdarkside for he's online map. I hope it does not scre up any parser. If so, then make a better one :P
                fprintf(f, "      <xpos>%f</xpos>\n", plr->GetPositionX ());
                fprintf(f, "      <ypos>%f</ypos>\n", plr->GetPositionY());
                fprintf(f, "      <ontime>%s</ontime>\n", otime);
                fprintf(f, "      <latency>%u</latency>\n", plr->GetSession()->GetLatency());
                fprintf(f, "    </plr>\n");
                if(plr->GetSession()->GetPermissionCount() > 0)
                    gms.push_back(plr);
            }
        }
        objmgr._playerslock.ReleaseReadLock();
        fprintf(f, "  </sessions>\n");

        
    }

    fprintf(f, "</serverpage>\n");
    fclose(f);
}
