#!/usr/bin/env sh

unameOut="$(uname -s)"
case "${unameOut}" in
    Linux*)     machine=Linux;;
    Darwin*)    machine=Mac;;
    CYGWIN*)    machine=Cygwin;;
    MINGW*)     machine=MinGw;;
    *)          machine="UNKNOWN:${unameOut}"
esac
echo ${machine}

if [ ${machine} != "Mac" ]; then
	echo "This installer is for Mac only"
	exit 1
fi

echo $0

p=`dirname "$0"`
cd "$p"
pwd
echo "This installation script will overwrite previous versions of HQStreamer"
echo "To install for all Users type 'a"
echo "To install for this user type 'u'"
echo "Instal for All or this user only?: \c"
read response
if [ "$response" == 'a' ]  || [ "$response" == 'A' ]
then
	echo "To install for all users your account needs to have administrative privileges"
	echo "You may be asked to type in your password"
	INSTDIR=/Library/Audio/Plug-Ins
	SUDO="sudo "
elif [ "$response" == 'u' ]  || [ "$response" == 'U' ]; then
	echo installing for this user
	INSTDIR=${HOME}/Library/Audio/Plug-Ins
	SUDO=" "
else
	echo "Nothing installed. Type any key to end: \c"
	read foo
	exit 1
fi
if [ -d "$INSTDIR/VST3" ]; then
	$SUDO mkdir -p "$INSTDIR/VST3"
fi
if [ -d "$INSTDIR/Components" ]; then
	$SUDO mkdir -p "$INSTDIR/Components"
fi
	
$SUDO cp -pR HQStreamer2.vst3 $INSTDIR/VST3/
$SUDO cp -pR HQStreamer2.appex $INSTDIR/Components/
$SUDO cp -pR HQStreamer2.app /Applications/
echo Installation complete
echo "type any key to close to end  \c"
read foo
