/*
Name:		CCostLib
Created:	31/12/2015 16:26:34
Author:	Germán Martín (gmag11@gmail.com)
Maintainer:Germán Martín (gmag11@gmail.com)
Editor:	http://www.visualmicro.com

Library to interface and store Current Cost sensor data
*/

#ifndef _CCOSTLIB_h
#define _CCOSTLIB_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#define DEBUG_CCOST_1
#define DEBUG_CCOST_2
#define DEBUG_CCOST_3
#define TEST

#include <Ticker.h>
#include <ArduinoJson.h>
#include "FS.h"
#include <Time2.h>
#include <TimeAlarms.h>

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

public:
	/**
	* Gets CurrentCost instance creating it if does not exist.
	* @param[out] Pointer to CurrentCost instance
	*/
	static CurrentCost *getInstance();

#ifdef TEST
	/**
	* Simulates CurrenCost measurement result
	* @param[in] Dummy string to make this call compatible with process_ccost_xml(String)
	*/
	void process_ccost_xml_test(String msg);
#endif //TEST

	/**
	* Processes CurrentCost XML message, extracting measurements and storing them on sensors
	* @param[in] XML String got from CurrentCost serial port
	*/
	void process_ccost_xml(String msg);

	/**
	* Gets sensor data to a String for output via Serial
	* @param[out] Prettified data
	*/
	String show_sensor_data();

private:
	#define MAX_SENSORS 2		//Max number of channels of Current Cost sensor
	#define MEAS_VALIDITY 30000	// Sensor measure validity time. Used to ignore a measure if I fail	to get messages from sensor.

	uint8_t last_read_sensor = -1; // Sensor read in last lecture

	sensor_t sensor[MAX_SENSORS]; // Sensor data storage

	Ticker checkValidMeasure_ticker; // Ticker to triger measurement validity by comparing last measure time with now()
	//boolean checkValidMeasure_flag = false; // Flag to triger measurement validity 
	#define CHECKVALIDMEASURE_FREQ 15 // How often measurement validity is going to be checked

	//Ticker checkTime_ticker; 
	//boolean checkTime_flag = false; // Flag to check if it is start of day, month or year

	Ticker kwh_ticker;	// Ticker to trigger save of measurements on flash memory
	//boolean kwh_flag = false; // Flag to trigger save of measurements on flash memory
	#define KWH_FREQ 60 // How often measures are stored on flash

#ifdef TEST
	Ticker sendTestMessage_ticker; // Ticker to simulate measurement result
#endif //TEST
	static bool instanceFlag; //Flag to control that instance has been created
	static CurrentCost *s_ccost; //pointer to this instance
#ifdef TEST
	/**
	 * Simulate CurrentCost message every n seconds and show data over Serial
	 */
	static void sendTestMessage();
#endif //TEST

	/**
	* Save accumulated values to EEPROM (SPPIFS) to recover them in case of restart
	*/
	static void save_kwh();

	/**
	* Recovers accumulated values from EEPROM (SPPIFS) after a restar
	* @param[out] True if everything went OK
	*/
	boolean load_kwh();

	/**
	* Check if last measurement is too old. MAsks it as invalid if so.
	*/
	static void checkValidMeasure_task();

	/**
	* Checks if a period (day, month, year) has started, to reset accumulated values.
	* @param[out] True if everything went OK
	*/
	static void checkTimeReset();

	/**
	* Generates a string with current time and date
	* @param[out] String with current time and date
	*/
	String digitalClockString();

	/**
	* Accesory function to correctly format time data
	* @param[in] a digit.
	* @param[out] Digit with leading zero if needed
	*/
	String fillDigits(int digits);
	
	/**
	* Constructs CurrentCost object and initiates periodic tasks
	* @param[out] CurrentCost object
	*/
	CurrentCost();
};

#endif

