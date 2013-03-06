// gpspps
//  5v,grnd,  pps to pin 3   INT 1  
//    gpx tx-pin 4/rx  rx-pin 5   serial interface NOT used

volatile unsigned long us;
volatile int tick=0;

void handler() {
	us = micros();
	tick=1;
}

void setup() {
	Serial.begin(9600);
	attachInterrupt(1,handler,RISING);
}

void loop() {
    static unsigned long prev = 0;
    unsigned long t;
    char str[32];
	if (tick)  {
            t= us-prev;
            sprintf(str,"%ld us  %ld ppm",t,t-1000000);
			Serial.println(str);
			tick=0;
            prev=us;
	}
}
