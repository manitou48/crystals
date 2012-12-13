// due rtt   32khz or RC oscillator
//  prescaler and alarms

#define BOARD_LED_PIN 13
#define PRESCALE 32
#define TICKSPERSEC 32768/PRESCALE
volatile static unsigned long ticks,us;
static unsigned long us0=0;
static uint8_t hr,min,sec,day,m,wk;
static uint16_t yr;


void RTT_Handler() {
	uint32_t status = RTT->RTT_SR;

	RTT->RTT_MR &=  ~RTT_MR_RTTINCIEN;  //disable interrupt
 //Serial.println(status,HEX);
    if (us0 == 0) us0 = micros();
     else {
        ticks++;
        us = micros() - us0;
    }  
    while(RTT->RTT_SR);   // wait for status to clear
	RTT->RTT_MR |=  RTT_MR_RTTINCIEN;  //enable interrupt
}

void setup() {

    pinMode(BOARD_LED_PIN,OUTPUT);
	Serial.begin(9600);

// comment these next two to run with the RC
	pmc_switch_sclk_to_32kxtal(PMC_OSC_XTAL);
	while (!pmc_osc_is_ready_32kxtal());
	NVIC_DisableIRQ(RTT_IRQn);
	NVIC_ClearPendingIRQ(RTT_IRQn);
	NVIC_SetPriority(RTT_IRQn, 0);
	RTT->RTT_MR = (PRESCALE |  RTT_MR_RTTRST | RTT_MR_RTTINCIEN);
 	NVIC_EnableIRQ(RTT_IRQn);
}

void loop() {
//	display();
	hostchk();      // with hostdrift -b 9600 -f 1024
}

void display() {
    // compare micros to pps
    int ppm, tprev=0;
    char str[64];

    while(1) {
        ppm = 1000000*ticks - us;
        ppm = 1.e6 * ppm/ (float)us / TICKSPERSEC;
        sprintf(str,"%d %d %f %d",ticks,ticks-tprev,us*1.e-6,ppm);
        Serial.println(str);
//        Serial.println(RTT->RTT_MR,HEX);
         tprev = ticks;
        delay(5000);
    }
}


void hostchk(){
    long t;
	static int b=0;

    while(Serial.available() < 4);   // wait for host request
    t = ticks;
    Serial.read();
    Serial.read();
    Serial.read();
    Serial.read();
    Serial.write((uint8_t *)&t,4);
  	digitalWrite(BOARD_LED_PIN, b = !b);  // toggle
}
