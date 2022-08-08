#ifndef _HAL_H_
#define _HAL_H_

#include  "../header/BSP.h"    		    // private library - BSP layer
#include  "../header/Application.h"     // private library - APP layer
#include  <msp430g2553.h>               // MSP430x2xx
//#include  <msp430xG46x.h>             // MSP430x4xx 

extern enum FSMstate state;   // global variable
extern enum SYSmode lpm_mode; // global variable

extern int tx;

// System
extern void sysConfig(void);
extern void enterLPM(unsigned char);

// Interrupts
void _buttonDebounceDelay(int button);

extern void sample(void);
extern void MoveJoyStick(void);

// Delays
extern void DelayMs(unsigned int);
extern void DelayUs(unsigned int);
extern void delay(unsigned int);
extern void delay_x(unsigned int);

// Timers
extern void ResetTimer();
extern void TIMERdelay(int);

#endif
