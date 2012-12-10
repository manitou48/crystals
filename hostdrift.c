//  hostdrift -f tickrate -i /dev/tty -b baud
//    default  -f 1000  -i /dev/ttyACM0    -b 9600
//  -d secs option for one byte uptick  (TODO flawed)
// hostdrift.c talk to drift.c
// send 4 bytes, get back 4 bytes millis() LSB first
//   uno default,   ds3234 ds3231  -f 1024        ds1307 -f 4096
//  jazz -i /dev/ttyUSB0 -b 19200
//  robot spin -i /dev/ttyUSB0 -b 115200
//  maple  -b 115200

#include <ctype.h>
#include <stdlib.h>
#include <termios.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>
#include <sys/types.h>

#define DATABITS CS8
#define STOPBITS 0
#define PARITY 0
#define PARITYON 0

static double tickrate = 1000.;
static int baud = B9600;
static char *devtty = "/dev/ttyACM0", *baudstr = "9600";;
static int uptick = 0;

/* rclock.c   return elapsed time */
double
rclock()
{
#include <sys/time.h>
    struct timeval t;
    (void) gettimeofday(&t, (struct timezone *)0);
    return(t.tv_sec+ t.tv_usec/1.e6);
}


void doopts(int argc, char *argv[]) {
	int c;

     while ((c = getopt (argc, argv, "f:i:b:d:")) != -1)
         switch (c)
           {
           case 'i':
             devtty = optarg;
             break;
           case 'b':
             baudstr = optarg;
			 if (strcmp(baudstr,"9600") == 0) baud = B9600;
				else if (strcmp(baudstr,"19200") == 0) baud = B19200;
				else if (strcmp(baudstr,"115200") == 0) baud = B115200;
             break;
           case 'd':
             uptick = atoi(optarg);
             break;
           case 'f':
             tickrate = atof(optarg);
             break;
           default:
             abort ();
           }
}

int dotty() {
	int fd;
	struct termios oldtio, newtio;

      fd = open(devtty, O_RDWR | O_NOCTTY );
      if (fd < 0)
      {
         perror(devtty);
         exit(-1);
      }
	tcgetattr(fd,&oldtio); // save current port settings
      newtio.c_cflag = baud |  DATABITS | STOPBITS | PARITYON | PARITY | CLOCAL | CREAD;
      newtio.c_iflag = IGNPAR;
      newtio.c_oflag = 0;
      newtio.c_lflag = 0;       //ICANON;
      newtio.c_cc[VMIN]=1;
      newtio.c_cc[VTIME]=0;
      tcflush(fd, TCIFLUSH);
      tcsetattr(fd,TCSANOW,&newtio);
	  return fd;
}

void douptick(int fd) {
	// remote sends a byte every uptick seconds
	// hostdrift -d 10
	double t,t0=0.0,t1;
	int n,i,secs=0,ppm;

	read(fd,&i,1);    // discard first
	while(1) {
		n = read(fd,&i,1);
		if (n !=1) {
			printf("read %d\n",n);
			perror("read");
		}
		if (t0 == 0.0) {
			t0 = rclock();
			secs = 0;
		} else {
			t = rclock();
			secs += uptick;
			ppm = 1.e6 * (secs - (t-t0))/(t-t0);
			printf("%f  %d %f  %d \n",t,secs,t-t0, ppm);
		}
		
	}
}


main(int argc, char *argv[]) {
	int fd,i,n,cnt=0,reps,ms,ms0=0,ppm;
	unsigned char in, byte = 'A',*v;
	double t,t0,t1;

	  doopts(argc,argv);
	  fprintf(stderr,"%s at %s baud with tick rate %.0f  uptick %d\n",
	   devtty,baudstr,tickrate,uptick);
	  fd = dotty();

	sleep(2);   // let bootloader get out of the way
	if (uptick) douptick(fd); // pps method
	v = (unsigned char *) &ms;
	t0=rclock();
	while(1) {
		time_t utime;
		t=rclock();
		utime = time(NULL);
		write(fd,&utime,4);		//  send unixtime
		n = read(fd,&v[0],1);
		if (n !=1) {
			printf("read %d\n",n);
			perror("read");
		}
		n = read(fd,&v[1],1);
		n = read(fd,&v[2],1);
		n = read(fd,&v[3],1);
		t1=(rclock() -t)/2;   // one way time   don't need this for drift
		if (ms0 == 0) {
			ms0 =ms ;
			t0 = t ;
		}
		ppm = 1.e6 * (((ms-ms0)/tickrate) - (t-t0))/(t-t0);
		printf("%f  %d %f %f %d \n",t,ms,t-t0, (ms-ms0)/tickrate,ppm);

		cnt++;
		sleep(10);
	}
}

