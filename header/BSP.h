#ifndef _BSP_H_
#define _BSP_H_

#include  <msp430g2553.h>          // MSP430x2xx
//#include  <msp430xG46x.h>        // MSP430x4xx 

//parameters
#define Phi 3.1415926



//  Timer abstraction
#define TIMER_DIR          P2DIR
#define TIMER_SEL          P2SEL
#define TIMER_IE           P2IE
#define TIMER_IFG          P2IFG
#define ClockC             TA0CTL //control reg of timer A1
#define CaptureC           TA0CCTL0 //Capture/Compare Control Register of timer A1
#define TimerFlag          TA0CCTL1
#define TimerFla2          TA0CCTL2
#define COUNTERclk         TA0CCR0 //Capture/Compare Register

//MOTOR - Port 2
#define MOTORPort           P2OUT
#define MOTORPortSel        P2SEL
#define MOTORPortDir        P2DIR

//RGB - Port 1
#define RGBPort           	P1OUT
#define RGBPortSel          P1SEL
#define RGBPortDir        	P1DIR
#define RGB_clear RGBPort &= ~(BIT5 + BIT6 + BIT7)     //clear RGB

//JOYSTICK - Port 1
#define JOYSTICKPortIN       P1IN
#define JOYSTICKPortSel      P1SEL
#define JOYSTICKPortDir      P1DIR
#define JOYSTICKIntEnable    P1IE
#define JOYSTICKIntPending   P1IFG
#define JOYSTICKIntEdgeSel   P1IES

//LED - Port 2
#define  LEDPortSel     P2SEL
#define  LEDPortDir     P2DIR
#define  LEDPortOUT     P2OUT

#define Leds_clear  LEDPortOUT &= ~(BIT4 + BIT5 + BIT6 + BIT7);

extern void GPIOconfig(void);
extern void TIMERconfig(void);
extern void ADCconfig(void);
extern void RGBconfig(void);
extern void UARTconfig(void);
extern void DelayyUs(unsigned int cnt);
extern void DelayyMs(unsigned int cnt);

extern unsigned int X_delay;


#endif
