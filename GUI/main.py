import Tkinter as tk
from Tkinter import StringVar, IntVar
from PIL import Image, ImageTk
import serial
import os
import re
serialPort = "/dev/ttyACM0"
baudRate = 9600
ser = serial.Serial(serialPort, baudRate, timeout=0, writeTimeout=0)
serBuffer = ""

PROGRAM_NAME = ' Infotainment '
MAX_WIDTH = 1280
MAX_HEIGHT = 720
TITLE_FONT = ("Helvetica", 18, "bold")
VOLUME_FONT = ("Helvetica", MAX_HEIGHT/4, "bold")

class MainApp(tk.Tk):
    def __init__(self, *args, **kwargs):
        tk.Tk.__init__(self, *args, **kwargs)
        self.bind('<Left>', self.left_key)
        self.bind('<Right>', self.right_key)
        container = tk.Frame(self)
        container.pack(side="top", fill="both", expand=True)
        container.grid_rowconfigure(0, weight=1)
        container.grid_columnconfigure(0, weight=1)

        self.frame_ptr = 0;
        self.frames = []
        for F in FRAME_LIST:
            page_name = F.__name__
            frame= F(parent=container, controller=self)
            self.frames.append(frame)
            frame.grid(row=0, column=0, sticky="nsew")

        self.update_view()

    def readSerial(self):
        while True:
            msg = ser.readline() # attempt to read a character from Serial
            msgs = re.sub(r'\\r|\\n', '', msg);
            if msgs == 'DOWN':
                print "DOWN SWIPE"
                self.event_generate("<<DOWN_SWIPE>>", when="tail")
            elif msgs == 'UP':
                print "UP SWIPE"
                self.event_generate("<<UP_SWIPE>>", when="tail")
            else:
                break
        self.after(10, self.readSerial) # check serial again soo

    def left_key(self, event):
        print "Left key pressed"
        self.show_prev_frame()

    def right_key(self, event):
        print "Right key pressed"
        self.show_next_frame()

    def show_next_frame(self):
        self.frame_ptr = (self.frame_ptr + 1) % len(self.frames)
        self.update_view()

    def show_prev_frame(self):
        self.frame_ptr = (self.frame_ptr - 1) % len(self.frames)
        self.update_view()

    def update_view(self):
        frame = self.frames[self.frame_ptr]
        frame.tkraise()
        frame.switch_bindings(self)

class FrameOne(tk.Frame):
    def __init__(self, parent, controller):
        tk.Frame.__init__(self, parent)
        self.controller = controller
        # self.configure(background='red')
        self.volume = IntVar()
        self.volume.set(50)

        insideFrame = tk.Frame(self)
        insideFrame.pack(side="top", pady=MAX_HEIGHT/2)

        path = "volume.png"
        img = Image.open(path).resize((250, 250), Image.ANTIALIAS)
        self.volume_pic = ImageTk.PhotoImage(img)
        labelVolImg = tk.Label(insideFrame, image=self.volume_pic)
        labelVolImg.pack(side="left", fill="x", pady=10, padx=50)

        labelVolPer = tk.Label(insideFrame, text="%", font=VOLUME_FONT)
        labelVolPer.pack(side="right", fill="x", pady=10, padx=50)
        labelVol = tk.Label(insideFrame, textvariable=self.volume, font=VOLUME_FONT)
        labelVol.pack(side="right", fill="x", pady=10, padx=20)

    def switch_bindings(self, controller):
        self.controller.unbind('<Up>')
        self.controller.unbind('<Down>')
        self.controller.bind('<Up>', self.up_key)
        self.controller.bind('<Down>', self.down_key)

    def up_key(self, event):
        print "increase volume"
        self.increase_vol()

    def down_key(self, event):
        print "decrease volume"
        self.decrease_vol()

    def increase_vol(self):
        # increase volume
        if self.volume.get() < 100:
            self.volume.set(self.volume.get() + 1)

    def decrease_vol(self):
        # decrease volume
        if self.volume.get() > 0:
            self.volume.set(self.volume.get() - 1)

