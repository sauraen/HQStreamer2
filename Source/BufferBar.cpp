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

#include "BufferBar.hpp"
#include "CircBuffer.hpp"

BufferBar::BufferBar(ModuleBackend& p) : parent(p) {
    startTimerHz(30);
}

BufferBar::~BufferBar(){
    
}

void BufferBar::paint (Graphics& g){
    int width = getWidth(), height = getHeight();
    if(width <= 0 || height <= 0) return;
    
    g.fillAll(Colours::black);
    if(!parent.buf) return;
    const ScopedReadLock lock(parent.mutex);
    
    int length = parent.buf->Length();
    if(length <= 0) return;
    int readhead = parent.buf->GetReadHead();
    int fill = parent.buf->NumSplsFilled();
    
    float proportionFilled = (float)fill / (float)length;
    float fillend = (float)width * proportionFilled;
    float okayness = 1.0f - (2.0f * abs(proportionFilled - 0.5f));
    Colour okaynessColor = Colour::fromHSV(0.33f*okayness, 1.0f, 1.0f, 255);
    
    //Draw background
    g.setColour(okaynessColor.darker());
    g.fillRect(0, 0, (int)fillend, height);

    //Draw audio
    g.setColour(okaynessColor.brighter(0.7f));
    int nc = parent.buf->NumChannels();
    float rowheight = height / (float)nc;
    for(int c=0; c<nc; ++c){
        int s = readhead;
        float subpixel = 0.0f;
        float delta = (float)width / (float)length; //Pixels per sample
        float lastsubpixel = 0.0f;
        float sample, maxsample = 0.0f;
        for(; subpixel < fillend; subpixel += delta){
            sample = abs(parent.buf->GetSample(c, s));
            s = (s+1) % length;
            if(sample >= 1.0f) sample = 1.0f;
            if(sample > maxsample) maxsample = sample;
            if(floor(lastsubpixel) != floor(subpixel)){
                //Done with a pixel
                g.fillRect((int)floor(lastsubpixel), 
                           (int)(floor((rowheight * (1.0f - maxsample)) + (c*rowheight))), 
                           1, 
                           (int)(ceil(rowheight * maxsample)));
                maxsample = 0.0f;
            }
            lastsubpixel = subpixel;
        }
    }
    
    //Draw playback head
    g.setColour(okaynessColor);
    g.fillRect((int)floor(fillend), 0, 2, height);
    
}

void BufferBar::resized(){
    
}
void BufferBar::timerCallback(){
    repaint();
}
