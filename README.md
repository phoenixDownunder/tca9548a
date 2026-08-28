# README.md

* Last update date August 28 2026.

--
# TCA9548A 8 Channel i2c Switch

* The TCA9548A is a very simple single register 8 channel i2c switch.

* The TCA9548A version used here is the SparkFun Qwiic TCA9548A-8-channel-switch-Breakout.
See the image at [SparkFun TCA9548A](https://github.com/phoenixDownunder/tca9548a/blob/main/SparkFun-©-TCA9548A-8-channel-switch-Breakout#:~:text=TCA9548A%2D8%2Dchannel%2D-,switch,-%2DBreakout.jpg)

--
# Contents

* TCA9548A 8 Channel i2c Switch
* Contents
* Device Reference
* Raspberry Pi and i2c
* Licence
* Quick Build
* Quick Remove
* Keeping It Simple
* Raspberry Pi Preparation
* Author

--
# Device Reference

* The PDF reference document for the 8-channel switch is [TCA9548A](https://www.ti.com/lit/ds/symlink/tca9548a.pdf).

--
# Raspberry Pi and i2c

* Raspberry Pi: A Raspberry Pi is used to manage the TCA9548A along with a range of other i2c connected devices. Only tested with RPi4 (4Gb) but should work with other versions. The software footprint and runtime requirements are very small.

* Interface: The interface is 400kb i2c and the Sparkfun version which is being used for testing, has a default i2c address 0x70. See separate section below.

* Load: On a Raspberry Pi 4 and using Raspbian Lite (Trixie at this time), there is no load on the the RPi.

* Descriptors: All i2c devices attached are described in the header file i2cDevicesHeader.h. The active device is defined in the **Main** routine, in this case, \#define TCA9548A . If any of the header files are altered, then the whole application should be re-compiled. See separate section below. The current i2cDevicesHeader.h file has been cut back and is basically static. New experimental device are attached occasionally.

--
# Licence

This software is released under the [BSD-3-Clause Licence](Licence.md) and a copy of the licence is provided at the top level. A copy of the said licence has been placed in each of the source files where possible or in the enclosing folder to satisfy the legal Dementors. 

--
# Quick build

* Prepare a Raspberry Pi(4+) with the i2c libraries (See Preparation below) in the usual way.
* Copy the tca9548a.d folder to the **HOME** directory.
* Copy or move the headers folder to the **HOME** directory.
* To build the tca9548a executable, run
 
```
cd tca9548a.d
make cleanall
make all
```
This is equivalent to

```
gcc -o tca9548a tca9548a.c -I$HOME/headers/ -Wall -pedantic -std=c2x -D_GNU_SOURCE -li2c -lm
```

then run the executable with any options such as those presented below.
Set the link below for convenience if required.
The examples below terminate after operation.

```
./tca9548a <--- displays current register setting if available
./tca9548a 0xFF  <--- enables all ports
./tca9548a nodebug 0x01 <--- silence and set port 0.

ln -s /home/(username)/tca9548a.d/tca9548a ~/bin/ma
```
--
### Quick Remove
To remove the whole lot, remove the directories tca9548a.d and headers. Nothing is saved elsewhere.

--
# Keeping It Simple

* The file tca9548a.c is the only source file used.

* The current control register value is saved in a temporary file **/tmp/tca9548a.txt**. This file holds a 2 character string between 00 and FF (0x00 and 0xFF)
* If the file does not exist, it is ignored and the routine continues.
* At startup, th file does not normally exist.
* Strictly speaking, the file value is not used, it is purely for reference.
* Everything is written in C. The software has been written and designed to be readable and therefore sometimes a little verbose. No hidden tricks, obfuscating code or short unreadable naming tricks are used.
* No separate third party libraries are used.
* Makefiles are used to compile into a local directory. Or you can use gcc directly. The c2x standard is used.
* The most important variable is of course the device file pointer, **fp**. If the device is not found, everything stops.

--
# Raspberry Pi Preparation

* The first and most obvious stage is to prepare a Raspberry Pi.
* Any version can be used but a RPi4 or faster with at least 1Gb of RAM is suggested. I haven't tried RPi3 or RPiZero. An RPi5 won't function any better than the RPi4 because of the tmp117 design in this case.
* A reasonably fast SDRAM card 16Gb or better is suggested.
* Download a recent 64bit version of Raspbian Lite, currently Trixie, in the usual way and prepare the SDRAM. Strictly speaking, only Raspbian Lite is required but you can install a GUI version if you so desire. On a Mac Terminal screen, run

```
sudo dd bs=16m if=./2026-06-18-raspios-trixie-arm64-lite.img of=/dev/rdisk2
```

* or use balenaEtcher or similar.
 
* Install and boot the RPi.

```
sudo dpkg-reconfigure console-setup
```

* (For example, try UTF-8 -> Guess -> Terminus -> 12x24 framebuffer...
to set console font size from the default. Works for 27" and 32" monitors)

* Once started, log in and run the command line routine...

```
sudo raspi-config
```

* to set the host name (for example RPi44), wifi connection, enable interface->i2c and interface->ssh. All other options are you own. Reboot as required.
Run the following command (or ip -4 a (enter)) to find your local ip address for a local ssh connection and then complete all sensible updates...

```
ifconfig
```

```
sudo apt update
sudo apt upgrade -y
sync
sudo apt dist-upgrade
sudo apt autoremove
sync
```

* Create /etc/sudoers.d/010_pi-nopasswd and put in... (yes I know that the username pi shouldn't be used but its convenient!)

```
pi ALL=(ALL) NOPASSWD: ALL
```
```
sudo shutdown -r now
```

* Reconnect and...

```
sudo apt install i2c-tools
sudo apt-get install libi2c-dev 
```

* Now, to use a fast i2c interface, edit the file /boot/firmware/config.txt and add...

```
# i2c  
dtparam=i2c=on,i2c_baudrate=400000
```

* There appears to be whole pile of variations of this exercise to achieve the same result. This works at the moment (Trixie 2026) but check.

--
# Basic tca9548a options...
```
tca9548a [nodebug] [0x??] 
```

[nodebug] By definition, debug is on. Use nodebug to silence.  
[0x??] where the HEX range is between 0x00 (off).. 0xFF (all ports enabled).  

For example...  
0x01 enables port 0  
0x02 enables port 1  
0x03 enables ports 0 and 1  
0xFF enables all 8 ports 0..7  
0x00 sets all the ports off  

--
# Disclaimer

This is a development framework and experimental software. It mostly works as designed. Hopefully there aren't too many bugs. Use at your own risk. No liability. If it crashes your Raspberry Pi (highly unlikely) or blows up your cat (more likely), it's your problem.
Hopefully, there won't be too many bugs and not too many errors in this document!

--
# Author

* Author: Bruce Stephens.
* Country of origin: Australia.
* Most Recent Update: August 2026.
* Version Number: soon...
* Errors : bruce @ eigenspaces.com.au

--