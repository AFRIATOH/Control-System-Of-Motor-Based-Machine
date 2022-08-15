#include  "../header/API.h"    		    // private library - API layer
#include  "../header/Application.h"    	// private library - APP layer

enum FSMstate state;
enum SYSmode lpm_mode;
enum MovingDiraction MoveDiraction;
enum PainterMode PaintMode;       // global variable

void main(void){
  
  sysConfig();
  UCA0CTL1 &= ~UCSWRST;       // Initialize USCI state machine
  IE2 &= ~UCA0TXIE;          // Disable TX interrupt
  IE2 |= UCA0RXIE;          // Enable RX interrupt
  state = state0;          // start in idle state on RESET
  lpm_mode = mode0;       // start in idle state on RESET

  while(1)
  {
    switch(state)
    {
      case state0:
            sleep(); // go to sleep
            break;
                  
      case state1: 
            manual_control(); // Manual control of motor based machine
            break;
  
      case state2:
            PC_painter(); // Joystick based PC painter
            break;

      case state3:
           calibration(); // Stepper Motor Calibration
           break;

      case state4: 
            //script_mode(); // Script Mode
          break;
  
      default:
          state = state0;
          break;
    }
  }
}
