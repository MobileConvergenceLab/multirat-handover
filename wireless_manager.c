/*
wireless_manager.c		- Contorl wireless interfaces
	void GetScanResult(int sock, char* ifname)
	char* CheckLink(int sock, int* rss, char* ifname)
	char* FindBestAP(int min)
	char* FindBestAP4CA(char* currentAP, int min)
	void ConnectAP(char* ifname, char* targetSSID)
	void DisconnectAP(char* ifname)
*/

#include <stdio.h>
#include <iwlib.h>

// AP_info struct, save AP's SSID and signal.
struct AP_info{
	char ssid[32];
	int signal;
};

// Store AP's SSID and signal result of AP scanning 
struct AP_info SCANNED_AP[100];
 
/*******************************************************************
void GetScanResult(int sock, char* ifname)
	int sock - socket strcuture for scan
	char* ifname - interface name of scan link

	Scan APs by parameter "ifname"
	Refresh global variable SCANNED_AP[100]
*******************************************************************/
void GetScanResult(int sock, char* ifname){
		wireless_scan_head head;
		wireless_scan *result;
		iwrange range;
		
		// scan APs
		// Result is stored along &head
		iw_get_range_info(sock, ifname, &range);
		iw_scan(sock, ifname, range.we_version_compiled, &head);
		
		// initialize variable, SCANNED_AP 
		result = head.result;
		int i;
		for(i=0; i<100; i++){
			SCANNED_AP[i].signal = 0;
		}
		i=0;

		// Parsing scanning result and store SSID and signal strength
		while (NULL != result) {
			sprintf(SCANNED_AP[i].ssid, "%s", result->b.essid);
			SCANNED_AP[i].signal = result->stats.qual.level-0x100;
			i++;
			result = result->next;
		}
}

/*******************************************************************
char* CheckLink(int sock, int* rss, char* ifname)
	int sock - socket strcuture for check link
	char* ifname - interface name of active link

	Check active link ssid and signal stregnth
	Return SSID by return value.
	If it needed, Caller can get signal stregnth of current link by rss parameter
*******************************************************************/
char* CheckLink(int sock, int* rss, char* ifname){
	
	struct wireless_info*	info = malloc(sizeof(struct wireless_info));
	iw_get_basic_config(sock, ifname, &(info->b));

	// get current state of link
	if(iw_get_stats(sock, ifname, &(info->stats),&info->range, info->has_range) >= 0){
		info->has_stats = 1;
	}
	
	// store signal strength in rss parameter
	int temp = info->stats.qual.level-0x100;
	*rss = temp;
	
	// return SSID of current link
	return info->b.essid;
}


/*******************************************************************
char* FindBestAP(int min)
	int min - minumum standard signal strength

	Find Best AP, which has better signal strength than min parameter
	Return SSID of found AP
*******************************************************************/
char* FindBestAP(int min){
	int bsignal = min;
	int bindex=0;
	int i;
	
	// Find Best AP in SCANEED_AP, 
	for(i=0; i<100; i++){
		
		//list is over.
		if(SCANNED_AP[i].signal == 0){
			break;
		}

		// Filter alternative AP
		// in this case we use AP named HO1 and HO3 to avoid the authentication problem
		else if(strcmp(SCANNED_AP[i].ssid, "HO1") != 0 && strcmp(SCANNED_AP[i].ssid, "HO3") !=0 ){
			continue;
		}

		// find highest signal strength
		else if(SCANNED_AP[i].signal >= bsignal){
			bindex = i;
			bsignal = SCANNED_AP[i].signal;
		}
	}
	return "";
}

/*******************************************************************
char* FindBestAP4CA(char* currentAP, int min)
	char* currentAP - SSID of currently connected with active link
	int min - minumum standard signal strength

	Find Best AP, which has better signal strength than min parameter but not the current AP 
	Return SSID of found AP
*******************************************************************/
char* FindBestAP4CA(char* currentAP, int min){
	int bsignal = min;
	int bindex=0;
	int i;
	for(i=0; i<100; i++){
		if(SCANNED_AP[i].signal == 0){
			break;
		}
		if(strncmp(SCANNED_AP[i].ssid, currentAP, 3) == 0){
			continue;
		}
		if(strcmp(SCANNED_AP[i].ssid, "HO1") != 0 && strcmp(SCANNED_AP[i].ssid, "HO2") !=0 ){
			continue;
		}
		if(SCANNED_AP[i].signal >= bsignal){
			bindex = i;
			bsignal = SCANNED_AP[i].signal;
		}
	}

	if(strcmp(SCANNED_AP[bindex].ssid, "HO1") == 0 || strcmp(SCANNED_AP[bindex].ssid, "HO2") ==0){
		return SCANNED_AP[bindex].ssid;
	}
	return "";
}

/*******************************************************************
void ConnectAP(char* ifname, char* targetSSID)
	char* ifname - interface name used for connection with AP (Ex. "wlan0")
	char* targetSSID - SSID of AP targeted at new connection

	connect interface to target AP
*******************************************************************/
void ConnectAP(char* ifname, char* targetSSID){
	char cmd[100];
	sprintf(cmd, "iwconfig %s essid %s", ifname,  targetSSID);
	system(cmd);
	
}

/*******************************************************************
void DisconnectAP(char* ifname)
	char* ifname - interface name used for disconnection (Ex. "wlan0")

	Disconnect interface with AP
*******************************************************************/
void DisconnectAP(char* ifname){
	char cmd[100];
	sprintf(cmd, "iwconfig %s essid 0", ifname);
	system(cmd);
}


