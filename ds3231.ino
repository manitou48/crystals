// DS3231 RTC i2c  temperature
// http://docs.macetech.com/doku.php/chronodot
//  A4/A5  SDA,SCL  (uno has internal pullups)   3.3-5v grnd
// SQ needs 4.5K/10k pullup for pulse out, external interrupt pin 3
// can't disable display and start up host program, so ifef
//  HOSTSET screws up data for HOSTCHECK

#include <Wire.h>
#include <SPI.h>
#include "RTClib.h"
#include "RTC_DS1307.h"

// define for hostdrift -f 1024    HOSTSET too if you want to set time
//#define HOSTCHECK
//#define HOSTSET

#define I2CID 0x68
#define LED_PIN 13

int mode;
RTC_DS1307 RTC;

volatile unsigned long ticks,us;

void handler() {
	us = micros(); // timestamp tick
	ticks++;
}
 
void setup()
{
  Wire.begin();
  Serial.begin(9600);
  pinMode(LED_PIN,OUTPUT);
#ifdef HOSTCHECK
	enablepps(0);
	check();
#endif
  dump();
 
  // clear /EOSC bit
  // Sometimes necessary to ensure that the clock
  // keeps running on just battery power. Once set,
  // it shouldn't need to be reset but it's a good
  // idea to make sure.
  Wire.beginTransmission(I2CID); // address DS3231
  Wire.write(0x0E); // select register
  Wire.write(0b00011100); // write register bitmap, bit 7 is /EOSC
  Wire.endTransmission();

  RTC.begin();
  if (! RTC.isrunning()) {
  	Serial.println("RTC is NOT running!");
  }
}

void loop() {
	uint32_t utime;
    while(!Serial.available() ) {
		showhms();
		Serial.print(ticks);
        Serial.println(" enter command");
        delay(3000);
    }
    mode = Serial.read();
    switch (mode) {
     case '2':
        dump();
        showtime();
        break;
     case '3':
        enablepps(0);
        dump();
        break;
     case '4':
        enablepps(1);     // pps 1 hz
		dump();
        chkpps();
        break;
     case '8':
        // adjust using unixtime
        utime = 1353885757L -5*3600;  // 11/25/12 18:22:37 ish
        RTC.adjust(utime);
        Serial.println("time adjusted");
        showtime();
        break;
     case '9':
        // following line sets the RTC to the date & time this sketch was compiled
        RTC.adjust(DateTime(__DATE__, __TIME__));
        Serial.println("time adjusted");
        showtime();
        break;
     case '0':
     default:
        showtime();
        break;

    }
}

 
void showtime () {
    DateTime now = RTC.now();

    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(' ');
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();
}

void showhms()
{
  int addr=0;
  // send request to receive data starting at register 0
  Wire.beginTransmission(I2CID); // 0x68 is DS3231 device address
  Wire.write(addr); // start at register 0
  Wire.endTransmission();
  Wire.requestFrom(I2CID, 3); // request three bytes (seconds, minutes, hours)
 
  while(Wire.available())
  { 
    int seconds = Wire.read(); // get seconds
    int minutes = Wire.read(); // get minutes
    int hours = Wire.read();   // get hours
 
    seconds = (((seconds & 0b11110000)>>4)*10 + (seconds & 0b00001111)); // convert BCD to decimal
    minutes = (((minutes & 0b11110000)>>4)*10 + (minutes & 0b00001111)); // convert BCD to decimal
    hours = (((hours & 0b00110000)>>4)*10 + (hours & 0b00001111)); // convert BCD to decimal (assume 24 hour mode)
 
    Serial.print(hours); Serial.print(":"); Serial.print(minutes); Serial.print(":"); Serial.println(seconds);
  }
}

void check() {
    static long cnt=0;
    unsigned long ms,utime;
	char *p = (char *) &utime;

  while(1) {
    while(Serial.available() < 4);   // wait for host request
    ms = ticks;
    p[0] = Serial.read();   // unix time
    p[1] = Serial.read();
    p[2] = Serial.read();
    p[3] = Serial.read();
#ifdef HOSTSET
    if (cnt == 0) RTC.adjust(DateTime(utime - 5*3600)); // adjust from host once
#endif
    Serial.write((uint8_t *)&ms,4);
    cnt++;
    digitalWrite(LED_PIN, cnt & 1);
  }
}

void enablepps(int dopps) {
    int addr= 0x0e;
    char c = 0x08; // 0x08 1 kHz   0 1hz  pps

	if (dopps) c = 0; // pps
    attachInterrupt(1,handler,FALLING);
    Wire.beginTransmission(I2CID);
    Wire.write(addr);
    Wire.write(c);
    Wire.endTransmission();
}


void chkpps() {
  // need to run pulse at 1 pps
  static unsigned long prev = 0;
  unsigned long t;
  char str[32];

  ticks = 0;
  while(!Serial.available()){
    if (ticks)  {
            t= us-prev;
            sprintf(str,"%ld us  %ld ppm",t,t-1000000);
            Serial.println(str);
            ticks=0;
            prev=us;
    }
  }
  return;
}

void dump() {
    int addr=0;
    unsigned char c;

    Wire.beginTransmission(I2CID);
    Wire.write(addr);
    Wire.endTransmission();
    Wire.requestFrom(I2CID, 19);

    while (Wire.available()) {
        c = Wire.read() ;
        Serial.print(c,HEX);
        Serial.print(" ");
    }
    Serial.println();
}

