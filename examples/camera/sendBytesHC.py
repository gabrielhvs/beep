import serial
import time

ser = serial.Serial(
    port = "/dev/ttyUSB0",
    baudrate = 9600,
    parity = serial.PARITY_NONE,
    stopbits = serial.STOPBITS_ONE,
    bytesize = serial.EIGHTBITS,
    timeout  = 0.1
)
print("Serial status: " + str(ser.isOpen()))


import threading
import queue
import time

while True:
    #print("Sending...")
    ser.write("->ML040MR150SV110\n".encode())
    time.sleep(0.1)