import serial
import time

ser = serial.Serial(
    port = "/dev/ttyUSB1",
    baudrate = 9600,
    parity = serial.PARITY_NONE,
    stopbits = serial.STOPBITS_ONE,
    bytesize = serial.EIGHTBITS,
    #timeout  = 0.1
)
print("Serial status: " + str(ser.isOpen()))


import threading
import queue
import time

def recieve():
    while True:
        try:
            data = ser.readline()
            print( data.decode() )
            # TODO - something with data
        except:
            print( "Error reading from port" )

def send():
    while True:
        #print("Sending...")
        ser.write("->ML040MR150SV110\n".encode())
        time.sleep(0.1)

fila = queue.Queue()
recieve_thread = threading.Thread(target=recieve)
send_thread = threading.Thread(target=send)
send_thread.start()
recieve_thread.start()
send_thread.join()
recieve_thread.join()
print("Threads finalizadas")