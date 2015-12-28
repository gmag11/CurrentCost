#include "helpers.h"

void flasher() {
	if (digitalRead(PIN_CONN) == 0)
		digitalWrite(PIN_CONN, HIGH);
	else
		digitalWrite(PIN_CONN, LOW);
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
#ifdef DEBUG
	Serial.println("Save config");
#endif
	StaticJsonBuffer<512> jsonBuffer;
	Serial.println("Buffer created");
	JsonObject& json = jsonBuffer.createObject();
	Serial.println("Object created");
	json["ssid"] = String(config.ssid);
	json["pass"] = String(config.pass);
	json["ntp"] = String(config.ntpServerName);
	json["timeZone"] = config.timeZone;
	json["daylight"] = config.daylight;
	json["deviceName"] = String(config.deviceName);

	File configFile = SPIFFS.open(CONFIG_FILE_NAME, "w");
	if (!configFile) {
		Serial.println("Failed to open config file for writing");
		return false;
	}
#ifdef DEBUG
	String temp;
	json.prettyPrintTo(temp);
	Serial.println(temp);
#endif

	json.printTo(configFile);
	return true;
}

boolean load_config() {
	File configFile = SPIFFS.open(CONFIG_FILE_NAME, "r");
	if (!configFile) {
		Serial.println("Failed to open config file");
		return false;
	}

	size_t size = configFile.size();
	if (size > 512) {
		Serial.println("Config file size is too large");
		return false;
	}

	// Allocate a buffer to store contents of the file.
	std::unique_ptr<char[]> buf(new char[size]);
	//std::

	// We don't use String here because ArduinoJson library requires the input
	// buffer to be mutable. If you don't use ArduinoJson, you may as well
	// use configFile.readString instead.
	configFile.readBytes(buf.get(), size);
#ifdef DEBUG
	Serial.print("JSON file size: "); Serial.print(size); Serial.println(" bytes");
#endif

	StaticJsonBuffer<512> jsonBuffer;
	JsonObject& json = jsonBuffer.parseObject(buf.get());

	if (!json.success()) {
		Serial.println("Failed to parse config file");
		return false;
	}
#ifdef DEBUG
	String temp;
	json.prettyPrintTo(temp);
	Serial.println(temp);
#endif
	//memset(ssid, 0, 28);
	//memset(pass, 0, 50);
	//String("Virus_Detected!!!").toCharArray(ssid, 28); // Assign WiFi SSID
	//String("LaJunglaSigloXX1@.2").toCharArray(pass, 50); // Assign WiFi PASS

	config.ssid = json["ssid"].asString();
	//String(ssid_str).toCharArray(config.ssid, 28);

	config.pass = json["pass"].asString();
	//String(pass_str).toCharArray(config.pass, 28);
	config.ntpServerName = json["ntp"].asString();
	config.timeZone = json["timeZone"];
	config.daylight = json["daylight"];
	config.deviceName = json["deviceName"].asString();

#ifdef DEBUG
	Serial.println("Data initialized.");
	Serial.print("SSID: "); Serial.println(config.ssid);
	Serial.print("NTP Server: "); Serial.println(config.ntpServerName);
	//Serial.print("PASS: "); Serial.println(pass);
#endif

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