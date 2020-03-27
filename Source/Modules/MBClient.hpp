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

#include "Module.hpp"
#include "HQSConnection.hpp"

class HCClient;

class MBClient : public ModuleBackend {
public:
    MBClient(PluginProcessor &processor);
    virtual ~MBClient() override;
    virtual ModuleType GetType() const override { return ModuleType::Client; }
    virtual int ComputeBufLen() const override { return (int)((float)fs*latencyratio)*2; }
    virtual bool IsSender() const override { return false; }
    
    virtual void processBlock(AudioBuffer<float> &audio) override;
    
    String GetLatency();
    void SetLatency(String lstr);
    
    void Connect(String host, String session);
    void Disconnect();
    bool IsConnected() { return connected; }
    String GetHostName() { return hostname; }
    String GetSessionName() { return sessionname; }
    
    int32_t rec_channels, rec_fs;
private:
    friend class HCClient;
    float latencyratio;
    bool connected;
    std::unique_ptr<HCClient> conn;
    String hostname, sessionname;
};

class HCClient : public HQSConnection {
public:
    HCClient(MBClient &p);
    virtual ~HCClient() override;
    
    virtual void connectionLost() override;
    
    virtual void VdPacketReceived(const MemoryBlock& packet, int32_t type) override;
private:
    MBClient &parent;
};
