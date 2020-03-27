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

#include "PluginEditor.hpp"
#include "PluginProcessor.hpp"

#include "ModuleChooser.hpp"
#include "Modules/MBSession.hpp"
#include "Modules/MISession.hpp"
#include "Modules/MBLocal.hpp"
#include "Modules/MILocal.hpp"
#include "Modules/MBClient.hpp"
#include "Modules/MIClient.hpp"


PluginEditor::PluginEditor(PluginProcessor& processor)
    : AudioProcessorEditor(&processor), proc(processor), lasttype(ModuleType::COUNT)
{
    changeModuleInterface();
    if(getWidth() <= 10 || getHeight() <= 10) setSize (800, 600);
    proc.addChangeListener(this);
}

PluginEditor::~PluginEditor()
{
    proc.removeChangeListener(this);
}

void PluginEditor::changeListenerCallback(ChangeBroadcaster *source){
    changeModuleInterface();
}
void PluginEditor::changeModuleInterface(){
    if(proc.GetModuleType() == lasttype) return;
    child = nullptr;
    switch(proc.GetModuleType()){
        case ModuleType::None:     child.reset(new ModuleChooser(proc)); break;
        case ModuleType::Session:  child.reset(new MISession (dynamic_cast<MBSession &>(*proc.GetModule()))); break;
        case ModuleType::Local:    child.reset(new MILocal   (dynamic_cast<MBLocal   &>(*proc.GetModule()))); break;
        case ModuleType::Client:   child.reset(new MIClient  (dynamic_cast<MBClient  &>(*proc.GetModule()))); break;
        default: return;
    }
    lasttype = proc.GetModuleType();
    addAndMakeVisible(child.get());
    setSize(child->getWidth(), child->getHeight());
}


void PluginEditor::paint(Graphics& g) { g.fillAll (Colours::magenta); }
void PluginEditor::resized() {}
