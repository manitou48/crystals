    Crystals


Calibrating crystals, resonators, RC oscillators using NTP host.
  Arduino UNO, nano, mega2560,  DUE, teensy 3/LC, maple, propeller,  RTCs


Files:

crystals.txt  measured ppm frequency offset for various devices

hostdrift.c  NTP host program to poll MCU

drift.ino    MCU program to send millis() to NTP host

drift.spin   propeller program to send micros to NTP host

Drift.java   java program to send millis to NTP host for ridgesoft robot

gpspps.spin  propeller program to use GPS pps and cnt

ds3231.ino   MCU program with RTC to send 1khz ticks to NTP host

rtt.ino      DUE sketch to measure RTT/RTC crystal frequency

gpspps.ino   UNO sketch to use GPS pps signal and micros() 

teensyRTC.pde teensy RTC experiments 32KHz crystal

--------------  details --------------------

A linux host running NTP on a good broadband network can adjust its
time to within a few milliseconds and discipline its frequency (drift) to within
one part-per-million(ppm).  I have a good network connection and a stratum 0
CDMA time server as well.  A simple linux host program polls the mcu
every 10 seconds or so and collects the millis() (or ticks for RTCs) from
the MCU and can calculate the difference in frequency between the NTP
host and MCU/RTC.  The longer the data collection runs, the more accurate
the frequency estimate (ppm).  Most devices I tested ran within their
specs -- except for one bad 32KHz crystal (see crystals.txt).

The AVR (arduino) processors can run off an internal 8MHz RC oscillator.
The RC circuit is precalibrated at the factory (10%), but you can adjust
the RC oscillaor by changing in the OSCCAL value. I measured frequency
offset of various 328 chips and adjust OSCCAL.  Frequency is affected
by temperature and voltage. Two of the 328p's I have had an optimum OSCCAL,
one other was improved by changing the OSCCAL value by a few units.
A unit of OSCCAL varies by chip, but it's around 4000 ppm though non-linear. 
See avr328p datasheet figure 29-175, sections 8.6, Table 28-1,
and www.atmel.com/Images/doc2555.pdf
and  http://arduino.cc/en/Tutorial/ArduinoToBreadboard

ridgesoft robot has ATMEGA128 with java VM.  No bill of materials but
we believe oscillator is 30-ppm 14.7456Mhz crystal.  The drift of 1736 ppm 
suggests the Java VM is using a prescaler of 64 (doesn't evenly divide
frequency) so millis() error is 400/230000 or 1739 ppm.  A different software
implementation could correct this error. Using hostdrift -f 1001.73913
results in drift error of only 2.3 ppm.

The Maxim DS-series RTC's have a SQuare wave output pin that can be enabled.
I connected the SQ output to an input pin counted ticks on FALLING edge
of pin interrupts.  The frequency of the square wave was set at 1024 ticks
per second (4096 for the DS1307).  The TCXO RTC's ran within 1 ppm of
the NTP host.

 http://www.maximintegrated.com/datasheet/index.mvp/id/4627?ver=C&utm_expid=50713806-2

I conencted a 32Khz crystal to Maple and to breadboard 328p (with and
without 22pf load capacitors). Frequency is affected by load capacitance,
temperature, voltage and aging.

I connected a 32KHz crystal to teensy 3.0 (no capacitors required).
It was accurate to within 4ppm using micros() on teensy and pulse-per-second
interrupt from the RTC.  The ARM processor RTC crystal on the teensy can be
configured with different load capacitance and has calibration registers to
adjust the frequency (see rtc_calibrate());  The MCU crystal capacitance
can also be adjusted on the teensy ARM processors (OSC0_CS).

As a second test, hooked up a GPS module's pps pulse to pin 3 on UNO
and used micros() to measure frequency difference.  Results matched test
with NTP host (sketch gpspps.ino), and Sparkfun GPS
  https://www.sparkfun.com/products/465
We ran a similar sketch on the Maple, teensy, propeller, pyboard,
and DUE with frequency measurements matching the NTP tests.  

Besides the cut of the crystals, frequency is affected by load capacitance,
temperature, voltage, and aging.
Here are similar experiments by Joris using the pps from a GPS module
at various temperatures.

  http://jorisvr.nl/arduino_frequency.html

and another temperature study
  http://forum.pjrc.com/threads/24628-Interesting-Temperature-Data

crystal frequency vs temperature
  https://ieee-uffc.org/frequency-control/educational-resources/introduction-to-quartz-frequency-standards-by-john-r-vig/introduction-to-quartz-frequency-standards-static-frequency-versus-temperature-stability/

“One accurate measurement is worth a thousand expert opinions” 
  - Adm Grace Murray Hopper ( Dec 9 1906 to Jan 1 1992)
