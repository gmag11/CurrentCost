// ntpClient.h

#ifndef _NTPCLIENT_h
#define _NTPCLIENT_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include <Time2.h>
#include <WiFiUdp.h>
#include <ESP8266WiFi.h>

//#include <Udp.h>

#define DEFAULT_NTP_SERVER "pool.ntp.org"
#define DEFAULT_NTP_PORT 123

#define NTP_SERVER_NAME_SIZE 60

const int NTP_PACKET_SIZE = 48; // NTP time is in the first 48 bytes of message


class ntpClient {
public:
	ntpClient() {
		ntpClient(DEFAULT_NTP_PORT, DEFAULT_NTP_SERVER);
	}

	ntpClient(int udpPort) {
		ntpClient(udpPort, DEFAULT_NTP_SERVER);
	}

	ntpClient(String ntpServerName) {
		ntpClient(DEFAULT_NTP_PORT, ntpServerName);
	}

	ntpClient (int udpPort, String ntpServerName);

	//~ntpClient ();

	//boolean begin();

	//boolean stop();

	time_t getNtpTime();

	//String getTimeString();

	boolean	setUdpPort(int port);
	int		getUdpPort();

	/*boolean setInterval(int interval);
	int		getInterval();*/

	boolean	setNtpServerName(String ntpServerName);
	String getNtpServerName();

	boolean setTimeZone(int timeZone);
	int getTimeZone();

protected:
	boolean sendNTPpacket(IPAddress &address);
	time_t decodeNtpMessage(byte *messageBuffer);
	void nullSyncProvider();
	String printDigits(int digits);

private:
	int _udpPort;
	char _ntpServerName[NTP_SERVER_NAME_SIZE];
	IPAddress _timeServerIP;
	//int interval;
	int _timeZone;
	WiFiUDP _udp;
	byte ntpPacketBuffer[NTP_PACKET_SIZE];
	


};

//extern ntpClient Ntp;

#endif