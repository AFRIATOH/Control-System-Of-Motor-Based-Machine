#include  "../header/HAL.h"     // private library - HAL layer
#include  "../header/API.h"    		// private library - API layer
#include  "../header/BSP.h"

int D = 50;
unsigned int i,j;
volatile unsigned int Vx=465;
volatile unsigned int Vy=495;
volatile unsigned int VxPrev=465;
volatile unsigned int VyPrev=495;
volatile unsigned int DiffX = 0;
volatile unsigned int DiffY = 0;
unsigned int Vin[2] = {465, 495};
unsigned int a, b;
double c;
double alpha;
int Op6 = 0;
int Op7 = 0;
volatile unsigned long angle;
volatile unsigned long curr_angle = 0;
volatile long StepSize = 666;
volatile int StepCounter = 0;
volatile int MotorDelay = 2;
volatile unsigned int MessegeType;
unsigned int InfoReq = 0;
volatile char BufferArray[10];
volatile unsigned int BufferLocation;
volatile int ScriptNum;
volatile int ScriptNumFlag = 1;
volatile char WriteOnFlash[64];
int ScriptIndex;
volatile char ScriptRx[10];
int ScriptReadIndex;
int CountScriptSize;
int CountScriptLines;
int ack = 0;
int WriteOnFlashFlag = 0;
int TxLocation = 0;
unsigned int StateFlag = 1;
unsigned int ArriveToZeroAngle = 0;
unsigned int MessegeDept = 1;

Scripts scriptt = {
    1,
    {0},
    {0},
    {0},
    {0},
    {(char*)0xF800, (char*)0xFA00, (char*)0xFC00}
};


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
    TA0CTL = TASSEL_2 + ID_3 + MC_1 + TACLR; 
    __bis_SR_register(LPM0_bits + GIE);
}

void Delay10Ms(int ms10){
    TA0CCR0 = ms10*1300;
    TA0CCTL0 = CCIE;     // CCR0 interrupt enabled
    TA0CTL = TASSEL_2 + ID_3 + MC_1 + TACLR;
    __bis_SR_register(LPM0_bits + GIE);
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

    if(MessegeType == '!'){
        StateFlag = 1;
        MessegeDept = 1;
        state = state0;
    }
    else if(StateFlag == 1 && MessegeDept == 1){
            state = UCA0RXBUF -'0';
            PaintMode = ignore;
            MessegeDept = 2;
            if((state == state1)){
                ArriveToZeroAngle = 0;
            }
            else if(state == state2){
                PaintMode = neutral;
            }
            else if(state == state3){
                MoveDiraction = hold;
            }
    }
    else if((MessegeDept == 2) && (state == state1) && (ArriveToZeroAngle == 0)){
        MoveDiraction = UCA0RXBUF -'0';
        if(MoveDiraction == stop){
            ArriveToZeroAngle = 1;
            curr_angle = 0;
            StateFlag = 0;
            MessegeDept = 1;
        }
    }
    else if((MessegeDept == 2) && (state == state3)){
        MoveDiraction = UCA0RXBUF -'0';
        if(MoveDiraction == stop){
            MessegeDept = 0;
            StateFlag = 0;
        }
    }
    else if((MessegeDept == 2) && (state == state4)){
        MessegeDept = 0;
        if(WriteOnFlashFlag == 0){
            if(ScriptNumFlag == 1){
                ScriptNumFlag = 0;
                ScriptNum = UCA0RXBUF - '0';
            }
            else
                ScriptRx[ScriptIndex++] = UCA0RXBUF;
        }
        else if(WriteOnFlashFlag == 1){
           WriteOnFlash[ScriptIndex++] = UCA0RXBUF;

        }

    }
    __bic_SR_register_on_exit(LPM0_bits + GIE);  // Exit LPM0 on return to main
}

//*********************************************************************
//            UART TX Interrupt Service Rotine
//*********************************************************************

#pragma vector=USCIAB0TX_VECTOR
__interrupt void USCI0TX_ISR(void){
    if(InfoReq == 1){
        UCA0TXBUF = BufferArray[TxLocation++];
//        BufferArray[TxLocation++] = 0;
        if(TxLocation == BufferLocation){
            BufferLocation = 0;
            TxLocation = 0;
            InfoReq = 0;
            IE2 &= ~UCA0TXIE;
        }
    }
}

