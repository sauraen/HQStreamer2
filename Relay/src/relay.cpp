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

#include "relay.hpp"
#include <cstring>

namespace ConfigFileHelpers {
	bool ReadProperty(String key, String &value){
		File configfile = File::getSpecialLocation(File::userApplicationDataDirectory)
				.getChildFile("HQStreamer2/hqs2relay.cfg");
		configfile.create();
		FileInputStream fis(configfile);
		if(fis.failedToOpen()){
			std::cout << "Could not open config file " << configfile.getFullPathName() << "!\n";
			return false;
		}
		while(!fis.isExhausted()){
			String line = fis.readNextLine();
			String k = fis.upToFirstOccurrenceOf("=", false, false);
			String v = fis.fromFirstOccurrenceOf("=", false, false);
			if(k == key){
				value = v;
				return true;
			}
		}
		return false;
	}
	bool WriteProperty(String key, String value){
		File configfile = File::getSpecialLocation(File::userApplicationDataDirectory)
				.getChildFile("HQStreamer2/hqs2relay.cfg");
		configfile.create();
		File configfileout = configfile.getSiblingFile("hqs2relay.cfg.temp");
		{
			FileInputStream fis(configfile);
			FileOutputStream fos(configfileout);
			if(fis.failedToOpen()){
				std::cout << "Could not open config file " << configfile.getFullPathName() << "!\n";
				return false;
			}
			if(fos.failedToOpen()){
				std::cout << "Could not open temporary config file!\n";
				return false;
			}
			bool wrotevalue = false;
			while(!fis.isExhausted()){
				String line = fis.readNextLine();
				String k = fis.upToFirstOccurrenceOf("=", false, false);
				String v = fis.fromFirstOccurrenceOf("=", false, false);
				if(k == key){
					fos.writeText(key + "=" + value + "\n", false, false, nullptr);
					wrotevalue = true;
				}else{
					fos.writeText(line + "\n", false, false, nullptr);
				}
			}
			if(!wrotevalue){
				fos.writeText(key + "=" + value + "\n", false, false, nullptr);
			}
		}
		configfile.deleteFile();
		configfileout.moveFileTo(configfile);
		return false;
	}
}

////////////////////////////////////////////////////////////////////////////////

HQS2Relay::HQS2Relay() {
    Random::getSystemRandom().setSeedRandomly();
	if(!ConfigFileHelpers::ReadProperty("passphrase_hashed", passphrase_hashed)){
		passphrase_hashed = "unset"; //not equal to the hash of any password
	}
    beginWaitingForSocket(HQS2_PORT);
    gc.reset(new ClientGCThread(*this));
    gc->startThread();
    stats.reset(new HostStatsThread(*this));
    stats->startThread();
    std::cout << "HQS2Relay running\n";
}
HQS2Relay::~HQS2Relay() {
    //
}

InterprocessConnection *HQS2Relay::createConnectionObject() {
    std::cout << "New connection opened\n";
    const ScopedWriteLock lock(mutex);
    HCRelay *conn = new HCRelay(*this);
    conns.add(conn);
    return conn;
}
void HQS2Relay::SendAudioPacket(const MemoryBlock &message, String session) {
    const ScopedReadLock lock(mutex);
    for(int c=0; c<conns.size(); ++c){
        if(conns[c]->mode == HCRelay::Mode::Client && conns[c]->sessionname == session){
            conns[c]->sendMessage(message);
        }
    }
}

void HQS2Relay::SetPassphrase(String pass) {
    String p = StorePassphrase(pass);
	if(p.isEmpty()) return;
    passphrase_hashed = p;
}
String HQS2Relay::StorePassphrase(String pass) {
	if(pass == ""){
        std::cout << "Won't set an empty passphrase!\n";
        return "";
    }
    if(pass.length() < 12){
        std::cout << "That's a really short passphrase, but okay!\n";
    }
	SHA256 sha(pass.toRawUTF8(), pass.getNumBytesAsUTF8());
	String pass_hashed = sha.toHexString();
	if(!ConfigFileHelpers::WriteProperty("passphrase_hashed", pass_hashed)){
		std::cout << "Error writing config file!\n";
	}
	return pass_hashed;
}

String HQS2Relay::GetSessionName(int s){
    const ScopedReadLock lock(mutex);
    if(s < 0 || s >= sessions.size()) return "Error";
    return sessions[s];
}
void HQS2Relay::KickSession(String sessionname){
    HCRelay *host = nullptr;
    {
        const ScopedReadLock lock(mutex);
        for(int c=0; c<conns.size(); ++c){
            if(conns[c]->mode == HCRelay::Mode::Host && conns[c]->sessionname == sessionname){
                host = conns[c];
                break;
            }
        }
    }
    if(!host){
        std::cout << "There is no session \"" << sessionname << "\"!\n";
        return;
    }
    host->disconnect();
}
int HQS2Relay::NumClients(String sessionname){
    const ScopedReadLock lock(mutex);
    int nc = 0;
    for(int c=0; c<conns.size(); ++c){
        if(conns[c]->mode == HCRelay::Mode::Client && conns[c]->sessionname == sessionname){
            ++nc;
        }
    }
    return nc;
}

