#ifndef _BSP_H_
#define _BSP_H_

#include  <msp430g2553.h>          // MSP430x2xx
//#include  <msp430xG46x.h>        // MSP430x4xx 

//parameters
#define Phi 3.1415926


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

#define Leds_clear  LEDPortOUT &= ~(BIT4 + BIT5 + BIT6 + BIT7);     //clear LED

extern void GPIOconfig(void);
extern void TIMERconfig(void);
extern void ADCconfig(void);
extern void RGBconfig(void);
extern void UARTconfig(void);



#endif
