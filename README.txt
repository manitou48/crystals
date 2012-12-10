Calibrating crystals, resonators, RC oscillators using NTP host.


Files:

crystals.txt  measured ppm frequency offset for various devices

hostdrift.c  NTP host program to pull MCU

drift.ino    MCU program to send millis() to NTP host

ds3231.ino   MCU program with RTC to send 1khz ticks to NTP host

URL's:



--------------  details --------------------

A linux host running NTP on a good broadband network can disipline its
time to within a few milliseconds and adjust its frequency (drift) to within
one part-per-million(ppm).  I have a good network connection and a stratum 0
CDMA time server as well.  A simple linux host program polls the mcu
every 10 seconds or so and collects the millis() (or ticks for RTCs) from
the MCU and can caculate the difference in frequency between the NTP
host and MCU/RTC.  The longer the data collection runs, the more accurate
the frequency estimate (ppm).

The AVR (arduino) processors can run off an internal 8MHz RC oscillator.
The RC circuit is precalibrated at the factory (1%), but you can adjust
the RC oscillaor by changing in the OSCCAL value. We measured frequency
offset of various 328 chips and adjust OSCCAL.  Frequency is affected
by temperature and voltage. Two of the 328p's I have had an optimum OSCCAL,
one other was improved by changing the OSCCAL value by a few units.
A unit of OSCCAL varies by chip, but it's around 4000 ppm. 
See avr328p datasheet figure 29-175 and www.atmel.com/Images/doc2555.pdf
and  http://arduino.cc/en/Tutorial/ArduinoToBreadboard


Using millis() to display offset of ...
