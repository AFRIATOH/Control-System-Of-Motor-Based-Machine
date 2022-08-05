#ifndef _BSP_H_
#define _BSP_H_

#include  <msp430g2553.h>          // MSP430x2xx
//#include  <msp430xG46x.h>        // MSP430x4xx 


#define   overflowvalue    65535
#define   debounceVal      10000

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

//RGB - Port 2
#define RGBPort           	P2OUT
#define RGBPortSel          P2SEL
#define RGBPortDir        	P2DIR
#define RGB_clear RGBPort &= ~(BIT0 + BIT1 + BIT2)     //clear RGBs

//JOYSTICK - Port 1
#define JOYSTICKPortIN       P1IN
#define JOYSTICKPortSel      P1SEL
#define JOYSTICKPortDir      P1DIR
#define JOYSTICKIntEnable    P1IE
#define JOYSTICKIntPending   P1IFG
#define JOYSTICKIntEdgeSel   P1IES

//JOYSTICK - Port 1
// P1 LEDs: 1.7,1.6,1.0, P2 LEDs: P2.7
// Port 1
#define  LEDPort1Sel     P1SEL
#define  LEDPort1Dir     P1DIR
#define  LEDPort1OUT     P1OUT
// Port 2
#define  LEDPort2Sel     P2SEL
#define  LEDPort2Dir     P2DIR
#define  LEDPort2OUT     P2OUT

#define Leds_CLR  LEDPort1OUT &= ~(BIT7 + BIT6 + BIT0);\
                  LEDPort2OUT &= ~(BIT7 + BIT6 + BIT5 + BIT4 + BIT2);

extern void GPIOconfig(void);
extern void TIMERconfig(void);
extern void ADCconfig(void);
extern void RGBconfig(void);
extern void UARTconfig(void);
extern void DelayyUs(unsigned int cnt);
extern void DelayyMs(unsigned int cnt);

extern unsigned int X_delay;
extern int count_up_val;
extern int count_down_val;


#endif
