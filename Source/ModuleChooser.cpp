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

//[Headers] You can add your own extra header files here...
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
//[/Headers]

#include "ModuleChooser.hpp"


//[MiscUserDefs] You can add your own user definitions and misc code here...
//[/MiscUserDefs]

//==============================================================================
ModuleChooser::ModuleChooser (PluginProcessor &processor)
    : proc(processor)
{
    //[Constructor_pre] You can add your own custom stuff here..
    //[/Constructor_pre]

    btnSession.reset (new TextButton ("btnSession"));
    addAndMakeVisible (btnSession.get());
    btnSession->setButtonText (TRANS("Host a session on a server"));
    btnSession->addListener (this);

    btnSession->setBounds (8, 40, 280, 24);

    lblTitle.reset (new Label ("lblTitle",
                               TRANS("HQStreamer2 - Copyright (C) 2020 Sauraen\n"
                               "Affero GPL 3 licensed, see LICENSE.txt")));
    addAndMakeVisible (lblTitle.get());
    lblTitle->setFont (Font (15.00f, Font::plain).withTypefaceStyle ("Regular"));
    lblTitle->setJustificationType (Justification::centredLeft);
    lblTitle->setEditable (false, false, false);
    lblTitle->setColour (TextEditor::textColourId, Colours::black);
    lblTitle->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    lblTitle->setBounds (0, 0, 296, 40);

    btnLocal.reset (new TextButton ("btnLocal"));
    addAndMakeVisible (btnLocal.get());
    btnLocal->setButtonText (TRANS("Host a session locally"));
    btnLocal->addListener (this);

    btnLocal->setBounds (8, 72, 280, 24);

    btnJoin.reset (new TextButton ("btnJoin"));
    addAndMakeVisible (btnJoin.get());
    btnJoin->setButtonText (TRANS("Join a session"));
    btnJoin->addListener (this);

    btnJoin->setBounds (8, 104, 280, 24);


    //[UserPreSize]
    //[/UserPreSize]

    setSize (296, 136);


    //[Constructor] You can add your own custom stuff here..
    //[/Constructor]
}

ModuleChooser::~ModuleChooser()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    btnSession = nullptr;
    lblTitle = nullptr;
    btnLocal = nullptr;
    btnJoin = nullptr;


    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void ModuleChooser::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (Colour (0xff323e44));

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void ModuleChooser::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void ModuleChooser::buttonClicked (Button* buttonThatWasClicked)
{
    //[UserbuttonClicked_Pre]
    //[/UserbuttonClicked_Pre]

    if (buttonThatWasClicked == btnSession.get())
    {
        //[UserButtonCode_btnSession] -- add your button handler code here..
        proc.chooseModuleFromGUI(ModuleType::Session);
        //[/UserButtonCode_btnSession]
    }
    else if (buttonThatWasClicked == btnLocal.get())
    {
        //[UserButtonCode_btnLocal] -- add your button handler code here..
        proc.chooseModuleFromGUI(ModuleType::Local);
        //[/UserButtonCode_btnLocal]
    }
    else if (buttonThatWasClicked == btnJoin.get())
    {
        //[UserButtonCode_btnJoin] -- add your button handler code here..
        proc.chooseModuleFromGUI(ModuleType::Client);
        //[/UserButtonCode_btnJoin]
    }

    //[UserbuttonClicked_Post]
    //[/UserbuttonClicked_Post]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...
//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Projucer information section --

    This is where the Projucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="ModuleChooser" componentName=""
                 parentClasses="public Component" constructorParams="PluginProcessor &amp;processor"
                 variableInitialisers="proc(processor)" snapPixels="8" snapActive="1"
                 snapShown="1" overlayOpacity="0.330" fixedSize="1" initialWidth="296"
                 initialHeight="136">
  <BACKGROUND backgroundColour="ff323e44"/>
  <TEXTBUTTON name="btnSession" id="217d17ad69d94acd" memberName="btnSession"
              virtualName="" explicitFocusOrder="0" pos="8 40 280 24" buttonText="Host a session on a server"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <LABEL name="lblTitle" id="400faebb01b0195f" memberName="lblTitle" virtualName=""
         explicitFocusOrder="0" pos="0 0 296 40" edTextCol="ff000000"
         edBkgCol="0" labelText="HQStreamer2 - Copyright (C) 2020 Sauraen&#10;Affero GPL 3 licensed, see LICENSE.txt"
         editableSingleClick="0" editableDoubleClick="0" focusDiscardsChanges="0"
         fontname="Default font" fontsize="15.0" kerning="0.0" bold="0"
         italic="0" justification="33"/>
  <TEXTBUTTON name="btnLocal" id="cd58c83e03c3c376" memberName="btnLocal" virtualName=""
              explicitFocusOrder="0" pos="8 72 280 24" buttonText="Host a session locally"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="btnJoin" id="1841f3e58d4f0d94" memberName="btnJoin" virtualName=""
              explicitFocusOrder="0" pos="8 104 280 24" buttonText="Join a session"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]

