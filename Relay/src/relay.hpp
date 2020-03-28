#include "main.hpp"
#include "../../Source/ANI.hpp"

class HCRelay;

class HQS2Relay : public InterprocessConnectionServer {
public:
    HQS2Relay();
    virtual ~HQS2Relay() override;
    
    virtual InterprocessConnection *createConnectionObject() override;
    void SendAudioPacket(const MemoryBlock &message);
    
    void SetPassphrase(String pass);
    
    int NumSessions();
    String GetSessionName(int s);
    void KickSession(int s);
    int NumClients(int s);
private:
    friend class HCRelay;
    OwnedArray<HCRelay> conns;
    StringArray sessions;
    ReadWriteLock mutex;
    String passphrase;
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
}
