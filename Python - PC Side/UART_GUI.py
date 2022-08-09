### Libraries
import sys
import serial as ser
import time
import PySimpleGUI as sg
#


### Global Variables
state = '0'
enableTX = True
#


### Functions
def state1(scom , enableTX , delay=0.30):
    #TX
    while scom.out_waiting or enableTX:
        layout_1 = [[sg.Button('Clockwise')],
                    [sg.Button('Counter Clockwise')],
                    [sg.Button('Stop')],
                    [sg.Button('Finish')],
                    [sg.Button('Back')]]
        window = sg.Window('Manual control of motor based machine', layout_1)
        while True:
            event, values = window.read()
            if event == 'Back' or event == sg.WIN_CLOSED:
                break
            if event == 'Clockwise':
                scom.write(bytes("Clockwise"))
                time.sleep(delay)
            if event == 'Counter Clockwise':
                scom.write(bytes("Counter_Clockwise"))
                time.sleep(delay)
            if event == 'Stop':
                scom.write(bytes("Stop"))
                time.sleep(delay)
            if event == 'Finish':
                scom.write(bytes("Finish"))
                time.sleep(delay)

def state1(scom , enableTX , delay=0.30):

    while True:
        break

def state2(scom , enableTX , delay=0.30):

    while True:
        break

def state3(scom , enableTX , delay=0.30):

    while True:
        break

def state4(scom , enableTX , delay=0.30):

    while True:
        break

def reset_com():
    scom = ser.Serial('COM4', baudrate=9600, bytesize=ser.EIGHTBITS,
                             parity=ser.PARITY_NONE, stopbits=ser.STOPBITS_ONE,
                             timeout=1)
    scom.reset_input_buffer()
    scom.reset_output_buffer()
    return scom
#



### Main
def main():
    global enableTX
    global state
    delay = 0.30
    sg.theme('DarkAmber')
    #sg.theme('Dark Grey 13')
    scom = reset_com()   #reset the buffers


    layout_main = [[sg.Button('Manual control of motor based machine')],
              [sg.Button('Joystick based PC painter')],
              [sg.Button('Stepper Motor Calibration')],
              [sg.Button('Script Mode')],
              [sg.Button('Sleep Mode')],
              [sg.Button('Exit')]]
    window = sg.Window('Final Project - DCS', layout_main)

    while True:
        event, values = window.read()

        if event == 'Exit' or event == sg.WIN_CLOSED:
            break
        if event == 'Manual control of motor based machine':
            state = '1'
            scom.write(bytes(state, 'ascii'))
            time.sleep(delay)
            state1(scom, enableTX)
        if event == 'Joystick based PC painter':
            state = '2'
            scom.write(bytes(state, 'ascii'))
            time.sleep(delay)
            state2(scom, enableTX)
        if event == 'Stepper Motor Calibration':
            state = '3'
            scom.write(bytes(state, 'ascii'))
            time.sleep(delay)
            state3(scom, enableTX)
        if event == 'Script Mode':
            state = '4'
            scom.write(bytes(state, 'ascii'))
            time.sleep(delay)
            state4(scom, enableTX)
        if event == 'Sleep Mode':
            state = '0'
            scom.write(bytes(state, 'ascii'))
            time.sleep(delay)
    window.close()

if __name__ == "__main__":
    main()
#