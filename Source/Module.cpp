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

ConfigFileHelper cfgfile("HQStreamer2");

ModuleBackend::ModuleBackend(PluginProcessor &processor)
        : buf(nullptr), proc(processor) {
    interface = nullptr;
    status.PushStatus(STATUS_INIT, "Initializing...", 5);
    fs = 44100;
    pingtime = -1.0f;
    audiotype = -1;
    volume = 1.0f;
}
    
ModuleBackend::~ModuleBackend(){
    //
}

void ModuleBackend::prepareToPlay(double sampleRate, int samplesPerBlock) {
    std::cout << "ModuleBackend::prepareToPlay()\n";
    fs = (int32_t)sampleRate;
    ignoreUnused(samplesPerBlock);
}

void ModuleBackend::processBlock(AudioBuffer<float> &audio) {
    std::cout << "ModuleBackend::processBlock()\n";
    int l = ComputeBufLen();
    if(!buf || buf->NumChannels() != audio.getNumChannels() || buf->Length() != l){
        const ScopedWriteLock lock(mutex);
        buf.reset(new CircBuffer(IsSender(), audio.getNumChannels(), l));
    }
}
void ModuleBackend::releaseResources() {
    std::cout << "ModuleBackend::releaseResources()\n";
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
int32_t ModuleBackend::StringToAudioType(String s){
    if(s == "(none)") return -1;
    if(s == "zeros") return PACKET_TYPE_AUDIO_ZEROS;
    if(s == "float32") return PACKET_TYPE_AUDIO_FLOAT32;
    if(s == "int16") return PACKET_TYPE_AUDIO_INT16;
    if(s == "DPCM") return PACKET_TYPE_AUDIO_DPCM;
    return -2;
}


///////////////////////////////////////////////////////////////////////

ModuleInterface::ModuleInterface(ModuleBackend &b) : backend(b) {
    backend.GetProc().addChangeListener(this);
    
    lblStatus.reset(new Label("lblStatus", backend.GetStatus()));
    addAndMakeVisible(lblStatus.get());
    lblStatus->setBounds(0, 0, 800, 40);
    
    sldVolume.reset(new Slider(Slider::LinearHorizontal, Slider::TextBoxLeft));
    sldVolume->setRange(-60.0, 12.0, 0.0);
    sldVolume->setDoubleClickReturnValue(true, 0.0);
    sldVolume->setValue(std::log10(backend.volume) * 20.0f);
    sldVolume->setTextBoxStyle(Slider::TextBoxLeft, true, 56, 20);
    sldVolume->setTextValueSuffix("dB");
    sldVolume->setNumDecimalPlacesToDisplay(1);
    addAndMakeVisible(sldVolume.get());
    sldVolume->addListener(this);
    sldVolume->setBounds(8, 96, 384, 24);
    
    barBuf.reset(new BufferBar(backend));
    addAndMakeVisible(barBuf.get());
    barBuf->setBounds(8, 96, 384, 96);
    
    startTimer(33);
}
ModuleInterface::~ModuleInterface() {
    backend.GetProc().removeChangeListener(this);
}

void ModuleInterface::resized(){
    lblStatus->setBounds(0, 0, getWidth(), 40);
    sldVolume->setSize(getWidth() - 16, 24);
    barBuf->setSize(getWidth() - 16, 96);
}
void ModuleInterface::changeListenerCallback(ChangeBroadcaster *source){
    ignoreUnused(source);
}
void ModuleInterface::sliderValueChanged(Slider *sliderThatWasChanged){
    ignoreUnused(sliderThatWasChanged);
    backend.volume = std::pow(10.0f, ((float)sldVolume->getValue() * 0.05f));
}
void ModuleInterface::timerCallback(){
    lblStatus->setText(backend.GetStatus(), dontSendNotification);
}

void ModuleInterface::SetOtherGUIBottom(int y){
    sldVolume->setTopLeftPosition(8, y);
    barBuf->setTopLeftPosition(8, 32+y);
}