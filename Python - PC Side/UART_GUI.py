### Libraries
import sys
import serial as ser
import time
import PySimpleGUI as sg
from PyQt5.QtWidgets import *
from PyQt5.QtGui import *
from PyQt5.QtCore import *
#


### Global Variables
state = '0'
enableTX = True
delay=0.30
#


### Class
class Window(QMainWindow):
    def __init__(self):
        super().__init__()

        # setting title
        self.setWindowTitle("Paint with PyQt5")

        # setting geometry to main window
        self.setGeometry(100, 100, 800, 600)

        # creating image object
        self.image = QImage(self.size(), QImage.Format_RGB32)

        # making image color to white
        self.image.fill(Qt.white)

        # variables
        # drawing flag
        self.drawing = False
        # default brush size
        self.brushSize = 2
        # default color
        self.brushColor = Qt.black

        # QPoint object to tract the point
        self.lastPoint = QPoint()

        # creating menu bar
        mainMenu = self.menuBar()

        # creating file menu for save and clear action
        fileMenu = mainMenu.addMenu("File")

        # adding brush size to main menu
        b_size = mainMenu.addMenu("Brush Size")

        # adding brush color to ain menu
        b_color = mainMenu.addMenu("Brush Color")

        # creating save action
        saveAction = QAction("Save", self)
        # adding short cut for save action
        saveAction.setShortcut("Ctrl + S")
        # adding save to the file menu
        fileMenu.addAction(saveAction)
        # adding action to the save
        saveAction.triggered.connect(self.save)

        # creating clear action
        clearAction = QAction("Clear", self)
        # adding short cut to the clear action
        clearAction.setShortcut("Ctrl + C")
        # adding clear to the file menu
        fileMenu.addAction(clearAction)
        # adding action to the clear
        clearAction.triggered.connect(self.clear)

        # creating options for brush sizes
        # creating action for selecting pixel of 4px
        pix_4 = QAction("4px", self)
        # adding this action to the brush size
        b_size.addAction(pix_4)
        # adding method to this
        pix_4.triggered.connect(self.Pixel_4)

        # similarly repeating above steps for different sizes
        pix_7 = QAction("7px", self)
        b_size.addAction(pix_7)
        pix_7.triggered.connect(self.Pixel_7)

        pix_9 = QAction("9px", self)
        b_size.addAction(pix_9)
        pix_9.triggered.connect(self.Pixel_9)

        pix_12 = QAction("12px", self)
        b_size.addAction(pix_12)
        pix_12.triggered.connect(self.Pixel_12)

        # creating options for brush color
        # creating action for black color
        black = QAction("Black", self)
        # adding this action to the brush colors
        b_color.addAction(black)
        # adding methods to the black
        black.triggered.connect(self.blackColor)

        # similarly repeating above steps for different color
        white = QAction("White", self)
        b_color.addAction(white)
        white.triggered.connect(self.whiteColor)

        green = QAction("Green", self)
        b_color.addAction(green)
        green.triggered.connect(self.greenColor)

        yellow = QAction("Yellow", self)
        b_color.addAction(yellow)
        yellow.triggered.connect(self.yellowColor)

        red = QAction("Red", self)
        b_color.addAction(red)
        red.triggered.connect(self.redColor)

    def init_uart(self):
        self.serial = QtSerialPort.QSerialPort()
        self.serial.setPortName(self.com_port_combo_box.currentText())  # todo find a way to find the correct port name
        self.serial.setBaudRate(str_to_baud[self.baud_rate_combo_box.currentText()])
        self.serial.setParity(str_to_parity[self.parity_combo_box.currentText()])
        self.serial.setDataBits(str_to_data_bits[self.data_bits_combo_box.currentText()])
        self.serial.setFlowControl(str_to_flow_control[self.flow_control_combo_box.currentText()])
        self.serial.setStopBits(str_to_stop_bits[self.stop_bits_combo_box.currentText()])
        self.serial.readyRead.connect(self.receive)
        self.serial.open(QIODevice.ReadWrite)
        return bool(self.serial)


    def mousePressEvent(self, event):

        # if left mouse button is pressed
            # make drawing flag true
        print("hi")
        self.init_uart()
        while QtSerialPort.in_waiting:
            pot = QtSerialPort.read(size=5).decode("ascii")  # read 3 byte from the input buffer
            pot = pot.replace('\0', '')
            volt = (str(3.3 * int(pot) / 1024))
        print(" Potentiometer value: " + volt[:5] + " [Volt]")

    # method for tracking mouse activity
    def JoystickMoveEvent(self, x,y):
        painter = QPainter(self.image)
        # set the pen of the painter
        painter.setPen(QPen(self.brushColor, self.brushSize,
                            Qt.SolidLine, Qt.RoundCap, Qt.RoundJoin))

        # draw line from the last point of cursor to the current point
        # this will draw only one step
        painter.drawLine(self.lastPoint, QPoint(x, y))

        # change the last point
        self.lastPoint = QPoint(x, y)
        # update
        self.update()

    # paint event
    def paintEvent(self, event):
        # create a canvas
        canvasPainter = QPainter(self)

        # draw rectangle on the canvas
        canvasPainter.drawImage(self.rect(), self.image, self.image.rect())

    # method for saving canvas
    def save(self):
        filePath, _ = QFileDialog.getSaveFileName(self, "Save Image", "",
                                                  "PNG(*.png);;JPEG(*.jpg *.jpeg);;All Files(*.*) ")

        if filePath == "":
            return
        self.image.save(filePath)

    # method for clearing every thing on canvas
    def clear(self):
        # make the whole canvas white
        self.image.fill(Qt.white)
        # update
        self.update()

    # methods for changing pixel sizes
    def Pixel_4(self):
        self.brushSize = 4

    def Pixel_7(self):
        self.brushSize = 7

    def Pixel_9(self):
        self.brushSize = 9

    def Pixel_12(self):
        self.brushSize = 12

    # methods for changing brush color
    def blackColor(self):
        self.brushColor = Qt.black

    def whiteColor(self):
        self.brushColor = Qt.white

    def greenColor(self):
        self.brushColor = Qt.green

    def yellowColor(self):
        self.brushColor = Qt.yellow

    def redColor(self):
        self.brushColor = Qt.red



