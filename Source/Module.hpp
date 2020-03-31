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
#include "ANI.hpp"
#include "ConfigFileHelper.hpp"

extern ConfigFileHelper cfgfile;

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
    float volume;
protected:
    PluginProcessor &proc;
    ModuleInterface *interface;
};

class BufferBar;

class ModuleInterface : public Component, ChangeListener, Slider::Listener, private Timer {
public:
    ModuleInterface(ModuleBackend &b);
    virtual ~ModuleInterface() override;
    
    virtual void resized() override;
    virtual void changeListenerCallback(ChangeBroadcaster *source) override;
    virtual void sliderValueChanged(Slider *sliderThatWasChanged) override;
    virtual void timerCallback() override;
    
    void SetOtherGUIBottom(int y);
private:
    ModuleBackend &backend;
    std::unique_ptr<Label> lblStatus;
    std::unique_ptr<Slider> sldVolume;
    std::unique_ptr<BufferBar> barBuf;
};

#include "BufferBar.hpp"
