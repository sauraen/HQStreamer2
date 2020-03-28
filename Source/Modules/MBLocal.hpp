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
#include "SenderThread.hpp"

class HCLocal;

class MBLocal : public ModuleBackend, public InterprocessConnectionServer {
public:
    MBLocal(PluginProcessor &processor);
    virtual ~MBLocal() override;
    virtual ModuleType GetType() const override { return ModuleType::Local; }
    virtual int ComputeBufLen() const override { return 20000; }
    virtual bool IsSender() const override { return true; }
    
    virtual void processBlock(AudioBuffer<float> &audio) override;
    
    virtual InterprocessConnection *createConnectionObject() override;
    virtual void SendAudioPacket(const MemoryBlock &message) override;
    
    void Connect(String session);
    void Disconnect();
    bool IsConnected() { return connected; }
    String GetSessionName() { return sessionname; }
    int GetNumActiveClients();
    inline int GetTotalNumClients() { return conns.size(); }
private:
    friend class HCLocal;
    bool connected;
    String sessionname;
    
    OwnedArray<HCLocal> conns;
    ReadWriteLock conns_mutex;
    SenderThread *sender;
};

class HCLocal : public HQSConnection {
public:
    HCLocal(MBLocal &p);
    virtual ~HCLocal() override;
    
    virtual void connectionLost() override;
    virtual void VdPacketReceived(const MemoryBlock& packet, int32_t type) override;
    virtual void UpdatePingTime(float ping) override;
    
    inline bool IsValid() const { return valid; }
private:
    MBLocal &parent;
    bool valid;
};
