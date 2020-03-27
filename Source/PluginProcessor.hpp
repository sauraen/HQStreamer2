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

enum class ModuleType : uint32_t { None, Session, Local, Client, COUNT };

class ModuleBackend;

class PluginProcessor : public AudioProcessor, public ChangeBroadcaster
{
public:
    PluginProcessor();
    ~PluginProcessor();

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void processBlock(AudioBuffer<float>& audio, MidiBuffer& midi) override;
    void releaseResources() override;

    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    const String getName() const override;
    double getTailLengthSeconds() const override;
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;
    bool hasEditor() const override;
    AudioProcessorEditor* createEditor() override;
    
    ModuleBackend *GetModule() { return mod; }
    ModuleType GetModuleType();
    void chooseModuleFromGUI(ModuleType t);

private:
    ModuleBackend *mod;
    bool p2p_needrun;
    double p2p_sampleRate;
    int p2p_samplesPerBlock;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginProcessor)
};
