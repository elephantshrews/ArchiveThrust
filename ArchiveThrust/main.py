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
import os


def main():
   
    username = os.getenv("ST_USERNAME")
    password = os.getenv("ST_PASSWORD")
    norad_id = input("Please enter NORAD ID: ") 
    # Login to SPACETRACK
    login(username, password)


    # Create storage for TLEs
    perm_stor_void = create_permanent_storage()
    download_value =  download(norad_id, perm_stor_void)

    if download_value == -1:
        exit()
    
    perm_stor = ctypes.cast(perm_stor_void, ctypes.POINTER(TleStor))
    maneuver_array_type = Maneuver * 200  # Assuming you want space for 100 maneuvers
    maneuvers = maneuver_array_type()
    detect_maneuvers(perm_stor, maneuvers)
    #print("this is the maneuver fluctuation")
    #print(maneuvers[0].fluctuations[0])
    dates_raw = [(int(maneuvers[i].startEpochDay),int(maneuvers[i].epochYear)) for i in range(len(maneuvers))]
    dates = [ (x,y) for (x,y) in dates_raw if (x,y) !=(0,0) ]
    cls  = [maneuvers[i].confidenceLevel for i in range(len(dates))]
    converted_dates = [datetime(year, 1, 1) + timedelta(days=day-1) for (day, year) in dates]

    # Plotting
    plt.figure(figsize=(10, 6))
    plt.plot(converted_dates, cls, marker='o', linestyle='-', color='b')

    # Format the date axis
    plt.gca().xaxis.set_major_locator(mdates.AutoDateLocator())
    plt.gca().xaxis.set_major_formatter(mdates.DateFormatter('%d-%Y'))  # Show day and year

    # Rotate x labels for better readability
    plt.gcf().autofmt_xdate()

    # Labels and title
    plt.xlabel('Date (Day-Year)')
    plt.ylabel('Confidence Level')
    plt.title('Confidence Levels over Time')
    # X and Y values
    x = [1, 2, 3, 4, 5, 6]
    A = [1, 2, 3, 4, 5, 6]
    B = [2, 3, 4, 5, 6, 7]
    C = [3, 4, 5, 6, 7, 8]
    D = [4, 5, 6, 7, 8, 9]

    # Text to be displayed
    text = "Hello, you wanted the data for the norad_id = 25544"
    
    # Create Tkinter window
    root = tk.Tk()
    root.title("ArhiveThrust")

    # Create plots for A, B, C, and D
    #create_individual_plot(root, days, cls, "Plot Maneuvers", 0,0)
    #create_individual_plot(root, x, A, "Plot A", 0, 0)
    #create_individual_plot(root, x, B, "Plot B", 0, 1)
    #create_individual_plot(root, x, C, "Plot C", 1, 0)
    #create_individual_plot(root, x, D, "Plot D", 1, 1)

    # Display the text label below the plots

    label = ttk.Label(root, text=text)
    label.grid(row=2, column=0, columnspan=2, pady=10)
    
    def on_closing():
        root.destroy()
        exit()
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

