#ifndef _API_H_
#define _API_H_

#include  "../header/HAL.h"     // private library - HAL layer

extern void int_to_str(char *str, unsigned int number);
extern int str_to_int( char volatile *str);

extern void sleep(void);
extern void manual_control(void);
extern void PC_painter(void);
extern void calibration(void);
extern void script_mode(void);


#endif
