#ifndef CCost_h
#define CCost_h
# define BUFFER_LENGTH 625


void process_ccost_xml_test(String msg) {
	//Serial.println("process_ccost_xml");
	//delay(1000);
	int sensor_id = random(0, MAX_SENSORS);
	//Serial.println (sensor_id);
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

void process_ccost_xml(String msg) {
	//sensor_t sensor_data;
	if (msg.indexOf("<hist>") == -1) {
		int idx = msg.indexOf("<sensor>");
		//String str_sensor = msg.substring(idx_sensor + 8, idx_sensor + 9);
		int sensor_id = (uint8_t)(msg.substring(idx + 8, idx + 9).toInt());
		//Serial.println(sensor_id);
		if ((sensor_id >= 0) || (sensor_id < MAX_SENSORS)) {
			sensor[sensor_id].sensor_id = sensor_id;
			idx = msg.indexOf("<watts>");
			sensor[sensor_id].watts = msg.substring(idx + 7, idx + 12).toInt();
			idx = msg.indexOf("<tmpr>");
			sensor[sensor_id].tempr = msg.substring(idx + 6, idx + 10).toFloat();

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
	}
}

void show_sensor_data() {
	if (last_read_sensor == 0 || last_read_sensor == 1) {
		Serial.print(digitalClockString());
		Serial.print(" ");
		Serial.print(sensor[last_read_sensor].kwh, 6);//Delta time: %d
		Serial.print(" kWh. for ");
		Serial.print((double)((double)sensor[last_read_sensor].diff / 1000));
		Serial.print(" Sec. ");
		Serial.print("Sensor: ");
		Serial.print(sensor[last_read_sensor].sensor_id);
		Serial.print(" --> ");
		Serial.print(sensor[last_read_sensor].watts);
		Serial.print(" W. Temperature: ");
		Serial.print(sensor[last_read_sensor].tempr);
		Serial.println("");
	}
}

boolean save_kwh() {
	flag_kwh = false;
#ifdef DEBUG
	Serial.println("Save kwh");
#endif
	StaticJsonBuffer<BUFFER_LENGTH> jsonBuffer;
	JsonObject& json = jsonBuffer.createObject();
	json["sensors"] = MAX_SENSORS;

	JsonArray& data1 = json.createNestedArray("kwh");
	for (int i = 0; i < MAX_SENSORS; i++)
		data1.add(double_with_n_digits(sensor[i].kwh, 8));

	JsonArray& data1a = json.createNestedArray("kwh_hour");
	for (int i = 0; i < MAX_SENSORS; i++)
		data1a.add(double_with_n_digits(sensor[i].kwh_hour, 8));

	JsonArray& data2 = json.createNestedArray("kwh_day");
	for (int i = 0; i < MAX_SENSORS; i++)
		data2.add(double_with_n_digits(sensor[i].kwh_day, 8));

	JsonArray& data3 = json.createNestedArray("kwh_month");
	for (int i = 0; i < MAX_SENSORS; i++)
		data3.add(double_with_n_digits(sensor[i].kwh_month, 8));

	JsonArray& data4 = json.createNestedArray("kwh_year");
	for (int i = 0; i < MAX_SENSORS; i++)
		data4.add(double_with_n_digits(sensor[i].kwh_year, 8));

	File configFile = SPIFFS.open("/kwh.json", "w");
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

boolean load_kwh() {
	File configFile = SPIFFS.open("/kwh.json", "r");
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

#endif // !CCost_h
