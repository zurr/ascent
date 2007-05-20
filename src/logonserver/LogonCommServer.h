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

#ifndef __LOGON_COMM_SERVER_H
#define __LOGON_COMM_SERVER_H

class LogonCommServerSocket : public Socket
{
    uint16 remaining;
    uint16 opcode;
public:
    LogonCommServerSocket(SOCKET fd);
    ~LogonCommServerSocket();

    void OnRead();
    void OnDisconnect();
    void SendPacket(WorldPacket * data);
    void HandlePacket(WorldPacket & recvData);

    void HandleRegister(WorldPacket & recvData);
    void HandlePing(WorldPacket & recvData);
    void HandleSessionRequest(WorldPacket & recvData);
    void HandleSQLExecute(WorldPacket & recvData);

    uint32 last_ping;
    bool removed;
    set<uint32> server_ids;
};

#endif
