#!/bin/bash
while :
do
	rm -f cout.log crash.log
	echo "Running relay..."
	if ! ./hqs2relay > cout.log ; then
		chmod a+rw crash.log
		echo " " >> all_crashes.log
		echo "=============================" >> all_crashes.log
		echo "Relay crashed at `date`"  >> all_crashes.log
		cat crash.log >> all_crashes.log
		echo "Relay crashed, press Ctrl+C within 10 seconds to abort restarting"
		sleep 10
	fi
done

