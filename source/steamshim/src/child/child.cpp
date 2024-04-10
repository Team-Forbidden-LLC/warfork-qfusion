/*
Copyright (C) 2023 coolelectronics

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
*/

#include <cstddef>
#include <cstdint>
#include <ctime>
#include <chrono>
#include <thread>

#include "child_private.h"
#include "../steamshim_private.h"
#include "../os.h"
#include "../steamshim.h"
#include "steam/isteamfriends.h"
#include "steam/isteammatchmaking.h"
#include "steam/isteamutils.h"
#include "steam/steam_api.h"
#include "steam/steam_gameserver.h"


static bool GRunServer = false;
static bool GRunClient = false;

static ISteamUserStats *GSteamStats = NULL;
static ISteamUtils *GSteamUtils = NULL;
static ISteamUser *GSteamUser = NULL;
static AppId_t GAppID = 0;
static uint64 GUserID = 0;
static ISteamGameServer *GSteamGameServer = NULL;
// static time_t time_since_last_pump = 0;

static SteamCallbacks *GSteamCallbacks;

static bool processCommand(PipeBuffer cmd, ShimCmd cmdtype, unsigned int len)
{
  #if 1
    if (false) {}
#define PRINTGOTCMD(x) else if (cmdtype && cmdtype == x) dbgprintf("Child got " #x ".\n")
    PRINTGOTCMD(SHIMCMD_BYE);
    // PRINTGOTCMD(SHIMCMD_PUMP);
    PRINTGOTCMD(SHIMCMD_REQUESTSTEAMID);
    PRINTGOTCMD(SHIMCMD_REQUESTPERSONANAME);
    PRINTGOTCMD(SHIMCMD_SETRICHPRESENCE);
    PRINTGOTCMD(SHIMCMD_REQUESTAUTHSESSIONTICKET);
    PRINTGOTCMD(SHIMCMD_BEGINAUTHSESSION);
    PRINTGOTCMD(SHIMCMD_ENDAUTHSESSION);
    PRINTGOTCMD(SHIMCMD_REQUESTAVATAR);
#undef PRINTGOTCMD
    else if (cmdtype != SHIMCMD_PUMP) printf("Child got unknown shimcmd %d.\n", (int) cmdtype);
#endif

    PipeBuffer msg;
    switch (cmdtype)
    {
        case SHIMCMD_PUMP:
            // time(&time_since_last_pump);
            if (GRunServer)
                SteamGameServer_RunCallbacks();
            if (GRunClient)
                SteamAPI_RunCallbacks();
            break;

        case SHIMCMD_BYE:
            return false;

        case SHIMCMD_REQUESTSTEAMID:
            {
                unsigned long long id = SteamUser()->GetSteamID().ConvertToUint64();

                msg.WriteByte(SHIMEVENT_STEAMIDRECIEVED);
                msg.WriteLong(id);
                msg.Transmit();
            }
            break;

        case SHIMCMD_REQUESTPERSONANAME:
            {
                const char* name = SteamFriends()->GetPersonaName();

                msg.WriteByte(SHIMEVENT_PERSONANAMERECIEVED);
                msg.WriteData((void*)name, strlen(name));
                msg.Transmit();
            }
            break;

        case SHIMCMD_SETRICHPRESENCE:
            {
                int num = cmd.ReadInt();

                for (int i=0; i < num;i++){
                    const char *key = cmd.ReadString();
                    const char *val = cmd.ReadString();
                    SteamFriends()->SetRichPresence(key,val);
                }
            }
            break;
        case SHIMCMD_REQUESTAUTHSESSIONTICKET:
            {
                char pTicket[AUTH_TICKET_MAXSIZE];
                uint32 pcbTicket;
                GSteamUser->GetAuthSessionTicket(pTicket,AUTH_TICKET_MAXSIZE, &pcbTicket);

                msg.WriteByte(SHIMEVENT_AUTHSESSIONTICKETRECIEVED);
                msg.WriteLong(pcbTicket);
                msg.WriteData(pTicket, AUTH_TICKET_MAXSIZE);
                msg.Transmit();
            }
            break;
        case SHIMCMD_BEGINAUTHSESSION:
            {
                uint64 steamID = cmd.ReadLong();
                long long cbAuthTicket = cmd.ReadLong();
                void* pAuthTicket = cmd.ReadData(AUTH_TICKET_MAXSIZE);


                int result = GSteamGameServer->BeginAuthSession(pAuthTicket, cbAuthTicket, steamID);

                msg.WriteByte(SHIMEVENT_AUTHSESSIONVALIDATED);
                msg.WriteInt(result);
                msg.Transmit();
            }
            break;
        case SHIMCMD_CREATEBEACON:
            {

                int openSlots = cmd.ReadInt();
                char *connectString = cmd.ReadString();
                char *metadata = cmd.ReadString();

                uint32 puNumLocations = 0;
                SteamParties()->GetNumAvailableBeaconLocations(&puNumLocations);
                if (puNumLocations <= 0) return 0;

                SteamPartyBeaconLocation_t *pLocationList = (SteamPartyBeaconLocation_t *)malloc(puNumLocations* sizeof(SteamPartyBeaconLocation_t) );
                SteamParties()->GetAvailableBeaconLocations(pLocationList, puNumLocations);

                SteamParties()->CreateBeacon(openSlots, pLocationList, connectString,metadata);
                free(pLocationList);
            }
            break;
        case SHIMCMD_REQUESTAVATAR:
            {
                uint64 id = cmd.ReadLong();

                if (!SteamFriends()->RequestUserInformation(id, false)){
                    TransmitAvatar(id);
                }
            }
            break;
        case SHIMCMD_ENDAUTHSESSION:
            {
                uint64 steamID = cmd.ReadLong();
                GSteamGameServer->EndAuthSession(steamID);
            }
            break;
        case SHIMCMD_OPENPROFILE:
            {
                uint64 steamID = cmd.ReadLong();
                SteamFriends()->ActivateGameOverlayToUser("steamid", steamID);
            }
            break;
        case SHIMCMD_REQUESTCOMMANDLINE:
            {
                char cmdline[1024] = {0};
                SteamApps()->GetLaunchCommandLine(cmdline, 1024);
                msg.WriteByte(SHIMEVENT_COMMANDLINERECIEVED);
                msg.WriteString(cmdline);
                msg.Transmit();
            }
            break;
    } // switch

    return true;
}


