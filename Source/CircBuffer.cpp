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

#include "CircBuffer.hpp"

CircBuffer::CircBuffer(bool sender, int ch, int spls)
        : sendermode(sender), channels(ch), length(spls) {
    data.setSize(4*channels*length);
    Reset();
}
CircBuffer::~CircBuffer(){
    ((void)0);
}
    
void CircBuffer::Reset(){
    data.fillWith(0);
    readhead = 0;
    writehead = sendermode ? 0 : length / 2;
}
    
float CircBuffer::Read(int c) const {
    return ((float*)data.getData())[readhead*channels + c];
}
void CircBuffer::ReadAdvance(){
    readhead = (readhead + 1) % length;
    if(readhead == writehead) Reset();
}
void CircBuffer::Write(int c, float d){
    ((float*)data.getData())[writehead*channels + c] = d;
}
void CircBuffer::WriteAdvance(){
    writehead = (writehead + 1) % length;
    if(writehead == readhead) Reset();
}
    
float CircBuffer::GetSample(int c, int s) const {
    return ((float*)data.getData())[s*channels + c];
}

int CircBuffer::NumSplsFilled() {
    return (writehead >= readhead) ? (writehead - readhead) : (length + writehead - readhead);
}
