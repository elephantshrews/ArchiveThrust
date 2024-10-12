from .modules import *

try:
    import tkinter as tk
except ImportError:
    raise ImportError("Tkinter is required to run this application. Please install it using your system package manager.")

from tkinter import ttk
import matplotlib.pyplot as plt
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
import matplotlib.dates as mdates
from datetime import datetime, timedelta
from matplotlib.lines import Line2D

# Function to create a plot for each dataset
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
    maneuver_array_type = Maneuver * 200  # Assuming you want space for 200 maneuvers
    maneuvers = maneuver_array_type()
    detect_maneuvers(perm_stor, maneuvers)

# Extracting the dates and maneuver types
    dates_raw = [(int(maneuvers[i].startEpochDay), int(maneuvers[i].epochYear) + 2000) for i in range(len(maneuvers))]
    dates = [(x, y) for (x, y) in dates_raw if (x, y) != (0, 0) and (x, y) != (0, 2000)]

# Extract maneuver types and confidence levels
    maneuverTypes = [maneuvers[i].maneuverType[1] for i in range(len(dates))]
    cls = [maneuvers[i].confidenceLevel for i in range(len(dates))]

# Remove entries with confidence level 0
    for i in range(len(dates) - 1, -1, -1):  # Iterate backwards to avoid index errors
        if cls[i] == 0:
            del cls[i]
            del dates[i]
            del maneuverTypes[i]

# Convert dates to datetime
    converted_dates = [datetime(year, 1, 1) + timedelta(days=day - 1) for (day, year) in dates]

# Define colors and markers for a galactic feel
    colors = ['#8A2BE2', '#7FFF00', '#FF6347', '#00BFFF', '#FF69B4', '#FFD700', '#DC143C','Orange']  # Vibrant colors
    markers = ['o', 's', 'D', 'v', '^', '<', '>','x']  # Different marker shapes
    labels = ['IN PLANE', 'OUT OF PLANE', 'ORBIT RAISING/LOWERING', 'PLANE CHANGE', 
              'STATION KEEPING', 'PERIGEE/APOGEE CHANGE', 'PHASING', 'DRAG COMPENSATION']

# Create figure with a dark background
    plt.figure(figsize=(10, 6), facecolor='black')
    plt.style.use('dark_background')  # Use dark background style

# Plot each maneuver with the corresponding color and marker
    for i in range(len(dates)):
        plt.scatter(converted_dates[i], cls[i], 
                    marker=markers[maneuverTypes[i]], 
                    color=colors[maneuverTypes[i]], 
                    s=100,  # Size of the markers
                    alpha=0.7,  # Transparency
                    edgecolor='white',  # Add white edges for a glowing effect
                    linewidth=0.5)

# Create custom legend handles
    legend_handles = [Line2D([0], [0], marker=markers[j], color='w', label=labels[j], 
                           markerfacecolor=colors[j], markersize=8) for j in range(len(labels))]

# Format the date axis
    plt.gca().xaxis.set_major_locator(mdates.AutoDateLocator())
    plt.gca().xaxis.set_major_formatter(mdates.DateFormatter('%d-%m-%Y'))  # Show day and year

# Rotate x labels for better readability
    plt.gcf().autofmt_xdate()
    plt.ylim(0, 1)

# Labels and title
    plt.xlabel('Date (Day-Year)', fontsize=14, color='white')
    plt.ylabel('Confidence Level', fontsize=14, color='white')
    plt.title('Confidence Levels over Time', fontsize=16, color='white')
    plt.legend(handles=legend_handles, title='Maneuver Types', loc='upper left', fontsize=10, title_fontsize=12, facecolor='black', framealpha=0.7, edgecolor='white')

# Show the plot
    plt.show()
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

    # Set the protocol for the window close event
    root.protocol("WM_DELETE_WINDOW", on_closing)

    # Start the Tkinter main loop
    root.mainloop()

# Uncomment these two lines if you want to run the script directly
# if __name__ == "__main__":
#     main()

