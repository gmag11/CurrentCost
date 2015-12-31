// CCostLib.h

#ifndef _CCOSTLIB_h
#define _CCOSTLIB_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include <Ticker.h>
#include <ArduinoJson.h>
#include "FS.h"
#include <Time2.h>
#include <TimeAlarms.h>

#define TEST
#define KWH_FILE_NAME "/kwh.json"

typedef struct {
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
} sensor_t;


class CurrentCost {
private:
	#define MAX_SENSORS 2		//Max number of channels of Current Cost sensor
	#define MEAS_VALIDITY 30000	// Sensor measure validity time. Used to ignore a measure if I fail	to get messages from sensor.

	uint8_t last_read_sensor = -1;

	sensor_t sensor[MAX_SENSORS];

	Ticker checkValidMeasure_ticker; // Ticker to triger measurement validity by comparing last measure time with now()
	//boolean checkValidMeasure_flag = false; // Flag to triger measurement validity 
	#define CHECKVALIDMEASURE_FREQ 15 // How often measurement validity is going to be checked

	//Ticker checkTime_ticker; 
	//boolean checkTime_flag = false; // Flag to check if it is start of day, month or year

	Ticker kwh_ticker;	// Ticker to trigger save of measurements on flash memory
	//boolean kwh_flag = false; // Flag to trigger save of measurements on flash memory
	#define KWH_FREQ 60 // How often measures are stored on flash

#ifdef TEST
	Ticker sendTestMessage_ticker;
#endif //TEST
	static bool instanceFlag; //Flag to control that instance has been created
	static CurrentCost *s_ccost; //pointer to this instance
#ifdef TEST
	static void sendTestMessage();
#endif //TEST
	static void save_kwh();
	boolean load_kwh();
	static void checkValidMeasure_task();
	static void checkTimeReset();
	String digitalClockString();
	String fillDigits(int digits);
	CurrentCost();

public:
	static CurrentCost *getInstance();
#ifdef TEST
	void process_ccost_xml_test(String msg);
#endif //TEST
	void process_ccost_xml(String msg);
	String show_sensor_data();

};

#endif

