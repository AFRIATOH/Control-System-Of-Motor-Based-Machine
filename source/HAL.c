#include  "../header/HAL.h"     // private library - HAL layer
#include  "../header/API.h"    		// private library - API layer

int D = 50;
unsigned int i,j;
volatile unsigned int Vx=1650;
volatile unsigned int Vy=1650;
unsigned int Vin[2] = {1650, 1650};
unsigned int a, b;
double c, alpha;
volatile unsigned long angle;
volatile unsigned long curr_angle = 0;
volatile unsigned long StepSize = 88;
volatile int StepCounter = 0;
volatile int MotorDelay = 2;
volatile unsigned int MessegeType;

Scripts script = {
    1,
    {0},
    {0},
    {0},
    {0},
    {(char*)0xF800, (char*)0xFA00, (char*)0xFC00}
};

unsigned int StateFlag = 0;
unsigned int MessegeDept = 0;

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

//******************************************************************
//          System Configuration 
//******************************************************************
void sysConfig(void)
{ 
	WDTCTL = WDTHOLD | WDTPW;		// Stop WDT

    GPIOconfig();
	UARTconfig();
	RGBconfig();
	MOTORconfig();
	JOYSTICKconfig();
	LEDconfig();
	TIMERconfig();
	ADCconfig();

	_BIS_SR(GIE);                     // enable interrupts globally
}

//******************************************************************
//          Delay msec functions
//******************************************************************
void DelayMs(int ms){
    TA0CCR0 = ms*130;
    TA0CCTL0 = CCIE;     // CCR0 interrupt enabled
    TACTL = MC_1;        // up mode
    __bis_SR_register(CPUOFF);
}

void Delay10Ms(int ms10){
    TA0CCR0 = ms10*1300;
    TA0CCTL0 = CCIE;     // CCR0 interrupt enabled
    TACTL = MC_1;        // up mode
    //if needed
    //TA1CCR0 = ms10*1300;
    //TA1CCTL0 = CCIE;     // CCR1 interrupt enabled
    //TA1CTL = MC_1;        // up mode
    __bis_SR_register(CPUOFF);
}

void DebounceDelay(int button){
    volatile unsigned int i;
    for(i = 1000; i > 0; i--);                     //delay, button debounce
    while(!(JOYSTICKPortIN & button));          // wait of release the button
    for(i = 1000; i > 0; i--);                     //delay, button debounce
    P1IFG &= ~button;             // manual clear of p1.button
}

//*********************************************************************
//            UART RX Interrupt Service Rotine
//*********************************************************************

#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void){
    MessegeType = UCA0RXBUF;
    
    if(MessegeType == "!"){
        StateFlag = 1;
        MessegeDept = 1;
    } else if(StateFlag == 1){
        if(MessegeDept == 1){
            state = UCA0RXBUF;
            MessegeDept = 2;
            PaintMode = ignore;
            MoveDiraction = hold;
            if((state == state1)){
                StateFlag = 0;
                MessegeDept = 0;
                __bis_SR_register(CPUOFF);
            }
            if(state == state2){
                PainterMode = neutral;
                MessegeDept = 0;
                StateFlag = 0;
                __bis_SR_register(CPUOFF);
            }  
        }else if((MessegeDept == 2) && (state == state3)){
            MoveDiraction = UCA0RXBUF;
            if(MoveDiraction == stop){
                MessegeDept = 0;
                StateFlag = 0;
            }
            __bis_SR_register(CPUOFF);
        }else if((MessegeDept == 2) && (state == state4)){
            //script mode
        }
    }
}

//*********************************************************************
//            UART TX Interrupt Service Rotine
//*********************************************************************

#pragma vector=USCIAB0TX_VECTOR
__interrupt void USCI0TX_ISR(void){           		

}

//*********************************************************************
//            Port1 Interrupt Service Rotine 
//*********************************************************************

#pragma vector=PORT1_VECTOR
__interrupt void PORT1_ISR(void){
    if(JOYSTICKIntPending & 0x01){       //JoyStick Button Press
        DebounceDelay(0x01);
        if(FSMstate == state2){
            if(PaintMode == neutral){
                PaintMode = write;
            }
            else if(PaintMode == write){
                PaintMode = erase;
            }
            else if(PaintMode == erase){
                PaintMode = neutral;
            }
        }
    }
}

