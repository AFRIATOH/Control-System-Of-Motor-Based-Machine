#include  "../header/HAL.h"     // private library - HAL layer
#include  "../header/API.h"    		// private library - API layer

unsigned int i,j;
int tx = 1;
volatile unsigned int Vx=460;
volatile unsigned int Vy=460;
unsigned int res[2] = {460, 460};
volatile unsigned long destiny_angle;
unsigned int a, b;
double fraction_a_b;
double alpha;

//--------------------------------------------------------------------
//             System Configuration  
//--------------------------------------------------------------------
void sysConfig(void)
{ 
	WDTCTL = WDTHOLD | WDTPW;		// Stop WDT

	UARTconfig();
	ADCconfig();
	RGBconfig();
	MOTORconfig();
	JOYSTICKconfig();
	LEDconfig();
	TIMERconfig();

	_BIS_SR(GIE);                     // enable interrupts globally
}

//---------------------------------------------------------------------
//            Polling based Delay function
//---------------------------------------------------------------------
void delay(unsigned int t){  // t[msec]
	volatile unsigned int i;
	for(i=t; i>0; i--);
}

//---------------------------------------------------------------------
//            Delay function
//---------------------------------------------------------------------
void delay_x(unsigned int t){  // t[msec]
  CCTL0 = CCIE;                             // CCR0 interrupt enabled
  CCR0 = t*130;
  __bis_SR_register(CPUOFF);
}

//---------------------------------------------------------------------
//            Enter from LPM0 mode
//---------------------------------------------------------------------
void enterLPM(unsigned char LPM_level){
	if (LPM_level == 0x00) 
	  _BIS_SR(LPM0_bits);     /* Enter Low Power Mode 0 */
        else if(LPM_level == 0x01) 
	  _BIS_SR(LPM1_bits);     /* Enter Low Power Mode 1 */
        else if(LPM_level == 0x02) 
	  _BIS_SR(LPM2_bits);     /* Enter Low Power Mode 2 */
	else if(LPM_level == 0x03) 
	  _BIS_SR(LPM3_bits);     /* Enter Low Power Mode 3 */
        else if(LPM_level == 0x04) 
	  _BIS_SR(LPM4_bits);     /* Enter Low Power Mode 4 */
}

//*********************************************************************
//            UART RX Interrupt Service Rotine
//*********************************************************************

#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void)
{
  if (state != state4){
	    if (UCA0RXBUF == '1')                          // '1' received?
		{
			state = state1;
			__bic_SR_register_on_exit(LPM0_bits + GIE);
		}
		else if (UCA0RXBUF == '2')                     // '2' received?
		{
			state = state2;
			__bic_SR_register_on_exit(LPM0_bits + GIE);
		}
		else if (UCA0RXBUF == '3')                     // '3' received?
		{
			state = state3;
			__bic_SR_register_on_exit(LPM0_bits + GIE);
		}
		else if (UCA0RXBUF == '4')                     // '4' received?
		{
			state = state4;
			__bic_SR_register_on_exit(LPM0_bits + GIE);
		}
		else if (UCA0RXBUF == '5')                     // '5' received?
		{
			state = state5;
			__bic_SR_register_on_exit(LPM0_bits + GIE);
		}
		else if (UCA0RXBUF == '6')                     // '6' received?
		{
			state = state6;
			__bic_SR_register_on_exit(LPM0_bits + GIE);
		}
		else if (UCA0RXBUF == '7')                     // '7' received?
		{
			state = state7;
			__bic_SR_register_on_exit(LPM0_bits + GIE);
		}
		else if (UCA0RXBUF == '8')                     // '8' received?
		{
			state = state0;
			//__bic_SR_register_on_exit(LPM0_bits + GIE);
		}
		else if (UCA0RXBUF == '9')                     // '9' received?
		{
			state = state8;
			__bic_SR_register_on_exit(LPM0_bits + GIE);
		}
  }
  else {
		new_X[j++] = UCA0RXBUF;
	  	if (new_X[j-1] == '\0'){
		  j = 0;
		  X_delay = str_to_int(new_X);
          state = state0;
	  }
  }
}

//*********************************************************************
//            UART TX Interrupt Service Rotine
//*********************************************************************

#pragma vector=USCIAB0TX_VECTOR
__interrupt void USCI0TX_ISR(void)

{           		
	if(state == state5){
		UCA0TXBUF = pot_val[i++];
		if (i == sizeof pot_val -1){					     // TX over?
			i = 0; 
			IE2 &= ~UCA0TXIE;        					 // Disable USCI_A0 TX interrupt
			IE2 |= UCA0RXIE;                 			 // Enable USCI_A0 RX interrupt
			state = state7;
			tx = 0;
            }
	}
	else if(tx){
        UCA0TXBUF = MENU[i++];
        if (i == sizeof MENU - 1){						 // TX over?
                i = 0; 
                IE2 &= ~UCA0TXIE;                		 // Disable USCI_A0 TX interrupt
                IE2 |= UCA0RXIE;                 		 // Enable USCI_A0 RX interrupt	
        }			
	}
	else{
	  IE2 &= ~UCA0TXIE;    
	}
}

