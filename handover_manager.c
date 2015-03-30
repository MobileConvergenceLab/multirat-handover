/*
handover_manager.c	 -	Algorithm specific functions
	char* HandoverDecision(char* currentSSID, int currentSignal)  
	void ChangeRole()
*/

// Bring from main.c
// interface name of each link ex) "wlan0", "wlan1"
extern char activeLink[7];
extern char standbyLink[7];

/*******************************************************************
char* HandoverDecision(char* currentSSID, int currentSignal)
	char* currentSSID - SSID of AP connected with Active link
	int currentSignal - Received Signal Strength of AP connected with Active link

	Make handover decision based on current SSID and current signal
	and return SSID of decision result
*******************************************************************/
char* HandoverDecision(char* currentSSID, int currentSignal){
	char* bSSID;

	// if current signal is lower than -50dB, comapre signals.
	if(currentSignal < -50){
		
		// Find best AP which has highst signal
		// if best AP is not the current linked AP, return SSID of best AP
		bSSID = FindBestAP(currentSignal);
		if(strcmp(bSSID, currentSSID) == 0)	
			return "";
		else				
			return bSSID;
	}
	return "";
}

/*******************************************************************
void ChangeRole()
	Change role of NICs
*******************************************************************/
void ChangeRole(){
	char temp[7];

	// change role of NICs by rename variables.
	// ex) activeLink = "wlan0" -> "wlan1"
	sprintf(temp, "%s", activeLink);
	sprintf(activeLink, "%s", standbyLink);
	sprintf(standbyLink, "%s", temp);
}
