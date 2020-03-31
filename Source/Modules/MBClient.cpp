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

#include "MBClient.hpp"

MBClient::MBClient(PluginProcessor &processor) : ModuleBackend(processor), connected(false) {
    if(!cfgfile.ReadProperty("hostname", hostname)) hostname = "";
    latencyratio = 0.5f;
    rec_channels = 0;
    rec_fs = 0;
    volume = 0.25f;
}
MBClient::~MBClient() {
    Disconnect();
}

void MBClient::processBlock(AudioBuffer<float> &audio) {
    ModuleBackend::processBlock(audio);
    if(!connected){
        audio.clear();
        return;
    }
    const ScopedReadLock lock(mutex);
    for(int s=0; s<audio.getNumSamples(); ++s){
        for(int c=0; c<audio.getNumChannels(); ++c){
            audio.getWritePointer(c)[s] = volume * buf->Read(c);
        }
        buf->ReadAdvance();
    }
}

String MBClient::GetLatency(){
    if(latencyratio >= 1.0f){
        return String(latencyratio) + "s";
    }else{
        return String(latencyratio*1000.0f) + "ms";
    }
}
void MBClient::SetLatency(String lstr){
    float r = -1.0f;
    if(lstr.endsWith("ms")){
        r = 0.001f * lstr.dropLastCharacters(2).getFloatValue();
    }else if(lstr.endsWith("s")){
        r = lstr.dropLastCharacters(1).getFloatValue();
    }
    if(r <= 0.0f || r >= 5.0f){
        status.PushStatus(STATUS_MISC, "Latency internal error!", 30);
        r = 0.5f;
    }
    latencyratio = r;
}

void MBClient::Connect(String host, String session){
    hostname = host;
    sessionname = session;
    conn.reset(new HCClient(*this));
    if(!conn->connectToSocket(host, HQS2_PORT, 1000)){
        status.PushStatus(STATUS_NOCONNECT, "Could not connect to host");
        return;
    }
    MemoryBlock packet;
    packet.setSize(8+HQS2_STRLEN);
    packet.fillWith(0);
    int32_t *s32ptr = (int32_t*)packet.getData();
    s32ptr[0] = 8 + HQS2_STRLEN;
    s32ptr[1] = PACKET_TYPE_REQJOIN;
    session.copyToUTF8((char*)packet.getData() + 8, HQS2_STRLEN);
    if(!conn->sendMessage(packet)){
        status.PushStatus(STATUS_NOCONNECT, "Could not send join request to host");
        return;
    }
    status.ClearStatus(STATUS_NOCONNECT);
    status.PushStatus(STATUS_EVENT, "Sent join request...", 30);
    const ScopedWriteLock lock(mutex);
    buf->Reset();
}
void MBClient::Disconnect(){
    status.PushStatus(STATUS_DISCONNECTED, "Disconnected", 30);
    connected = false;
    if(conn){
        conn->disconnect();
    }
    proc.sendChangeMessage();
}


HCClient::HCClient(MBClient &p) : HQSConnection(p), parent(p) {
    //
}
HCClient::~HCClient() {
    //
}

void HCClient::connectionLost(){
    HQSConnection::connectionLost();
    parent.connected = false;
    parent.GetProc().sendChangeMessage();
    parent.status.PushStatus(STATUS_DISCONNECTED, "Connection lost", 30);
}

#define CHECK_PACKET_BOUNDS(i) if((i) >= packet.getSize()) { parent.status.PushStatus(STATUS_BADSIZE, "DPCM ran off end of packet!"); return; } ((void)0)

