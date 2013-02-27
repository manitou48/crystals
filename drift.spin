'  drift run with  hostdrift, send millis on recv 4 bytes
CON

  _clkmode = xtal1 + pll16x
  _xinfreq = 5_000_000

VAR
  long ms,ms0,t0,t1,host

OBJ

   pst : "Parallax Serial Terminal"


PUB drift                       ' Method declaration
    pst.start(115200)                 ' Start PST on another cog.
    dira[17] :=1
    t0 := cnt
    ms := 0

    repeat                            ' Endless loop prevents
                                      ' program from ending.

        host.byte[0] := pst.CharIn           ' Get value
        host.byte[1] := pst.CharIn           ' Get value
        host.byte[2] := pst.CharIn           ' Get value
        host.byte[3] := pst.CharIn           ' Get value
        t1 := cnt
        ms0 := ||( t1-t0  ) / (clkfreq /1_000)                  ' ms
	ms := ms + ms0
	t0 := t1

        pst.Char(ms.byte[0])
        pst.Char(ms.byte[1])
        pst.Char(ms.byte[2])
        pst.Char(ms.byte[3])
        !outa[17]               ' toggle

