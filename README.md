#MultiRAT-Handover



##Introduction

This program is for efficient handover of dual WLAN device.
It uses each WLAN as active link and standby link.
Active link is used for communicating and standby link is used for scanning APs.
Program make link overlapped on previous link of active link when handover needed.

####This program is comprise of follow codes
* main.c			Take charge of whole phase of program
* handover_manager.c		Algorithm specific functions
* wireless_manager.c		Control wireless interfaces

* getXYZ.py			Get accelerometer sensor value
* adxl345.pyc			for getXYZ.py

* makefile

##How to execute program

1. Install open vswitch
  `apt-get install openvswitch-switch openvswitch-brcompat openvswitch-datapath-source`

2. Configure local network
	>>* Open NetConfigure.sh
	>>* Modify MAC Address table and IP address of br0
	>>* `./NetConfigure.sh`

3. Make execution file `make`

4. Start handover program `sudo ./test`


