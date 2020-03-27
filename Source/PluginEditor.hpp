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
#include "PluginProcessor.hpp"

class PluginEditor : public AudioProcessorEditor, public ChangeListener
{
public:
    PluginEditor(PluginProcessor &processor);
    virtual ~PluginEditor() override;

    void paint (Graphics&) override;
    void resized() override;

    void changeListenerCallback(ChangeBroadcaster *source) override;
    
private:
    PluginProcessor& proc;
    std::unique_ptr<Component> child;
    ModuleType lasttype;
    
    void changeModuleInterface();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginEditor)
};
