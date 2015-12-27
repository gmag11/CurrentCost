// CCostLib.h

#ifndef _CCOSTLIB_h
#define _CCOSTLIB_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#define MAX_SENSORS 2		//Max number of channels of Current Cost sensor
#define MEAS_VALIDITY 30000	// Sensor measure validity time. Used to ignore a measure if I fail to get messages from sensor.



#endif

