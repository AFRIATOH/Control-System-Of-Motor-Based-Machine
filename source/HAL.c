#include  "../header/HAL.h"     // private library - HAL layer
#include  "../header/API.h"    		// private library - API layer

int D = 50;
unsigned int i,j;
int tx = 1;
volatile unsigned int Vx=460;
volatile unsigned int Vy=460;
unsigned int Vin[2] = {460, 460};
unsigned int a, b;
double c, alpha;
volatile unsigned long angle;
volatile unsigned long curr_angle = 0;

volatile unsigned long left;
volatile unsigned long right;
volatile int arrive_left = 0;
volatile int arrive_right = 0;

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
    
}

//*********************************************************************
//            UART TX Interrupt Service Rotine
//*********************************************************************

#pragma vector=USCIAB0TX_VECTOR
__interrupt void USCI0TX_ISR(void){           		

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
    Vx = Vin[0];
    Vy = Vin[1];
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
     ADC10CTL0 &= ~ENC;                       // Disable ADC10
     while (ADC10CTL1 & ADC10BUSY);           // Wait if ADC10 active
     ADC10SA = (int)Vin;                      // Data buffer start
     ADC10CTL0 |= ENC + ADC10SC;              // Sampling and conversion start
     __bis_SR_register(CPUOFF);               // LPM0, ADC10_ISR will force exit
     ADC10CTL0 &= ~ADC10ON;                   // ADC10 OFF
}

//******************************************************************
// move stepper 
//******************************************************************

void angle_increase(void){
    volatile unsigned long curr_angle_tmp = curr_angle;
    curr_angle_tmp += step;
    if (curr_angle_tmp > 360000){
        curr_angle_tmp =0;
    }
    curr_angle = curr_angle_tmp%360000;
}

void angle_decrease(void){
    volatile unsigned long curr_angle_tmp = curr_angle;
    curr_angle_tmp -= step;
    if (curr_angle_tmp < 0){
        curr_angle_tmp =360000;
    }
    curr_angle = curr_angle_tmp%360000;
}

void forward(volatile long angle){
    while(angle > 0){
        step_forward();
        angle_increase();
        angle -= step;
    }
}
void backward(volatile long angle){
        while(angle > 0){
        step_backward();
        angle_decrease();
        angle -= step;
    }
}
void step_forward(void){
    MOTORPort = 0x10;
    Timer0_A_delay_ms(StepperDelay);
    MOTORPort = 0x80;
    Timer0_A_delay_ms(StepperDelay);
    MOTORPort = 0x40;
    Timer0_A_delay_ms(StepperDelay);
    MOTORPort = 0x20;
    Timer0_A_delay_ms(StepperDelay);
}

void step_backward(void){
    MOTORPort = 0x80;
    Timer0_A_delay_ms(StepperDelay);
    MOTORPort = 0x10;
    Timer0_A_delay_ms(StepperDelay);
    MOTORPort = 0x20;
    Timer0_A_delay_ms(StepperDelay);
    MOTORPort = 0x40;
    Timer0_A_delay_ms(StepperDelay);
}

void half_step_forward(void){
    MOTORPort = 0x80;
    Timer0_A_delay_ms(StepperDelay);
    MOTORPort = 0xC0;
    Timer0_A_delay_ms(StepperDelay);
    MOTORPort = 0x40;
    Timer0_A_delay_ms(StepperDelay);
    MOTORPort = 0x60;
    Timer0_A_delay_ms(StepperDelay);
    MOTORPort = 0x20;
    Timer0_A_delay_ms(StepperDelay);
    MOTORPort = 0x30;
    Timer0_A_delay_ms(StepperDelay);
    MOTORPort = 0x10;
    Timer0_A_delay_ms(StepperDelay);
    MOTORPort = 0x90;
    Timer0_A_delay_ms(StepperDelay);
}
//******************************************************************
// move stepper to angle
//******************************************************************

void move_to_angle(unsigned long angle){
    if(curr_angle > angle){              //check which angle is bigger
        if(curr_angle-angle < 180000){   //check which direction is shortest
            backward(curr_angle-angle);
        } else{
            forward(curr_angle-angle);
        }
    }else{
        if(angle-curr_angle > 180000){  //check which direction is shortest
            backward(curr_angle-angle);
        } else{
            forward(curr_angle-angle);
        }
    }
}

//******************************************************************
// move joystick 
//******************************************************************
void MoveJoyStick(void){
    if (Vx > 900){                                                     // first or fourth quarter of x-y
        if(Vy > 460){                                                 // first quarter
            a = Vx - 460;
            b = Vy - 460;
            c = a/b;                                                // Assign the value we will find the atan of
            alpha = (c - ((c^3)/3) + ((c^5)/5)) * 180 / Phi;       // taylor series of arctan
        } else if (Vy < 459){                                     // fourth quarter
            a = Vx - 460;
            b = 460 - Vy;
            c = a/b;                                            // Assign the value we will find the atan of
            alpha = (c - ((c^3)/3) + ((c^5)/5)) * 180 / Phi;   // taylor series of arctan
            alpha = 180 - alpha;
        }
        else{                                                // Vy in [radius_min, radius_max] => direction is on x axis
            alpha = 90;
        }

    } else if (Vx < 100){                                              // second or third quarter of x-y
        if(Vy > 460){                                                 // second quarter
            a = 460 - Vx;
            b = Vy - 460;
            c = a/b;                                                // Assign the value we will find the atan of
            alpha = (c - ((c^3)/3) + ((c^5)/5)) * 180 / Phi;       // taylor series of arctan
            alpha = 360 - alpha;
        } else if (Vy < 459){                                    // third quarter
            a = 460 - Vx;
            b = 460 - Vy;
            c = a/b;                                           // Assign the value we will find the atan of
            alpha = (c - ((c^3)/3) + ((c^5)/5)) * 180 / Phi;  // taylor series of arctan
            alpha = 180 + alpha;
        } else {                                             // Vy in [radius_min, radius_max] => direction is on x axis
            alpha = 270;
        }
    }
    else{                                                 // Vx in [radius_min, radius_max] => direction is on y axis
        if(Vy > 900){
            alpha = 0;
        } else if (Vy < 100){
            alpha = 180;
        }
    }
    angle = (unsigned long)alpha;
    //angle = alpha;
    move_to_angle(angle*1000);
    Vx = 460;
    Vy = 460;
}

//---------------------------------------------------------------------
//             script funcs
//---------------------------------------------------------------------

void bling_RGB(int X){
    for(i = 0 ; i < X ; i++){
        RGBPort = 0x01;
        delay_x(D);
        RGBPort = 0x02;
        delay_x(D);
        RGBPort = 0x04;
        delay_x(D);
    }
}

void rlc_LED(int X){
    for(i = 0 ; i < X ; i++){

    }
}

void rrc_LED(int X){
    for(i = 0 ; i < X ; i++){

    }
}


void scan_step(unsigned long l, unsigned long r){

    left = l*1000;
    right = r*1000;
    arrive_left = 0;
    arrive_right = 0;

    // moving to Left angle
    move_to_angle(left);
    // Tell PC that motor arrived to Left angle
    if ((state == ScriptMode) && (scan_mode == 1)){
        Got_to_left_flg = 1;
        enable_transmition();
        Timer0_A_delay_ms(250);
    }

    // after getting to Left angle, scan area to Right angle
    scan_to_right();

    if ((state == ScriptMode) && (scan_mode == 1)){
        Got_to_right_flg = 1;   // update PC that motor arrived to Right angle
        enable_transmition();
        Timer0_A_delay_ms(250);

    }

}




