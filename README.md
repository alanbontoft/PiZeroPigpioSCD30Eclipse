# PiZeroPigpioSCD30Eclipse
SCD30 sensor connected to PiZero via I2C.  Uses Eclipse and SysProgs compiler from gnutoolchains.com

In order to debug, need to run on target as follows: (gpio library requires sudo priviledges)

sudo gdbserver :2345 /home/pi/target/pizeropigpio

Debug config must also be set to use 'Manual Remote Debugging Launcher'

