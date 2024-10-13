# 
#
#  Archive Thrust - main.py 
#
#  Authors: Michiel Snoeken & Freddy Spaulding
#  GNU General Public License
#
#
#



# Import functions from C-library
# that are converted to Python functions in modules.py
from .modules import *

# Import used modules
import tkinter as tk
from   tkinter import ttk
import matplotlib.pyplot as plt
from   matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
import matplotlib.dates as mdates
from   datetime import datetime, timedelta
from   matplotlib.lines import Line2D
import warnings
import os


def main():
    # obtain username and password as environment variables
    username = os.getenv("ST_USERNAME") 
    password = os.getenv("ST_PASSWORD") 

     # Check if they are None
    if username is None or password is None:
        raise ValueError("""Username and password must be set as environment variables. 
                            If you did, please try restarting the terminal.  """)

    # Ask user for NORAD ID
    norad_id = input("Please enter NORAD ID: ")

    # Simulated function calls (replace with actual code)
    login(username, password)
    perm_stor_void = create_permanent_storage()
    download_value = download(norad_id, perm_stor_void)

    if download_value == -1:
        exit()

    perm_stor = ctypes.cast(perm_stor_void, ctypes.POINTER(TleStor))
    maneuver_array_type = Maneuver * 200
    maneuvers = maneuver_array_type()
    detect_maneuvers(perm_stor, maneuvers)

    text = """
Description of Maneuver Types:
1. ORBIT RAISING/LOWERING: Adjusting the altitude of the spacecraft's orbit.
2. PLANE CHANGE: Changing the orientation of the spacecraft's orbit.
3. STATION KEEPING: Maintaining a position in orbit relative to another object.
4. PERIGEE/APOGEE CHANGE: Adjusting the closest or farthest point of the orbit.
5. PHASING: Adjusting the timing of maneuvers to rendezvous with another object.
6. DRAG COMPENSATION: Counteracting atmospheric drag on low orbits.
7. In PLANE: A maneuver which does not change its orbital energy.
8. OUT OF PLANE: A maneuver which changes its orbital energy.
""" 


    # Create Tkinter window
    root = tk.Tk()
    root.title("ArchiveThrust")

    # Set the background color of the window to black
    root.configure(bg='black')

    # Plot the maneuvers to the left side (column 0)
    plot_maneuvers(root, maneuvers, 0, 0)

    # Create label with white text on a black background, next to the plot
    label = ttk.Label(root, text=text, foreground="white", background="black", justify=tk.LEFT)
    label.grid(row=0, column=1, padx=20, sticky='nw')


    def on_closing():
        root.quit()
        root.destroy()



    # Set the protocol for the window close event
    root.protocol("WM_DELETE_WINDOW", on_closing)

    # Start the Tkinter main loop
    root.mainloop()



def plot_maneuvers(root, maneuvers, row, col):
    # Create the figure and axis
    fig, ax = plt.subplots(figsize=(13, 10))
    plt.style.use('dark_background')

    # Set the plot background (axes and figure) to black
    fig.patch.set_facecolor('black')
    ax.set_facecolor('black')

    # Set axis labels, title, and ticks to white
    ax.set_xlabel('X Axis Label', color='white')
    ax.set_ylabel('Y Axis Label', color='white')
    ax.set_title("Maneuvers Plot", color='white')
    ax.tick_params(axis='x', colors='white')
    ax.tick_params(axis='y', colors='white')

    # Make the window frame (spines) white
    ax.spines['top'].set_color('white')
    ax.spines['bottom'].set_color('white')
    ax.spines['left'].set_color('white')
    ax.spines['right'].set_color('white')

    # Extract the dates and maneuver types
    dates_raw = [(int(maneuvers[i].startEpochDay), int(maneuvers[i].epochYear) + 2000) for i in range(len(maneuvers))]
    dates = [(x, y) for (x, y) in dates_raw if (x, y) != (0, 0) and (x, y) != (0, 2000)]

    # Extract maneuver types and confidence levels
    maneuverTypes = [int(maneuvers[i].maneuverType[1] - 2) for i in range(len(dates))]
    maneuverTypePlane = [maneuvers[i].maneuverType[0] for i in range(len(dates))]
    cls = [maneuvers[i].confidenceLevel for i in range(len(dates))]

    # Remove entries with confidence level 0
    for i in range(len(dates) - 1, -1, -1):
        if cls[i] == 0:
            del cls[i]
            del dates[i]
            del maneuverTypes[i]

    # Convert dates to datetime
    converted_dates = [datetime(year, 1, 1) + timedelta(days=day - 1) for (day, year) in dates]

    # Define colors and markers for the plot
    colors = ['#8A2BE2', '#7FFF00', '#FF6347', '#00BFFF', '#FF69B4', '#FFD700']
    markers = ['o', 's', 'D', 'v', '^', '<']
    labels_plane = ['IN PLANE', 'OUT OF PLANE']
    labels = ['ORBIT RAISING/LOWERING', 'PLANE CHANGE', 'STATION KEEPING',
              'PERIGEE/APOGEE CHANGE', 'PHASING', 'DRAG COMPENSATION']

    # Plot each maneuver with color, marker, and annotation
    for i in range(len(dates)):
        ax.scatter(converted_dates[i], cls[i],
                   marker=markers[maneuverTypes[i]],
                   color=colors[maneuverTypes[i]],
                   s=100, alpha=0.7, edgecolor='white', linewidth=0.5)

        # Add "I" for IN PLANE and "O" for OUT OF PLANE
        offset = 0.02
        if maneuverTypePlane[i] == 0:  # IN PLANE
            ax.text(converted_dates[i], cls[i] + offset, 'I', color='white', fontsize=10, ha='center', va='bottom')
        else:  # OUT OF PLANE
            ax.text(converted_dates[i], cls[i] + offset, 'O', color='white', fontsize=10, ha='center', va='bottom')

    # Set y-axis limit dynamically
    max_confidence = max(cls) + 0.7
    ax.set_ylim(0, max_confidence)

    # Create custom legends for maneuver types and plane types
    legend_handles = [Line2D([0], [0], marker=markers[j], color='w', label=labels[j],
                             markerfacecolor=colors[j], markersize=8) for j in range(len(labels))]
    legend_handles_plane = [
        Line2D([0], [0], marker='None', color='w', label='In Plane (I)', linestyle='None', markersize=8),
        Line2D([0], [0], marker='None', color='w', label='Out of Plane (O)', linestyle='None', markersize=8)
    ]
    ax.legend(handles=legend_handles + legend_handles_plane, title='Maneuver Types and Plane', loc='upper left',
              fontsize=10, title_fontsize=12, facecolor='black', framealpha=0.7, edgecolor='white')

    # Format the date axis
    ax.xaxis.set_major_locator(mdates.DayLocator())
    ax.xaxis.set_major_formatter(mdates.DateFormatter('%d-%m-%Y'))
    ax.set_xticks(converted_dates)
    plt.gcf().autofmt_xdate()

    # Add labels and title
    ax.set_xlabel('Date (Day-Year)', fontsize=14, color='white')
    ax.set_ylabel('Confidence Level', fontsize=14, color='white')
    ax.set_title("Maneuvers", fontsize=16, color='white')

    # Embed the plot into the Tkinter window
    canvas = FigureCanvasTkAgg(fig, master=root)
    canvas.draw()
    canvas.get_tk_widget().grid(row=row, column=col, padx=10, pady=10)
