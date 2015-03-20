# python 2.7 only

from Tkinter import *
import Tkinter as tk
import subprocess 

WINDOW_SIZE = "800x400"

def Go():
	output = subprocess.check_output('scons ', shell=True)
	text.insert(tk.END, output)
	text.see(tk.END)


top = tk.Tk()
top.geometry(WINDOW_SIZE)
top.title("Megalodrone")
# Code to add widgets will go here...


text = tk.Text(master=top)
text.pack(side=tk.RIGHT, fill=X, expand=0)

btns = tk.Frame()
btns.pack(side=tk.LEFT, fill=BOTH, expand=1)

GoBtn = tk.Button(btns, text ="Go", command = Go)
GoBtn.pack(ipadx =30, ipady=5)

tk.Button(btns, text='Exit', command=top.destroy).pack(side=tk.BOTTOM, ipadx =30, ipady=5)

top.mainloop()

