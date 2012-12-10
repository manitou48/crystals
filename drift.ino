// drift   run with hostdrift, it sends 4 bytes, we reply with millis()

#define LED_PIN 13
void setup() {
	Serial.begin(9600);
	pinMode(LED_PIN,OUTPUT);
        OSCCAL = 0xA5;     // new 328p A6 for 8mhz mode
}

void loop(){
	static long cnt=0;
	long ms;

	while(Serial.available() < 4);   // wait for host request
	ms = millis();
	Serial.read();
	Serial.read();
	Serial.read();
	Serial.read();
	Serial.write((uint8_t *)&ms,4);
        cnt++;
	digitalWrite(LED_PIN, cnt & 1);
}

