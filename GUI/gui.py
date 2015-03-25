# python 2.7 only

from Tkinter import *
import Tkinter as tk
import subprocess 

WINDOW_SIZE = "800x400"

def Go():
	text.config(state=NORMAL)
	output = subprocess.check_output('scons ', shell=True)
	text.insert(tk.END, output)
	text.insert(tk.END, "Done uploading!!!\n")
	text.see(tk.END)
	text.config(state=DISABLED)

def MoveForward():
	text.config(state=NORMAL)
	text.insert(tk.END, "Forward 1 meter\n")
	text.see(tk.END)
	text.config(state=DISABLED)

def TurnLeft():
	text.config(state=NORMAL)
	text.insert(tk.END, "Turn Left\n")
	text.see(tk.END)
	text.config(state=DISABLED)

def TurnRight():
	text.config(state=NORMAL)
	text.insert(tk.END, "Turn Right\n")
	text.see(tk.END)	
	text.config(state=DISABLED)

def ReturnToOrigin():
	text.config(state=NORMAL)
	text.insert(tk.END, "Go Home!\n")
	text.see(tk.END)
	text.config(state=DISABLED)

def Undo():
	text.config(state=NORMAL)
	text.delete("end-2c linestart", "end")
	text.config(state=DISABLED)

def Clear():
	text.config(state=NORMAL)
	text.delete(1.0, tk.END)
	text.config(state=DISABLED)


top = tk.Tk()
top.geometry(WINDOW_SIZE)
top.title("Megalodrone")
# Code to add widgets will go here...


text = tk.Text(master=top)
text.pack(side=tk.RIGHT, fill=X, expand=0)

btns = tk.Frame()
btns.pack(side=tk.LEFT, fill=BOTH, expand=1)

MoveForwardBtn = tk.Button(btns, text ="Move Forward", command = MoveForward, width=20)
MoveForwardBtn.pack(ipady=5)

TurnLeftBtn = tk.Button(btns, text ="Turn Left", command = TurnLeft, width=20)
TurnLeftBtn.pack(ipady=5)

TurnRightBtn = tk.Button(btns, text ="Turn Right", command = TurnRight, width=20)
TurnRightBtn.pack(ipady=5)

ReturnToOriginBtn = tk.Button(btns, text ="Return to Origin", command = ReturnToOrigin, width=20)
ReturnToOriginBtn.pack(ipady=5)

UndoBtn = tk.Button(btns, text ="Undo", command = Undo, width=10)
UndoBtn.pack(ipady=5, side=tk.LEFT)

ClearBtn = tk.Button(btns, text ="Clear All", command = Clear, width=10)
ClearBtn.pack(ipady=5, side=tk.LEFT)

tk.Button(btns, text='Exit', command=top.destroy, width=10).pack(ipady=5, side=tk.BOTTOM)
tk.Button(btns, text ="Upload", command = Go, width=20).pack()

MoveForwardBtn = tk.Button(btns, text ="Move Forward", command = MoveForward)
MoveForwardBtn.pack(ipady=5)

TurnLeftBtn = tk.Button(btns, text ="Turn Left", command = TurnLeft)
TurnLeftBtn.pack(ipady=5)

TurnRightBtn = tk.Button(btns, text ="Turn Right", command = TurnRight)
TurnRightBtn.pack(ipady=5)

ReturnToOriginBtn = tk.Button(btns, text ="Return to Origin", command = ReturnToOrigin)
ReturnToOriginBtn.pack(ipady=5)

UndoBtn = tk.Button(btns, text ="Undo", command = Undo)
UndoBtn.pack(ipady=5, side=tk.LEFT)

ClearBtn = tk.Button(btns, text ="Clear All", command = Clear)
ClearBtn.pack(ipady=5, side=tk.RIGHT)

tk.Button(btns, text='Exit', command=top.destroy).pack(side=tk.BOTTOM, ipadx =30, ipady=5)
tk.Button(btns, text ="Upload", command = Go).pack(ipadx=30, ipady=5, side=tk.BOTTOM)
>>>>>>> 5fd611384733ef5d44535e692cdb08b846732d96

top.mainloop()


