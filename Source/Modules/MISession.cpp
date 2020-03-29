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

#include "MISession.hpp"


//[MiscUserDefs] You can add your own user definitions and misc code here...
//[/MiscUserDefs]

//==============================================================================
MISession::MISession (MBSession &b)
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

    lblPass.reset (new Label ("lblPass",
                              TRANS("Host passphrase:")));
    addAndMakeVisible (lblPass.get());
    lblPass->setFont (Font (15.00f, Font::plain).withTypefaceStyle ("Regular"));
    lblPass->setJustificationType (Justification::centredLeft);
    lblPass->setEditable (false, false, false);
    lblPass->setColour (TextEditor::textColourId, Colours::black);
    lblPass->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    lblPass->setBounds (0, 64, 120, 24);

    txtPass.reset (new TextEditor ("txtPass"));
    addAndMakeVisible (txtPass.get());
    txtPass->setMultiLine (false);
    txtPass->setReturnKeyStartsNewLine (false);
    txtPass->setReadOnly (false);
    txtPass->setScrollbarsShown (true);
    txtPass->setCaretVisible (true);
    txtPass->setPopupMenuEnabled (true);
    txtPass->setText (String());

    txtPass->setBounds (120, 64, 272, 24);

    lblSession.reset (new Label ("lblSession",
                                 TRANS("Session name/ID:")));
    addAndMakeVisible (lblSession.get());
    lblSession->setFont (Font (15.00f, Font::plain).withTypefaceStyle ("Regular"));
    lblSession->setJustificationType (Justification::centredLeft);
    lblSession->setEditable (false, false, false);
    lblSession->setColour (TextEditor::textColourId, Colours::black);
    lblSession->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    lblSession->setBounds (0, 88, 128, 24);

    txtSession.reset (new TextEditor ("txtSession"));
    addAndMakeVisible (txtSession.get());
    txtSession->setMultiLine (false);
    txtSession->setReturnKeyStartsNewLine (false);
    txtSession->setReadOnly (false);
    txtSession->setScrollbarsShown (true);
    txtSession->setCaretVisible (true);
    txtSession->setPopupMenuEnabled (true);
    txtSession->setText (String());

    txtSession->setBounds (128, 88, 264, 24);

    lblStats.reset (new Label ("lblStats",
                               TRANS("X channels, YYY kHz, ping ZZZ, H clients")));
    addAndMakeVisible (lblStats.get());
    lblStats->setFont (Font (15.00f, Font::plain).withTypefaceStyle ("Regular"));
    lblStats->setJustificationType (Justification::centredLeft);
    lblStats->setEditable (false, false, false);
    lblStats->setColour (TextEditor::textColourId, Colours::black);
    lblStats->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    lblStats->setBounds (0, 112, 232, 24);

    cbxFormat.reset (new ComboBox ("cbxFormat"));
    addAndMakeVisible (cbxFormat.get());
    cbxFormat->setEditableText (false);
    cbxFormat->setJustificationType (Justification::centredLeft);
    cbxFormat->setTextWhenNothingSelected (String());
    cbxFormat->setTextWhenNoChoicesAvailable (TRANS("(no choices)"));
    cbxFormat->addItem (TRANS("DPCM"), 1);
    cbxFormat->addItem (TRANS("int16"), 2);
    cbxFormat->addItem (TRANS("float32"), 3);
    cbxFormat->addListener (this);

    cbxFormat->setBounds (232, 112, 80, 24);

    btnConnect.reset (new TextButton ("btnConnect"));
    addAndMakeVisible (btnConnect.get());
    btnConnect->setButtonText (TRANS("Connect"));
    btnConnect->addListener (this);

    btnConnect->setBounds (312, 112, 80, 24);


    //[UserPreSize]

    barBuf->setTopLeftPosition(8, 144);
    txtPass->setPasswordCharacter('*');

    //[/UserPreSize]

    setSize (400, 248);


    //[Constructor] You can add your own custom stuff here..
    changeListenerCallback(nullptr);
    //[/Constructor]
}

MISession::~MISession()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    lblServer = nullptr;
    txtServer = nullptr;
    lblPass = nullptr;
    txtPass = nullptr;
    lblSession = nullptr;
    txtSession = nullptr;
    lblStats = nullptr;
    cbxFormat = nullptr;
    btnConnect = nullptr;


    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void MISession::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (Colour (0xff323e44));

    {
        int x = 8, y = 144, width = 384, height = 96;
        Colour fillColour = Colour (0xff2aa59c);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.fillRect (x, y, width, height);
    }

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void MISession::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void MISession::comboBoxChanged (ComboBox* comboBoxThatHasChanged)
{
    //[UsercomboBoxChanged_Pre]
    //[/UsercomboBoxChanged_Pre]

    if (comboBoxThatHasChanged == cbxFormat.get())
    {
        //[UserComboBoxCode_cbxFormat] -- add your combo box handling code here..
        backend.audiotype = ModuleBackend::StringToAudioType(cbxFormat->getText());
        //[/UserComboBoxCode_cbxFormat]
    }

    //[UsercomboBoxChanged_Post]
    //[/UsercomboBoxChanged_Post]
}

