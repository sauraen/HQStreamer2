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

#include "MIClient.hpp"


//[MiscUserDefs] You can add your own user definitions and misc code here...
//[/MiscUserDefs]

//==============================================================================
MIClient::MIClient (MBClient &b)
    : ModuleInterface(b), backend(b)
{
    //[Constructor_pre] You can add your own custom stuff here..
    //[/Constructor_pre]

    lblServer.reset (new Label ("lblServer",
                                TRANS("Server IP/URL:")));
    addAndMakeVisible (lblServer.get());
    lblServer->setFont (Font (15.00f, Font::plain).withTypefaceStyle ("Regular"));
    lblServer->setJustificationType (Justification::centredLeft);
    lblServer->setEditable (false, false, false);
    lblServer->setColour (TextEditor::textColourId, Colours::black);
    lblServer->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    lblServer->setBounds (0, 40, 104, 24);

    txtServer.reset (new TextEditor ("txtServer"));
    addAndMakeVisible (txtServer.get());
    txtServer->setMultiLine (false);
    txtServer->setReturnKeyStartsNewLine (false);
    txtServer->setReadOnly (false);
    txtServer->setScrollbarsShown (true);
    txtServer->setCaretVisible (true);
    txtServer->setPopupMenuEnabled (true);
    txtServer->setText (String());

    txtServer->setBounds (104, 40, 288, 24);

    lblSession.reset (new Label ("lblSession",
                                 TRANS("Session name/ID:")));
    addAndMakeVisible (lblSession.get());
    lblSession->setFont (Font (15.00f, Font::plain).withTypefaceStyle ("Regular"));
    lblSession->setJustificationType (Justification::centredLeft);
    lblSession->setEditable (false, false, false);
    lblSession->setColour (TextEditor::textColourId, Colours::black);
    lblSession->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    lblSession->setBounds (0, 64, 128, 24);

    txtSession.reset (new TextEditor ("txtSession"));
    addAndMakeVisible (txtSession.get());
    txtSession->setMultiLine (false);
    txtSession->setReturnKeyStartsNewLine (false);
    txtSession->setReadOnly (false);
    txtSession->setScrollbarsShown (true);
    txtSession->setCaretVisible (true);
    txtSession->setPopupMenuEnabled (true);
    txtSession->setText (String());

    txtSession->setBounds (128, 64, 264, 24);

    lblStats.reset (new Label ("lblStats",
                               TRANS("X channels, YYY kHz, format, ping ZZZ")));
    addAndMakeVisible (lblStats.get());
    lblStats->setFont (Font (15.00f, Font::plain).withTypefaceStyle ("Regular"));
    lblStats->setJustificationType (Justification::centredLeft);
    lblStats->setEditable (false, false, false);
    lblStats->setColour (TextEditor::textColourId, Colours::black);
    lblStats->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    lblStats->setBounds (0, 88, 280, 24);

    btnConnect.reset (new TextButton ("btnConnect"));
    addAndMakeVisible (btnConnect.get());
    btnConnect->setButtonText (TRANS("Connect"));
    btnConnect->addListener (this);

    btnConnect->setBounds (280, 88, 112, 24);

    lblLatency.reset (new Label ("lblLatency",
                                 TRANS("Latency (incoming buffer size):")));
    addAndMakeVisible (lblLatency.get());
    lblLatency->setFont (Font (15.00f, Font::plain).withTypefaceStyle ("Regular"));
    lblLatency->setJustificationType (Justification::centredLeft);
    lblLatency->setEditable (false, false, false);
    lblLatency->setColour (TextEditor::textColourId, Colours::black);
    lblLatency->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    lblLatency->setBounds (0, 112, 208, 24);

    cbxLatency.reset (new ComboBox ("cbxLatency"));
    addAndMakeVisible (cbxLatency.get());
    cbxLatency->setEditableText (false);
    cbxLatency->setJustificationType (Justification::centredLeft);
    cbxLatency->setTextWhenNothingSelected (String());
    cbxLatency->setTextWhenNoChoicesAvailable (TRANS("(no choices)"));
    cbxLatency->addItem (TRANS("50ms"), 1);
    cbxLatency->addItem (TRANS("100ms"), 2);
    cbxLatency->addItem (TRANS("200ms"), 3);
    cbxLatency->addItem (TRANS("500ms"), 4);
    cbxLatency->addItem (TRANS("1s"), 5);
    cbxLatency->addItem (TRANS("2s"), 6);
    cbxLatency->addListener (this);

    cbxLatency->setBounds (208, 112, 88, 24);


    //[UserPreSize]

    barBuf->setTopLeftPosition(8, 144);

    //[/UserPreSize]

    setSize (400, 248);


    //[Constructor] You can add your own custom stuff here..
    changeListenerCallback(nullptr);
    //[/Constructor]
}

