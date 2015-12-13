// 
// 
// 

#include "ntpClient.h"
//#include "Arduino.h"

ntpClient::ntpClient(int udpPort, String ntpServerName) {
	_udpPort = udpPort;
	memset(_ntpServerName, 0, NTP_SERVER_NAME_SIZE);
	memset(ntpPacketBuffer, 0, NTP_PACKET_SIZE);
	//_ntpServerName = ntpServerName.c_str();
	ntpServerName.toCharArray(_ntpServerName, NTP_SERVER_NAME_SIZE);
	Serial.println(_ntpServerName);
	//this->interval = interval;
	_timeZone = 1;
}

/*boolean ntpClient::begin() {
	setSyncProvider(this->getNtpTime);
	setSyncInterval(this->interval);
}

boolean ntpClient::stop() {
	setSyncProvider(this->nullSyncProvider);
	return true;
}*/

time_t ntpClient::decodeNtpMessage(byte *messageBuffer) {
	unsigned long secsSince1900;
	// convert four bytes starting at location 40 to a long integer
	secsSince1900 = (unsigned long)messageBuffer[40] << 24;
	secsSince1900 |= (unsigned long)messageBuffer[41] << 16;
	secsSince1900 |= (unsigned long)messageBuffer[42] << 8;
	secsSince1900 |= (unsigned long)messageBuffer[43];
	
#define SEVENTY_YEARS 2208988800UL
	return secsSince1900 - SEVENTY_YEARS + _timeZone * SECS_PER_HOUR;
}

void ntpClient::nullSyncProvider() {

}

time_t ntpClient::getNtpTime() {
	if (WiFi.status() == WL_CONNECTED) {
#ifdef DEBUG
		Serial.println("Starting UDP");
#endif
		_udp.begin(_udpPort);
#ifdef DEBUG
		Serial.print("Local port: ");
		Serial.println(_udp.localPort());
#endif
		while (_udp.parsePacket() > 0); // discard any previously received packets
		Serial.print("NTP Server hostname: ");
		uint8_t dnsResult = WiFi.hostByName(_ntpServerName, _timeServerIP);
		Serial.print(dnsResult);
		Serial.print(" ");
		Serial.println(_timeServerIP);
		Serial.println("-- Wifi Connected. Waiting for sync");
#ifdef DEBUG
		Serial.println("-- Transmit NTP Request");
#endif
		if (dnsResult != 0) {
			sendNTPpacket(_timeServerIP);
			uint32_t beginWait = millis();
			while (millis() - beginWait < 1500) {
				int size = _udp.parsePacket();
				if (size >= NTP_PACKET_SIZE) {
#ifdef DEBUG
					Serial.println("-- Receive NTP Response");
#endif
					_udp.read(ntpPacketBuffer, NTP_PACKET_SIZE);  // read packet into the buffer
					time_t timeValue = decodeNtpMessage(ntpPacketBuffer);
					setSyncInterval(/*NTP_SYNC_PERIOD*/86000);
					Serial.println("Sync Period set low");
					_udp.stop();

					return timeValue;
				}
			}
			Serial.println("-- No NTP Response :-(");
			_udp.stop();

			return 0; // return 0 if unable to get the time 
		}
		else {
			Serial.println("-- Invalid address :-((");
			_udp.stop();

			return 0; // return 0 if unable to get the time 
		}
	}
	else {
#ifdef DEBUG
		Serial.println("-- NTP Error. Not connected");
#endif // DEBUG
		return 0;
	}
}

/*String ntpClient::getTimeString() {
	String timeStr = "";
	timeStr += String(hour());
	timeStr += ":";
	timeStr += printDigits(minute());
	timeStr += ":";
	timeStr += printDigits(second());
	timeStr += " ";
	timeStr += printDigits(day());
	timeStr += "/";
	timeStr += printDigits(month());
	timeStr += "/";
	timeStr += String(year());

	return timeStr;
}*/

String ntpClient::printDigits(int digits) {
	// utility for digital clock display: prints preceding colon and leading 0
	String digStr = "";

	if (digits < 10)
		digStr += '0';
	digStr += String(digits);

	return digStr;
}


boolean ntpClient::setUdpPort(int port)
{
	if (port > 0 || port < 65535) {
		_udpPort = port;
		return true;
	}
	else
		return false;
}

int ntpClient::getUdpPort()
{
	return _udpPort;
}

/*boolean ntpClient::setInterval(int interval)
{
	if (interval > 15) {
		if (this->interval != interval)	{
			this->interval = interval;
			setSyncInterval(this->interval);
		}
		return true;
	} else
		return false;
}

int ntpClient::getInterval()
{
	return this->interval;
}*/

boolean ntpClient::setNtpServerName(String ntpServerName) {
	memset(_ntpServerName, 0, NTP_SERVER_NAME_SIZE);
	ntpServerName.toCharArray(_ntpServerName, NTP_SERVER_NAME_SIZE);
	return true;
}

String ntpClient::getNtpServerName()
{
	return String(_ntpServerName);
}

boolean ntpClient::setTimeZone(int timeZone)
{
	if (timeZone >= -13 || timeZone <= 13) {
		_timeZone = timeZone;
		return true;
	} else 
		return false;
}

int ntpClient::getTimeZone()
{
	return _timeZone;
}

// send an NTP request to the time server at the given address
boolean ntpClient::sendNTPpacket(IPAddress &address) {
	// set all bytes in the buffer to 0
	memset(ntpPacketBuffer, 0, NTP_PACKET_SIZE);
	// Initialize values needed to form NTP request
	// (see URL above for details on the packets)
	ntpPacketBuffer[0] = 0b11100011;   // LI, Version, Mode
	ntpPacketBuffer[1] = 0;     // Stratum, or type of clock
	ntpPacketBuffer[2] = 6;     // Polling Interval
	ntpPacketBuffer[3] = 0xEC;  // Peer Clock Precision
							 // 8 bytes of zero for Root Delay & Root Dispersion
	ntpPacketBuffer[12] = 49;
	ntpPacketBuffer[13] = 0x4E;
	ntpPacketBuffer[14] = 49;
	ntpPacketBuffer[15] = 52;
	// all NTP fields have been given values, now
	// you can send a packet requesting a timestamp:                 
	_udp.beginPacket(address, 123); //NTP requests are to port 123
	_udp.write(ntpPacketBuffer, NTP_PACKET_SIZE);
	_udp.endPacket();
	return true;
}
