#include "C8051F340.h"
#include <intrins.h>

sbit LED1 = P1^0;
sbit LED2 = P1^1;
sbit LED3 = P1^2;
sbit LED4 = P1^3;
sbit p_48 = P0^6;
bit led2_on = 0;

void setup() {
    PCA0MD &= ~0x40;
    //WDTE = 0;
    //int n = RSTSRC;

// Timer Init
    TMOD      = 0x22;
    CKCON     = 0x3C;
    TH0       = 0xB0;
    TH1       = 0xB0;
//    TMR2CN    = 0x08;
//    TMR2RLL   = 0xF6;
//    TMR2RLH   = 0xF6;
    TMR3RLL   = 0xAF;
    TMR3RLH   = 0x3C;

// Port IO Init
    // P0.0  -  SCK  (SPI0), Open-Drain, Digital
    // P0.1  -  MISO (SPI0), Open-Drain, Digital
    // P0.2  -  MOSI (SPI0), Open-Drain, Digital
    // P0.3  -  NSS  (SPI0), Open-Drain, Digital
    // P0.4  -  SDA (SMBus), Open-Drain, Digital
    // P0.5  -  SCL (SMBus), Open-Drain, Digital
    // P0.6  -  Unassigned,  Open-Drain, Digital
    // P0.7  -  Unassigned,  Open-Drain, Digital

    // P1.0  -  Unassigned,  Open-Drain, Digital
    // P1.1  -  Unassigned,  Open-Drain, Digital
    // P1.2  -  Unassigned,  Open-Drain, Digital
    // P1.3  -  Unassigned,  Open-Drain, Digital
    // P1.4  -  Unassigned,  Open-Drain, Digital
    // P1.5  -  Skipped,     Open-Drain, Analog
    // P1.6  -  Unassigned,  Open-Drain, Digital
    // P1.7  -  Unassigned,  Open-Drain, Digital

    // P2.0  -  Skipped,     Open-Drain, Analog
    // P2.1  -  Skipped,     Open-Drain, Analog
    // P2.2  -  Skipped,     Open-Drain, Analog
    // P2.3  -  Skipped,     Open-Drain, Analog
    // P2.4  -  Unassigned,  Open-Drain, Digital
    // P2.5  -  Unassigned,  Open-Drain, Digital
    // P2.6  -  Unassigned,  Open-Drain, Digital
    // P2.7  -  Unassigned,  Open-Drain, Digital

    // P3.0  -  Unassigned,  Open-Drain, Digital
    // P3.1  -  Unassigned,  Open-Drain, Digital
    // P3.2  -  Unassigned,  Open-Drain, Digital
    // P3.3  -  Unassigned,  Open-Drain, Digital
    // P3.4  -  Unassigned,  Open-Drain, Digital
    // P3.5  -  Unassigned,  Open-Drain, Digital
    // P3.6  -  Unassigned,  Open-Drain, Digital
    // P3.7  -  Unassigned,  Open-Drain, Digital

    P1MDIN    = 0xDF;
    P2MDIN    = 0xF0;
    P1SKIP    = 0x20;
    P2SKIP    = 0x0F;
    XBR0      = 0x07;
    XBR1      = 0xC0; //disable Pullup 0xC0;
}

enum {
    ON = 0,
    OFF,
    ON_2, // For Heart-beats
    OFF_LONG // For Heart-beats
};

void loop_sleep(int count) {
    int i;
    for (i = 0; i < count; i ++);
}

void led1_stm(int on_low, int on_high) { // breath
    static int PERIOD_LENGTH = 200;
    static int period_count = 0;
    static int state = ON;
    static int accumulator = 0;
    static int dly_div = 50;
    static bit growing = 1;

    switch (state) {
        case OFF:
            if (accumulator++ >= PERIOD_LENGTH) {
                state = ON;
                accumulator = dly_div;
                LED1 = 0;
            }
            break;
        case ON:
            if (accumulator-- <= 0) {
                state = OFF;
                accumulator = dly_div;
                LED1 = 1;
            }
            break;
        default:
            state = OFF;
    }

    if (period_count++ == 200) {
        period_count = 0;
        if (growing) {
            if (dly_div++ == on_high) growing = 0;
        } else {
            if (dly_div-- == on_low)  growing = 1;
        }
    }
}

void led2_stm() { // Heart beat
    static int period_count = 0;
    static int state = ON;

		period_count++;
    switch (state) {
        case ON:
            if (period_count == 500) {
                period_count = 0;
                state = OFF;
                LED3 = 1;
            }
            break;
        case OFF:
            if (period_count == 2500) {
                period_count = 0;
                state = ON_2;
                LED3 = 0;
            }
            break;
        case ON_2:
            if (period_count == 500) {
                period_count = 0;
                state = OFF_LONG;
                LED3 = 1;
            }
            break;
        case OFF_LONG:
            if (period_count == 6500) {
                period_count = 0;
                state = ON;
                LED3 = 0;
            }
            return;
        default:
            state = ON;
    }
}

void check_port48() {
    static bit last = 0;
    
    if (p_48 == 1 && last == 0) {
      led2_on = ~led2_on;
    }
    last = p_48;
}

// void check_port48() {
//     static char last = 0;
//     static int ind = 0;
//     
//     if (p_48 == 1 && last == 0)
//         led2_on = ~led2_on;
//     last ^= (- ((int) p_48) ^ last) & (1 << ind);
//     if (ind++ == 7) ind = 0;
// }

void loop() {
    led1_stm(-1, 100);
    if (led2_on)
        led2_stm();
    else if (LED2 == 0)
        LED2 = 1;
    check_port48();
}

void main() {
    setup();
    while (1) {
        loop();
				_nop_();
        //loop_sleep(2);
    }
}
