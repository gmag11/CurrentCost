#include "helpers.h"
#include "Arduino.h"
//#include "memory"
//#include <bits/unique_ptr.h>

boolean config_save_flag = false;

void flasher(int pin) {
	if (digitalRead(pin) == 0)
		digitalWrite(pin, HIGH);
	else
		digitalWrite(pin, LOW);
}

void notifyConn(uint8_t pin, boolean connected) {
	if (connected) {
		digitalWrite(pin, HIGH);
	}
	else {
		digitalWrite(pin, LOW);
	}
}

boolean save_config() {
	config_save_flag = false;
#ifdef DEBUG_HELPER_2
	Serial.println("--Save config");
#endif //DEBUG_HELPER_2
	StaticJsonBuffer<512> jsonBuffer;
#ifdef DEBUG_HELPER_3
	Serial.println("--Buffer created");
#endif // DEBUG_HELPER_3
	JsonObject& json = jsonBuffer.createObject();
#ifdef DEBUG_HELPER_3
	Serial.println("--Object created");
#endif // DEBUG_HELPER_3
	json["ssid"] = String(config.ssid);
	json["pass"] = String(config.pass);
	json["ntp"] = String(config.ntpServerName);
	json["timeZone"] = config.timeZone;
	json["daylight"] = config.daylight;
	json["deviceName"] = String(config.deviceName);

	File configFile = SPIFFS.open(CONFIG_FILE_NAME, "w");
	if (!configFile) {
#ifdef DEBUG_HELPER_1
		Serial.println("------Failed to open config file for writing");
#endif // DEBUG_HELPER_1
		return false;
	}
#ifdef DEBUG_HELPER_2
	String temp;
	json.prettyPrintTo(temp);
	Serial.println(temp);
#endif // DEBUG_HELPER_2

	json.printTo(configFile);
	return true;
}

boolean load_config() {
#ifdef DEBUG_HELPER_3
	Serial.println("Load_config start");
#endif // DEBUG_HELPER_3
	File configFile = SPIFFS.open(CONFIG_FILE_NAME, "r");
	if (!configFile) {
#ifndef DEBUG_HELPER_1
		Serial.println("Failed to open config file");
#endif // DEBUG_HELPER_1
		return false;
	} else 
#ifdef DEBUG_HELPER_3
		Serial.println("Config file open.");
#endif // DEBUG_HELPER_3
	size_t size = configFile.size();
	if (size > 512) {
#ifdef DEBUG_HELPER_1
		Serial.println("Config file size is too large");
#endif // DEBUG_HELPER_1
		return false;
	} 

	// Allocate a buffer to store contents of the file.
	std::unique_ptr<char[]> buf(new char[size]);
	
	// We don't use String here because ArduinoJson library requires the input
	// buffer to be mutable. If you don't use ArduinoJson, you may as well
	// use configFile.readString instead.

	configFile.readBytes(buf.get(), size);
#ifdef DEBUG_HELPER_2
	Serial.print("JSON file size: "); Serial.print(size); Serial.println(" bytes");
#endif // DEBUG_HELPER_2
	StaticJsonBuffer<512> jsonBuffer;
	
	JsonObject& json = jsonBuffer.parseObject(buf.get());
	if (!json.success()) {
#ifdef DEBUG_HELPER_1
		Serial.println("Failed to parse config file");
#endif // DEBUG_HELPER_1
		return false;
	}
	String temp;
#ifdef DEBUG_HELPER_2
	json.prettyPrintTo(temp);
	Serial.println(temp);
#endif // DEBUG_HELPER_2

	config.ssid = json["ssid"].asString();
	config.pass = json["pass"].asString();

	config.ntpServerName = json["ntp"].asString();
	config.timeZone = json["timeZone"];
	config.daylight = json["daylight"];
	config.deviceName = json["deviceName"].asString();

#ifdef DEBUG_HELPER_1
	Serial.println("Data initialized.");
	Serial.print("SSID: "); Serial.println(config.ssid);
	Serial.print("NTP Server: "); Serial.println(config.ntpServerName);
	//Serial.print("PASS: "); Serial.println(pass);
#endif // DEBUG_HELPER_1

	return true;
}

unsigned char h2int(char c)
{
	if (c >= '0' && c <= '9') {
		return((unsigned char)c - '0');
	}
	if (c >= 'a' && c <= 'f') {
		return((unsigned char)c - 'a' + 10);
	}
	if (c >= 'A' && c <= 'F') {
		return((unsigned char)c - 'A' + 10);
	}
	return(0);
}

String urldecode(String input) // (based on https://code.google.com/p/avr-netino/)
{
	char c;
	String ret = "";

	for (byte t = 0; t < input.length(); t++)
	{
		c = input[t];
		if (c == '+') c = ' ';
		if (c == '%') {


			t++;
			c = input[t];
			t++;
			c = (h2int(c) << 4) | h2int(input[t]);
		}

		ret.concat(c);
	}
	return ret;
}