//*********************************************************************
//            Timer A0 Interrupt Service Rotine 
//*********************************************************************

#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A0(void){
    TACCTL0 &= ~CCIE;                   // CCR0 interrupt disabled
	TACTL = MC_0;                       // stop clock
	__bic_SR_register_on_exit(CPUOFF);  // Exit LPM0 on return to main
}

//*********************************************************************
//            Timer A1 Interrupt Service Rotine 
//*********************************************************************

#pragma vector=TIMER0_A1_VECTOR
__interrupt void Timer_A1(void){
    TA1CCTL0 &= ~CCIE;                   // CCR0 interrupt disabled
	TA1CTL = MC_0;                       // stop clock
	__bic_SR_register_on_exit(CPUOFF);  // Exit LPM0 on return to main
}

//*********************************************************************
//            ADC Interrupt Service Rotine 
//*********************************************************************

#pragma vector=ADC10_VECTOR
    __interrupt void ADC10_ISR(void){
	ADC10CTL0 &= ~ADC10IFG;                          // clear interrupt flag
    Vx = Vin[0];
    Vy = Vin[1];
    __bic_SR_register_on_exit(CPUOFF);        // Clear CPUOFF bit from 0(SR)
}


//******************************************************************
//            smaple
//******************************************************************
void sample(void){

     ADC10CTL0 |= ADC10ON;                    // ADC ON
     ADC10CTL0 &= ~ENC;                       // Disable ADC10
     while (ADC10CTL1 & ADC10BUSY);           // Wait if ADC10 active
     ADC10SA = (int)Vin;                      // Data buffer address
     ADC10CTL0 |= ENC + ADC10SC;              // Enable ADC10
     __bis_SR_register(CPUOFF);               // LPM0
     ADC10CTL0 &= ~ADC10ON;                   // ADC10 OFF
}

//******************************************************************
//            move stepper 
//******************************************************************

void StepCalculation(void){
    curr_angle =0;
    StepSize = 360000/StepCounter;
}

void continuous_move(void){
    while(MoveDiraction != stop){
        while (MoveDiraction == clockwise){ // moving clockwise until diraction changes
            step_clockwise();
            StepCounter++;
            angle_increase();
        }
        while (MoveDiraction == counterclockwise){ // moving counterclockwise until diraction changes
            step_counterclockwise();
            StepCounter--;
            angle_decrease();
        }
    }
}

void angle_increase(void){
    curr_angle += StepSize;
    if (curr_angle > 360000){
        curr_angle = curr_angle%360000;
    }
}

void angle_decrease(void){
    curr_angle -= StepSize;
    if (curr_angle < 0){
        curr_angle =360000;
    }
    curr_angle = curr_angle%360000;
}

void forward(volatile long angle){
    while(angle > 0){
        step_clockwise();
        angle_increase();
        angle -= StepSize;
    }
}
void backward(volatile long angle){
        while(angle > 0){
        step_counterclockwise();
        angle_decrease();
        angle -= StepSize;
    }
}
void step_clockwise(void){
    MOTORPort = 0x01;
    Timer0_A_delay_ms(MotorDelay);
    MOTORPort = 0x08;
    Timer0_A_delay_ms(MotorDelay);
    MOTORPort = 0x04;
    Timer0_A_delay_ms(MotorDelay);
    MOTORPort = 0x02;
    Timer0_A_delay_ms(MotorDelay);
}

void step_counterclockwise(void){
    MOTORPort = 0x08;
    Timer0_A_delay_ms(MotorDelay);
    MOTORPort = 0x01;
    Timer0_A_delay_ms(MotorDelay);
    MOTORPort = 0x02;
    Timer0_A_delay_ms(MotorDelay);
    MOTORPort = 0x04;
    Timer0_A_delay_ms(MotorDelay);
}

