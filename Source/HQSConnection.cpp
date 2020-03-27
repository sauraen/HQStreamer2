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
    /*
    if(type == PACKET_TYPE_INIT){
        statusMsg = "Got INIT";
        uint32 role = u32ptr[2];
        uint32 sr = u32ptr[3];
        if(isserver){
            if((isreceiver ^ (role == 1)) || sr != samplerate){
                connectionLost();
                NativeMessageBox::showMessageBoxAsync(AlertWindow::WarningIcon, "HQStreamer", "Client rejected connection (role " + String(role) + ")!\n"
                    + "Client wants sample rate of " + String(sr) + " but this DAW is at " + String(samplerate) + "--one of you will have to change your project sample rate.");
                return;
            }
            updategui = true;
        }else if(isclient){
            isreceiver = (role == 1);
            issender = !isreceiver;
            //Send a response
            MemoryBlock ret;
            ret.setSize(16);
            uint32* u32ptr2 = (uint32*)ret.getData();
            u32ptr2[0] = 16;
            u32ptr2[1] = PACKET_TYPE_INIT;
            u32ptr2[2] = isreceiver ? 0 : 1;
            u32ptr2[3] = samplerate;
            if(!sendPacket(ret)){
                statusMsg = "Could not send INIT";
            }else{
                statusMsg = "Sent INIT";
            }
            if(sr != samplerate){
                connectionLost();
                NativeMessageBox::showMessageBoxAsync(AlertWindow::WarningIcon, "HQStreamer", "Rejecting connection (role " + String(role) + ")!\n"
                    + "Server wants sample rate of " + String(sr) + " but this DAW is at " + String(samplerate) + "--one of you will have to change your project sample rate.");
                return;
            }
            updategui = true;
        }else{
            return;
        }
        for(c=0; c<getNumInputChannels(); c++){
            buffers[c]->resetRun(issender);
        }
        if(issender){
            senderthread = new SenderThread(*this, samplesperblock << 1, samplesperblock + (samplesperblock >> 1));
            senderthread->startThread();
        }
    }else{
        statusMsg = "Bad type field " + String((int)type);
        return;
    }
    */
}

void HQSConnection::UpdatePingTime(float ping){
    parent.pingtime = ping;
    parent.GetProc().sendChangeMessage();
}
