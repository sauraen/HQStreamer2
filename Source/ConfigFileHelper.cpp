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

#include "ConfigFileHelper.hpp"

ConfigFileHelper::ConfigFileHelper(String exename){
    configfile = File::getSpecialLocation(File::userApplicationDataDirectory)
				.getChildFile("HQStreamer2/" + exename + ".cfg");
}
ConfigFileHelper::~ConfigFileHelper() {}
    
bool ConfigFileHelper::ReadProperty(String key, String &value){
    configfile.create();
    FileInputStream fis(configfile);
    if(fis.failedToOpen()){
        std::cout << "Could not open config file " << configfile.getFullPathName() << "!\n";
        return false;
    }
    while(!fis.isExhausted()){
        String line = fis.readNextLine();
        String k = line.upToFirstOccurrenceOf("=", false, false);
        String v = line.fromFirstOccurrenceOf("=", false, false);
        if(k == key){
            value = v;
            return true;
        }
    }
    return false;
}

bool ConfigFileHelper::WriteProperty(String key, String value){
    configfile.create();
    File configfileout = configfile.getSiblingFile("hqs2relay.cfg.temp");
    {
        FileInputStream fis(configfile);
        FileOutputStream fos(configfileout);
        if(fis.failedToOpen()){
            std::cout << "Could not open config file " << configfile.getFullPathName() << "!\n";
            return false;
        }
        if(fos.failedToOpen()){
            std::cout << "Could not open temporary config file!\n";
            return false;
        }
        bool wrotevalue = false;
        while(!fis.isExhausted()){
            String line = fis.readNextLine();
            String k = line.upToFirstOccurrenceOf("=", false, false);
            String v = line.fromFirstOccurrenceOf("=", false, false);
            if(k == key){
                fos.writeText(key + "=" + value + "\n", false, false, nullptr);
                wrotevalue = true;
            }else{
                fos.writeText(line + "\n", false, false, nullptr);
            }
        }
        if(!wrotevalue){
            fos.writeText(key + "=" + value + "\n", false, false, nullptr);
        }
    }
    configfile.deleteFile();
    configfileout.moveFileTo(configfile);
    return true;
}
