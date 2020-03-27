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

#include "PluginProcessor.hpp"
#include "PluginEditor.hpp"
#include "Module.hpp"

#include "Modules/MBSession.hpp"
#include "Modules/MBLocal.hpp"
#include "Modules/MBClient.hpp"

//==============================================================================
PluginProcessor::PluginProcessor() : AudioProcessor(BusesProperties()
       .withInput  ("Input",  AudioChannelSet::stereo(), true)
       .withOutput ("Output", AudioChannelSet::stereo(), true)),
       mod(nullptr), p2p_needrun(false)
{
    //
}

PluginProcessor::~PluginProcessor()
{
    if(mod != nullptr){
        delete mod; mod = nullptr;
    }
}


void PluginProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    if(mod != nullptr){
        mod->prepareToPlay(sampleRate, samplesPerBlock);
    }else{
        //Call prepareToPlay once the module is created
        p2p_needrun = true;
        p2p_sampleRate = sampleRate;
        p2p_samplesPerBlock = samplesPerBlock;
    }
}

void PluginProcessor::processBlock(AudioBuffer<float>& audio, MidiBuffer& midi)
{
    ScopedNoDenormals noDenormals;
    ignoreUnused(midi);
    if(mod != nullptr){
        mod->processBlock(audio);
        return;
    }
    audio.clear();
}

void PluginProcessor::releaseResources()
{
    if(mod != nullptr) mod->releaseResources();
}


ModuleType PluginProcessor::GetModuleType() {
    return mod == nullptr ? ModuleType::None : mod->GetType();
}

void PluginProcessor::chooseModuleFromGUI(ModuleType t){
    jassert(mod == nullptr);
    if(t == ModuleType::Session){
        mod = new MBSession(*this);
    }else if(t == ModuleType::Local){
        mod = new MBLocal(*this);
    }else if(t == ModuleType::Client){
        mod = new MBClient(*this);
    }else{
        jassertfalse;
        return;
    }
    if(p2p_needrun){
        mod->prepareToPlay(p2p_sampleRate, p2p_samplesPerBlock);
        p2p_needrun = false;
    }
    sendChangeMessage();
}

void PluginProcessor::getStateInformation (MemoryBlock& destData) { destData.setSize(0); }
void PluginProcessor::setStateInformation (const void* data, int sizeInBytes) { ignoreUnused(data); jassert(sizeInBytes == 0); }
const String PluginProcessor::getName() const { return JucePlugin_Name; }
double PluginProcessor::getTailLengthSeconds() const { return 0.0; }
bool PluginProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const { ignoreUnused(layouts); return true; }
bool PluginProcessor::acceptsMidi() const { return false; }
bool PluginProcessor::producesMidi() const { return false; }
bool PluginProcessor::isMidiEffect() const { return false; }
int PluginProcessor::getNumPrograms() { return 1; }
int PluginProcessor::getCurrentProgram() { return 0; }
void PluginProcessor::setCurrentProgram (int index) { ignoreUnused(index); }
const String PluginProcessor::getProgramName (int index) { ignoreUnused(index); return "(None)"; }
void PluginProcessor::changeProgramName (int index, const String& newName) { ignoreUnused(index); ignoreUnused(newName); }
bool PluginProcessor::hasEditor() const { return true; }
AudioProcessorEditor* PluginProcessor::createEditor() { return new PluginEditor (*this); }
AudioProcessor* JUCE_CALLTYPE createPluginFilter() { return new PluginProcessor(); }
