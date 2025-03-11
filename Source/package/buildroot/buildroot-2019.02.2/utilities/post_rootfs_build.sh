#!/bin/bash


PLATFORM_ROOTFS_PATCH_PATH=$SVN_HOME_DIR/Briefcase/SWD_Briefcase/GenericSoftwareModules/Platform/SWD/Trunk/Source/package/buildroot/rootfs/
BOARD_ROOTFS_PATCH_PATH=$SVN_HOME_DIR/Briefcase/SWD_Briefcase/GenericSoftwareModules/Platform/SWD/Trunk/Source/board/ARGO_FR/package/rootfs/

ROOTFS_OUTPUT_TARGET_PATH=$1

echo "Inside the post build mib script"

#remove the unwanted inforamtion/file from target
rm -rf $ROOTFS_OUTPUT_TARGET_PATH/etc/init.d/S*
rm -rf $ROOTFS_OUTPUT_TARGET_PATH/etc/iproute2
rm -rf $ROOTFS_OUTPUT_TARGET_PATH/etc/libnl
rm -rf $ROOTFS_OUTPUT_TARGET_PATH/etc/ppp/radius
rm -rf $ROOTFS_OUTPUT_TARGET_PATH/etc/udev/hwdb.d
rm -rf $ROOTFS_OUTPUT_TARGET_PATH/lib/udev/rules.d/*.rules


#copy the Generic platform files into the rootfs
chmod -R 755 $PLATFORM_ROOTFS_PATCH_PATH
rsync -a $PLATFORM_ROOTFS_PATCH_PATH $ROOTFS_OUTPUT_TARGET_PATH


#copy the Product Specific platform files into the rootfs
chmod -R 755 $BOARD_ROOTFS_PATCH_PATH
rsync -a $BOARD_ROOTFS_PATCH_PATH $ROOTFS_OUTPUT_TARGET_PATH

mkdir -p $ROOTFS_OUTPUT_TARGET_PATH/matrix/log

#trigger the application build 


