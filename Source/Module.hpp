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

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.hpp"
#include "StatusStack.hpp"
#include "CircBuffer.hpp"

#define HQS2_PORT 21568
#define HQS2_STRLEN 512

#define PACKET_TYPE_PING 0
#define PACKET_TYPE_PINGRESPONSE 1
#define PACKET_TYPE_AUDIO_ZEROS 10
#define PACKET_TYPE_AUDIO_FLOAT32 11
#define PACKET_TYPE_AUDIO_INT16 12
#define PACKET_TYPE_AUDIO_DPCM 13
#define PACKET_TYPE_REQJOIN 20
#define PACKET_TYPE_ACKJOIN 21
#define PACKET_TYPE_NAKJOIN 22

#define STATUS_DEBUG 0
#define STATUS_PACKETERR 1
#define STATUS_BADSIZE 2
#define STATUS_BADAPARAM 3
#define STATUS_NOCONNECT 11
#define STATUS_WRONGCHN 20
#define STATUS_WRONGFS 21
#define STATUS_EVENT 30
#define STATUS_MISC 31
#define STATUS_DISCONNECTED 50
#define STATUS_CONNECTED 51
#define STATUS_INIT 99

class ModuleInterface;

class ModuleBackend {
public:
    ModuleBackend(PluginProcessor &processor);
    virtual ~ModuleBackend();
    
    virtual ModuleType GetType() const = 0;
    virtual int ComputeBufLen() const = 0;
    virtual bool IsSender() const = 0;
    
    inline PluginProcessor &GetProc() { return proc; }
    inline String GetStatus() { return status.GetStatus(); }
    inline void RegisterInterface(ModuleInterface *i) { interface = i; }
    
    virtual void prepareToPlay(double sampleRate, int samplesPerBlock);
    virtual void processBlock(AudioBuffer<float> &audio);
    virtual void releaseResources();
    
    virtual void SendAudioPacket(const MemoryBlock &message);
    
    static String AudioTypeToString(int32_t at);
    static int32_t StringToAudioType(String s);
    
    ReadWriteLock mutex;
    StatusStack status;
    
    std::unique_ptr<CircBuffer> buf;
    int32_t audiotype;
    int32_t fs;
    float pingtime;
protected:
    PluginProcessor &proc;
    ModuleInterface *interface;
};

class BufferBar;

class ModuleInterface : public Component, ChangeListener, private Timer {
public:
    ModuleInterface(ModuleBackend &b);
    virtual ~ModuleInterface() override;
    
    virtual void changeListenerCallback(ChangeBroadcaster *source) override;
    virtual void resized() override;
    virtual void timerCallback() override;
protected:
    std::unique_ptr<BufferBar> barBuf;
private:
    ModuleBackend &backend;
    std::unique_ptr<Label> lblStatus;
};

#include "BufferBar.hpp"
