// CCostLib.h

#ifndef _CCOSTLIB_h
#define _CCOSTLIB_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

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
	boolean checkValidMeasure_flag = false; // Flag to triger measurement validity 
	#define CHECKVALIDMEASURE_FREQ 15 // How often measurement validity is going to be checked

	//Ticker checkTime_ticker; 
	boolean checkTime_flag = false; // Flag to check if it is start of day, month or year

	Ticker kwh_ticker;	// Ticker to trigger save of measurements on flash memory
	boolean kwh_flag = false; // Flag to trigger save of measurements on flash memory
	#define KWH_FREQ 60 // How often measures are stored on flash

#ifdef TEST
	Ticker t_sendTestMessage;
	boolean flag_sendTestMessage = false;
#endif //TEST

	boolean save_kwh();
	boolean load_kwh();

public:
#ifdef TEST
	void CurrentCost::process_ccost_xml_test(String msg);
#endif //TEST
	void CurrentCost::process_ccost_xml(String msg);
	String CurrentCost::show_sensor_data();

};

#endif