### Functions
def state1(scom , enableTX , delay=0.30):
    #TX
    layout_1 = [[sg.Button('Clockwise')],
                [sg.Button('Counter Clockwise')],
                [sg.Button('Stop')],
                [sg.Button('Finish')],
                [sg.Button('Back')]]
    window = sg.Window('Manual control of motor based machine', layout_1)
    #while scom.out_waiting or enableTX:
    while True:
        event, values = window.read()
        if event == 'Back' or event == sg.WIN_CLOSED:
           break
        if event == 'Clockwise':
            while scom.out_waiting or enableTX:
                scom.write(bytes('1', 'ascii'))
                time.sleep(delay)
                if (scom.out_waiting == 0):
                    break
        if event == 'Counter Clockwise':
            while scom.out_waiting or enableTX:
                scom.write(bytes('2', 'ascii'))
                time.sleep(delay)
                if (scom.out_waiting == 0):
                    break
        if event == 'Stop':
            while scom.out_waiting or enableTX:
                scom.write(bytes('0', 'ascii'))
                time.sleep(delay)
                if (scom.out_waiting == 0):
                    break
        if event == 'Finish':
            while scom.out_waiting or enableTX:
                scom.write(bytes('3', 'ascii'))
                time.sleep(delay)
                if (scom.out_waiting == 0):
                    break
    window.close()


def state2(scom , enableTX , delay=0.30):
    # create pyqt5 app
    App = QApplication(sys.argv)

    # create the instance of our Window
    window = Window()

    # showing the window
    window.show()

    # start the app
    App.exec()

def state3(scom , enableTX , delay=0.30):

    while True:
        break

def state4(scom , enableTX , delay=0.30):

    while True:
        break

def reset_com():
    scom = ser.Serial('COM3', baudrate=9600, bytesize=ser.EIGHTBITS,
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
    global delay
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
            while scom.out_waiting or enableTX:
                scom.write(bytes(state, 'ascii'))
                time.sleep(delay)
                if (scom.out_waiting == 0):
                    break
            state1(scom, enableTX)
            while scom.out_waiting or enableTX:
                scom.write(bytes('!', 'ascii'))
                time.sleep(delay)
                if (scom.out_waiting == 0):
                    break
        if event == 'Joystick based PC painter':
            state = '2'
            while scom.out_waiting or enableTX:
                scom.write(bytes(state, 'ascii'))
                time.sleep(delay)
                if (scom.out_waiting == 0):
                    break
            #state2(scom, enableTX)
            enableTX = False
            while scom.in_waiting: #or not(enableTX):
                pot = scom.read(size=6).decode("ascii")
                print(pot)
           # while scom.out_waiting or enableTX:
               # scom.write(bytes('!', 'ascii'))
                #time.sleep(delay)
                #if (scom.out_waiting == 0):
                    #break
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