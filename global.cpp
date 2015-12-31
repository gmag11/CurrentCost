#include "global.h"

strConfig config;
Ticker checkConnection_ticker;
boolean checkConnection_flag = false;
SoftwareSerial swSer(2, -1, 256);
ESP8266WebServer server(80);
CurrentCost *ccost;
ntpClient *ntp;
char ssid_ap[] = "ESP_WiFi";
uint8_t WiFi_connection_fails = 0;
conn_mode WiFi_mode = STA_MODE;
int AdminTimeOutCounter = 0;
boolean AdminEnabled = true;

void setDefaultConfig() {
	config.ssid = "SSID";
	config.pass = "PASS";
	config.ntpServerName = "0.europe.pool.ntp.org";
	config.timeZone = 1;
	config.daylight = true;
	config.deviceName = "WebConfig";
}
