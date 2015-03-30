/*
 This program for efficient handover of dual WLAN device.
It uses each WLAN as active link and standby link.
Active link is used for communicating and standby link is dused for scanning APs.
Program make link overlapped on previous link of active link when handover needed.

This program is consisted of follow codes:
	main.c					- Take charge of whole phase of program
	handover_manager.c		- Algorithm specific functions
	wireless_manager.c		- Contorl wireless interfaces

	getXYZ.py				- Get accelerometer sensor value
	adxl345.pyc				- for getXYZ.py

	and makefile

main.c

 1. Initalizing WLAN interface to active link and stnadby link
 2. Start scan, link, move thread 
	2.1 scan_thread 
		: Standby link scan nearbty APs 
	2.2 link_thread
		:  Active link watch RSS of device
		   Decide handover needs and perform handover process
	2.3 move_thread
		:  Measure movement  of device
	
	Repeat step  2
*/
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

// Sensitivity of device movement
#define MOVE_THRESH 0.00001

void Init();		// Initialize wlan

void ScanThread();		//  scan APs
void LinkThread();		// watch active link and perform handover
void MoveThread();		// Measure movement  of device


int move_lock;		// move_lock is for indicate movement or stop
					// 0 -> move,  1- > stop

// Store logical interface name ex)wlan0
char activeLink[7];
char standbyLink[7];


int main(){

	// Preparation
	Init();

	// Start thread functions
	pthread_t scan_thread, link_thread, move_thread;

	pthread_create (&scan_thread, NULL, (void *)&ScanThread, (void *)NULL);
	pthread_create (&link_thread, NULL, (void *)&LinkThread, (void *)NULL);
	pthread_create (&move_thread, NULL, (void *)&MoveThread, (void *)NULL);
	
	pthread_join(scan_thread, NULL);
	pthread_join(link_thread, NULL);
	pthread_join(move_thread, NULL);

	exit(0);
}

/*******************************************************************
void Init()
	initialize vairables and  WLANs
*******************************************************************/
void Init(){
	
	//  initialize variable
	move_lock=0;

	// check wlan0 link state 
	int skfd = iw_sockets_open();
	char* ap;
	int dummy;
	ap = CheckLink(skfd, &dummy, "wlan0");
	
	// if wlan0 has connection decide it to active link
	if(strcmp(ap, "0") == 0 ){
		sprintf(activeLink, "%s\0", "wlan1");
		sprintf(standbyLink, "%s\0", "wlan0");
	}
	// else decide wlan1 to active link
	else{
		sprintf(activeLink, "%s\0", "wlan0");
		sprintf(standbyLink, "%s\0", "wlan1");
	}
}

/*******************************************************************
void ScanThread()
	Scan nearby AP
*******************************************************************/
void ScanThread()
{
	// Scan nearby APs every  7s by standby link
	int skfd = iw_sockets_open();
	GetScanResult(skfd, standbyLink);
	
	while(1){
		if(move_lock == 0){
			GetScanResult(skfd,standbyLink);
		}
		sleep(7);
	}
	pthread_exit (0);

} 

/*******************************************************************
void LinkThread()
		Active link watch RSS of device
		Decide handover needs and perform handover process
*******************************************************************/
void LinkThread()
{
	char* bestSSID;
	char* currentSSID;
	int currentSignal;
	
	int sock = iw_sockets_open();

	while(1){
		sleep(1);
		// if Device is moving, check active link state every second.
		if(move_lock == 0){
			// Get SSID and signal strength of active link  and make hadover decision
			currentSSID = CheckLink(sock, &currentSignal, activeLink);	
			bestSSID = HandoverDecision(currentSSID, currentSignal);	

		// If handover needed,  hadover to standby link and change role of links
			if(strcmp(bestSSID,"") != 0){
				ConnectAP(standbyLink, bestSSID);
				DisconnectAP(activeLink);
				ChangeRole();
			}
		}		
	}
	pthread_exit (0);
}  

/*******************************************************************
void MoveThread(){
		Monitor move of device through accelerometer
		CA connection
*******************************************************************/
void MoveThread(){

	// Bring sensor value by python code
	FILE *fp;
	char buff[8];
	double x,y,z;
	double xt,yt,zt;

	//  count repeated stop state for stable measurement
	int stop_delay;

	// CA connection
	int sock = iw_sockets_open();
	int dummy;
	char* bSSID;
				

	while(1){
		sleep(1);
		
		// get XYZ value of accelerometer 
		// ref) getXYZ.py
		fp = popen("sudo python getXYZ.py", "r");
		fgets(buff, 8, fp);
		x = atof(buff);
		fgets(buff, 8, fp);
		y = atof(buff);
		fgets(buff, 8, fp);
		z = atof(buff);
		
		//  Check a change amount a amount of acceleration
		if(abs(abs(xt)-abs(x))>MOVE_THRESH || 
		   abs(abs(yt)-abs(y))>MOVE_THRESH ||
		   abs(abs(zt)-abs(z))>MOVE_THRESH )
		{
			// if device is moving,  at CA mode, disconnect stanbdy link, which used for CA link.
			stop_delay=0;
			if(move_lock==1){
				DisconnectAP(standbyLink);
				move_lock =0;
			}
		} 
		// if device have just decided stopped stably by stop_delay pass over 3, 
		// connect stadnby link for CA
		else{
			stop_delay++;
			if(stop_delay > 3 && move_lock!=1){
				move_lock =1;

				bSSID = FindBestAP4CA(CheckLink(sock, &dummy, activeLink), -256);
				ConnectAP(standbyLink, bSSID);
			}				
		}

		// record previous sensor valute for calculate difference
		xt=x;
		yt=y;
		zt=z;
	
        	pclose(fp);
	}
	pthread_exit (0);
}
