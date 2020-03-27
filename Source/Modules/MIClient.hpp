/*
  ==============================================================================

  This is an automatically generated GUI class created by the Projucer!

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Created with Projucer version: 5.4.7

  ------------------------------------------------------------------------------

  The Projucer is part of the JUCE library.
  Copyright (c) 2017 - ROLI Ltd.

  ==============================================================================
*/

#pragma once

//[Headers]     -- You can add your own extra header files here --
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
#include <JuceHeader.h>
#include "Modules/MBClient.hpp"
#include "BufferBar.hpp"
//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Projucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class MIClient  : public ModuleInterface,
                  public Button::Listener,
                  public ComboBox::Listener
{
public:
    //==============================================================================
    MIClient (MBClient &b);
    ~MIClient() override;

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
    virtual void changeListenerCallback(ChangeBroadcaster *source) override;
    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;
    void buttonClicked (Button* buttonThatWasClicked) override;
    void comboBoxChanged (ComboBox* comboBoxThatHasChanged) override;



private:
    //[UserVariables]   -- You can add your own custom variables in this section.
    MBClient &backend;

    std::unique_ptr<BufferBar> barBuf;
    //[/UserVariables]

    //==============================================================================
    std::unique_ptr<Label> lblServer;
    std::unique_ptr<TextEditor> txtServer;
    std::unique_ptr<Label> lblSession;
    std::unique_ptr<TextEditor> txtSession;
    std::unique_ptr<Label> lblStats;
    std::unique_ptr<TextButton> btnConnect;
    std::unique_ptr<Label> lblLatency;
    std::unique_ptr<ComboBox> cbxLatency;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MIClient)
};

//[EndFile] You can add extra defines here...
//[/EndFile]

