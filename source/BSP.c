#include  "../header/API.h"    		      //private library - API layer
#include  "../header/Application.h"     //private library - APP layer
#include  "../header/BSP.h"             //private library - BSP layer

//------------------------------------------------------------------------------------- 
//           GPIO configuration 
//-------------------------------------------------------------------------------------
void GPIOconfig(void){
  if (CALBC1_1MHZ==0xFF)					//If calibration constant erased
  {											
    while(1);                     //do not load, trap CPU!!	
  }
  DCOCTL = 0;                     //Select lowest DCOx and MODx settings
  BCSCTL1 = CALBC1_1MHZ;          //Set DCO
  DCOCTL = CALDCO_1MHZ;
}

//------------------------------------------------------------------------------------- 
//           UART configuration 
//-------------------------------------------------------------------------------------
void UARTconfig(void){
//   P2DIR = 0xFF;                //All P2.x outputs
//   P2OUT = 0;                   //All P2.x reset
  P1SEL |= BIT1 + BIT2 ;          //P1.1 = RXD, P1.2=TXD
  P1SEL2 |= BIT1 + BIT2 ;         //P1.1 = RXD, P1.2=TXD
  P1OUT &= ~(BIT1 + BIT2);        //reset P1.1 P1.2
  
  UCA0CTL1 |= UCSSEL_2;            //CLK = SMCLK
  UCA0BR0 = 104;
  UCA0BR1 = 0x00;
  UCA0MCTL = UCBRS0;
}

//------------------------------------------------------------------------------------- 
//            ADC congiguration 
//-------------------------------------------------------------------------------------
void ADCconfig(void){
  ADC10CTL0 = MSC + ADC10SHT_2 + ADC10ON + ADC10IE;   //Multiple sample and conversion,  sample-and-hold, ADC on, interrupt enable
  ADC10CTL1 = INCH_4 + CONSEQ_3+ ADC10SSEL_0;         //Repeat-sequence-of-channels, ADC clock, input channel A4
  ADC10DTC1 = 0x02;                                   //2 transfers in each block
  ADC10AE0 |= 0x18;                            //p1.3 p1.4 input
}   

//------------------------------------------------------------------------------------- 
//            RGB congiguration 
//-------------------------------------------------------------------------------------
void RGBconfig(void){
	RGBPortSel &= ~(BIT5 + BIT6 + BIT7);				//RGB: P1.5-1.7 as GPIO
	RGBPortDir |= (BIT5 + BIT6 + BIT7);         //RGB GPIO-output
	RGB_clear;	                                //clear RGB
}   

//------------------------------------------------------------------------------------- 
//            Motor configuration 
//-------------------------------------------------------------------------------------
void MOTORconfig(void){
  MOTORPortSel &= ~(BIT0 + BIT1 + BIT2 + BIT3); //MOTOR: P2.0-2.3 as GPIO
  MOTORPortDir |=  (BIT0 + BIT1 + BIT2 + BIT3); //MOTOR: GPIO-output
}

//------------------------------------------------------------------------------------- 
//            Joystick configuration 
//-------------------------------------------------------------------------------------
void JOYSTICKconfig(void){
	JOYSTICKPortSel &= ~(BIT3 + BIT4 + BIT0); //JOYSTICK: P1.0 p1.3-1.4 as GPIO
	JOYSTICKPortDir &= ~(BIT3 + BIT4 + BIT0); //JOYSTICK: GPIO-input
  JOYSTICKIntPending  &= ~BIT0;             //clear interrupt
  JOYSTICKIntEnable   |=  BIT0;             //interrupt enable
  JOYSTICKIntEdgeSel  |=  BIT0;             //high-to-low transition
}

//------------------------------------------------------------------------------------- 
//           LED congiguration 
//-------------------------------------------------------------------------------------
void LEDconfig(void){
  LEDPortSel  &= ~(BIT4 + BIT5 + BIT6 + BIT7);   //LEDs: P2.4-2.7 as GPIO
  LEDPortDir  |= (BIT4 + BIT5 + BIT6 + BIT7);    //LEDs: GPIO-output
  Leds_clear;
}

//------------------------------------------------------------------------------------- 
//            Timers congiguration 
//-------------------------------------------------------------------------------------
void TIMERconfig(void){
  TA0CTL = TASSEL_2 + ID_3 + MC_1 + TACLR;     // SMCLK/8 = 131072[Hz], upmode 
  TA1CTL = TASSEL_2 + ID_3 + MC_1 + TACLR;     // SMCLK/8 = 131072[Hz], upmode
}

//******************************************************************
// Delay usec functions
//******************************************************************
void DelayyUs(unsigned int cnt){
	unsigned char i;
  for(i=cnt ; i>0 ; i--) asm(" nop"); // tha command asm("nop") takes raphly 1usec
}
//******************************************************************
// Delay msec functions
//******************************************************************
void DelayyMs(unsigned int cnt){
	unsigned char i;
  for(i=cnt ; i>0 ; i--) DelayUs(1000); // tha command asm("nop") takes raphly 1usec
}
//******************************************************************
