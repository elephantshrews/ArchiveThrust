from .modules import *

import tkinter as tk
from tkinter import ttk
import matplotlib.pyplot as plt
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg


# Function to create a plot for each dataset


def create_individual_plot(root, x, y, title, row, col):
    fig, ax = plt.subplots()
    ax.plot(x, y)
    ax.set_xlabel('X values')
    ax.set_ylabel('Y values')
    ax.set_title(title)

    # Embed plot into the Tkinter window
    canvas = FigureCanvasTkAgg(fig, master=root)
    canvas.draw()
    canvas.get_tk_widget().grid(row=row, column=col, padx=10, pady=10)

# Main function to set up the Tkinter window
def main():
    username = "m.snoeken@campus.tu-berlin.de"
    password = "UfQx95rK5Bj4haRhiKBP"
   
    norad_id = input("Please enter NORAD ID: ") 


    # Simulated function calls (replace with actual code)
    login(username, password)
    perm_stor_void = create_permanent_storage()
    download_value =  download(norad_id, perm_stor_void)

    if download_value == -1:
        exit()
    
    perm_stor = ctypes.cast(perm_stor_void, ctypes.POINTER(TleStor))
    maneuvers = Maneuver()
    detect_maneuvers(perm_stor, ctypes.byref(maneuvers))
    print("This is the fluctuation!!: {maneuvers[0].fluctuations[1]}")
    

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
    create_individual_plot(root, x, A, "Plot A", 0, 0)
    create_individual_plot(root, x, B, "Plot B", 0, 1)
    create_individual_plot(root, x, C, "Plot C", 1, 0)
    create_individual_plot(root, x, D, "Plot D", 1, 1)

    # Display the text label below the plots
    label = ttk.Label(root, text=text)
    label.grid(row=2, column=0, columnspan=2, pady=10)

    # Start the Tkinter main loop
    root.mainloop()

# Uncomment these two lines if you want to run the script directly
# if __name__ == "__main__":
#     main()
