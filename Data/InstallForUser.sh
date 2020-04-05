#!/usr/bin/env sh

if [ $(uname -s) != "Darwin" ]; then
	echo "This installer is for Mac only"
	exit 1
fi

echo $0

p=`dirname "$0"`
cd "$p"
pwd

INSTPREFIX=${HOME}
SUDO=" "

$SUDO mkdir -p $INSTPREFIX/Applications
$SUDO mkdir -p $INSTPREFIX/Library/Audio/Plug-Ins/VST3/
$SUDO mkdir -p $INSTPREFIX/Library/Audio/Plug-Ins/Components/

$SUDO cp -pR HQStreamer2.app       $INSTPREFIX/Applications/
$SUDO cp -pR HQStreamer2.vst3      $INSTPREFIX/Library/Audio/Plug-Ins/VST3/
$SUDO cp -pR HQStreamer2.component $INSTPREFIX/Library/Audio/Plug-Ins/Components/
$SUDO cp -pR HQStreamer2.appex     $INSTPREFIX/Library/Audio/Plug-Ins/Components/

echo "HQStreamer2 has been installed only for this user."
echo ""
echo "Please note that many hosts will not support AU / AUv3 plugins"
echo "installed for only one user, and it may be difficult for you"
echo "to find the HQStreamer2 standalone app now that it has been"
echo "installed this way."
echo ""
echo "Press Enter to finish"
read foo
