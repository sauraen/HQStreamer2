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

class StatusStack {
public:
    StatusStack() {}
    ~StatusStack() {}
    
    void PushStatus(int priority, String message, int timeout = -1){
        const ScopedLock lock(mutex);
        int pos = 0;
        while(pos < data.size()){
            if(priority < data[pos].priority) break;
            else if(priority == data[pos].priority){
                data.getReference(pos).message = message;
                data.getReference(pos).timeout = timeout;
                return;
            }
            ++pos;
        }
        data.insert(pos, SSStatus(priority, message, timeout));
    }
    void ClearStatus(int priority){
        const ScopedLock lock(mutex);
        for(int pos=0; pos<data.size(); ++pos){
            if(data[pos].priority == priority){
                data.remove(pos);
                --pos;
            }
        }
    }
    String GetStatus() {
        const ScopedLock lock(mutex);
        if(data.size() == 0) return "OK";
        String ret = data[0].message;
        for(int pos=0; pos<data.size(); ++pos){
            if(data[pos].timeout > 0){
                --data.getReference(pos).timeout;
                if(data[pos].timeout == 0){
                    data.remove(pos);
                    --pos;
                }
            }
        }
        return ret;
    }
private:
    CriticalSection mutex;
    
    struct SSStatus {
        int priority;
        String message;
        int timeout;
        SSStatus() = default;
        SSStatus(int p, String m, int t) : priority(p), message(m), timeout(t) {}
    };
    Array<SSStatus> data;
};
