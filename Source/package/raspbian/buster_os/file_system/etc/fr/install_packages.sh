#!/bin/sh

allSuccess=1

for packageName in $(cat /opt/fr/data/packages/package_list);
do
	packageStatusBool=0

	echo "Package Name : $packageName"
	packageNameShort=$(echo $packageName | awk '{print $1}' FS=_)
	echo "Package Name Short : $packageNameShort"
	dpkg -s $packageNameShort > /dev/null
	if [ $? -eq 0 ];
	then
		packageStatus=$(dpkg -s $packageNameShort | grep Status)
		echo $packageStatus
		if [ "$packageStatus" = "Status: install ok installed" ];
		then
			echo "No need to install this package again"
			packageStatusBool=1
		fi
	fi

	if [ $packageStatusBool -ne 1 ];
	then
		echo "Installing package"
		dpkg -i --force-confold /opt/fr/data/packages/$packageName
		if [ $? -ne 0 ];
		then
			allSuccess=0
		fi
	fi
done

if [ $allSuccess -eq 1 ];
then
	for packageName in $(cat /opt/fr/data/packages/package_list);
	do
		rm "/opt/fr/data/packages/${packageName}"
	done
	rm "/opt/fr/data/packages/package_list"
fi
echo "allSuccess = $allSuccess"
#If all dpkg command are successful then delete the listed packages and package_list file

hostapd_version=`dpkg -s hostapd | awk '$1=="Version:" {print $2}'`
if [ $hostapd_version = "2:2.7+git20190128+0c1e29f-6+deb10u1" ];
then
	systemctl unmask hostapd.service
fi


