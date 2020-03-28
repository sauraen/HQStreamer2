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

#include "MBLocal.hpp"

MBLocal::MBLocal(PluginProcessor &processor) : ModuleBackend(processor), connected(false), sender(nullptr) {
    audiotype = PACKET_TYPE_AUDIO_DPCM;
}
MBLocal::~MBLocal() {
    Disconnect(nullptr);
}

void MBLocal::processBlock(AudioBuffer<float> &audio) {
    ModuleBackend::processBlock(audio);
    if(connected){
        const ScopedReadLock lock(mutex);
        for(int s=0; s<audio.getNumSamples(); ++s){
            for(int c=0; c<audio.getNumChannels(); ++c){
                buf->Write(c, audio.getReadPointer(c)[s]);
            }
            buf->WriteAdvance();
        }
    }
}

void MBLocal::Connect(String session){
    const ScopedWriteLock lock(conns_mutex);
    jassert(!connected);
    sessionname = session;
    beginWaitingForSocket(HQS2_PORT);
    status.PushStatus(STATUS_EVENT, "Waiting for clients...", 30);
    connected = true;
    jassert(sender == nullptr);
    sender = new SenderThread(*this, 128, 256);
    sender->startThread();
    proc.sendChangeMessage();
}
void MBLocal::Disconnect(HCLocal *conn){
    if(conn){
        if(!connected) return;
        std::cout << "MBLocal::Disconnect individual\n";
        status.PushStatus(STATUS_EVENT, "A client disconnected", 30);
        conn->disconnect();
        const ScopedWriteLock lock(conns_mutex);
        conns.removeObject(conn);
        std::cout << "MBLocal::Disconnect individual done\n";
    }else if(connected){
        std::cout << "MBLocal::Disconnect general\n";
        status.PushStatus(STATUS_EVENT, "Disconnected", 30);
        stop();
        connected = false;
        std::cout << "MBLocal::Disconnect stopping sender\n";
        sender->stopThread(10);
        std::cout << "MBLocal::Disconnect deleting sender\n";
        delete sender; sender = nullptr;
        std::cout << "MBLocal::Disconnect stopping conns\n";
        const ScopedWriteLock lock(conns_mutex);
        while(conns.size() > 0){
            std::cout << "MBLocal::Disconnect disconnecting conn\n";
            conns.getLast()->disconnect();
            std::cout << "MBLocal::Disconnect deleting conn\n";
            conns.removeLast();
        }
        std::cout << "MBLocal::Disconnect general done\n";
    }
    proc.sendChangeMessage();
}

InterprocessConnection *MBLocal::createConnectionObject(){
    const ScopedWriteLock lock(conns_mutex);
    HCLocal *conn = new HCLocal(*this);
    conns.add(conn);
    status.PushStatus(STATUS_EVENT, "A client has connected!", 30);
    return conn;
}

void MBLocal::SendAudioPacket(const MemoryBlock &message){
    const ScopedReadLock lock(conns_mutex);
    if(!connected) return;
    for(int i=0; i<conns.size(); ++i){
        if(conns[i]->IsValid()){
            conns[i]->sendMessage(message);
        }
    }
}

HCLocal::HCLocal(MBLocal &p) : HQSConnection(p), parent(p), valid(false) {
    //
}
HCLocal::~HCLocal() {
    //
}

void HCLocal::connectionLost(){
    HQSConnection::connectionLost();
    parent.Disconnect(this);
}

void HCLocal::VdPacketReceived(const MemoryBlock& packet, int32_t type) {
    if(type == PACKET_TYPE_REQJOIN){
        if(packet.getSize() != 8 + HQS2_STRLEN){
            parent.status.PushStatus(STATUS_BADSIZE, "Bad packet size " + String((int)packet.getSize()) + " for REQJOIN type!");
            return;
        }
        String session = String(CharPointer_UTF8((char*)packet.getData() + 8), HQS2_STRLEN);
        MemoryBlock packet2;
        packet2.setSize(20);
        packet2.fillWith(0);
        int32_t *s32ptr = (int32_t*)packet2.getData();
        s32ptr[0] = 20;
        if(session != parent.sessionname){
            parent.status.PushStatus(STATUS_BADAPARAM, "Client tried to connect to bogus session " + session + "!");
            s32ptr[1] = PACKET_TYPE_NAKJOIN;
            sendMessage(packet2);
            parent.Disconnect(this);
            return;
        }
        s32ptr[1] = PACKET_TYPE_ACKJOIN;
        sendMessage(packet2);
        valid = true;
        return;
    }else{
        parent.status.PushStatus(STATUS_BADAPARAM, "Bad packet type " + String((int)type) + " received!", 30);
        return;
    }
}
