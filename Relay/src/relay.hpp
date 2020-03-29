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
    
    inline int NumSessions() { return sessions.size(); }
    String GetSessionName(int s);
    void KickSession(String sessionname);
    int NumClients(String sessionname);
    
    void RunGC();
    void RunStats();
private:
    friend class HCRelay;
    OwnedArray<HCRelay> conns;
    StringArray sessions;
    ReadWriteLock mutex;
    String passphrase;
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
    
    enum class Mode { Invalid, Client, Host };
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
    HostStatsThread& parent;
};