void half_step_clockwise(void){
    MOTORPort = 0x08;
    Timer0_A_delay_ms(MotorDelay);
    MOTORPort = 0x0C;
    Timer0_A_delay_ms(MotorDelay);
    MOTORPort = 0x04;
    Timer0_A_delay_ms(MotorDelay);
    MOTORPort = 0x06;
    Timer0_A_delay_ms(MotorDelay);
    MOTORPort = 0x02;
    Timer0_A_delay_ms(MotorDelay);
    MOTORPort = 0x03;
    Timer0_A_delay_ms(MotorDelay);
    MOTORPort = 0x01;
    Timer0_A_delay_ms(MotorDelay);
    MOTORPort = 0x09;
    Timer0_A_delay_ms(MotorDelay);
}
//******************************************************************
//            move stepper to angle
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
//            move according to joystick
//******************************************************************
void MoveMotorToJoyStick(void){
    if (Vx > 1700){                                                     // Vx dir is right
        if(Vy > 1700){                                                 // Vy dir is up
            a = Vx - 1650;
            b = Vy - 1650;
            c = a/b;                                                // Assign the value we will find the atan of
            alpha = (c - ((c^3)/3) + ((c^5)/5)) * 180 / Phi;       // taylor series of arctan
        } else if (Vy < 1600){                                    // Vy dir is down
            a = Vx - 1650;
            b = 1650 - Vy;
            c = a/b;                                            // Assign the value we will find the atan of
            alpha = (c - ((c^3)/3) + ((c^5)/5)) * 180 / Phi;   // taylor series of arctan
            alpha = 180 - alpha;
        }
        else{                                                // Vy is in the middle -> angle is 90
            alpha = 90;
        }

    } else if (Vx < 1580){                                              // Vx dir is left
        if(Vy > 1700){                                                 // Vy dir is up
            a = 1650 - Vx;
            b = Vy - 1650;
            c = a/b;                                                // Assign the value we will find the atan of
            alpha = (c - ((c^3)/3) + ((c^5)/5)) * 180 / Phi;       // taylor series of arctan
            alpha = 360 - alpha;
        } else if (Vy < 1600){                                    // Vy dir is down
            a = 1650 - Vx;
            b = 1650 - Vy;
            c = a/b;                                           // Assign the value we will find the atan of
            alpha = (c - ((c^3)/3) + ((c^5)/5)) * 180 / Phi;  // taylor series of arctan
            alpha = 180 + alpha;
        } else {                                             // Vy is in the middle -> angle is 270
            alpha = 270;
        }
    }
    else{                                                 // Vx is in the middle
        if(Vy > 1700){                                   // Vy dir is up -> angle is 0
            alpha = 0;
        } else if (Vy < 1600){                          // Vy dir is down -> angle is 180
            alpha = 180;
        }
    }
    angle = (unsigned long)alpha;
    //angle = alpha;
    move_to_angle(angle*1000);
    Vx = 1650;
    Vy = 1650;
}


//******************************************************************
//            script funcs
//******************************************************************

void read_script(void){
    __bis_SR_register(CPUOFF);               // LPM0
    script.num = ScriptNum;
    for
}

//1
void bling_RGB(int X){
    for(i = 0 ; i < X ; i++){
        RGBPort = 0x20;
        Delay10Ms(D);
        RGBPort = 0x40;
        Delay10Ms(D);
        RGBPort = 0x80;
        Delay10Ms(D);
    }
}

//2
void rlc_LED(int X){
    for(i = 0 ; i < X ; i++){
        LEDPortOUT = 0x10;
        Delay10Ms(D);
        LEDPortOUT = 0x20;
        Delay10Ms(D);
        LEDPortOUT = 0x40;
        Delay10Ms(D);
        LEDPortOUT = 0x80;
        Delay10Ms(D);
    }
}

//3
void rrc_LED(int X){
    for(i = 0 ; i < X ; i++){
        LEDPortOUT = 0x80;
        Delay10Ms(D);
        LEDPortOUT = 0x40;
        Delay10Ms(D);
        LEDPortOUT = 0x20;
        Delay10Ms(D);
        LEDPortOUT = 0x10;
        Delay10Ms(D);
    }
}

//4
void set_delay(int X){
    //תלוי באיך שנשלח אותו
}

//5
void clear_all(int X){
    RGB_clear;
    Leds_clear;
}

//6
void stepper_deg(unsigned long angle){
    move_to_angle(angle);
}

//7
void stepper_scan(unsigned long l, unsigned long r){

    // moving to Left angle
    move_to_angle(l*1000);

    // Tell PC that motor arrived to Left angle

    DelayMs(1000);

    // moving to Right angle
    if(r >= l){
        forward((r-l)*1000);
    } else{
        forward((360+r-l)*1000);
    }

    // Tell PC that motor arrived to Left angle

}




