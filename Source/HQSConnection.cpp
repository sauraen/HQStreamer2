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

#include "HQSConnection.hpp"
#include "PingThread.hpp"

HQSConnection::HQSConnection(ModuleBackend& p) : InterprocessConnection(false, 0x32535148), parent(p){
    
}
HQSConnection::~HQSConnection(){
    if(pingthread != nullptr){
        pingthread->stopThread(10);
        pingthread.reset(nullptr);
    }
}
    
void HQSConnection::connectionMade(){
    parent.status.PushStatus(STATUS_CONNECTED, "Connection established!", 30);
    pingthread.reset(new PingThread(*this));
    pingthread->startThread();
}
void HQSConnection::connectionLost(){
    parent.status.PushStatus(STATUS_DISCONNECTED, "Connection lost!", 30);
    if(pingthread != nullptr){
        pingthread->stopThread(10);
        pingthread.reset(nullptr);
    }
}

void HQSConnection::messageReceived(const MemoryBlock& message){
    if(message.getSize() < 20 || message.getSize() > 1000000){
        parent.status.PushStatus(STATUS_PACKETERR, "Bad size packet! " + String((int)message.getSize()), 30);
        return;
    }
    int32_t* s32ptr = (int32_t*)message.getData();
    int32_t size = s32ptr[0];
    if(size != (int32_t)message.getSize()){
        parent.status.PushStatus(STATUS_PACKETERR, "Bad size field " + String((int)size), 30);
        return;
    }
    int32_t type = s32ptr[1];
    if(type == PACKET_TYPE_PING){
        //Send a copy back
        MemoryBlock ret;
        ret.setSize(20);
        int32_t* s32ptr2 = (int32_t*)ret.getData();
        s32ptr2[0] = 20;
        s32ptr2[1] = PACKET_TYPE_PINGRESPONSE;
        s32ptr2[2] = s32ptr[2];
        s32ptr2[3] = s32ptr[3];
        s32ptr2[4] = 0;
        if(!sendMessage(ret)){
            parent.status.PushStatus(STATUS_MISC, "Could not send PINGRESPONSE", 30);
        }
    }else if(type == PACKET_TYPE_PINGRESPONSE){
        //Send it to the ping thread
        pingthread->gotPingResponse(message);
    }else{
        VdPacketReceived(message, type);
    }
}

void HQSConnection::UpdatePingTime(float ping){
    parent.pingtime = ping;
    parent.GetProc().sendChangeMessage();
}
