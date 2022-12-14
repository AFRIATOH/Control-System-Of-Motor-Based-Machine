#include  "../header/API.h"    		// private library - API layer
#include  "../header/HAL.h"     // private library - HAL layer
//#include  "../source/HAL.c"



//******************************************************************
//function utitly
//******************************************************************
void int_to_str(char *str, unsigned int number){

    int j;
    int size = 0;
    long tmp = number;
    long len = 0;

    while(tmp){
        len++;
        tmp /= 10;
    }

    for(j = len - 1; j >= 0; j--){
        str[j] = (number % 10) + '0';
        number /= 10;
    }
    size += len;
    str[size] = '\0';
}

int str_to_int_ScriptRx( char volatile *str)
{
    int i,res = 0;
    for (i = ScriptIndex; str[i] != '-'; ++i) {
        if (str[i]> '9' || str[i]<'0')
            return -1;
        res = res * 10 + str[i] - '0';
        ScriptIndex++;
    }

    return res;
}

//******************************************************************
//state0
//******************************************************************
void sleep(void){
    RGB_clear;
    Leds_clear;
    __bis_SR_register(LPM0_bits + GIE);   // Enter LPM0
}

//******************************************************************
//state1
//******************************************************************
void manual_control(void){
    while(state == state1 && ArriveToZeroAngle == 0){
        continuous_move();
    }
    while(state == state1 && ArriveToZeroAngle == 1){
        sample();
        CheckDiff();
        if ((Vx>=510 || Vx<=420 || Vy>=540 || Vy<=450) && ((DiffX > 30) || (DiffY > 30))){
            MoveMotorToJoyStick();
        }
    }
}

//******************************************************************
//state2
//******************************************************************
void PC_painter(void){
    RGB_clear;
    Leds_clear;
    while(state == state2){
        sample();
        CheckDiff();
        if (((DiffX > 20) || (DiffY > 20))){
            SendInfo();
            VxPrev = Vx;
            VyPrev = Vy;
            Delay10Ms(3);
        }
    }
}

//******************************************************************
//state3
//******************************************************************
void calibration(void){
    RGB_clear;
    Leds_clear;
    StepCounter = 0;
    continuous_move();
    StepCalculation();
    SendInfo();
    state = state0;
}

//******************************************************************
//state4
//******************************************************************
void script_mode(void){
   RGB_clear;
   Leds_clear;
   read_script();
   execute_script();
}


