#include "relay.hpp"

////////////////////////////////////////////////////////////////////////////////

HQS2Relay::HQS2Relay() {
    Random::getSystemRandom().setSeedRandomly();
    //Set the passphrase to some random garbage so people don't leave it on the default
    passphrase = String(Random::getSystemRandom().nextInt64());
    beginWaitingForSocket(HQS2_PORT);
    gc.reset(new ClientGCThread(*this));
    gc->startThread();
    stats.reset(new HostStatsThread(*this));
    stats->startThread();
    std::cout << "HQS2Relay running\n";
}
HQS2Relay::~HQS2Relay() {
    //
}

InterprocessConnection *HQS2Relay::createConnectionObject() {
    std::cout << "New connection opened\n";
    const ScopedWriteLock lock(mutex);
    HCRelay *conn = new HCRelay(*this);
    conns.add(conn);
    return conn;
}
void HQS2Relay::SendAudioPacket(const MemoryBlock &message, String session) {
    const ScopedReadLock lock(mutex);
    for(int c=0; c<conns.size(); ++c){
        if(conns[c]->mode == HCRelay::Mode::Client && conns[c]->sessionname == session){
            conns[c]->sendMessage(message);
        }
    }
}

void HQS2Relay::SetPassphrase(String pass) {
    if(pass == ""){
        std::cout << "Won't set an empty passphrase!\n";
        return;
    }
    if(pass.length() < 12){
        std::cout << "That's a really short passphrase, but okay!\n";
    }
    passphrase = pass;
}

String HQS2Relay::GetSessionName(int s){
    const ScopedReadLock lock(mutex);
    if(s < 0 || s >= sessions.size()) return "Error";
    return sessions[s];
}
void HQS2Relay::KickSession(String sessionname){
    HCRelay *host = nullptr;
    {
        const ScopedReadLock lock(mutex);
        for(int c=0; c<conns.size(); ++c){
            if(conns[c]->mode == HCRelay::Mode::Host && conns[c]->sessionname == sessionname){
                host = conns[c];
                break;
            }
        }
    }
    if(!host){
        std::cout << "There is no session \"" << sessionname << "\"!\n";
        return;
    }
    host->disconnect();
}
int HQS2Relay::NumClients(String sessionname){
    const ScopedReadLock lock(mutex);
    int nc = 0;
    for(int c=0; c<conns.size(); ++c){
        if(conns[c]->mode == HCRelay::Mode::Client && conns[c]->sessionname == sessionname){
            ++nc;
        }
    }
    return nc;
}

void HQS2Relay::RunGC(){
    const ScopedWriteLock lock(mutex);
    for(int c=0; c<conns.size(); ++c){
        if(!conns[c]->isConnected()){
            std::cout << "Removed stale connection " << c << "\n";
            conns.remove(c);
            --c;
        }
    }
}

void HQS2Relay::RunStats(){
    const ScopedReadLock lock(mutex);
    for(int c=0; c<conns.size(); ++c){
        if(conns[c]->mode == HCRelay::Mode::Host){
            int nc = NumClients(conns[c]->sessionname);
            MemoryBlock ret;
            ret.setSize(20);
            ret.fillWith(0);
            int32_t* s32ptr2 = (int32_t*)ret.getData();
            s32ptr2[0] = 20;
            s32ptr2[1] = PACKET_TYPE_HOSTSTATS;
            s32ptr2[2] = (int32_t)nc;
            if(!conns[c]->sendMessage(ret)){
                
            }
        }
    }
}

void HQS2Relay::PrintSessionsInfo(){
    const ScopedReadLock lock(mutex);
    std::cout << sessions.size() << " sessions:\n";
    for(int s=0; s<sessions.size(); ++s){
        std::cout << s << ". " << sessions[s] << " (" << NumClients(sessions[s]) << " clients)\n";
    }
}

void HQS2Relay::PrintSessionInfo(String sessionname){
    const ScopedReadLock lock(mutex);
    if(!sessions.contains(sessionname)){
        std::cout << "There is no session \"" << sessionname << "\"\n";
        return;
    }
    std::cout << "Session " << sessionname << ": " << NumClients(sessionname) << " clients\n";
    HCRelay *host = nullptr;
    for(int c=0; c<conns.size(); ++c){
        if(conns[c]->sessionname != sessionname) continue;
        if(conns[c]->mode == HCRelay::Mode::Host){
            host = conns[c];
        }else if(conns[c]->mode == HCRelay::Mode::Client){
            std::cout << "Client " << c << ": " << conns[c]->getSocket()->getHostName() << "\n";
        }
    }
    if(!host){
        std::cout << "(No host?!)\n";
    }else{
        std::cout << "Host: " << host->getSocket()->getHostName() << "\n";
    }
}

////////////////////////////////////////////////////////////////////////////////

HCRelay::HCRelay(HQS2Relay &p) : InterprocessConnection(false, 0x32535148), parent(p) {
    mode = Mode::Invalid;
    challenge = Random::getSystemRandom().nextInt64();
    sessionname = "";
}
HCRelay::~HCRelay(){
    //
}

