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


#include "MBSession.hpp"

MBSession::MBSession(PluginProcessor &processor) : ModuleBackend(processor), connected(false) {
    audiotype = PACKET_TYPE_AUDIO_DPCM;
    numclients = 0;
}
MBSession::~MBSession() {
    Disconnect();
}

void MBSession::processBlock(AudioBuffer<float> &audio) {
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

void MBSession::Connect(String server, String pass, String session){
    if(session.isEmpty()){
        status.PushStatus(STATUS_NOCONNECT, "Session name must not be blank!");
        return;
    }
    servername = server;
    passphrase = pass;
    sessionname = session;
    conn.reset(new HCSession(*this));
    if(!conn->connectToSocket(servername, HQS2_PORT, 1000)){
        status.PushStatus(STATUS_NOCONNECT, "Could not connect to server");
        return;
    }
    MemoryBlock packet;
    packet.setSize(20);
    packet.fillWith(0);
    int32_t *s32ptr = (int32_t*)packet.getData();
    s32ptr[0] = 20;
    s32ptr[1] = PACKET_TYPE_REQHOST;
    if(!conn->sendMessage(packet)){
        status.PushStatus(STATUS_NOCONNECT, "Could not send host request to server");
        return;
    }
    status.ClearStatus(STATUS_NOCONNECT);
    status.PushStatus(STATUS_EVENT, "Sent host request...", 30);
    const ScopedWriteLock lock(mutex);
    buf->Reset();
}
void MBSession::Disconnect(){
    status.PushStatus(STATUS_DISCONNECTED, "Disconnected", 30);
    connected = false;
    if(sender){
        sender->stopThread(10);
        sender.reset(nullptr);
    }
    if(conn){
        conn->disconnect();
    }
    proc.sendChangeMessage();
}

void MBSession::SendAudioPacket(const MemoryBlock &message){
    std::cout << "MBSession::SendAudioPacket\n";
    if(!connected || !conn || !conn->isConnected()) return;
    conn->sendMessage(message);
    std::cout << "Actually send packet\n";
}


HCSession::HCSession(MBSession &p) : HQSConnection(p), parent(p) {
    //
}
HCSession::~HCSession() {
    //
}

void HCSession::connectionLost(){
    HQSConnection::connectionLost();
    parent.connected = false;
    if(parent.sender){
        parent.sender->stopThread(10);
        parent.sender.reset(nullptr);
    }
    parent.GetProc().sendChangeMessage();
    parent.status.PushStatus(STATUS_DISCONNECTED, "Connection lost", 30);
}

void HCSession::VdPacketReceived(const MemoryBlock& packet, int32_t type) {
    if(packet.getSize() != 20){
        parent.status.PushStatus(STATUS_BADSIZE, "Bad packet size " + String((int)packet.getSize()) 
            + " for type" + String((int)type) + "!", 30);
        return;
    }
    if(type == PACKET_TYPE_CHLHOST){
        MemoryBlock packet2;
        packet2.setSize(16+HQS2_STRLEN);
        packet2.fillWith(0);
        int32_t *s32ptr2 = (int32_t*)packet2.getData();
        int64_t *s64ptr2 = (int64_t*)packet2.getData();
        s32ptr2[0] = 16+HQS2_STRLEN;
        s32ptr2[1] = PACKET_TYPE_RESPHOST;
        s64ptr2[1] = (parent.passphrase + String(((int64_t*)packet.getData())[1])).hashCode64();
        parent.sessionname.copyToUTF8((char*)packet2.getData() + 16, HQS2_STRLEN);
        sendMessage(packet2);
    }else if(type == PACKET_TYPE_ACKHOST){
        parent.connected = true;
        parent.sender.reset(new SenderThread(parent, 128, 256));
        parent.sender->startThread();
        parent.status.PushStatus(STATUS_CONNECTED, "Connected!", 30);
        parent.GetProc().sendChangeMessage();
    }else if(type == PACKET_TYPE_NAKHOST){
        parent.status.PushStatus(STATUS_NOCONNECT, "Server rejected connection!", 30);
    }else if(type == PACKET_TYPE_NAMECOLLHOST){
        parent.status.PushStatus(STATUS_NOCONNECT, "This server already has a session \"" + parent.sessionname + "\"!", 30);
    }else if(type == PACKET_TYPE_HOSTSTATS){
        parent.numclients = ((int32_t*)packet.getData())[2];
        parent.GetProc().sendChangeMessage();
    }else{
        parent.status.PushStatus(STATUS_BADAPARAM, "Bad packet type " + String((int)type) + " received!", 30);
    }
}
