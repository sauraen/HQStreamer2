/*
* HQStreamer2 - Stream audio between DAWs across the internet
* Copyright (C) 2020 Sauraen, <sauraen@gmail.com>
* 
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU Affero General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
* 
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Affero General Public License for more details.
* 
* You should have received a copy of the GNU Affero General Public License
* along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "main.hpp"
#include "../../Source/ANI.hpp"

class HCRelay;
class ClientGCThread;
class HostStatsThread;

class HQS2Relay : public InterprocessConnectionServer {
public:
    HQS2Relay();
    virtual ~HQS2Relay() override;
    
    virtual InterprocessConnection *createConnectionObject() override;
    void SendAudioPacket(const MemoryBlock &message, String session);
    
    void SetPassphrase(String pass);
	static String StorePassphrase(String pass);
    
    inline int NumSessions() { return sessions.size(); }
    String GetSessionName(int s);
    void KickSession(String sessionname);
    int NumClients(String sessionname);
    
    void RunGC();
    void RunStats();
    void PrintSessionsInfo();
    void PrintSessionInfo(String sessionname);
private:
    friend class HCRelay;
    OwnedArray<HCRelay> conns;
    StringArray sessions;
    ReadWriteLock mutex;
    String passphrase_hashed;
    std::unique_ptr<ClientGCThread> gc;
    std::unique_ptr<HostStatsThread> stats;
};

class HCRelay : public InterprocessConnection {
public:
    HCRelay(HQS2Relay &p);
    virtual ~HCRelay() override;
    
    virtual void connectionMade() override;
    virtual void connectionLost() override;
    virtual void messageReceived(const MemoryBlock& message) override;
    
    enum class Mode { Init, Client, Host, Bad };
    Mode mode;
    String sessionname;
private:
    int64_t challenge;
    HQS2Relay& parent;
};

class ClientGCThread : public Thread {
public:
    ClientGCThread(HQS2Relay &p) : Thread("ClientGCThread"), parent(p) {}
    virtual ~ClientGCThread() override {}
    
    virtual void run() override;
private:
    HQS2Relay& parent;
};

class HostStatsThread : public Thread {
public:
    HostStatsThread(HQS2Relay &p) : Thread("HostStatsThread"), parent(p) {}
    virtual ~HostStatsThread() override {}
    
    virtual void run() override;
private:
    HQS2Relay& parent;
};