class FrameTwo(tk.Frame):
    def __init__(self, parent, controller):
        tk.Frame.__init__(self, parent)
        self.controller = controller
        self.current_station = IntVar()
        self.current_station.set(980)

        insideFrame = tk.Frame(self)
        insideFrame.pack(side="top", pady=MAX_HEIGHT/2)

        path = "volume.png"
        img = Image.open(path).resize((250, 250), Image.ANTIALIAS)
        self.volume_pic = ImageTk.PhotoImage(img)
        labelVolImg = tk.Label(insideFrame, image=self.volume_pic)
        labelVolImg.pack(side="left", fill="x", pady=10, padx=50)

        labelVolPer = tk.Label(insideFrame, text="AM", font=VOLUME_FONT)
        labelVolPer.pack(side="right", fill="x", pady=10, padx=50)
        labelVol = tk.Label(insideFrame, textvariable=self.current_station, font=VOLUME_FONT)
        labelVol.pack(side="right", fill="x", pady=10, padx=20)

    def switch_bindings(self, controller):
        self.controller.unbind('<Up>')
        self.controller.unbind('<Down>')
        self.controller.bind('<Up>', self.up_key)
        self.controller.bind('<Down>', self.down_key)

    def up_key(self, event):
        print "increase volume"
        self.increase_freq()

    def down_key(self, event):
        print "decrease volume"
        self.decrease_freq()

    def increase_freq(self):
        if self.current_station.get() < 1650:
            self.current_station.set(self.current_station.get() + 5)

    def decrease_freq(self):
        if self.current_station.get() > 525:
            self.current_station.set(self.current_station.get() - 5)

class FrameThree(tk.Frame):
    def __init__(self, parent, controller):
        tk.Frame.__init__(self, parent)
        self.controller = controller
        self.configure(background='green')
        insideFrame = tk.Frame(self)
        insideFrame.pack(side="top", pady=MAX_HEIGHT/2)
        label = tk.Label(insideFrame, text="Music", font=VOLUME_FONT)
        label.pack(side="top", fill="x", pady=10)

    def switch_bindings(self, controller):
        self.controller.unbind('<Up>')
        self.controller.unbind('<Down>')
        self.controller.bind('<Up>', self.up_key)
        self.controller.bind('<Down>', self.down_key)

    def up_key(self, event):
        print "increase volume"

    def down_key(self, event):
        print "decrease volume"

class FrameFour(tk.Frame):
    def __init__(self, parent, controller):
        tk.Frame.__init__(self, parent)
        self.controller = controller
        self.configure(background='orange')
        label = tk.Label(self, text="A/C", font=TITLE_FONT)
        label.pack(side="top", fill="x", pady=10)

    def switch_bindings(self, controller):
        self.controller.unbind('<Up>')
        self.controller.unbind('<Down>')
        self.controller.bind('<Up>', self.up_key)
        self.controller.bind('<Down>', self.down_key)

    def up_key(self, event):
        print "increase"

    def down_key(self, event):
        print "decrease"

class FrameFive(tk.Frame):
    def __init__(self, parent, controller):
        tk.Frame.__init__(self, parent)
        self.controller = controller
        self.configure(background='purple')
        label = tk.Label(self, text="Fan", font=TITLE_FONT)
        label.pack(side="top", fill="x", pady=10)

    def switch_bindings(self, controller):
        self.controller.unbind('<Up>')
        self.controller.unbind('<Down>')
        self.controller.bind('<Up>', self.up_key)
        self.controller.bind('<Down>', self.down_key)

    def up_key(self, event):
        print "increase"

    def down_key(self, event):
        print "decrease"

FRAME_LIST = (FrameOne, FrameTwo, FrameThree, FrameFour, FrameFive)




if __name__ == '__main__':
    app = MainApp();
    app.geometry('{}x{}'.format(MAX_WIDTH, MAX_HEIGHT))
    app.resizable(width=False, height=False)
    app.attributes("-fullscreen", True)
    app.title(PROGRAM_NAME)
    print "Starting app"
    app.after(100, app.readSerial)
    app.mainloop()
