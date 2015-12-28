#ifndef global_h
#define global_h

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <SoftwareSerial.h>
#include <ArduinoJson.h>
#include <NtpClientLib.h>
#include <Ticker.h>

ESP8266WebServer server(80);	// The Webserver

/*#ifndef TIME_SYNC_PERIOD
#define TIME_SYNC_PERIOD	86400 // How often (seconds) ntp time is checked for sync if time has been set. 86400 = 1 day
#define TIME_SYNC_PERIOD_HIGH 30	// How often (seconds) ntp time is checked for sync if no time has been set. 30 sec.
#endif*/

SoftwareSerial swSer(2, -1, 256); // Serial port connected to Current Cost base station. Only Rx is connected.



boolean AdminEnabled = true;		// Enable Admin Mode for a given Time
#define ADMIN_TIMEOUT 3600  // Defines the Time in Seconds, when the Admin-Mode will be diabled
int AdminTimeOutCounter = 0;// Counter for Disabling the AdminMode

Ticker t_checkConnection; // Ticker to trigger WiFi connection checking
boolean flag_checkConnection = false; // Flag to trigger WiFi connection checking
#define CHECKCONNECTION_FREQ 25 // WiFi connection checking period

typedef enum {
	STA_MODE,
	AP_MODE
} conn_mode;

conn_mode WiFi_mode = STA_MODE; // Mode of WiFi, normally it is Station Mode. AP mode is selected if WiFi connection dails several times
uint8_t WiFi_connection_fails = 0; // Number of WiFi connection failures
#define MAX_WIFI_CONN_FAILS 5 // Number of connection failures before changing to AP Mode

char ssid_ap[] = "ESP_WiFi";

ntpClient *ntp;

void setDefaultConfig() {
	config.ssid = "SSID";
	config.pass = "PASS";
	config.ntpServerName = "0.europe.pool.ntp.org";
	config.timeZone = 1;
	config.daylight = true;
	config.deviceName = "WebConfig";
}

#endif // !global_h
