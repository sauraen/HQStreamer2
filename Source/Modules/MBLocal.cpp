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
    jassert(!connected);
    sessionname = session;
    beginWaitingForSocket(HQS2_PORT);
    status.PushStatus(STATUS_MISC, "Waiting for clients...", 30);
    connected = true;
    jassert(sender == nullptr);
    sender = new SenderThread(*this, 128, 256);
    sender->startThread();
}
void MBLocal::Disconnect(HCLocal *conn){
    if(conn){
        conn->disconnect();
        conns.removeObject(conn);
        status.PushStatus(STATUS_MISC, "A client disconnected", 30);
    }else if(connected){
        connected = false;
        stop();
        sender->stopThread(10);
        delete sender; sender = nullptr;
        while(conns.size() > 0){
            conns.getLast()->disconnect();
            conns.removeLast();
        }
        status.PushStatus(STATUS_MISC, "Disconnected", 30);
    }
    proc.sendChangeMessage();
}

InterprocessConnection *MBLocal::createConnectionObject(){
    HCLocal *conn = new HCLocal(*this);
    conns.add(conn);
    status.PushStatus(STATUS_MISC, "A client has connected!", 30);
    return conn;
}

void MBLocal::SendAudioPacket(const MemoryBlock &message){
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
            parent.status.PushStatus(STATUS_BADSIZE, "Client tried to connect to bogus session " + session + "!");
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