static void processCommands()
{
  PipeBuffer buf;
  while (1){
    // if (time_since_last_pump != 0){
    //     time_t delta = time(NULL) - time_since_last_pump;
    //     if (delta > 5) // we haven't gotten a pump in 5 seconds, safe to assume the main process is either dead or unresponsive and we can terminate
    //         return;
    // }

    if (!buf.Recieve())
      continue;

    if (buf.hasmsg){

        volatile unsigned int evlen =buf.ReadInt();

        ShimCmd cmd = (ShimCmd)buf.ReadByte();

        if (!processCommand(buf, cmd, evlen))
            return; // we were told to exit
    } else {
        std::this_thread::sleep_for(std::chrono::microseconds(1000));
    }
  }
} 


static bool initSteamworks(PipeType fd)
{
    if (GRunServer) {
        // this will fail if port is in use
        if (!SteamGameServer_Init(0, 27016, 0,eServerModeNoAuthentication,"0.0.0.0"))
            return 0;
        GSteamGameServer = SteamGameServer();
        if (!GSteamGameServer)
            return 0;
        
    }
    if (GRunClient){
        // this can fail for many reasons:
        //  - you forgot a steam_appid.txt in the current working directory.
        //  - you don't have Steam running
        //  - you don't own the game listed in steam_appid.txt
        if (!SteamAPI_Init())
            return 0;

        GSteamStats = SteamUserStats();
        GSteamUtils = SteamUtils();
        GSteamUser = SteamUser();

        GAppID = GSteamUtils ? GSteamUtils->GetAppID() : 0;
	    GUserID = GSteamUser ? GSteamUser->GetSteamID().ConvertToUint64() : 0;
    }
    
    GSteamCallbacks = new SteamCallbacks();
    return 1;
} 

static void deinitSteamworks() {
    if (GRunServer) {
        SteamGameServer_Shutdown();
    }

    if (GRunClient) {
        SteamAPI_Shutdown();
    }
}

static int initPipes(void)
{
    char buf[64];
    unsigned long long val;

    if (!getEnvVar("STEAMSHIM_READHANDLE", buf, sizeof (buf)))
        return 0;
    else if (sscanf(buf, "%llu", &val) != 1)
        return 0;
    else
        GPipeRead = (PipeType) val;

    if (!getEnvVar("STEAMSHIM_WRITEHANDLE", buf, sizeof (buf)))
        return 0;
    else if (sscanf(buf, "%llu", &val) != 1)
        return 0;
    else
        GPipeWrite = (PipeType) val;
    
    return ((GPipeRead != NULLPIPE) && (GPipeWrite != NULLPIPE));
} /* initPipes */

int main(int argc, char **argv)
{
    signal(SIGPIPE, SIG_IGN);
    signal(SIGINT, SIG_IGN);
#ifndef _WIN32

    if( argc > 1 && strcmp(argv[1], "steamdebug") ) {
        debug = true;
    }
#endif

    dbgprintf("Child starting mainline.\n");


    char buf[64];
    if (!initPipes())
        fail("Child init failed.\n");
    
    if (getEnvVar("STEAMSHIM_RUNCLIENT", buf, sizeof(buf)))
        GRunClient = true;
    if (getEnvVar("STEAMSHIM_RUNSERVER", buf, sizeof(buf)))
        GRunServer = true;

    if (!initSteamworks(GPipeWrite)) {
        char failure = 0;
        writePipe(GPipeWrite, &failure, sizeof failure);
        fail("Failed to initialize Steamworks");
    }

    char success = 1;
    writePipe(GPipeWrite, &success, sizeof success);


    dbgprintf("Child in command processing loop.\n");

    // Now, we block for instructions until the pipe fails (child closed it or
    //  terminated/crashed).
    processCommands();

    dbgprintf("Child shutting down.\n");

    // Close our ends of the pipes.
    closePipe(GPipeRead);
    closePipe(GPipeWrite);

    deinitSteamworks();

    return 0;
}

#ifdef _WIN32

// from win_sys
#define MAX_NUM_ARGVS 128
int argc;
char *argv[MAX_NUM_ARGVS];

int CALLBACK WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{
    if( strstr( GetCommandLineA(), "steamdebug" ) ) {
        debug = true;
        FreeConsole();
        AllocConsole();
        freopen( "CONOUT$", "w", stdout );
    }
	return main( argc, argv );
} // WinMain
#endif