void HQS2Relay::RunGC(){
    const ScopedWriteLock lock(mutex);
    for(int c=0; c<conns.size(); ++c){
        if(conns[c]->mode == HCRelay::Mode::Bad){
            std::cout << "Removing stale connection " << conns[c]->getConnectedHostName() << "\n";
			if(conns[c]->isConnected()) conns[c]->disconnect();
            conns.remove(c);
            --c;
        }
    }
}

void HQS2Relay::RunStats(){
    const ScopedReadLock lock(mutex);
    for(int c=0; c<conns.size(); ++c){
        if(conns[c]->mode == HCRelay::Mode::Host){
            int nc = NumClients(conns[c]->sessionname);
            MemoryBlock ret;
            ret.setSize(20);
            ret.fillWith(0);
            int32_t* s32ptr2 = (int32_t*)ret.getData();
            s32ptr2[0] = 20;
            s32ptr2[1] = PACKET_TYPE_HOSTSTATS;
            s32ptr2[2] = (int32_t)nc;
            if(!conns[c]->sendMessage(ret)){
                std::cout << "Could not send host stats to host " << conns[c]->getConnectedHostName() 
						<< " of session " << conns[c]->sessionname << "\n";
            }
        }
    }
}

void HQS2Relay::PrintSessionsInfo(){
    const ScopedReadLock lock(mutex);
    std::cout << sessions.size() << " sessions:\n";
    for(int s=0; s<sessions.size(); ++s){
        std::cout << s << ". " << sessions[s] << " (" << NumClients(sessions[s]) << " clients)\n";
    }
}

void HQS2Relay::PrintSessionInfo(String sessionname){
    const ScopedReadLock lock(mutex);
    if(!sessions.contains(sessionname)){
        std::cout << "There is no session \"" << sessionname << "\"\n";
        return;
    }
    std::cout << "Session " << sessionname << ": " << NumClients(sessionname) << " clients\n";
    HCRelay *host = nullptr;
    for(int c=0; c<conns.size(); ++c){
        if(conns[c]->sessionname != sessionname) continue;
        if(conns[c]->mode == HCRelay::Mode::Host){
            host = conns[c];
        }else if(conns[c]->mode == HCRelay::Mode::Client){
            std::cout << "Client " << c << ": " << conns[c]->getConnectedHostName() << "\n";
        }
    }
    if(!host){
        std::cout << "(No host?!)\n";
    }else{
        std::cout << "Host: " << host->getConnectedHostName() << "\n";
    }
}

////////////////////////////////////////////////////////////////////////////////

HCRelay::HCRelay(HQS2Relay &p) : InterprocessConnection(false, 0x32535148), parent(p) {
    mode = Mode::Init;
    challenge = Random::getSystemRandom().nextInt64();
    sessionname = "";
}
HCRelay::~HCRelay(){
    //
}

