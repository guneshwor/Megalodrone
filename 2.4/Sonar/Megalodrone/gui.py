# python 2.7 only

from Tkinter import *
import Tkinter as tk
import subprocess 
import sys
import fileinput

WINDOW_SIZE = "800x400"

cmds = []
ComPort = 'COM1'

def SetCmds():
	for line in fileinput.input('MegalodroneMain.cpp', inplace=True):
		if line.startswith('const char Cmds[] PROGMEM'):
			sys.stdout.write('const char Cmds[] PROGMEM = { ' + str(cmds)[1:-1] + ' };\n')
		else:
			sys.stdout.write(line)

def AddCmd(cmd):
	global cmds 
	if cmds is None: 
		cmds = cmd
	else:
		cmds.append(cmd)

def Upload(): 
	ComPort = ComPortBox.get()
	SetCmds()
	text.config(state=NORMAL)
	output = ''
	try:
		output = subprocess.check_output(
			'scons ARDUINO_PORT=' + ComPort + ' upload',
			stderr=subprocess.STDOUT,
			shell=True
		)
	except:
		pass
	output = output + "\nDone uploading!!!\n" if output is not '' else 'Error occured\n'
	text.insert(tk.END, output )
	text.see(tk.END)
	text.config(state=DISABLED)
	global cmds
	del cmds[:]

def MoveForward():
	AddCmd('F')
	text.config(state=NORMAL)
	text.insert(tk.END, "Forward \n")
	text.see(tk.END)
	text.config(state=DISABLED)

def TurnLeft():
	AddCmd('L')
	text.config(state=NORMAL)
	text.insert(tk.END, "Turn Left\n")
	text.see(tk.END)
	text.config(state=DISABLED)

def TurnRight():
	AddCmd('R')
	text.config(state=NORMAL)
	text.insert(tk.END, "Turn Right\n")
	text.see(tk.END)	
	text.config(state=DISABLED)

def ReturnToOrigin():
	AddCmd('O')
	text.config(state=NORMAL)
	text.insert(tk.END, "Go Home!\n")
	text.see(tk.END)
	text.config(state=DISABLED)

def Undo():
	global cmds
	if cmds:
		text.config(state=NORMAL)
		text.delete("end-2c linestart", "end")
		text.insert(tk.END, "\n") 
		cmds.pop()
	text.config(state=DISABLED)

def Clear():
	global cmds
	del cmds[:]
	text.config(state=NORMAL)
	text.delete(1.0, tk.END)
	text.config(state=DISABLED)


top = tk.Tk()
top.geometry(WINDOW_SIZE)
top.title("Megalodrone")


text = tk.Text(master=top)
text.pack(side=tk.RIGHT, fill=X, expand=True)

btns = tk.Frame()

MoveForwardBtn = tk.Button(btns, text ="Move Forward", command = MoveForward, width=20)
MoveForwardBtn.pack(ipady=5)

TurnLeftBtn = tk.Button(btns, text ="Turn Left", command = TurnLeft, width=20)
TurnLeftBtn.pack(ipady=5)

TurnRightBtn = tk.Button(btns, text ="Turn Right", command = TurnRight, width=20)
TurnRightBtn.pack(ipady=5)

ReturnToOriginBtn = tk.Button(btns, text ="Return to Origin", command = ReturnToOrigin, width=20)
ReturnToOriginBtn.pack(ipady=5)

UndoBtn = tk.Button(btns, text ="Undo", command = Undo, width=10)
ClearBtn = tk.Button(btns, text ="Clear All", command = Clear, width=10)

UndoBtn.pack(ipady=5)
ClearBtn.pack(ipady=5)

tk.Button(btns, text='Exit', command=top.destroy, width=20).pack(ipady=5, side=tk.BOTTOM)
tk.Button(btns, text ="Upload", command = Upload, width=20).pack(ipady=5, side=tk.BOTTOM)

ComPortBox = tk.Entry(btns, bg='green', width=18, font='Courier 10', justify=CENTER, textvariable=ComPort)
ComPortBox.insert(tk.END, "COM1")
ComPortBox.pack(side=tk.BOTTOM)

btns.pack(side=tk.LEFT, fill=BOTH, expand=1)
top.mainloop()
