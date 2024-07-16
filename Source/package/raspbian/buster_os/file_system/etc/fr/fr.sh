#!/bin/sh

/etc/fr/install_packages.sh > /opt/fr/data/packages/package_install_log.txt 2> /opt/fr/data/packages/package_install_errlog.txt
#passwordupdate

ifconfig eth0 192.168.50.2

rm -f /var/log/etc_fr_run.log
rm -f /var/log/fr_run.log
rm -f /var/log/upgrade.log
rm -f /var/log/upgrade_stage.txt
rm -f /opt/fr/rm_tmp_versionInfo.txt

if [ -h /opt/fr/app ]
then
	echo "app directory softlink is created" &> /var/log/etc_fr_run.log
else
	echo "app directory softlink is not created" &> /var/log/etc_fr_run.log
	APP_VERSION=$(awk -F '=' '/CURRENT_APP_VER_INFO/ {print $2}' /opt/fr/info/versionInfo.txt  | cut -d . -f -2)
	echo "/opt/fr/app-${APP_VERSION}" >> /var/log/etc_fr_run.log
	echo "ln -s /opt/fr/app-${APP_VERSION} /opt/fr/app"
	ln -n -s /opt/fr/app-${APP_VERSION} /opt/fr/app
fi

sudo chmod +x /opt/fr/app/scripts/fr.sh
/opt/fr/app/scripts/fr.sh
