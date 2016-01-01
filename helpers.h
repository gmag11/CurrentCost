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
#include "memory"

#define DEBUG_HELPER_1 // Debug light
#define DEBUG_HELPER_2 // Debug medium
#define DEBUG_HELPER_3 // Debug deep
#define PIN_CONN 0 // Pin where LED is connected
#define CONFIG_FILE_NAME "/config.json"

extern boolean config_save_flag; // Flag to trigger config saving

/**
 * Flashes LED alternatively to signal events
 * @param[in] output pin where led is connected.
 */
void flasher(int pin);

/**
 * Turn on of off led to signal if connection to Internet is stablished or not
 * @param[in] output pin where led is connected.
 */
void notifyConn(uint8_t pin, boolean connected);

/**
 * Saves config to EEPROM (SPIFFS)
 * @param[out] true if everything went OK
 */
boolean save_config();

/**
 * Loads config to EEPROM (SPIFFS)
 * @param[out] true if everything went OK
 */
boolean load_config();

/**
 * convert a single hex digit character to its integer value
 * (from https://code.google.com/p/avr-netino/)
 * @param[in] hex digit as char
 * @param[out] number as char
 */
unsigned char h2int(char c);

/**
* 
* (based on https://code.google.com/p/avr-netino/)
* @param[in]
* @param[out]
*/
String urldecode(String input);

#endif // !helpers_h
