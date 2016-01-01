#include "global.h"

strConfig config;					  // General config (SSID, PASS, ntp server, time zone, etc)
Ticker checkConnection_ticker;		  // Ticker to trigger WiFi connection checking
boolean checkConnection_flag = false; // Flag to trigger WiFi connection checking
SoftwareSerial swSer(2, -1, 256);	  // Serial port connected to Current Cost base station. Only Rx is connected.
ESP8266WebServer server(80);		  // The Webserver
CurrentCost *ccost;					  // CurrentCost sensor instance
ntpClient *ntp;						  // NTP Client instance
char ssid_ap[] = "ESP_WiFi";		  // WiFi AP SSID name for admin web interface
uint8_t WiFi_connection_fails = 0;	  // Number of WiFi connection failures
conn_mode WiFi_mode = STA_MODE;		  // Mode of WiFi, normally it is Station Mode. AP mode is selected if WiFi connection dails several times
int AdminTimeOutCounter = 0;		  // Counter for Disabling the AdminMode
boolean AdminEnabled = true;		  // Enable Admin Mode for a given Time

void setDefaultConfig() {
	config.ssid = "SSID";
	config.pass = "PASS";
	config.ntpServerName = "0.europe.pool.ntp.org";
	config.timeZone = 1;
	config.daylight = true;
	config.deviceName = "WebConfig";
}
