{{ gpspps
  measure PPS from GPS,  on pin 8
  https://www.sparkfun.com/products/465  
  5v,grnd,  pps is 2.87v OK
}}

CON
  _clkmode = xtal1 + pll16x
  _xinfreq = 5_000_000

  CR            = $0D

VAR
  long Cycles, theCog

OBJ
  pst           : "Parallax Serial Terminal"


PUB Main  | ppm

  pst.Start(115200)
  theCog := cognew (@gpsPPS, @Cycles)                   'start with LED on P16 
  waitcnt(clkfreq*4 + cnt)      ' Wait seconds
  pst.str(string(" cog started",CR))

  repeat
    Cycles := 0
    repeat while Cycles == 0
    ppm:= (||Cycles/(clkfreq / 1_000_000))
    ppm -= 1_000_000
    pst.Dec(Cycles)
    pst.Char($20)
    pst.Dec(ppm)
    pst.str(string(" ppm",CR))

DAT
        {measure pusle width on gps pin}

              org 0                             'Begin at Cog RAM addr 0
gpsPPS        andn dira, gpsPin                 'Set to input 
              
Chkpin        waitpeq gpsPin,gpsPin		'wait til high
'Chkpin        test gpsPin, ina  wz
'     if_z    jmp #Chkpin				'loop if still low
              mov Now, cnt
              sub Prev, Now             'delta cycles
              wrlong Prev, par			'write result to caller
              mov Prev, Now
              mov ScratchMem, cnt               'Calculate delay time
              add ScratchMem, Delay               'Calculate delay time
              waitcnt ScratchMem, #0          'Wait a bit for pin to reset
              jmp #Chkpin

                             
gpsPin        long      $00000100             'Pin bit mask for pin 8
Prev          long      0
Now           long      0
Delay         long      1_000_000              'Clock cycles to delay
ScratchMem    res       1                      'System Counter Workspace
