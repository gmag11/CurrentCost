#define DEBUG
//#define WEB_TIME
#define NTP_TIME

#include <WiFiServer.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <Ticker.h>
#include <ArduinoJson.h>
#include <Time2.h>
#include <TimeAlarms.h>
#include <SoftwareSerial.h>
#include "FS.h"
#include <NtpClientLib.h>
#include "global.h"
#include "helpers.h"
#include "CCostLib.h"


#include "Page_Admin.h"
#include "Page_Script.js.h"
#include "Page_Style.css.h"
#include "Page_NetworkConfiguration.h"

void setFlagCheckConnection() {
	checkConnection_flag = true;
}

void checkConnection_task() {
	Serial.print("Check connection... ");
	if (WiFi_mode == STA_MODE) {
		Serial.print("Station Mode... ");
		if (WiFi_connection_fails >= MAX_WIFI_CONN_FAILS) {
			WiFi_connection_fails = 0;
			WiFi.disconnect();
			WiFi_mode = AP_MODE;
			Serial.println("Changing to AP_Mode");
			return;
		}
		else {
			if (WiFi.status() != WL_CONNECTED) {
				Serial.println("not connected");
				notifyConn(PIN_CONN, false);
				//setSyncInterval(TIME_SYNC_PERIOD_HIGH);
				//Serial.println("Sync Period set high");
			} else if (WiFi.status() == WL_CONNECTED) {
				WiFi_connection_fails = 0;
				notifyConn(PIN_CONN, true);
				Serial.print("Connected... ");
				Serial.print(WiFi.status());
				Serial.print(" WiFi connected to ");
				Serial.print(WiFi.SSID());
				Serial.print(" at ");
				Serial.print(WiFi.localIP());
				Serial.print(" RSSI: ");
				Serial.print(WiFi.RSSI());
				Serial.println("dBm");
			}

		}
	}
	if (WiFi_mode == AP_MODE) {
		Serial.println("AP Only Mode... Activated");
	}

}

void loop () {
	String mensaje;
	
	server.handleClient();

#ifdef TEST // Test mode to generate random data and avoid the need to hace CCost base connected for developing
	/*if (flag_sendTestMessage //int i = swSer.available()/ /) {
		mensaje = "";//swSer.readStringUntil('\n');
		flag_sendTestMessage = false;
		process_ccost_xml_test(mensaje);
		show_sensor_data();
	}*/
#else // Normal mode code, reading from serial
	if (swSer.available()) {
		swSer.readStringUntil('\n');
		ccost->process_ccost_xml(mensaje);
		Serial.println(ccost->show_sensor_data());
	}
#endif
	/*
	//------------------- Save kWh data -----------------------
	if (flag_kwh) { //Save kWh data
		if (!save_kwh()) {
			Serial.println("Error saving kwh");
		}
	}
	//------------------- Chech measures age-------------------
	if (flag_checkValidMeasure) { //Check for every measure age
		for (int i = 0; i < MAX_SENSORS; i++) {
			if (sensor[i].diff > MEAS_VALIDITY) {
				sensor[i].valid = false;

				Serial.print(sensor[i].diff);
				Serial.print(" sec. ");
				Serial.print("Invalidate measure ");
				Serial.println(i);
			}
		}
		flag_checkValidMeasure = false;
	}
	
	//------------------- Reset historic values---------------
	if (flag_checkTime) { //Check if historic values need reset
		flag_checkTime = false;
		checkTimeReset();
	}*/

	//------------------- Check connection -------------------
	if (checkConnection_flag) { //Connection check to turn LED on or off
		checkConnection_flag = false;
		checkConnection_task();
	}
}

void setup() {
	// Prepare serial ports
	Serial.println("Serial");
	Serial.begin(115200); // Debug port
	swSer.begin(57600); // CCost base port
	
	pinMode(PIN_CONN, OUTPUT); // Configure LED pin
	notifyConn(PIN_CONN, false); // Set LED off
	Serial.println("test");

	// Start flash file system
	if (!SPIFFS.begin()) {
		if (SPIFFS.format()) { // Format if something went wrong. Does not work. I needed to format manually.
			Serial.println("Failed to mount file system. Formatting...");
			Serial.println("Format successful");
			SPIFFS.begin();
		}
		else {
			Serial.println("Failed to mount file system.");
			return;
		}

	}
#ifdef DEBUG
	else {
		Serial.println("FS started");
	}
#endif
	// load configuration file from flash
	if (!load_config()) {
		//WiFi_mode = STA_MODE;
		//WiFi_mode = AP_MODE;
		setDefaultConfig();
		if (!save_config())
			Serial.println("Error creating config file");

	}
	if (AdminEnabled) {
		WiFi.mode(WIFI_AP_STA);
		WiFi.softAP(ssid_ap);
		Serial.print("AP Started. SSID: ");
		Serial.print(ssid_ap);
		Serial.print(" IP address: ");
		Serial.println(WiFi.softAPIP());
		Alarm.timerOnce(ADMIN_TIMEOUT, []() {
			AdminEnabled = false;
			Serial.println("Admin Mode disabled!");
			WiFi.mode(WIFI_STA);
		});
	}
	else {
		WiFi.mode(WIFI_STA);
	}

	WiFi.begin(config.ssid.c_str(), config.pass.c_str());

	checkConnection_task(); // Wait for WiFi and set ntp time
	
	ntp = ntpClient::getInstance(config.ntpServerName, config.timeZone);
	if (ntp->begin()) {
		Serial.println("NTP Client set");
	} else {
		Serial.println("Error creating NTP Client");
	}
	ccost = CurrentCost::getInstance();
	//save_config();
	//t_kwh.attach(KWH_FREQ, do_save_kwh); // Program measure saving task
	//t_checkValidMeasure.attach(CHECKVALIDMEASURE_FREQ, invalidate_measure_task); // Program measure vaidity check task
	//Alarm.alarmRepeat(0, 0, 0, doCheckTimeReset); // Program reset time task
	checkConnection_ticker.attach(CHECKCONNECTION_FREQ, setFlagCheckConnection); // Program connection check task

/*#ifdef TEST
	t_sendTestMessage.attach(6, do_sendTestMessage);
#endif

	// Initialize sensor data
	for (int i = 0; i < MAX_SENSORS; i++) {
		sensor[i].time_sensor = millis();
		sensor[i].last_time_sensor = millis();
		sensor[i].kwh = 0;
		sensor[i].kwh_hour = 0;
		sensor[i].kwh_day = 0;
		sensor[i].kwh_month = 0;
		sensor[i].kwh_year = 0;
	}
	load_kwh(); // Load measurement data from flash*/
#ifdef DEBUG
	Serial.println("\nCC Parser test started");
#endif
	// WebServer init
	server.on("/favicon.ico", []() { Serial.println("favicon.ico"); server.send(200, "text/html", "");   });
	server.on("/admin.html", []() { Serial.println("admin.html"); server.send(200, "text/html", Page_AdminMainPage);   });
	server.on("/style.css", []() { Serial.println("style.css"); server.send(200, "text/plain", Page_Style_css);  });
	server.on("/microajax.js", []() { Serial.println("microajax.js"); server.send(200, "text/plain", Page_microajax_js);  });
	server.on("/config.html", send_network_configuration_html);
	server.on("/admin/connectionstate", send_connection_state_values_html);
	server.onNotFound([]() { Serial.println("Page Not Found"); server.send(400, "text/html", "Page not Found: " + server.uri());   });
	server.begin();
	Serial.println("HTTP server started");
}


