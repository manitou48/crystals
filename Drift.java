/*
 * Drift.java with hostdrift.c
 *  ridgesoft intellibrain2 robot with atmega128 with 14.7mhz oscillator
 */

import com.ridgesoft.intellibrain.IntelliBrain;
import javax.comm.SerialPort;
import java.io.InputStream;
import java.io.OutputStream;
import com.ridgesoft.io.LED;

public class Drift {
	static final int BUFSIZE = 64;
    public static void main(String args[]) {
        try {
            // uncomment one line to choose a COM port
         SerialPort comPort = IntelliBrain.getCom1();
//           SerialPort comPort = IntelliBrain.getCom2();

            comPort.setSerialPortParams(115200, SerialPort.DATABITS_8,
                    SerialPort.STOPBITS_1, SerialPort.PARITY_NONE);
            InputStream inputStream = comPort.getInputStream();
            OutputStream outputStream = comPort.getOutputStream();
            int bytes,ms;
			byte[] buffer = new byte[BUFSIZE];

			LED led1 = IntelliBrain.getUserLed(1);
			LED led2 = IntelliBrain.getUserLed(2);
			led1.toggle();
			while(true) {
				bytes = inputStream.available();
				if (bytes == -1) break;
				if (bytes > 3) {
					ms = (int) System.currentTimeMillis();
					inputStream.read(buffer,0,bytes);
					led1.toggle();
					buffer[0] = (byte) (ms );
					buffer[1] = (byte) (ms >> 8);
					buffer[2] = (byte) (ms >> 16);
					buffer[3] = (byte) (ms >> 24);
					outputStream.write(buffer,0,4);
				}
			}
			led1.off();
			led2.off();
        }
        catch (Exception e) {
            e.printStackTrace();
        }
    }
}
