// 
// 
// 

#include "CCostLib.h"

# define BUFFER_LENGTH 625

bool CurrentCost::instanceFlag = false;
CurrentCost *CurrentCost::s_ccost = NULL;

extern CurrentCost currentcost;

CurrentCost *CurrentCost::getInstance() {
	if (!instanceFlag) {
		s_ccost = new CurrentCost();
		//atexit(&DestroyNtpClient);
		instanceFlag = true;
		return s_ccost;
	}
	else {
		Serial.println("Instance flag was true");
		return s_ccost;
	}
}

#ifdef TEST
void CurrentCost::process_ccost_xml_test(String msg) {
	
	int sensor_id = random(0, MAX_SENSORS);
	
	sensor[sensor_id].sensor_id = sensor_id;
	sensor[sensor_id].watts = random(100, 1000);
	sensor[sensor_id].tempr = 20;
	sensor[sensor_id].time_sensor = millis();
	sensor[sensor_id].diff = sensor[sensor_id].time_sensor - sensor[sensor_id].last_time_sensor;
	sensor[sensor_id].last_time_sensor = sensor[sensor_id].time_sensor;
	
	double sum = (double)((double)sensor[sensor_id].watts / 1000 * (double)sensor[sensor_id].diff / (1000 * 3600));
	
	sensor[sensor_id].kwh += sum;
	sensor[sensor_id].kwh_hour += sum;
	sensor[sensor_id].kwh_day += sum;
	sensor[sensor_id].kwh_month += sum;
	sensor[sensor_id].kwh_year += sum;
	sensor[sensor_id].valid = true;
	last_read_sensor = sensor_id;

}

void CurrentCost::sendTestMessage() {
	CurrentCost *c_cost = s_ccost;

	c_cost->process_ccost_xml_test("");
	Serial.println(c_cost->show_sensor_data());
}
#endif //TEST

void CurrentCost::process_ccost_xml(String msg) {
	//sensor_t sensor_data;
	if (msg.indexOf("<hist>") == -1) { // If history message, discard it
		int idx = msg.indexOf("<sensor>"); // Look for sensor tag
		int sensor_id = (uint8_t)(msg.substring(idx + 8, idx + 9).toInt()); // Extract sensor id
		//Serial.println(sensor_id);
		if ((sensor_id >= 0) || (sensor_id < MAX_SENSORS)) {
			sensor[sensor_id].sensor_id = sensor_id;
			idx = msg.indexOf("<watts>"); // look for watts tag
			sensor[sensor_id].watts = msg.substring(idx + 7, idx + 12).toInt();
			idx = msg.indexOf("<tmpr>"); // Look for tmpr tag
			sensor[sensor_id].tempr = msg.substring(idx + 6, idx + 10).toFloat();

			// TODO time_sensor is not needed. It is used temporally only.
			sensor[sensor_id].time_sensor = millis(); // Assign current time
			sensor[sensor_id].diff = sensor[sensor_id].time_sensor - sensor[sensor_id].last_time_sensor; // Calculate last measurement age
			sensor[sensor_id].last_time_sensor = sensor[sensor_id].time_sensor;
			double sum = (double)((double)sensor[sensor_id].watts / 1000 * (double)sensor[sensor_id].diff / (1000 * 3600)); //Calculate kWh in this period using diff as time extension
			//TODO If last sensor measure is not valid it should not add kWh.
			sensor[sensor_id].kwh += sum;
			sensor[sensor_id].kwh_hour += sum;
			sensor[sensor_id].kwh_day += sum;
			sensor[sensor_id].kwh_month += sum;
			sensor[sensor_id].kwh_year += sum;
			sensor[sensor_id].valid = true; 
			last_read_sensor = sensor_id;
		}
	}
}

String CurrentCost::show_sensor_data() {
	String temp = "";
	if (last_read_sensor >= 0 && last_read_sensor < MAX_SENSORS) {
		
		temp += digitalClockString();
		temp += " ";
		temp += String(sensor[last_read_sensor].kwh, 6);//Delta time: %d
		temp += " kWh. for ";
		temp += String((double)((double)sensor[last_read_sensor].diff / 1000));
		temp += " Sec. ";
		temp += "Sensor: ";
		temp += sensor[last_read_sensor].sensor_id;
		temp += " --> ";
		temp += sensor[last_read_sensor].watts;
		temp += " W. Temperature: ";
		temp += sensor[last_read_sensor].tempr;
	}
	return temp;
}

String CurrentCost::digitalClockString() {
	// digital clock display of the time
	String str;
	str = String(hour());
	str += fillDigits(minute());
	str += fillDigits(second());
	str += " ";
	str += String(day());
	str += ".";
	str += String(month());
	str += ".";
	str += String (year());
	
	return str;
}

String CurrentCost::fillDigits(int digits) {
	// utility for digital clock display: prints preceding colon and leading 0
	String str;

	str = ":";
	if (digits < 10)
		str += '0';
	str += String(digits);

	return str;
}

