#!/bin/bash
set -x

ROOTFS_OUTPUT_TARGET_PATH=$1
PLATFORM_ROOTFS_PATCH_PATH=$2
BOARD_ROOTFS_PATCH_PATH=$3

#APPL_FOLDER_NAME=$4
#APPL_FOLDER_NAME=app-$APPL_FOLDER_NAME
#SOFT_LIK_NAME=app

MATRIX_FOLDER_NAME=matrix

#remove the unwanted inforamtion/file from target
rm -rf $ROOTFS_OUTPUT_TARGET_PATH/etc/init.d/S10atomic_commit.sh
rm -rf $ROOTFS_OUTPUT_TARGET_PATH/etc/init.d/S05NPU_init
rm -rf $ROOTFS_OUTPUT_TARGET_PATH/etc/init.d/S50launcher
rm -rf $ROOTFS_OUTPUT_TARGET_PATH/etc/init.d/S50usbdevice
rm -rf $ROOTFS_OUTPUT_TARGET_PATH/etc/init.d/S21mountall.sh
rm -rf $ROOTFS_OUTPUT_TARGET_PATH/etc/init.d/S98_lunch_init
rm -rf $ROOTFS_OUTPUT_TARGET_PATH/etc/init.d/S51n4
rm -rf $ROOTFS_OUTPUT_TARGET_PATH/etc/init.d/.usb_config
rm -rf $ROOTFS_OUTPUT_TARGET_PATH/etc/profile.d/RkEnv.sh
rm -rf $ROOTFS_OUTPUT_TARGET_PATH/etc/cron.d
rm -rf $ROOTFS_OUTPUT_TARGET_PATH/etc/iproute2
rm -rf $ROOTFS_OUTPUT_TARGET_PATH/etc/power-key.sh
rm -rf $ROOTFS_OUTPUT_TARGET_PATH/etc/ssh
rm -rf $ROOTFS_OUTPUT_TARGET_PATH/etc/usb_modeswitch.d
rm -rf $ROOTFS_OUTPUT_TARGET_PATH/etc/xdg
rm -rf $ROOTFS_OUTPUT_TARGET_PATH/lib/udev/rules.d/*

#rm -rf $ROOTFS_OUTPUT_TARGET_PATH/etc/libnl
#rm -rf $ROOTFS_OUTPUT_TARGET_PATH/etc/ppp/radius
#rm -rf $ROOTFS_OUTPUT_TARGET_PATH/etc/udev/hwdb.d
#rm -rf $ROOTFS_OUTPUT_TARGET_PATH/lib/udev/rules.d/*.rules

#copy the Generic platform files into the rootfs
#chmod -R 755 $PLATFORM_ROOTFS_PATCH_PATH
#rsync -a $PLATFORM_ROOTFS_PATCH_PATH $ROOTFS_OUTPUT_TARGET_PATH


mkdir -p $ROOTFS_OUTPUT_TARGET_PATH/home/se
mkdir -p $ROOTFS_OUTPUT_TARGET_PATH/$MATRIX_FOLDER_NAME
mkdir -p $ROOTFS_OUTPUT_TARGET_PATH/$MATRIX_FOLDER_NAME/bin
mkdir -p $ROOTFS_OUTPUT_TARGET_PATH/$MATRIX_FOLDER_NAME/config
mkdir -p $ROOTFS_OUTPUT_TARGET_PATH/$MATRIX_FOLDER_NAME/config/appConfig
mkdir -p $ROOTFS_OUTPUT_TARGET_PATH/$MATRIX_FOLDER_NAME/config/xmlConfig
mkdir -p $ROOTFS_OUTPUT_TARGET_PATH/$MATRIX_FOLDER_NAME/event/
mkdir -p $ROOTFS_OUTPUT_TARGET_PATH/$MATRIX_FOLDER_NAME/kernel
mkdir -p $ROOTFS_OUTPUT_TARGET_PATH/$MATRIX_FOLDER_NAME/ko
mkdir -p $ROOTFS_OUTPUT_TARGET_PATH/$MATRIX_FOLDER_NAME/ko/extdrv
mkdir -p $ROOTFS_OUTPUT_TARGET_PATH/$MATRIX_FOLDER_NAME/lib
mkdir -p $ROOTFS_OUTPUT_TARGET_PATH/$MATRIX_FOLDER_NAME/log
mkdir -p $ROOTFS_OUTPUT_TARGET_PATH/$MATRIX_FOLDER_NAME/media
mkdir -p $ROOTFS_OUTPUT_TARGET_PATH/$MATRIX_FOLDER_NAME/scripts/
mkdir -p $ROOTFS_OUTPUT_TARGET_PATH/$MATRIX_FOLDER_NAME/zip


#copy the Product Specific platform files into the rootfs
chmod -R 755 $BOARD_ROOTFS_PATCH_PATH
rsync -a $BOARD_ROOTFS_PATCH_PATH/* $ROOTFS_OUTPUT_TARGET_PATH

#remove unwated service file
#rm -rf $ROOTFS_OUTPUT_TARGET_PATH/etc/rc.d/S09ftpservices
#rm -rf $ROOTFS_OUTPUT_TARGET_PATH/etc/rc.d/S15recoveryservice
#rm -rf $ROOTFS_OUTPUT_TARGET_PATH/etc/rc.d/S25upgrade
rm -rf $ROOTFS_OUTPUT_TARGET_PATH/userdata
rm -rf $ROOTFS_OUTOUT_TARGET_PATH/busybox.config
rm -rf $ROOTFS_OUTPUT_TARGET_PATH/sdcard
rm -rf $ROOTFS_OUTPUT_TARGET_PATH/timestamp
rm -rf $ROOTFS_OUTPUT_TARGET_PATH/oem
rm -rf $ROOTFS_OUTPUT_TARGET_PATH/media
rm -rf $ROOTFS_OUTPUT_TARGET_PATH/data
rm -rf $ROOTFS_OUTPUT_TARGET_PATH/udisk

#misc change 
rm -rf $ROOTFS_OUTPUT_TARGET_PATH/misc