void MISession::buttonClicked (Button* buttonThatWasClicked)
{
    //[UserbuttonClicked_Pre]
    //[/UserbuttonClicked_Pre]

    if (buttonThatWasClicked == btnConnect.get())
    {
        //[UserButtonCode_btnConnect] -- add your button handler code here..
        if(backend.IsConnected()) backend.Disconnect();
        else backend.Connect(txtServer->getText(), txtPass->getText(), txtSession->getText());
        //[/UserButtonCode_btnConnect]
    }

    //[UserbuttonClicked_Post]
    //[/UserbuttonClicked_Post]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...

void MISession::changeListenerCallback(ChangeBroadcaster *source){
    ignoreUnused(source);
    txtServer->setEnabled(!backend.IsConnected());
    txtPass->setEnabled(!backend.IsConnected());
    txtSession->setEnabled(!backend.IsConnected());
    btnConnect->setButtonText(backend.IsConnected() ? "Disconnect" : "Connect");
    txtServer->setText(backend.GetServerName(), dontSendNotification);
    txtSession->setText(backend.GetSessionName(), dontSendNotification);
    cbxFormat->setText(ModuleBackend::AudioTypeToString(backend.audiotype));
    lblStats->setText(String(backend.buf ? backend.buf->NumChannels() : 0) + " chns, "
        + String(backend.fs) + " Hz, ping "
        + String(backend.pingtime, 1) + " ms, "
        + String(backend.GetNumClients()) + " clients", dontSendNotification);
}

//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Projucer information section --

    This is where the Projucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="MISession" componentName=""
                 parentClasses="public ModuleInterface" constructorParams="MBSession &amp;b"
                 variableInitialisers="ModuleInterface(b), backend(b)" snapPixels="8"
                 snapActive="1" snapShown="1" overlayOpacity="0.330" fixedSize="1"
                 initialWidth="400" initialHeight="248">
  <BACKGROUND backgroundColour="ff323e44">
    <RECT pos="8 144 384 96" fill="solid: ff2aa59c" hasStroke="0"/>
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
  <LABEL name="lblPass" id="1935bdd7dfb3e987" memberName="lblPass" virtualName=""
         explicitFocusOrder="0" pos="0 64 120 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Host passphrase:" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="15.0" kerning="0.0" bold="0" italic="0" justification="33"/>
  <TEXTEDITOR name="txtPass" id="b8689bec79aa615f" memberName="txtPass" virtualName=""
              explicitFocusOrder="0" pos="120 64 272 24" initialText="" multiline="0"
              retKeyStartsLine="0" readonly="0" scrollbars="1" caret="1" popupmenu="1"/>
  <LABEL name="lblSession" id="97708be0f814da45" memberName="lblSession"
         virtualName="" explicitFocusOrder="0" pos="0 88 128 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Session name/ID:" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="15.0" kerning="0.0" bold="0" italic="0" justification="33"/>
  <TEXTEDITOR name="txtSession" id="409609c8db9fe0d3" memberName="txtSession"
              virtualName="" explicitFocusOrder="0" pos="128 88 264 24" initialText=""
              multiline="0" retKeyStartsLine="0" readonly="0" scrollbars="1"
              caret="1" popupmenu="1"/>
  <LABEL name="lblStats" id="2cad943723b05d77" memberName="lblStats" virtualName=""
         explicitFocusOrder="0" pos="0 112 232 24" edTextCol="ff000000"
         edBkgCol="0" labelText="X channels, YYY kHz, ping ZZZ, H clients"
         editableSingleClick="0" editableDoubleClick="0" focusDiscardsChanges="0"
         fontname="Default font" fontsize="15.0" kerning="0.0" bold="0"
         italic="0" justification="33"/>
  <COMBOBOX name="cbxFormat" id="13b5ed9dadb9b1f0" memberName="cbxFormat"
            virtualName="" explicitFocusOrder="0" pos="232 112 80 24" editable="0"
            layout="33" items="DPCM&#10;int16&#10;float32" textWhenNonSelected=""
            textWhenNoItems="(no choices)"/>
  <TEXTBUTTON name="btnConnect" id="16362dc1c9d88ce6" memberName="btnConnect"
              virtualName="" explicitFocusOrder="0" pos="312 112 80 24" buttonText="Connect"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]