void CurrentCost::save_kwh() {
#ifdef DEBUG
	Serial.println("Save kwh");
#endif

	CurrentCost *currentcost = s_ccost;

	StaticJsonBuffer<BUFFER_LENGTH> jsonBuffer;
	JsonObject& json = jsonBuffer.createObject();
	json["sensors"] = MAX_SENSORS;

	JsonArray& data1 = json.createNestedArray("kwh");
	for (int i = 0; i < MAX_SENSORS; i++)
		data1.add(double_with_n_digits(currentcost->sensor[i].kwh, 8));

	JsonArray& data1a = json.createNestedArray("kwh_hour");
	for (int i = 0; i < MAX_SENSORS; i++)
		data1a.add(double_with_n_digits(currentcost->sensor[i].kwh_hour, 8));

	JsonArray& data2 = json.createNestedArray("kwh_day");
	for (int i = 0; i < MAX_SENSORS; i++)
		data2.add(double_with_n_digits(currentcost->sensor[i].kwh_day, 8));

	JsonArray& data3 = json.createNestedArray("kwh_month");
	for (int i = 0; i < MAX_SENSORS; i++)
		data3.add(double_with_n_digits(currentcost->sensor[i].kwh_month, 8));

	JsonArray& data4 = json.createNestedArray("kwh_year");
	for (int i = 0; i < MAX_SENSORS; i++)
		data4.add(double_with_n_digits(currentcost->sensor[i].kwh_year, 8));

	File configFile = SPIFFS.open(KWH_FILE_NAME, "w");
	if (!configFile) {
		Serial.println("Failed to open config file for writing");
		//return false;
	}

#ifdef DEBUG
	String temp;
	json.prettyPrintTo(temp);
	Serial.println(temp);
#endif

	json.printTo(configFile);
	//return true;
}

boolean CurrentCost::load_kwh() {
	File configFile = SPIFFS.open(KWH_FILE_NAME, "r");
	if (!configFile) {
		Serial.println("Failed to open config file");
		return false;
	}

	size_t size = configFile.size();
	if (size > BUFFER_LENGTH) {
		Serial.println("Config file size is too large");
		return false;
	}

	// Allocate a buffer to store contents of the file.
	std::unique_ptr<char[]> buf(new char[size]);

	// We don't use String here because ArduinoJson library requires the input
	// buffer to be mutable. If you don't use ArduinoJson, you may as well
	// use configFile.readString instead.
	configFile.readBytes(buf.get(), size);
#ifdef DEBUG
	Serial.print("JSON file size: "); Serial.print(size); Serial.println(" bytes");
#endif

	StaticJsonBuffer<BUFFER_LENGTH> jsonBuffer;
	JsonObject& json = jsonBuffer.parseObject(buf.get());

#ifdef DEBUG
	String temp;
	json.prettyPrintTo(temp);
	Serial.println(temp);
#endif
	if (!json.success()) {
		Serial.println("Failed to parse config file");
		return false;
	}

	int sensor_number = json["sensors"];
	if (sensor_number > 0 && sensor_number <= MAX_SENSORS) {
		for (int i = 0; i < sensor_number; i++) {
			sensor[i].kwh = json["kwh"][i];
			sensor[i].kwh_hour = json["kwh_hour"][i];
			sensor[i].kwh_day = json["kwh_day"][i];
			sensor[i].kwh_month = json["kwh_month"][i];
			sensor[i].kwh_year = json["kwh_year"][i];
#ifdef DEBUG
			Serial.print("Data initialized. Sensor ");
			Serial.println(i);
#endif
		}

	}

	return true;
}

void CurrentCost::checkValidMeasure_task() {
	CurrentCost *currentcost = s_ccost;

	for (int i = 0; i < MAX_SENSORS; i++) {
		if (currentcost->sensor[i].diff > MEAS_VALIDITY) {
			currentcost->sensor[i].valid = false;
#ifdef DEBUG
			Serial.print(currentcost->sensor[i].diff);
			Serial.print(" sec. ");
			Serial.print("Invalidate measure ");
			Serial.println(i);
#endif // DEBUG

		}
	}
}

void CurrentCost::checkTimeReset() {
#ifdef DEBUG
	Serial.println("Check time reset");
#endif //DEBUG
	CurrentCost *currentcost = s_ccost;

	for (int i = 0; i < MAX_SENSORS; i++) {
#ifdef DEBUG
		Serial.print("Sensor ");
		Serial.print(i);
		Serial.print(". kWh today: ");
		Serial.print(currentcost->sensor[i].kwh_day);
		//Serial.print(" ");  
#endif // DEBUG
		currentcost->sensor[i].kwh_day = 0; // Time is 00:00:00. Time to reset sum
		if (day() == 1) { //1st day of month
#ifdef DEBUG
			Serial.print(". kWh this month: ");
			Serial.print(currentcost->sensor[i].kwh_month);
			//Serial.print(" ");  
#endif // DEBUG
			currentcost->sensor[i].kwh_month = 0;
			if (month() == 1) {//January
#ifdef DEBUG
				Serial.print(". kWh this year: ");
				Serial.print(currentcost->sensor[i].kwh_year);
				//Serial.print(" ");
#endif //DEBUG
				currentcost->sensor[i].kwh_year = 0;
			}
		}
	}

}


CurrentCost::CurrentCost() {
	// Initialize sensor data
	Serial.println("Initialize sensor data.");
	for (int i = 0; i < MAX_SENSORS; i++) {
		sensor[i].time_sensor = millis();
		sensor[i].last_time_sensor = millis();
		sensor[i].kwh = 0;
		sensor[i].kwh_hour = 0;
		sensor[i].kwh_day = 0;
		sensor[i].kwh_month = 0;
		sensor[i].kwh_year = 0;
	}
	load_kwh(); // Load measurement data from 
	s_ccost = this;
	CurrentCost *currentcost = s_ccost;
	
#ifdef DEBUG
	Serial.println("CurrentCost instance.");
#endif // DEBUG
	kwh_ticker.attach(KWH_FREQ, save_kwh); // Program measure saving task
	checkValidMeasure_ticker.attach(CHECKVALIDMEASURE_FREQ, currentcost->checkValidMeasure_task); // Program measure vaidity check task
	Alarm.alarmRepeat(0, 0, 0, currentcost->checkTimeReset); // Program reset time task
#ifdef TEST
	sendTestMessage_ticker.attach(6, currentcost->sendTestMessage); // Program send test message
#ifdef DEBUG
	Serial.println("\nCC Parser test started");
#endif
#endif
	
}
