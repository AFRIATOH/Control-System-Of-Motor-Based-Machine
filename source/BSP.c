#include  "../header/API.h"    		    // private library - API layer
#include  "../header/Application.h"     // private library - APP layer
#include  "../header/BSP.h"             // private library - BSP layer


//test
//------------------------------------------------------------------------------------- 
//           UART configuration 
//-------------------------------------------------------------------------------------
void UARTconfig(void)
{
  
  if (CALBC1_1MHZ==0xFF)					// If calibration constant erased
  {											
    while(1);                               // do not load, trap CPU!!	
  }
  DCOCTL = 0;                               // Select lowest DCOx and MODx settings
  BCSCTL1 = CALBC1_1MHZ;                    // Set DCO
  DCOCTL = CALDCO_1MHZ;
  
//   P2DIR = 0xFF;                             // All P2.x outputs
//   P2OUT = 0;                                // All P2.x reset
  P1SEL |= BIT1 + BIT2 ;                     // P1.1 = RXD, P1.2=TXD
  P1SEL2 |= BIT1 + BIT2 ;                     // P1.1 = RXD, P1.2=TXD
  //P1DIR |= RXLED + TXLED;
  P1OUT &= ~(BIT1 + BIT2);
  
  UCA0CTL1 |= UCSSEL_2;                     // CLK = SMCLK
  UCA0BR0 = 104;                           // 
  UCA0BR1 = 0x00;                           //
  UCA0MCTL = UCBRS0;               // 
}

//------------------------------------------------------------------------------------- 
//            ADC congiguration 
//-------------------------------------------------------------------------------------
void ADCconfig(void){

  ADC10CTL0 = ADC10SHT_2 + MSC + ADC10ON + ADC10IE;
  ADC10CTL1 = INCH_4 + CONSEQ_3+ ADC10SSEL_0;
  ADC10DTC1 = 0x02;
  ADC10AE0 |= 0x18;

}   

//------------------------------------------------------------------------------------- 
//            RGB congiguration 
//-------------------------------------------------------------------------------------
void RGBconfig(void){
	
	RGBPortSel &= ~(BIT0 + BIT1 + BIT2);				// Leds GPIO
	RGBPortDir |= (BIT0 + BIT1 + BIT2);         // Leds GPIO-output
	RGB_clear;	
}   

//------------------------------------------------------------------------------------- 
//            Motor configuration 
//-------------------------------------------------------------------------------------
void MOTORconfig(void){

  MOTORPortSel &= ~(BIT4 + BIT5 + BIT6 + BIT7);
  MOTORPortDir |=  (BIT4 + BIT5 + BIT6 + BIT7);

}

//------------------------------------------------------------------------------------- 
//            Joystick configuration 
//-------------------------------------------------------------------------------------
void JOYSTICKconfig(void){

	JOYSTICKPortSel &= ~(BIT3 + BIT4 + BIT5);
	JOYSTICKPortDir &= ~(BIT3 + BIT4 + BIT5);
  JOYSTICKIntPending  &= ~BIT5;
  JOYSTICKIntEnable   |=  BIT5;
  JOYSTICKIntEdgeSel  |=  BIT5;

}

//------------------------------------------------------------------------------------- 
//           LED congiguration 
//-------------------------------------------------------------------------------------
void LEDconfig(void){

  LEDPort1Sel  &= ~(BIT7 + BIT6 + BIT0);   //LEDs: 1.7,1.6,1.0 as GPIO
  LEDPort1Dir  |= (BIT7 + BIT6 + BIT0);    //LEDs: as GPIO-input
  LEDPort2Sel  &= ~(BIT3);                 //LEDs: 2.3 as GPIO
  LEDPort2Dir  |= ~(BIT3);                 //LEDs: as GPIO-input
  Leds_CLR;

}

//------------------------------------------------------------------------------------- 
//            Timers congiguration 
//-------------------------------------------------------------------------------------
void TIMERconfig(void){

  CCR0 = X_delay*130;
  TACTL = TASSEL_2 + ID_3 + MC_1 + TACLR; 

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
