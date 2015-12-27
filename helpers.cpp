#include "helpers.h"

void flasher() {
	if (digitalRead(PIN_CONN) == 0)
		digitalWrite(PIN_CONN, HIGH);
	else
		digitalWrite(PIN_CONN, LOW);
}
