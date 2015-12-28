#ifndef helpers_h
#define helpers_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

#include <ESP8266WiFi.h>
#include "FS.h"
#include "global.h"

#define PIN_CONN 0 // Pin where LED is connected
#define CONFIG_FILE_NAME "/config.json"

boolean config_save_flag = false; // Flag to trigger config saving

typedef struct {
	String ssid;	// Network SSID (name) buffer
	String pass;	// Network password buffer
	String ntpServerName;	//NTP Server name
	int timeZone;
	boolean daylight;
	String deviceName;
} strConfig;

strConfig config;

void flasher();
void notifyConn(uint8_t pin, boolean connected);
boolean save_config();
boolean load_config();


// convert a single hex digit character to its integer value (from https://code.google.com/p/avr-netino/)
unsigned char h2int(char c);

String urldecode(String input); // (based on https://code.google.com/p/avr-netino/)

/*String printDigits(int digits) {
	// utility for digital clock display: prints preceding colon and leading 0
	String digStr = "";

	if (digits < 10)
		digStr += '0';
	digStr += String(digits);

	return digStr;
}*/

/*String digitalClockString() {
	// digital clock display of the time
	String timeStr = "";
	timeStr += String(hour());
	timeStr += ":";
	timeStr += printDigits(minute());
	timeStr += ":";
	timeStr += printDigits(second());
	timeStr += " ";
	timeStr += printDigits(day());
	timeStr += "/";
	timeStr += printDigits(month());
	timeStr += "/";
	timeStr += String(year());

	return timeStr;
}*/



#endif // !helpers_h
