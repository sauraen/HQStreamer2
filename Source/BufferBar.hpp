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
#include "Module.hpp"

class BufferBar : public Component, public Timer{
public:
    BufferBar(ModuleBackend& p);
    virtual ~BufferBar() override;

    virtual void paint (Graphics&) override;
    virtual void resized() override;
    
    virtual void timerCallback() override;

private:
    ModuleBackend& parent;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BufferBar)
};