//*********************************************************************
//            Port1 Interrupt Service Rotine 
//*********************************************************************

#pragma vector=PORT1_VECTOR
__interrupt void PORT1_ISR(void){
    if(JOYSTICKIntPending & 0x01){       //JoyStick Button Press
        DebounceDelay(0x01);
        if(state == state2){
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
    TA0CTL = TACLR;
	TACTL = MC_0;                       // stop clock
	__bic_SR_register_on_exit(LPM0_bits + GIE);  // Exit LPM0 on return to main
}

//*********************************************************************
//            ADC Interrupt Service Rotine 
//*********************************************************************

 #pragma vector=ADC10_VECTOR
     __interrupt void ADC10_ISR(void){
 	ADC10CTL0 &= ~ADC10IFG;                          // clear interrupt flag
     Vx = Vin[0];
     Vy = Vin[1];
     __bic_SR_register_on_exit(LPM0_bits + GIE);        // Clear LPM0_bits + GIE bit from 0(SR)
 }

//******************************************************************
//            UART functions
//******************************************************************
//void SendInfo(void){
//    BufferLocation = 0;
//    if(state == state2){
//        BufferArray[BufferLocation++]='!';
//        BufferLocation += 2;
//        AddToBuffer(Vx);
//        AddToBuffer(Vy);
//        int len = BufferLocation;
//        BufferArray[2]= (len%10) + '0'; //send char of num
//        BufferArray[1]= (len/10) + '0'; //send char of num
//    } else if(state == state3){
//        BufferArray[BufferLocation++]='@';
//        BufferLocation += 2;
//        AddToBuffer(StepCounter);
//        AddToBuffer(StepSize);
//        int len = BufferLocation;
//        BufferArray[2]= (len%10) + '0'; //send char of num
//        BufferArray[1]= (len/10) + '0'; //send char of num
//    }
//    InfoReq = 1;
//    IE2 |= UCA0TXIE;
//}

void SendInfo(void){     //send using TX
    BufferLocation = 0;
    if(state == state2){
        BufferArray[BufferLocation++] = PaintMode + '0'; //send char of num
        AddVToBuffer(Vx);
        AddVToBuffer(Vy);
    } else if(state == state3){
        AddStepToBuffer(StepCounter);
    } else if((state == state4) && (Op6)){
        AddAngleToBuffer(curr_angle/1000);
        Op6 = 0;
    } else if((state == state4) && (Op7)){
        AddAngleToBuffer(curr_angle/1000);
        Op7 = 0;
    }
    InfoReq = 1;
    UCA0CTL1 &= ~UCSWRST;
    IE2 |= UCA0TXIE;
}

void AddStepToBuffer(int steps){
    if(steps < 0){ //abs of step number
        steps = 0 - steps;
    }
    BufferArray[BufferLocation+4] = (steps%10) + '0'; //send char of num
    steps = steps/10;
    BufferArray[BufferLocation+3] = (steps%10) + '0'; //send char of num
    steps = steps/10;
    BufferArray[BufferLocation+2] = (steps%10) + '0'; //send char of num
    steps = steps/10;
    BufferArray[BufferLocation+1] = (steps%10) + '0'; //send char of num
    steps = steps/10;
    BufferArray[BufferLocation] = (steps%10) + '0'; //send char of num
    BufferLocation = BufferLocation + 5;
}

void AddVToBuffer(unsigned int volt){
    BufferArray[BufferLocation+3] = (volt%10) + '0'; //send char of num
    volt = volt/10;
    BufferArray[BufferLocation+2] = (volt%10) + '0'; //send char of num
    volt = volt/10;
    BufferArray[BufferLocation+1] = (volt%10) + '0'; //send char of num
    volt = volt/10;
    BufferArray[BufferLocation] = (volt%10) + '0'; //send char of num
    BufferLocation = BufferLocation + 4;
}

void AddAngleToBuffer(volatile unsigned long ang){
    BufferArray[BufferLocation+2] = (ang%10) + '0'; //send char of num
    ang = ang/10;
    BufferArray[BufferLocation+1] = (ang%10) + '0'; //send char of num
    ang = ang/10;
    BufferArray[BufferLocation] = (ang%10) + '0'; //send char of num
    BufferLocation = BufferLocation + 3;
}

// void AddToBuffer(unsigned int num){
//     if(num < 10){
//         BufferArray[BufferLocation++] = (num%10) + '0'; //send char of num
//     } else if(num < 100){
//         BufferArray[BufferLocation+1] = (num%10) + '0'; //send char of num
//         num = num/10;
//         BufferArray[BufferLocation] = (num%10) + '0'; //send char of num
//         BufferLocation += 2;
//     } else if(num < 1000){
//         BufferArray[BufferLocation+2] = (num%10) + '0'; //send char of num
//         num = num/10;
//         BufferArray[BufferLocation+1] = (num%10) + '0'; //send char of num
//         num = num/10;
//         BufferArray[BufferLocation] = (num%10) + '0'; //send char of num
//         BufferLocation += 3;    
//     } else if(num < 10000){
//         BufferArray[BufferLocation+3] = (num%10) + '0'; //send char of num
//         num = num/10;
//         BufferArray[BufferLocation+2] = (num%10) + '0'; //send char of num
//         num = num/10;
//         BufferArray[BufferLocation+1] = (num%10) + '0'; //send char of num
//         num = num/10;
//         BufferArray[BufferLocation] = (num%10) + '0'; //send char of num
//         BufferLocation += 4;    
//     } else if(num < 100000){
//         BufferArray[BufferLocation+4] = (num%10) + '0'; //send char of num
//         num = num/10;
//         BufferArray[BufferLocation+3] = (num%10) + '0'; //send char of num
//         num = num/10;
//         BufferArray[BufferLocation+2] = (num%10) + '0'; //send char of num
//         num = num/10;
//         BufferArray[BufferLocation+1] = (num%10) + '0'; //send char of num
//         num = num/10;
//         BufferArray[BufferLocation] = (num%10) + '0'; //send char of num
//         BufferLocation += 5;    
//     }
//     BufferArray[BufferLocation++] = '-';
// }
//******************************************************************
//            Sample
//******************************************************************
void sample(void){

    ADC10CTL0 |= ADC10ON;                    // ADC ON
    ADC10CTL0 &= ~ENC;                       // Disable ADC10
    while (ADC10CTL1 & ADC10BUSY);           // Wait if ADC10 active
    ADC10SA = (int)Vin;                      // Data buffer address
    ADC10CTL0 |= ENC + ADC10SC;              // Enable ADC10
    __bis_SR_register(LPM0_bits + GIE);               // LPM0
    ADC10CTL0 &= ~ADC10ON;                   // ADC10 OFF
}

//******************************************************************
//            move stepper 
//******************************************************************

void StepCalculation(void){
    if(StepCounter < 0){  //abs of step number
        StepCounter = 0 - StepCounter;
    }
    curr_angle = 0;
    StepSize = 360000/StepCounter;
}

void continuous_move(void){
    while(MoveDiraction != stop){
        while (MoveDiraction == clockwise){ // moving clockwise until diraction changes
            step_clockwise();
            StepCounter++;
            angle_increase();
            angle_decrease();
        }
        while (MoveDiraction == counterclockwise){ // moving counterclockwise until diraction changes
            step_counterclockwise();
            StepCounter--;
            angle_decrease();
        }
        __bis_SR_register(LPM0_bits + GIE);
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
    if (curr_angle < (2*StepSize)){
        curr_angle =360000;
    }
    curr_angle = curr_angle%360000;
}

void forward(volatile long angleF){
    while(angleF > (2*StepSize)){
        step_clockwise();
        angle_increase();
        angleF -= StepSize;
    }
}
void backward(volatile long angleB){
        while(angleB > (2*StepSize)){
        step_counterclockwise();
        angle_decrease();
        angleB -= StepSize;
    }
}
void step_counterclockwise(void){
    MOTORPort = 0x01;
    DelayMs(MotorDelay);
    MOTORPort = 0x08;
    DelayMs(MotorDelay);
    MOTORPort = 0x04;
    DelayMs(MotorDelay);
    MOTORPort = 0x02;
    DelayMs(MotorDelay);
}

void step_clockwise(void){
    MOTORPort = 0x08;
    DelayMs(MotorDelay);
    MOTORPort = 0x01;
    DelayMs(MotorDelay);
    MOTORPort = 0x02;
    DelayMs(MotorDelay);
    MOTORPort = 0x04;
    DelayMs(MotorDelay);
}

void half_step_clockwise(void){
    MOTORPort = 0x08;
    DelayMs(MotorDelay);
    MOTORPort = 0x0C;
    DelayMs(MotorDelay);
    MOTORPort = 0x04;
    DelayMs(MotorDelay);
    MOTORPort = 0x06;
    DelayMs(MotorDelay);
    MOTORPort = 0x02;
    DelayMs(MotorDelay);
    MOTORPort = 0x03;
    DelayMs(MotorDelay);
    MOTORPort = 0x01;
    DelayMs(MotorDelay);
    MOTORPort = 0x09;
    DelayMs(MotorDelay);
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
            backward(angle-curr_angle);
        } else{
            forward(angle-curr_angle);
        }
    }
}

//****************************************************************** 
//           joystick Tracker
//******************************************************************

//check if its working
void MoveMotorToJoyStick(void){
    if(Vx > 510){
        a = Vx - 465;
    } else if(Vx < 460){
        a = 465 - Vx;
    } else if((420 <= Vx) && (Vx <= 510)){
        a = 0;
    }
    if(Vy > 540){
        b = Vy - 495;
    } else if(Vy < 450){
        b = 495 - Vy;
    } else if((450 <= Vy) && (Vy <= 540)){
        b = 0;
    }
    if(a == 0){
        if(Vy > 540){
            alpha = 0;
        } else{
            alpha = 180;
        }
    } else if(b == 0){
        if(Vx > 510){
            alpha = 90;
        } else{
            alpha = 270;
        }
    } else{
        c = a/b;
        if(Vx > 510){
            alpha = (c - (c*c*c)/3 + (c*c*c*c*c)/5) * 180 / Phi;
        } else{
            alpha = (c - (c*c*c)/3 + (c*c*c*c*c)/5) * 180 / Phi;
            alpha = 180 + alpha;
        }
    }
    angle = (unsigned long)alpha;
    VxPrev = Vx;
    VyPrev = Vx;
    move_to_angle(angle*1000);
}


void CheckDiff(void){
    if(Vx > VxPrev){
        DiffX = Vx - VxPrev;
    } else{
        DiffX = VxPrev - Vx;
    }
    if(Vy > VyPrev){
        DiffY = Vy - VyPrev;
    } else{
        DiffY = VyPrev - Vy;
    }
}

//******************************************************************
//            script funcs
//******************************************************************

// void read_script(void){
// }

// void execute_script(void){
//     ack = 0;
//     int opcode = 0;
//     int NumScriptE = scriptt.num-1;
//     if (!scriptt.first_Written[NumScriptE]){
//         scriptt.first_Written[NumScriptE] = 1;
//     }else if(scriptt.first_Written[NumScriptE]){
//         CountScriptLines = scriptt.line[NumScriptE];
//     int l = 0;
//     for(l=0;l<CountScriptLines;l++){
//         opcode = read_opcode();
//     }

// }

// void read_opcode(void){
//     int i;
//     char m;
//     for(i=0;i<2;i++){
//         m = read(i);
//         if(m==0){
//             return 0;
//         } else if(m > (9 + '0')){

//         }
//     }
// }
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

}

//5
void clear_all(int X){
    RGB_clear;
    Leds_clear;
}

//6
void stepper_deg(unsigned long angle){
    move_to_angle(angle*1000);
    Op6 = 1;
    SendInfo();
}

//7
void scan_step(unsigned long l, unsigned long r){

    // moving to Left angle
    move_to_angle(l*1000);
    Op7 = 1;
    SendInfo(); // Tell PC that motor arrived to Left angle

    DelayMs(1000);

    // moving to Right angle
    if(r >= l){
        forward((r-l)*1000);
    } else{
        forward((360+r-l)*1000);
    }

    Op7 = 1;
    SendInfo(); // Tell PC that motor arrived to Left angle

}


