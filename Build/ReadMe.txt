//***************************************Package versions*************************************************
//for all packages refer location /AccessControlSystem/SDT/Device/Common/Platform/Package/
1) Kernel(extract in AccessControlSystem/SDT/Device/DirectDoor/GateController/Source/V1) -> linux-3.2.0-psp04.06.00.10
2) U-boot(extract in AccessControlSystem/SDT/Device/DirectDoor/GateController/Source/V1) -> u-boot-2013.01.01-psp05.07.00.00
3) Buildroot(extract in AccessControlSystem/SDT/Device/DirectDoor/GateController/Source/V1/Buildroot) -> buildroot-2012.11.tar.bz2
4) Toolchain(extract in /opt) -> arm-2012.03-57-arm-none-linux-gnueabi-i686-pc-linux-gnu.tar.bz2

//In case don't want to compile buildroot and toolchain one can use directly compiled toolchaing to compile application, this can not be used for release
5) Compiled toolchain(extract in /opt)(directly application can compile form it) -> GateController_Toolchain.tar.bz2
//*********************************************************************************************************



//****************************Flashing Images into Board*****************************************************
For Flasing images into board refer "GateController_boardUp.txt" file.
//***********************************************************************************************************



//************************************Wifi firmware config file genration step********************************
1) first compile wlconf using following command
	make CC=toolchain ARCH=arm

2)Now, after successful compilation copy wlconf, wl18xx-conf-default.bin, struct.bin, example.ini, example.conf,
dictionary.txt, default.conf, official_inis files into device.

3) Now rmmod wlcore_sdio.

4) now give following command to generate "wl18xx-conf.bin" file.
	./wlconf -o /lib/firmware/ti-connectivity/wl18xx-conf.bin -I ./official_inis/WL8_System_parameters.ini
	./wlconf -i /lib/firmware/ti-connectivity/wl18xx-conf.bin -o /lib/firmware/ti-connectivity/wl18xx-conf.bin -s wl18xx.ht.mode=0x01
	./wlconf -i /lib/firmware/ti-connectivity/wl18xx-conf.bin -o /lib/firmware/ti-connectivity/wl18xx-conf.bin -s wl18xx.phy.number_of_assembled_ant2_4=0x01
	./wlconf -i /lib/firmware/ti-connectivity/wl18xx-conf.bin -o /lib/firmware/ti-connectivity/wl18xx-conf.bin -s wl18xx.phy.number_of_assembled_ant5=0x0
	./wlconf -i /lib/firmware/ti-connectivity/wl18xx-conf.bin -o /lib/firmware/ti-connectivity/wl18xx-conf.bin -s wl18xx.phy.board_type=0x04

5) now "wl18xx-conf.bin" file generated successfully.
//**************************************************************************************************************