void HCClient::VdPacketReceived(const MemoryBlock& packet, int32_t type) {
    const ScopedReadLock lock(parent.mutex);
    if(!parent.buf){
        parent.status.PushStatus(STATUS_MISC, "No buffer (internal error)!", 30);
        return;
    }
    int32_t nchannels = -1, nsamples = -1, fs;
    int c, s;
    int32_t* s32ptr = (int32_t*)packet.getData();
    if(type == PACKET_TYPE_ACKJOIN){
        cfgfile.WriteProperty("hostname", parent.hostname);
        parent.status.PushStatus(STATUS_EVENT, "Connected to host!", 30);
        parent.connected = true;
        parent.GetProc().sendChangeMessage();
    }else if(type == PACKET_TYPE_NAKJOIN){
        parent.status.PushStatus(STATUS_NOCONNECT, "Host rejected connection!", 30);
    }else if(type == PACKET_TYPE_AUDIO_ZEROS 
          || type == PACKET_TYPE_AUDIO_FLOAT32
          || type == PACKET_TYPE_AUDIO_INT16
          || type == PACKET_TYPE_AUDIO_DPCM){
        nchannels = s32ptr[2];
        nsamples = s32ptr[3];
        fs = s32ptr[4];
        if(nchannels <= 0 || nchannels > 128 || nsamples < 16 || nsamples > 100000 || fs <= 0 || fs >= 1000000){
            parent.status.PushStatus(STATUS_BADAPARAM, "Bad audio params in packet!", 30);
            return;
        }
        parent.rec_channels = nchannels;
        parent.rec_fs = fs;
        parent.audiotype = type;
        if(nchannels != parent.buf->NumChannels()){
            parent.status.PushStatus(STATUS_WRONGCHN, "Host is sending " + String((int)nchannels) 
                + " channels but your DAW is giving this plugin " + String((int)parent.buf->NumChannels()) + " channels!");
            return;
        }else{
            parent.status.ClearStatus(STATUS_WRONGCHN);
        }
        if(fs != parent.fs){
            parent.status.PushStatus(STATUS_WRONGFS, "Host is sending " + String((int)fs) 
                + " Hz but your DAW is set to " + String((int)parent.fs) + " Hz!");
            return;
        }else{
            parent.status.ClearStatus(STATUS_WRONGFS);
        }
    }else{
        parent.status.PushStatus(STATUS_BADAPARAM, "Bad packet type " + String((int)type) + " received!", 30);
        return;
    }
    if(type == PACKET_TYPE_AUDIO_ZEROS){
        if(packet.getSize() != 20){
            parent.status.PushStatus(STATUS_BADSIZE, "Bad packet size " + String((int)packet.getSize()) + " for ZEROS type!");
            return;
        }else{
            parent.status.ClearStatus(STATUS_BADSIZE);
        }
        for(s=0; s<nsamples; ++s){
            for(c=0; c<nchannels; ++c){
                parent.buf->Write(c, 0.0f);
            }
            parent.buf->WriteAdvance();
        }
    }else if(type == PACKET_TYPE_AUDIO_FLOAT32){
        if(packet.getSize() != 20+4*nsamples*nchannels){
            parent.status.PushStatus(STATUS_BADSIZE, "Bad packet size " + String((int)packet.getSize()) + " for FLOAT32 type!");
            return;
        }else{
            parent.status.ClearStatus(STATUS_BADSIZE);
        }
        float* fptr = (float*)packet.getData();
        for(s=0; s<nsamples; ++s){
            for(c=0; c<nchannels; ++c){
                parent.buf->Write(c, fptr[5+c*nsamples+s]);
            }
            parent.buf->WriteAdvance();
        }
    }else if(type == PACKET_TYPE_AUDIO_INT16){
        if(packet.getSize() != 20+2*nsamples*nchannels){
            parent.status.PushStatus(STATUS_BADSIZE, "Bad packet size " + String((int)packet.getSize()) + " for INT16 type!");
            return;
        }else{
            parent.status.ClearStatus(STATUS_BADSIZE);
        }
        int16* iptr = (int16*)packet.getData();
        for(s=0; s<nsamples; ++s){
            for(c=0; c<nchannels; ++c){
                float temp = (float)(iptr[10+c*nsamples+s]);
                temp /= 32768.0f;
                parent.buf->Write(c, temp);
            }
            parent.buf->WriteAdvance();
        }
    }else if(type == PACKET_TYPE_AUDIO_DPCM){
        if(packet.getSize() > 20+2*nsamples*nchannels){
            parent.status.PushStatus(STATUS_BADSIZE, "Bad packet size " + String((int)packet.getSize()) + " for DPCM type!");
            return;
        }
        uint8* bptr = (uint8*)packet.getData();
        uint32 i = 20; //index of bptr
        uint8 b; int16 value, delta; uint16 tempu16; float tempf;
        MemoryBlock tempdata(4*nchannels*nsamples);
        float* d = (float*)tempdata.getData();
        for(c=0; c<nchannels; ++c){
            //Get initial value
            CHECK_PACKET_BOUNDS(i);
            CHECK_PACKET_BOUNDS(i+1);
            value = (int16)((uint16)bptr[i] | ((uint16)bptr[i+1] << 8));
            i += 2;
            tempf = (float)value;
            tempf /= 32768.0f;
            d[c*nsamples+0] = tempf;
            //Get DPCM samples
            for(s=1; s<nsamples; ++s){
                CHECK_PACKET_BOUNDS(i);
                b = bptr[i++];
                if(b & 0x80){
                    //Two-byte
                    CHECK_PACKET_BOUNDS(i);
                    tempu16 = (uint16)(b & 0x7F) | ((uint16)(bptr[i++]) << 7);
                    tempu16 |= (tempu16 & 0x4000) << 1; //Sign bit
                    delta = (int16)tempu16;
                }else{
                    //One-byte
                    b |= (b & 0x40) << 1; //Sign bit
                    delta = (int16)((int8)b); //Cast to s8 then s16
                }
                value += delta;
                tempf = (float)value;
                tempf /= 32768.0f;
                d[c*nsamples+s] = tempf;
            }
        }
        parent.status.ClearStatus(STATUS_BADSIZE);
        for(s=0; s<nsamples; ++s){
            for(c=0; c<nchannels; ++c){
                parent.buf->Write(c, d[c*nsamples+s]);
            }
            parent.buf->WriteAdvance();
        }
    }
}
