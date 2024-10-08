import tkinter as tk
import serial
import time


ser = serial.Serial(
    port = "/dev/ttyUSB1",
    baudrate = 1200,
    parity = serial.PARITY_NONE,
    stopbits = serial.STOPBITS_ONE,
    bytesize = serial.EIGHTBITS,
    timeout  = 0.1
)
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
        msg = ""
        msg = msg + str(left)
        msg = msg + ";"
        msg = msg + str(right)
        msg = msg + ";\n"

        print(msg)
        ser.write(msg.encode())
        time.sleep(0.1)


if __name__ == "__main__":
    root = tk.Tk()
    Example(root).pack(fill="both", expand=True)
    root.mainloop()