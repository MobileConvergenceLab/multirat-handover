#MultiRAT-Handover



##Introduction

This program is for efficient handover of dual WLAN device.
It uses each WLAN as active link and standby link.
Active link is used for communicating and standby link is used for scanning APs.
Program make link overlapped on previous link of active link when handover needed.

This program is comprise of follow codes:
	main.c				- Take charge of whole phase of program
	handover_manager.c		- Algorithm specific functions
	wireless_manager.c		- Control wireless interfaces

	getXYZ.py			- Get accelerometer sensor value
	adxl345.pyc			- for getXYZ.py

	and makefile

##How to execute program
1. Install OpenvSwitch "'apt-get install openvswitch-switch openvswitch-brcompat openvswitch-datapath-source"'
