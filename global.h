#define MAX_SENSORS 2		//Max number of channels of Current Cost sensor
#define MEAS_VALIDITY 30000	// Sensor measure validity time. Used to ignore a measure if I fail to get messages from sensor.

#define PIN_CONN 0 // Pin where LED is connected


ESP8266WebServer server(80);	// The Webserver

struct strConfig {
	String ssid;	// Network SSID (name) buffer
	String pass;	// Network password buffer
	String ntpServerName;	//NTP Server name
	int timeZone;
	boolean daylight;
	String deviceName;
} config;

#ifndef TIME_SYNC_PERIOD
#define TIME_SYNC_PERIOD	86400 // How often (seconds) ntp time is checked for sync if time has been set. 86400 = 1 day
#define TIME_SYNC_PERIOD_HIGH 30	// How often (seconds) ntp time is checked for sync if no time has been set. 30 sec.
#endif

SoftwareSerial swSer(2, -1, 256); // Serial port connected to Current Cost base station. Only Rx is connected.

struct sensor_t {
	uint8_t sensor_id;
	long time_sensor;
	long last_time_sensor;
	int diff;
	int watts;
	float tempr;
	double kwh;
	double kwh_hour;
	double kwh_day;
	double kwh_month;
	double kwh_year;
	boolean valid = false;
};

uint8_t last_read_sensor = -1;

sensor_t sensor[MAX_SENSORS];

Ticker t_kwh;	// Ticker to trigger save of measurements on flash memory
boolean flag_kwh = false; // Flag to trigger save of measurements on flash memory
#define KWH_FREQ 60 // How often measures are stored on flash

boolean flag_config = false; // Flag to trigger config saving

Ticker t_checkValidMeasure; // Ticker to triger measurement validity by comparing last measure time with now()
boolean flag_checkValidMeasure = false; // Flag to triger measurement validity 
#define CHECKVALIDMEASURE_FREQ 15 // How often measurement validity is going to be checked

										//Ticker t_checkTime; 
boolean flag_checkTime = false; // Flag to check if it is start of day, month or year

boolean AdminEnabled = true;		// Enable Admin Mode for a given Time
#define ADMIN_TIMEOUT 3600  // Defines the Time in Seconds, when the Admin-Mode will be diabled
int AdminTimeOutCounter = 0;// Counter for Disabling the AdminMode

#ifdef TEST
Ticker t_sendTestMessage;
boolean flag_sendTestMessage = false;
#endif

Ticker t_checkConnection; // Ticker to trigger WiFi connection checking
boolean flag_checkConnection = false; // Flag to trigger WiFi connection checking
#define CHECKCONNECTION_FREQ 25 // WiFi connection checking period

//char ssid_ap[] = "ESP_WiFi";

//char ssid[28];	// Network SSID (name) buffer
//char pass[50];	// Network password buffer



enum conn_mode {
STA_MODE,
AP_MODE
};

conn_mode WiFi_mode = STA_MODE; // Mode of WiFi, normally it is Station Mode. AP mode is selected if WiFi connection dails several times
uint8_t WiFi_connection_fails = 0; // Number of WiFi connection failures
#define MAX_WIFI_CONN_FAILS 5 // Number of connection failures before changing to AP Mode
//boolean time_syncd = false;

//WiFiUDP udp;
//unsigned int localPort = 2390;      // local port to listen for UDP packets

char ssid_ap[] = "ESP_WiFi";

ntpClient ntp("0.europe.pool.ntp.org");

void setDefaultConfig() {
	config.ssid = "SSID";
	config.pass = "PASS";
	config.ntpServerName = "0.europe.pool.ntp.org";
	config.timeZone = 1;
	config.daylight = true;
	config.deviceName = "WebConfig";
}
