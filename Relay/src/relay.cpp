#include "relay.hpp"






HCRelay::HCRelay(HQS2Relay &p) : InterprocessConnection(false, 0x32535148), parent(p) {
    Random.getSystemRandom().setSeedRandomly();
    challenge = Random.getSystemRandom().nextInt64();
    mode = Mode::Invalid;
}
HCRelay::~HCRelay(){
    //
}

void HCRelay::connectionMade(){
    std::cout << "HCRelay::connectionMade()\n";
}
void HCRelay::connectionLost(){
    std::cout << "HCRelay::connectionLost()\n";
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
        challenge = Random.getSystemRandom().nextInt64();
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
                sessions.add(sessionname);
                mode = Mode::Host;
            }
        }else{
            s32ptr2[1] = PACKET_TYPE_NAKHOST;
            std::cout << "Wrong passphrase!\n";
        }
    }else if(type == PACKET_TYPE_REQJOIN){
        if(packet.getSize() != 8 + HQS2_STRLEN){
            std::cout << "Bad size REQJOIN! " << String((int)message.getSize()) << "\n";
            return;
        }
        sessionname = String(CharPointer_UTF8((char*)packet.getData() + 8), HQS2_STRLEN);
        const ScopedReadLock lock(parent.mutex);
        if(!parent.sessions.contains(sessionname)){
            s32ptr[1] = PACKET_TYPE_NAKJOIN;
            std::cout << "Client tried to join nonexistent session " << sessionname << "!\n";
        }else{
            s32ptr[1] = PACKET_TYPE_ACKJOIN;
            std::cout << "New client on session " << sessionname << "\n";
            mode = Mode::Client;
        }
    }else{
        parent.status.PushStatus(STATUS_BADAPARAM, "Bad packet type " + String((int)type) + " received!", 30);
        return;
    }
    if(!sendMessage(ret)){
        std::cout << "Could not send response packet\n";
    }
}