//------------------------------------------------------------------
void DebounceDelay(int button){
    volatile unsigned int i;
    for(i = 1000; i > 0; i--);                     //delay, button debounce
    while(!(P1IN & button));          // wait of release the button
    for(i = 1000; i > 0; i--);                     //delay, button debounce
    P1IFG &= ~button;             // manual clear of p1.button
}
//------------------------------------------------------------------
//           Port1 Interrupt Service Rotine 
//------------------------------------------------------------------

#pragma vector=PORT1_VECTOR
__interrupt void PORT1_ISR(void){
    if(P1IFG & 0x10){
        DebounceDelay(0x10);
        tx = 1;
        UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
        IE2 |= UCA0TXIE;                          // Enable USCI_A0 TX interrupt
    }
}

//------------------------------------------------------------------
//           Timer A0 Interrupt Service Rotine 
//------------------------------------------------------------------

#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A(void){
	CCTL0 &= ~CCIE;                        	     // CCR0 interrupt enabled
	__bic_SR_register_on_exit(CPUOFF);  // Exit LPM0 on return to main
}

//------------------------------------------------------------------
//           ADC Interrupt Service Rotine 
//------------------------------------------------------------------

#pragma vector=ADC10_VECTOR
    __interrupt void ADC10_ISR(void){
	ADC10CTL0 &= ~ADC10IFG;                          // clear interrupt flag
    Vx = res[0];
    Vy = res[1];
    __bic_SR_register_on_exit(CPUOFF);        // Clear CPUOFF bit from 0(SR)
}

//******************************************************************
// Delay usec functions
//******************************************************************
void DelayUs(unsigned int cnt){

	unsigned char i;
        for(i=cnt ; i>0 ; i--) asm(" nop"); // tha command asm("nop") takes raphly 1usec

}

//******************************************************************
// Delay msec functions
//******************************************************************
void DelayMs(unsigned int cnt){

	unsigned char i;
        for(i=cnt ; i>0 ; i--) DelayUs(1000); // tha command asm("nop") takes raphly 1usec

}


//******************************************************************
// smaple
//******************************************************************
void sample(void){

     ADC10CTL0 |= ADC10ON;                    // ADC10 ON
     ADC10CTL0 &= ~ENC;
     while (ADC10CTL1 & ADC10BUSY);           // Wait if ADC10 core is active
     ADC10SA = (int)res;                      // Data buffer start
     ADC10CTL0 |= ENC + ADC10SC;              // Sampling and conversion start
     __bis_SR_register(CPUOFF);      // LPM0, ADC10_ISR will force exit
     ADC10CTL0 &= ~ADC10ON;                   // ADC10 OFF
}

//******************************************************************
// move joystick
//******************************************************************
void MoveJoyStick(void){
    if (Vx > 900){                          // first or fourth quarter of x-y
        if(Vy > 460){                             // first quarter
            a = Vx - 460;
            b = Vy - 460;
            fraction_a_b = a/b;                          // Assign the value we will find the atan of
            alpha = atan(fraction_a_b) * 180 / PI;       // Calculate the Arc Tangent of value
        } else if (Vy < 459){                     // fourth quarter
            a = Vx - origin;
            b = origin - Vy;
            fraction_a_b = a/b;                          // Assign the value we will find the atan of
            alpha = atan(fraction_a_b) * 180 / PI;       // Calculate the Arc Tangent of value
            alpha = 180 - alpha;
        }
        else{            // Vy in [radius_min, radius_max] => direction is on x axis
            alpha = 90;
        }

    } else if (Vx < 100){                   // second or third quarter of x-y
        if(Vy > 460){                             // second quarter
            a = origin - Vx;
            b = Vy - origin;
            fraction_a_b = a/b;                          // Assign the value we will find the atan of
            alpha = atan(fraction_a_b) * 180 / PI;       // Calculate the Arc Tangent of value
            alpha = 360 - alpha;
        } else if (Vy < 459){                     // third quarter
            a = origin - Vx;
            b = origin - Vy;
            fraction_a_b = a/b;                          // Assign the value we will find the atan of
            alpha = atan(fraction_a_b) * 180 / PI;       // Calculate the Arc Tangent of value
            alpha = 180 + alpha;
        } else {        // Vy in [radius_min, radius_max] => direction is on x axis
            alpha = 270;
        }
    }
    else{               // Vx in [radius_min, radius_max] => direction is on y axis
        if(Vy > 900){
            alpha = 0;
        } else if (Vy < 100){
            alpha = 180;
        }
    }
    destiny_angle = (unsigned long)alpha;
    stepper_deg(destiny_angle);
    Vx = 460;
    Vy = 460;
}

//---------------------------------------------------------------------
//            Enable interrupts
//---------------------------------------------------------------------
void enable_interrupts(){
  _BIS_SR(GIE);
}
//---------------------------------------------------------------------
//            Disable interrupts
//---------------------------------------------------------------------
void disable_interrupts(){
  _BIC_SR(GIE);
}

