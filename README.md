HQStreamer2 - Stream audio between DAWs across the internet
Copyright (C) 2020 Sauraen, <sauraen@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU Affero General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.

## Compatibility

Windows 7/8/10 x64: VST3, Standalone, Relay
Mac OS 10.7 (Lion) - 10.14 (Mojave): VST3, AU, AUv3, Standalone, Relay
Linux: Standalone, Relay

The standalone plugins run on their own without a DAW, and can be connected to any system audio I/O devices. "Relay" is the component of HQStreamer2 which runs on a server and routes audio from hosts to many clients each.

A VST2 version of HQStreamer2 can technically be built, but due to Steinberg licensing restrictions cannot be distributed.

Due to technical limitations present in MacOS 10.15 (Catalina) designed to enforce Apple licensing policy, experimental third-party apps like HQStreamer2 *will never be* compatible with MacOS 10.15 (Catalina) or later.


## Download

Downloads are in the `Downloads` folder above. 

On Mac, packages such as apps and plugins (like `HQStreamer2.vst3`) are actually folders, so you can't directly download the entire folder from GitHub. The easiest workaround is to simply download the entire repo from the green `Clone or download` button above, and choose `Download ZIP`. Inside the ZIP file (Downloads/macos_x64), your Mac OS Finder will treat the packages as individual files.

If you are a developer, please clone (or fork) the repo. Pull requests are welcome!


## Installation

### VST3

Move or copy HQStreamer2.vst3 into your system (or user) 64-bit VST3 folder. Create this folder if it doesn't exist.
* Windows: `C:\Program Files\Common Files\VST3`
* Mac OS system VST3 folder (for all users): `/Library/Audio/Plug-Ins/VST3`
* Mac OS user VST3 folder: `/Users/your_name/Library/Audio/Plug-Ins/VST3` (Note that the folder `/Users/your_name/Library` DOES already exist, it's hidden by default.)
If your DAW does not automatically detect HQStreamer2 on startup, tell it to rescan for new/modified plugins. If your DAW still does not detect the plugin on Windows, and you're sure your DAW supports VST3 plugins (Pro Tools and older DAWs will not), try installing [the Visual C++ Redistributable 2017 from Microsoft](https://aka.ms/vs/16/release/vc_redist.x64.exe).

### AU (Mac OS only)

Move or copy HQStreamer2.component into your system or user AU components folder. Some DAWs do not support the user AU components folder, so use the system one if possible.
* System (for all users): `/Library/Audio/Plug-Ins/Components`
* User: `/Users/your_name/Library/Audio/Plug-Ins/Components` (Note that the folder `/Users/your_name/Library` DOES already exist, it's hidden by default.)
If your DAW does not automatically detect HQStreamer2 on startup, tell it to rescan for new/modified plugins. It has been reported that some AU plugins are not detected until the computer is restarted, though we have not found this to be the case for HQStreamer2.

Note that HQStreamer2 is by nature not "sandbox safe" (it opens network connections). Apple requires plugins to be "sandbox safe" to be loaded in newer versions of GarageBand.

### AUv3 (Mac OS only)

The developer's host of choice (Reaper) does not support AUv3 plugins, so we have no idea if the AUv3 plugin we put up (HQStreamer2.appex) works or not.

Typically AUv3 plugins are installed in the same `Components` folder as AU plugins above.

### Standalone

Simply run the executable (HQStreamer2.exe on Windows or HQStreamer2.app on Mac), it can be in any folder.


### Relay

Open port 21568 for bidirectional TCP. Run hqs2relay in the background (Linux/Mac: `./hqs2relay > relay_cout.txt &`).


## HQStreamer2 Usage

### Common (all plugin versions or standalone)




## TODOs

* Socket based live config for relay
* Documentation
* AU versions
