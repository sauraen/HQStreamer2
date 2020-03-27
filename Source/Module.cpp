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

#include "Module.hpp"

ModuleBackend::ModuleBackend(PluginProcessor &processor)
        : buf(nullptr), proc(processor) {
    interface = nullptr;
    status.PushStatus(STATUS_INIT, "Initializing...", 5);
    fs = 44100;
    pingtime = -1.0f;
    audiotype = -1;
}
    
ModuleBackend::~ModuleBackend(){
    //
}

void ModuleBackend::prepareToPlay(double sampleRate, int samplesPerBlock) {
    ignoreUnused(sampleRate);
    ignoreUnused(samplesPerBlock);
}

void ModuleBackend::processBlock(AudioBuffer<float> &audio) {
    ignoreUnused(audio);
}
void ModuleBackend::releaseResources() {
    //
}

void ModuleBackend::SendAudioPacket(const MemoryBlock &message){
    ignoreUnused(message); //Overridden in MBLocal (broadcast) and MBSession (unicast)
}

String ModuleBackend::AudioTypeToString(int32_t at){
    switch(at){
        case -1: return "(none)";
        case PACKET_TYPE_AUDIO_ZEROS: return "zeros";
        case PACKET_TYPE_AUDIO_FLOAT32: return "float32";
        case PACKET_TYPE_AUDIO_INT16: return "int16";
        case PACKET_TYPE_AUDIO_DPCM: return "DPCM";
        default: return "Error";
    }
}


///////////////////////////////////////////////////////////////////////

ModuleInterface::ModuleInterface(ModuleBackend &b) : backend(b) {
    backend.GetProc().addChangeListener(this);
    
    lblStatus.reset(new Label("lblStatus", backend.GetStatus()));
    addAndMakeVisible(lblStatus.get());
    lblStatus->setBounds(0, 0, 800, 40);
    
    startTimer(33);
}
ModuleInterface::~ModuleInterface() {
    backend.GetProc().removeChangeListener(this);
}

void ModuleInterface::changeListenerCallback(ChangeBroadcaster *source){
    ((void)0);
}
void ModuleInterface::resized(){
    lblStatus->setBounds(0, 0, getWidth(), 40);
}
void ModuleInterface::timerCallback(){
    lblStatus->setText(backend.GetStatus(), dontSendNotification);
}
