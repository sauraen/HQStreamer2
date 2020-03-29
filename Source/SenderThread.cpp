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

#include "SenderThread.hpp"

SenderThread::SenderThread(ModuleBackend& p, int sampleGap, int samplesPerPacket) 
        : Thread("SenderThread"), parent(p), 
        samplegap(sampleGap), samplesperpacket(samplesPerPacket) {
    //
}
SenderThread::~SenderThread(){
    //
}

void SenderThread::run(){
    std::cout << "SenderThread starting\n";
    int c, s;
    while(!threadShouldExit()){
        do{
            if(parent.buf == nullptr) break;
            //See if there's enough data in the buffer to send
            if(parent.buf->NumSplsFilled() < samplegap + samplesperpacket) break;
            int nsamples = samplesperpacket;
            int nchannels = parent.buf->NumChannels();
            //Copy buffer from critical one to local copy, for multiple use
            OwnedArray<MemoryBlock> localbuffer;
            for(c=0; c<nchannels; ++c){
                localbuffer.add(new MemoryBlock(4*nsamples));
            }
            {
                const ScopedReadLock lock(parent.mutex);
                for(s=0; s<nsamples; ++s){
                    for(c=0; c<nchannels; ++c){
                        ((float*)(localbuffer[c]->getData()))[s] = parent.buf->Read(c);
                    }
                    parent.buf->ReadAdvance();
                }
            }
            //Create packet and write header
            int32 packetlen = 20;
            int32 audioType = parent.audiotype;
            if(audioType == PACKET_TYPE_AUDIO_FLOAT32){
                packetlen += (4 * nsamples * nchannels);
            }else if(audioType == PACKET_TYPE_AUDIO_INT16){
                packetlen += (2 * nsamples * nchannels);
            }else if(audioType == PACKET_TYPE_AUDIO_DPCM){
                packetlen += (2 * nsamples * nchannels); //but it will shrink
            }else{
                jassertfalse;
                return;
            }
            MemoryBlock packet(packetlen);
            int32* s32p = (int32*)packet.getData();
            s32p[0] = packetlen;
            s32p[1] = audioType;
            s32p[2] = nchannels;
            s32p[3] = nsamples;
            s32p[4] = parent.fs;
            //Check for all zeros
            bool flag = true;
            const float epsilon = 1.0f / 32768.0f;
            for(c=0; c<nchannels; ++c){
                float *channelData = (float*)localbuffer[c]->getData();
                for(s=0; s<nsamples; ++s){
                    if(channelData[s] >= epsilon || channelData[s] <= (0.0f-epsilon)){
                        flag = false;
                        break;
                    }
                }
                if(!flag) break;
            }
            if(flag){
                //Use PACKET_TYPE_AUDIO_ZEROS
                s32p[0] = 20;
                s32p[1] = PACKET_TYPE_AUDIO_ZEROS;
                packet.setSize(20);
            }else{
                //Write data
                bool clipped = false;
                if(audioType == PACKET_TYPE_AUDIO_FLOAT32){
                    float* fp = (float*)packet.getData();
                    uint32 i = 5; //index of fp
                    for(c=0; c<nchannels; ++c){
                        float *channelData = (float*)localbuffer[c]->getData();
                        for(s=0; s<nsamples; ++s){
                            fp[i++] = channelData[s];
                        }
                    }
                }else if(audioType == PACKET_TYPE_AUDIO_INT16){
                    int16* s16p = (int16*)packet.getData();
                    uint32 i = 10; //index of s16p
                    for(c=0; c<nchannels; ++c){
                        float *channelData = (float*)localbuffer[c]->getData();
                        for(s=0; s<nsamples; ++s){
                            s16p[i++] = encodeFloat(channelData[s], clipped);
                        }
                    }
                }else if(audioType == PACKET_TYPE_AUDIO_DPCM){
                    uint8* u8p = (uint8*)packet.getData();
                    uint32 i = 20; //index of u8p
                    int16 value, lastsentvalue, delta;
                    for(c=0; c<nchannels; ++c){
                        float *channelData = (float*)localbuffer[c]->getData();
                        //Write initial value
                        value = encodeFloat(channelData[0], clipped);
                        u8p[i++] = value & 0x00FF;
                        u8p[i++] = (value & 0xFF00) >> 8;
                        lastsentvalue = value;
                        //Write DPCM samples
                        for(s=1; s<nsamples; ++s){
                            value = encodeFloat(channelData[s], clipped);
                            delta = value - lastsentvalue;
                            if(delta >= 0x4000) delta = 0x3FFF;
                            if(delta < -0x4000) delta = -0x4000;
                            if(delta < 0x0040 && delta >= -0x0040){
                                //One-byte
                                u8p[i++] = delta & 0x7F;
                            }else{
                                //Two-byte
                                u8p[i++] = 0x80 | (delta & 0x7F);
                                u8p[i++] = ((delta >> 7) & 0xFF);
                            }
                            lastsentvalue += delta; //in case it wasn't enough
                        }
                    }
                    //Change length
                    s32p[0] = i;
                    packet.setSize(i);
                }else{
                    jassertfalse;
                    return;
                }
                if(clipped){
                    parent.status.PushStatus(STATUS_CLIPPING, "Clipping detected! This will sound distorted to the clients!", 30);
                }
            }
            //Send
            parent.SendAudioPacket(packet);
        }while(true);
        //Done
        if(threadShouldExit()) return;
        wait(5); //Wait for 5ms and try again
    }
    std::cout << "SenderThread exiting\n";
}

int16 SenderThread::encodeFloat(float f, bool &clipped){
    f *= 32768.0f;
    if(f > 32767.0f) { f = 32767.0f; clipped = true; }
    if(f < -32768.0f) { f = -32768.0f; clipped = true; }
    return (int16)f;
}