void HCRelay::connectionMade(){
    std::cout << "HCRelay::connectionMade()\n";
}
void HCRelay::connectionLost(){
    std::cout << "HCRelay::connectionLost()\n";
    if(mode == Mode::Host){
        const ScopedReadLock lock(parent.mutex);
        for(int c=0; c<parent.conns.size(); ++c){
            if(parent.conns[c]->mode == HCRelay::Mode::Client && parent.conns[c]->sessionname == sessionname){
                parent.conns[c]->disconnect();
            }
        }
    }
    mode = Mode::Invalid;
    sessionname = "";
}
void HCRelay::messageReceived(const MemoryBlock& message){
    if(message.getSize() < 20 || message.getSize() > 1000000){
        std::cout << "Bad size packet! " << String((int)message.getSize()) << "\n";
        return;
    }
    int32_t* s32ptr = (int32_t*)message.getData();
    int32_t size = s32ptr[0];
    if(size != (int32_t)message.getSize()){
        std::cout << "Bad size field " + String((int)size) << "\n";
        return;
    }
    int32_t type = s32ptr[1];
    MemoryBlock ret;
    ret.setSize(20);
    ret.fillWith(0);
    int32_t* s32ptr2 = (int32_t*)ret.getData();
    int64_t* s64ptr2 = (int64_t*)ret.getData();
    s32ptr2[0] = 20;
    if(type == PACKET_TYPE_PING){
        //Send a copy back
        s32ptr2[1] = PACKET_TYPE_PINGRESPONSE;
        s32ptr2[2] = s32ptr[2];
        s32ptr2[3] = s32ptr[3];
    }else if(type == PACKET_TYPE_REQHOST){
        //Send challenge
        challenge = Random::getSystemRandom().nextInt64();
        s32ptr2[1] = PACKET_TYPE_CHLHOST;
        s64ptr2[1] = challenge;
    }else if(type == PACKET_TYPE_RESPHOST){
        if(message.getSize() != 16 + HQS2_STRLEN){
            std::cout << "Bad size RESPHOST! " << String((int)message.getSize()) << "\n";
            return;
        }
        int64_t hash_should = (parent.passphrase + String(challenge)).hashCode64();
        int64_t hash_actual = ((int64_t*)message.getData())[1];
        if(hash_should == hash_actual){
            sessionname = String(CharPointer_UTF8((char*)message.getData() + 16), HQS2_STRLEN);
            const ScopedWriteLock lock(parent.mutex);
            if(parent.sessions.contains(sessionname)){
                s32ptr2[1] = PACKET_TYPE_NAMECOLLHOST;
                std::cout << "Name collision! " << sessionname << "\n";
            }else{
                s32ptr2[1] = PACKET_TYPE_ACKHOST;
                std::cout << "New session started, name " << sessionname << "\n";
                mode = Mode::Host;
                const ScopedWriteLock lock(parent.mutex);
                parent.sessions.add(sessionname);
            }
        }else{
            s32ptr2[1] = PACKET_TYPE_NAKHOST;
            std::cout << "Wrong passphrase!\n";
        }
    }else if(type == PACKET_TYPE_REQJOIN){
        if(message.getSize() != 8 + HQS2_STRLEN){
            std::cout << "Bad size REQJOIN! " << String((int)message.getSize()) << "\n";
            return;
        }
        sessionname = String(CharPointer_UTF8((char*)message.getData() + 8), HQS2_STRLEN);
        const ScopedReadLock lock(parent.mutex);
        if(!parent.sessions.contains(sessionname)){
            s32ptr[1] = PACKET_TYPE_NAKJOIN;
            std::cout << "Client tried to join nonexistent session " << sessionname << "!\n";
        }else{
            s32ptr[1] = PACKET_TYPE_ACKJOIN;
            std::cout << "New client on session " << sessionname << "\n";
            mode = Mode::Client;
        }
    }else if(type == PACKET_TYPE_AUDIO_ZEROS 
          || type == PACKET_TYPE_AUDIO_FLOAT32
          || type == PACKET_TYPE_AUDIO_INT16
          || type == PACKET_TYPE_AUDIO_DPCM){
        if(mode != Mode::Host){
            std::cout << "Connection other than a host is sending audio!\n";
            return;
        }
        int32_t nchannels = s32ptr[2], nsamples = s32ptr[3], fs = s32ptr[4];
        if(nchannels <= 0 || nchannels > 128 || nsamples < 16 || nsamples > 100000 || fs <= 0 || fs >= 1000000){
            std::cout << "Bad audio params in packet!\n";
            return;
        }
        if(type == PACKET_TYPE_AUDIO_ZEROS && message.getSize() != 20 ||
                type == PACKET_TYPE_AUDIO_FLOAT32 && message.getSize() != 20+4*nsamples*nchannels ||
                type == PACKET_TYPE_AUDIO_INT16 && message.getSize() != 20+2*nsamples*nchannels ||
                type == PACKET_TYPE_AUDIO_DPCM && message.getSize() > 20+2*nsamples*nchannels){
            std::cout << "Bad audio packet size " << (int)message.getSize() << " for type " << (int)type << "!\n";
            return;
        }
        parent.SendAudioPacket(message, sessionname);
        return;
    }else{
        std::cout << "Bad packet type " << String((int)type) << " received!\n";
        return;
    }
    if(!sendMessage(ret)){
        std::cout << "Could not send response packet\n";
    }
}

////////////////////////////////////////////////////////////////////////////////

void ClientGCThread::run(){
    while(!threadShouldExit()){
        parent.RunGC();
        wait(2973);
    }
}

void HostStatsThread::run(){
    while(!threadShouldExit()){
        parent.RunStats();
        wait(1023);
    }
}
