#ifndef global_h
#define global_h

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <SoftwareSerial.h>
#include <ArduinoJson.h>
#include <NtpClientLib.h>
#include <Ticker.h>
#include "CCostLib.h"

extern ESP8266WebServer server;	// The Webserver


#define TIME_SYNC_PERIOD	86400 // How often (seconds) ntp time is checked for sync if time has been set. 86400 = 1 day
#define TIME_SYNC_PERIOD_HIGH 30	// How often (seconds) ntp time is checked for sync if no time has been set. 30 sec.

//extern SoftwareSerial swSer; // Serial port connected to Current Cost base station. Only Rx is connected.

extern boolean AdminEnabled;		// Enable Admin Mode for a given Time
#define ADMIN_TIMEOUT 3600  // Defines the Time in Seconds, when the Admin-Mode will be diabled
extern int AdminTimeOutCounter;// Counter for Disabling the AdminMode

extern Ticker checkConnection_ticker; // Ticker to trigger WiFi connection checking
extern boolean checkConnection_flag; // Flag to trigger WiFi connection checking
#define CHECKCONNECTION_FREQ 25 // WiFi connection checking period

typedef enum {
	STA_MODE,
	AP_MODE
} conn_mode;

extern conn_mode WiFi_mode; // Mode of WiFi, normally it is Station Mode. AP mode is selected if WiFi connection dails several times
extern uint8_t WiFi_connection_fails; // Number of WiFi connection failures
#define MAX_WIFI_CONN_FAILS 5 // Number of connection failures before changing to AP Mode

extern char ssid_ap[];

extern ntpClient *ntp;
extern CurrentCost *ccost;

typedef struct {
	String ssid;			// Network SSID (name) buffer
	String pass;			// Network password buffer
	String ntpServerName;	// NTP Server name
	int timeZone;			// Time offset over UTC
	boolean daylight;		// Daylight configuration active?
	String deviceName;		// Device name
} strConfig;

extern strConfig config;

/**
* Sets default general configuration if any error happens when reading config file
*/
void setDefaultConfig();

#endif // !global_h
