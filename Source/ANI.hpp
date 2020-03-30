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

#define HQS2_PORT 21568
#define HQS2_STRLEN 512

#define PACKET_TYPE_PING 0
#define PACKET_TYPE_PINGRESPONSE 1
#define PACKET_TYPE_HOSTSTATS 2
#define PACKET_TYPE_AUDIO_ZEROS 10
#define PACKET_TYPE_AUDIO_FLOAT32 11
#define PACKET_TYPE_AUDIO_INT16 12
#define PACKET_TYPE_AUDIO_DPCM 13
#define PACKET_TYPE_REQJOIN 20
#define PACKET_TYPE_ACKJOIN 21
#define PACKET_TYPE_NAKJOIN 22
#define PACKET_TYPE_REQHOST 30
#define PACKET_TYPE_CHLHOST 31
#define PACKET_TYPE_RESPHOST 32
#define PACKET_TYPE_ACKHOST 33
#define PACKET_TYPE_NAKHOST 34
#define PACKET_TYPE_NAMECOLLHOST 35

#define STATUS_DEBUG 0
#define STATUS_PACKETERR 1
#define STATUS_BADSIZE 2
#define STATUS_BADAPARAM 3
#define STATUS_NOCONNECT 11
#define STATUS_DISCONNECTED 12
#define STATUS_CONNECTED 13
#define STATUS_WRONGCHN 20
#define STATUS_WRONGFS 21
#define STATUS_CLIPPING 25
#define STATUS_EVENT 30
#define STATUS_MISC 31
#define STATUS_INIT 99 
