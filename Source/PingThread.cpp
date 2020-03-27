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

#include "PingThread.hpp"


PingThread::PingThread(HQSConnection& c) : Thread("PingThread"), conn(c){
    pingtime = -1.0f;
}
PingThread::~PingThread(){
    
}
    
void PingThread::run(){
    MemoryBlock data;
    //Don't send first packet quite immediately
    wait(10 + Random::getSystemRandom().nextInt(100));
    while(!threadShouldExit()){
        //Send ping packet
        data.setSize(20);
        int32_t* s32ptr = (int32_t*)data.getData();
        int64* s64ptr = (int64*)data.getData();
        s32ptr[0] = 20;
        s32ptr[1] = PACKET_TYPE_PING;
        starttime = Time::getCurrentTime();
        s64ptr[1] = starttime.toMilliseconds();
        s32ptr[4] = 0;
        if(!conn.sendMessage(data)){
            //Send failed
            pingtime = -1.0f;
        }else{
            //Wait for response
            bool gotResponse = wait(2000); //assuming ping won't take more than 2 seconds
            if(threadShouldExit()) return;
            if(gotResponse){
                //Calculate result
                int64 delta = endtime.toMilliseconds() - starttime.toMilliseconds();
                if(pingtime < 0){
                    pingtime = delta;
                }else{
                    pingtime = (0.1f * (float)delta) + (0.9f * pingtime);
                }
            }
        }
        conn.UpdatePingTime(pingtime);
        if(threadShouldExit()) return;
        //Wait for a second
        wait(985 + Random::getSystemRandom().nextInt(30));
    }
}
    
void PingThread::gotPingResponse(const MemoryBlock& message){
    int64* s64ptr = (int64*)message.getData();
    starttime = Time(s64ptr[1]);
    endtime = Time::getCurrentTime();
    notify();
}
