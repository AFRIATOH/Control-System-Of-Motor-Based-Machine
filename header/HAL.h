#ifndef _HAL_H_
#define _HAL_H_

#include  "../header/BSP.h"    		    // private library - BSP layer
#include  "../header/Application.h"     // private library - APP layer
#include  <msp430g2553.h>               // MSP430x2xx
//#include  <msp430xG46x.h>             // MSP430x4xx 

extern enum FSMstate state;              // global variable
extern enum SYSmode lpm_mode;           // global variable
extern enum MovingDiraction MoveDiraction; // global variable
extern enum PainterMode PaintMode;         // global variable

typedef struct Scripts{
    int num;
    int size[3];
    int line[3];
    int Written[3];
    int first_Written[3];
    char *scripte_loc[3];
}Scripts;

extern volatile unsigned int MessegeType;
extern volatile int MotorDelay;
extern volatile unsigned long curr_angle;
extern Scripts scriptt;
extern volatile unsigned int Vx;
extern volatile unsigned int Vy;
extern volatile unsigned int VxPrev;
extern volatile unsigned int VyPrev;
extern unsigned int ArriveToZeroAngle;
extern volatile int ScriptNum;
extern volatile int ScriptNumFlag;
extern volatile int WriteOnFlash;
extern int ScriptIndex;
extern volatile char ScriptRx[10];
extern int ScriptReadIndex;
extern int ack;
extern int WriteOnFlashFlag;
extern int CountScriptSize;

// System
extern void sysConfig(void);
extern void enterLPM(unsigned char);

// Interrupts
void _buttonDebounceDelay(int button);

// Delays
extern void DelayMs(int);
extern void DelayUs(unsigned int);
extern void delay(unsigned int);
extern void delay_x(unsigned int);

//sample
extern void sample(void);
extern void MoveMotorToJoyStick(void);

//move motor
extern void StepCalculation(void);
extern void continuous_move(void);
extern void angle_increase(void);
extern void angle_decrease(void);
extern void forward(volatile long);
extern void backward(volatile long);
extern void step_clockwise(void);
extern void step_counterclockwise(void);
extern void half_step_clockwise(void);
extern void move_to_angle(unsigned long);

// script funcs
extern void scan_step(unsigned long, unsigned long);

//service funcs
__interrupt void ADC10_ISR(void);
__interrupt void Timer_A0(void);
__interrupt void Timer_A1(void);
__interrupt void USCI0RX_ISR(void);
__interrupt void USCI0TX_ISR(void);
__interrupt void PORT1_ISR(void);
#endif
