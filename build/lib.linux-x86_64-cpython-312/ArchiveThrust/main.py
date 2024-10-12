# Please enter SPACKETRACK login credentials here!
username = "m.snoeken@campus.tu-berlin.de"
password = "UfQx95rK5Bj4haRhiKBP"


# 
#
#  Archive Thrust - main.py 
#
#  Authors: Michiel Snoeken & Freddy Spaulding
#  GNU General Public License
#
#
#









# Import functions from the C-library
# that are converted to Python functions in modules.py
from .modules import *


# Import Tkinter and matplotlib for GUI
from tkinter import ttk
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
import matplotlib.pyplot as plt
import tkinter as tk



def main():
   
    norad_id = input("Please enter NORAD ID: ") 
    # Login to SPACETRACK
    login(username, password)


    # Create storage for TLEs
    perm_stor_void = create_permanent_storage()
    download_value =  download(norad_id, perm_stor_void)

    if download_value == -1:
        exit()
    
    perm_stor = ctypes.cast(perm_stor_void, ctypes.POINTER(TleStor))
    maneuver_array_type = Maneuver * 3000  # Space for 100 maneuvers
    maneuvers = maneuver_array_type()
    
    detect_maneuvers(perm_stor, maneuvers)
    print("this is the maneuver fluctuation")
    print(maneuvers[0].fluctuations[0])
    
    days = [maneuvers[i].startEpochDay for i in range(len(maneuvers))]
    cls  = [maneuvers[i].confidenceLevel for i in range(len(maneuvers))]


    # Text to be displayed
    text = "Hello, you wanted the data for the norad_id = 25544"
    
    # Create Tkinter window
    root = tk.Tk()
    root.title("ArhiveThrust")

    create_individual_plot(root, days, cls, "Plot Maneuvers", 0,0)

    label = ttk.Label(root, text=text)
    label.grid(row=2, column=0, columnspan=2, pady=10)
    
    def on_closing():
        root.destroy()

    # Set the protocol for the window close event
    root.protocol("WM_DELETE_WINDOW", on_closing)

    # Start the Tkinter main loop
    root.mainloop()


def create_individual_plot(root, x, y, title, row, col):
    fig, ax = plt.subplots()
    ax.plot(x, y)
    ax.set_xlabel('Day')
    ax.set_ylabel('Confidence Level')
    ax.set_title("Maneuvers")

    # Embed plot into the Tkinter window
    canvas = FigureCanvasTkAgg(fig, master=root)
    canvas.draw()
    canvas.get_tk_widget().grid(row=row, column=col, padx=10, pady=10)
