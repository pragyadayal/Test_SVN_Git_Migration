#!/bin/sh
#
#################################################################################################
# PURPOSE	: This script start the SSH service for login purpose
###################################################################################################

BOARD_TYPE=$1

#get ethernet address from cmdline
if [ "$1" = "pvr" ] || [ "$1" = "wldoor" ]
then
ETHADDR=`ifconfig eth0 | grep HWaddr | awk -F "HWaddr " '{print $2}'`
else
ETHADDR=`cat /proc/cmdline | awk -F "ethaddr=" '{print $2}' | awk '{print $1}'`
fi

echo "Info: changing se and root passwords"
chmod +x /usr/sbin/passwordupdate 

#passed the ethaddr as the command line argument
/usr/sbin/passwordupdate root $ETHADDR  $BOARD_TYPE
/usr/sbin/passwordupdate se $ETHADDR	$BOARD_TYPE

# SSH starting
if [ -e /bin/ssh-keygen ]
then 
	mkdir -p /var/empty
	chown root:sshd /var/empty
	chmod 755 /var/empty
	chmod 755 /bin/ssh-keygen
	chmod 755 /sbin/sshd
	touch /var/log/btmp
	chmod 600 /var/log/btmp

	if [ ! -e /etc/ssh/ssh_host_dsa_key ]
	then
		/bin/ssh-keygen -t dsa -f /etc/ssh/ssh_host_dsa_key -N ""
	fi

	if [ ! -e /etc/ssh/ssh_host_rsa_key ]
	then
		/bin/ssh-keygen -t rsa -f /etc/ssh/ssh_host_rsa_key -N ""
	fi

	chmod 600 /etc/ssh/ssh_host_dsa_key
	chmod 600 /etc/ssh/ssh_host_rsa_key

	chmod 644 /etc/ssh/ssh_host_dsa_key.pub
	chmod 644 /etc/ssh/ssh_host_rsa_key.pub
else
	echo "Error: /bin/ssh-keygen file not exit"
fi

# Starting SSH
chmod +x /etc/ssh/startstop.sh
/etc/ssh/startstop.sh 1

#End of Script
