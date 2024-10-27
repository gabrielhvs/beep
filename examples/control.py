import tkinter as tk
import serial
import time


import socket
import struct
import time
import threading
import os

HOST = '192.168.123.123'
PORT = 10000
BUFFER_SIZE = 1024
value = 0
i = 0


class Example(tk.Frame):
    def __init__(self, parent):
        tk.Frame.__init__(self, parent, width=400,  height=400)

        self.label = tk.Label(self, text="last key pressed:  ", width=20)
        self.label.pack(fill="both", padx=100, pady=100)

        self.label.bind("<w>", self.on_wasd)
        self.label.bind("<a>", self.on_wasd)
        self.label.bind("<s>", self.on_wasd)
        self.label.bind("<d>", self.on_wasd)

        # give keyboard focus to the label by default, and whenever
        # the user clicks on it
        self.label.focus_set()
        self.label.bind("<1>", lambda event: self.label.focus_set())

    def on_wasd(self, event):
        self.label.configure(text="last key pressed: " + event.keysym);
        if(event.keysym == "w"):
            right = 255
            left = 255
        if(event.keysym == "a"):
            right = 510
            left = 255
        if(event.keysym == "d"):
            right = 255
            left = 510
        if(event.keysym == "s"):
            right = 510
            left = 510
        msg = "$"
        msg = msg + str(left)
        msg = msg + ";"
        msg = msg + str(right)
        msg = msg + ";$\n"

        print(msg)
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            s.connect((HOST, PORT))
            s.sendall(msg.encode())
            #data = s.recv(1024)
            #print(f"Received {data!r}")
            s.close()
        time.sleep(0.1)


if __name__ == "__main__":
    root = tk.Tk()
    Example(root).pack(fill="both", expand=True)
    root.mainloop()