MIClient::~MIClient()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    lblServer = nullptr;
    txtServer = nullptr;
    lblSession = nullptr;
    txtSession = nullptr;
    lblStats = nullptr;
    btnConnect = nullptr;
    lblLatency = nullptr;
    cbxLatency = nullptr;


    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void MIClient::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (Colour (0xff323e44));

    {
        int x = 8, y = 144, width = 384, height = 96;
        Colour fillColour = Colour (0xff1d61d2);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.fillRect (x, y, width, height);
    }

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void MIClient::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void MIClient::buttonClicked (Button* buttonThatWasClicked)
{
    //[UserbuttonClicked_Pre]
    //[/UserbuttonClicked_Pre]

    if (buttonThatWasClicked == btnConnect.get())
    {
        //[UserButtonCode_btnConnect] -- add your button handler code here..
        if(backend.IsConnected()) backend.Disconnect();
        else backend.Connect(txtServer->getText(), txtSession->getText());
        //[/UserButtonCode_btnConnect]
    }

    //[UserbuttonClicked_Post]
    //[/UserbuttonClicked_Post]
}

void MIClient::comboBoxChanged (ComboBox* comboBoxThatHasChanged)
{
    //[UsercomboBoxChanged_Pre]
    //[/UsercomboBoxChanged_Pre]

    if (comboBoxThatHasChanged == cbxLatency.get())
    {
        //[UserComboBoxCode_cbxLatency] -- add your combo box handling code here..
        backend.SetLatency(cbxLatency->getText());
        //[/UserComboBoxCode_cbxLatency]
    }

    //[UsercomboBoxChanged_Post]
    //[/UsercomboBoxChanged_Post]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...

void MIClient::changeListenerCallback(ChangeBroadcaster *source){
    ignoreUnused(source);
    txtServer->setEnabled(!backend.IsConnected());
    txtSession->setEnabled(!backend.IsConnected());
    btnConnect->setButtonText(backend.IsConnected() ? "Disconnect" : "Connect");
    txtServer->setText(backend.GetHostName(), dontSendNotification);
    txtSession->setText(backend.GetSessionName(), dontSendNotification);
    cbxLatency->setText(backend.GetLatency(), dontSendNotification);
    lblStats->setText(String(backend.rec_channels) + " channels, " + String(backend.rec_fs) + " Hz, "
        + ModuleBackend::AudioTypeToString(backend.audiotype) + ", ping "
        + String(backend.pingtime, 1) + " ms", dontSendNotification);
}

//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Projucer information section --

    This is where the Projucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="MIClient" componentName=""
                 parentClasses="public ModuleInterface" constructorParams="MBClient &amp;b"
                 variableInitialisers="ModuleInterface(b), backend(b)" snapPixels="8"
                 snapActive="1" snapShown="1" overlayOpacity="0.330" fixedSize="1"
                 initialWidth="400" initialHeight="248">
  <BACKGROUND backgroundColour="ff323e44">
    <RECT pos="8 144 384 96" fill="solid: ff1d61d2" hasStroke="0"/>
  </BACKGROUND>
  <LABEL name="lblServer" id="18490dd44ac9c420" memberName="lblServer"
         virtualName="" explicitFocusOrder="0" pos="0 40 104 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Server IP/URL:" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="15.0" kerning="0.0" bold="0" italic="0" justification="33"/>
  <TEXTEDITOR name="txtServer" id="c9c7b76e7fc26e9b" memberName="txtServer"
              virtualName="" explicitFocusOrder="0" pos="104 40 288 24" initialText=""
              multiline="0" retKeyStartsLine="0" readonly="0" scrollbars="1"
              caret="1" popupmenu="1"/>
  <LABEL name="lblSession" id="97708be0f814da45" memberName="lblSession"
         virtualName="" explicitFocusOrder="0" pos="0 64 128 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Session name/ID:" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="15.0" kerning="0.0" bold="0" italic="0" justification="33"/>
  <TEXTEDITOR name="txtSession" id="409609c8db9fe0d3" memberName="txtSession"
              virtualName="" explicitFocusOrder="0" pos="128 64 264 24" initialText=""
              multiline="0" retKeyStartsLine="0" readonly="0" scrollbars="1"
              caret="1" popupmenu="1"/>
  <LABEL name="lblStats" id="2cad943723b05d77" memberName="lblStats" virtualName=""
         explicitFocusOrder="0" pos="0 88 280 24" edTextCol="ff000000"
         edBkgCol="0" labelText="X channels, YYY kHz, format, ping ZZZ"
         editableSingleClick="0" editableDoubleClick="0" focusDiscardsChanges="0"
         fontname="Default font" fontsize="15.0" kerning="0.0" bold="0"
         italic="0" justification="33"/>
  <TEXTBUTTON name="btnConnect" id="16362dc1c9d88ce6" memberName="btnConnect"
              virtualName="" explicitFocusOrder="0" pos="280 88 112 24" buttonText="Connect"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <LABEL name="lblLatency" id="9169aef853d3ccfc" memberName="lblLatency"
         virtualName="" explicitFocusOrder="0" pos="0 112 208 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Latency (incoming buffer size):" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="15.0" kerning="0.0" bold="0" italic="0" justification="33"/>
  <COMBOBOX name="cbxLatency" id="45f50e6c66e2bab" memberName="cbxLatency"
            virtualName="" explicitFocusOrder="0" pos="208 112 88 24" editable="0"
            layout="33" items="50ms&#10;100ms&#10;200ms&#10;500ms&#10;1s&#10;2s"
            textWhenNonSelected="" textWhenNoItems="(no choices)"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]

