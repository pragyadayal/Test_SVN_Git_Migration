#!/bin/sh

# For Open SSH start and stop

SERVICE_NAME="startstop.sh"

if [ "$1" -eq 1 ]
then
	if [ -e /bin/ssh-keygen ]
	then 
		/sbin/sshd -f /etc/ssh/sshd_config  &
	else
		echo "Error: /bin/ssh-keygen is not exit to run sshd"
	fi
elif [ "$1" -eq 0 ]
then
	if [ -e /bin/ssh-keygen ]
	then 
		killall sshd
	else
		echo "Error: /bin/ssh-keygen is not exit to stop sshd"
	fi
else
	echo "Error: etc/ssh/startstop.sh is called with invalid passing parameter"
fi