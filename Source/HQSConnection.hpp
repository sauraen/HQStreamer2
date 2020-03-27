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

class PingThread;

class HQSConnection : public InterprocessConnection {
public:
    HQSConnection(ModuleBackend& p);
    virtual ~HQSConnection() override;
    
    virtual void messageReceived(const MemoryBlock& message) override;
    virtual void connectionMade() override;
    virtual void connectionLost() override;
    
    virtual void VdPacketReceived(const MemoryBlock& packet, int32_t type) = 0;
    virtual void UpdatePingTime(float ping);
private:
    ModuleBackend& parent;
    std::unique_ptr<PingThread> pingthread;
};