void HCRelay::connectionMade(){
    std::cout << "HCRelay::connectionMade()\n";
}
void HCRelay::connectionLost(){
    std::cout << "HCRelay::connectionLost()\n";
    if(mode == Mode::Host){
        const ScopedReadLock lock(parent.mutex);
		parent.sessions.removeString(sessionname);
        for(int c=0; c<parent.conns.size(); ++c){
            if(parent.conns[c]->mode == HCRelay::Mode::Client && parent.conns[c]->sessionname == sessionname){
                parent.conns[c]->disconnect();
            }
        }
    }
    mode = Mode::Bad;
    sessionname = "";
}
void HCRelay::messageReceived(const MemoryBlock& message){
    if(message.getSize() < 20 || message.getSize() > 1000000){
        std::cout << "Bad size packet! " << String((int)message.getSize()) << "\n";
		mode = Mode::Bad;
        return;
    }
    int32_t* s32ptr = (int32_t*)message.getData();
    int32_t size = s32ptr[0];
    if(size != (int32_t)message.getSize()){
        std::cout << "Bad size field " + String((int)size) << "\n";
		mode = Mode::Bad;
        return;
    }
    int32_t type = s32ptr[1];
    //std::cout << "Packet type " << (int)type << " received\n";
    MemoryBlock ret(20, true);
    int32_t* s32ptr2 = (int32_t*)ret.getData();
    int64_t* s64ptr2 = (int64_t*)ret.getData();
    s32ptr2[0] = 20;
    if(type == PACKET_TYPE_PING){
        //Send a copy back
        s32ptr2[1] = PACKET_TYPE_PINGRESPONSE;
        s32ptr2[2] = s32ptr[2];
        s32ptr2[3] = s32ptr[3];
    }else if(type == PACKET_TYPE_REQHOST){
        //Send challenge
        challenge = Random::getSystemRandom().nextInt64();
        s32ptr2[1] = PACKET_TYPE_CHLHOST;
        s64ptr2[1] = challenge;
    }else if(type == PACKET_TYPE_RESPHOST){
        if(message.getSize() != 40 + HQS2_STRLEN){
            std::cout << "Bad size RESPHOST! " << String((int)message.getSize()) << "\n";
			mode = Mode::Bad;
            return;
        }
		String temp = parent.passphrase_hashed + String(challenge);
		SHA256 sha(temp.toRawUTF8(), temp.getNumBytesAsUTF8());
        if(memcmp(sha.getRawData().begin(), (char*)message.getData() + 8, 32) == 0){
            sessionname = String(CharPointer_UTF8((char*)message.getData() + 40), HQS2_STRLEN);
			if(sessionname.isEmpty()){
				s32ptr2[1] = PACKET_TYPE_NAKHOST;
				std::cout << "Host tried to start session with blank name!\n";
				mode = Mode::Bad;
			}else{
				const ScopedWriteLock lock(parent.mutex);
				if(parent.sessions.contains(sessionname)){
					s32ptr2[1] = PACKET_TYPE_NAMECOLLHOST;
					std::cout << "Name collision! " << sessionname << "\n";
					mode = Mode::Bad;
				}else{
					s32ptr2[1] = PACKET_TYPE_ACKHOST;
					std::cout << "New session started, name " << sessionname << "\n";
					const ScopedWriteLock lock(parent.mutex);
					parent.sessions.add(sessionname);
					mode = Mode::Host;
				}
			}
        }else{
            s32ptr2[1] = PACKET_TYPE_NAKHOST;
            std::cout << "Wrong passphrase!\n";
            mode = Mode::Bad;
        }
    }else if(type == PACKET_TYPE_REQJOIN){
        if(message.getSize() != 8 + HQS2_STRLEN){
            std::cout << "Bad size REQJOIN! " << String((int)message.getSize()) << "\n";
			mode = Mode::Bad;
            return;
        }
        sessionname = String(CharPointer_UTF8((char*)message.getData() + 8), HQS2_STRLEN);
        const ScopedReadLock lock(parent.mutex);
        if(!parent.sessions.contains(sessionname)){
            s32ptr2[1] = PACKET_TYPE_NAKJOIN;
            std::cout << "Client tried to join nonexistent session " << sessionname << "!\n";
            mode = Mode::Bad;
        }else{
            s32ptr2[1] = PACKET_TYPE_ACKJOIN;
            std::cout << "New client on session " << sessionname << "\n";
            mode = Mode::Client;
        }
    }else if(type == PACKET_TYPE_AUDIO_ZEROS 
          || type == PACKET_TYPE_AUDIO_FLOAT32
          || type == PACKET_TYPE_AUDIO_INT16
          || type == PACKET_TYPE_AUDIO_DPCM){
        if(mode != Mode::Host){
            std::cout << "Connection other than a host is sending audio!\n";
			mode = Mode::Bad;
            return;
        }
        int32_t nchannels = s32ptr[2], nsamples = s32ptr[3], fs = s32ptr[4];
        if(nchannels <= 0 || nchannels > 128 || nsamples < 16 || nsamples > 100000 || fs <= 0 || fs >= 1000000){
            std::cout << "Bad audio params in packet!\n";
			mode = Mode::Bad;
            return;
        }
        if(type == PACKET_TYPE_AUDIO_ZEROS && message.getSize() != 20 ||
                type == PACKET_TYPE_AUDIO_FLOAT32 && message.getSize() != 20+4*nsamples*nchannels ||
                type == PACKET_TYPE_AUDIO_INT16 && message.getSize() != 20+2*nsamples*nchannels ||
                type == PACKET_TYPE_AUDIO_DPCM && message.getSize() > 20+2*nsamples*nchannels){
            std::cout << "Bad audio packet size " << (int)message.getSize() << " for type " << (int)type << "!\n";
			mode = Mode::Bad;
            return;
        }
        parent.SendAudioPacket(message, sessionname);
        return;
    }else{
        std::cout << "Bad packet type " << String((int)type) << " received!\n";
		mode = Mode::Bad;
        return;
    }
    if(!sendMessage(ret)){
        std::cout << "Could not send response packet\n";
    }
}

////////////////////////////////////////////////////////////////////////////////

void ClientGCThread::run(){
    while(!threadShouldExit()){
        parent.RunGC();
        wait(507);
    }
}

void HostStatsThread::run(){
    while(!threadShouldExit()){
        parent.RunStats();
        wait(1023);
    }
}
