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
import tkinter as tk
from tkinter import ttk
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
import matplotlib.dates as mdates
from datetime import datetime, timedelta
from matplotlib.lines import Line2D
import warnings
warnings.filterwarnings("ignore", category=UserWarning)



# Function to create a plot for each dataset
def create_individual_plot(root, x, y, title, row, col):
    fig, ax = plt.subplots(figsize=(10, 6))  # Set a default size that can be adjusted
    ax.plot(x, y)
    ax.set_xlabel('Day')
    ax.set_ylabel('Confidence Level')
    ax.set_title(title)

def main():
    username = "m.snoeken@campus.tu-berlin.de"
    password = "UfQx95rK5Bj4haRhiKBP"
    norad_id = input("Please enter NORAD ID: ")

    # Simulated function calls (replace with actual code)
    login(username, password)


    # Create storage for TLEs
    perm_stor_void = create_permanent_storage()
    download_value = download(norad_id, perm_stor_void)

    if download_value == -1:
        exit()

    perm_stor = ctypes.cast(perm_stor_void, ctypes.POINTER(TleStor))
    maneuver_array_type = Maneuver * 200
    maneuvers = maneuver_array_type()
    detect_maneuvers(perm_stor, maneuvers)

    # Extracting the dates and maneuver types
    dates_raw = [(int(maneuvers[i].startEpochDay), int(maneuvers[i].epochYear) + 2000) for i in range(len(maneuvers))]
    dates = [(x, y) for (x, y) in dates_raw if (x, y) != (0, 0) and (x, y) != (0, 2000)]
    
    # Extract maneuver types and confidence levels
    maneuverTypes = [int(maneuvers[i].maneuverType[1]-2) for i in range(len(dates))]
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

    # Define colors and markers for a galactic feel
    colors = ['#8A2BE2', '#7FFF00', '#FF6347', '#00BFFF', '#FF69B4', '#FFD700']
    markers = ['o', 's', 'D', 'v', '^', '<']
    labels_plane = ['IN PLANE', 'OUT OF PLANE']
    labels = ['ORBIT RAISING/LOWERING', 'PLANE CHANGE', 'STATION KEEPING', 
              'PERIGEE/APOGEE CHANGE', 'PHASING', 'DRAG COMPENSATION']

    # Create figure with a dark background
    plt.figure(figsize=(13, 10), facecolor='black')
    plt.style.use('dark_background')

    # Plot each maneuver with the corresponding color, marker, and add "O" or "I" annotation
    for i in range(len(dates)):
        plt.scatter(converted_dates[i], cls[i], 
                    marker=markers[maneuverTypes[i]], 
                    color=colors[maneuverTypes[i]], 
                    s=100, 
                    alpha=0.7, 
                    edgecolor='white',
                    linewidth=0.5)

        # Add "I" for IN PLANE and "O" for OUT OF PLANE
        offset = 0.02  # Vertical offset for text
        if maneuverTypePlane[i] == 0:  # IN PLANE
            plt.text(converted_dates[i], cls[i] + offset, 'I', color='white', fontsize=10, ha='center', va='bottom')
        else:  # OUT OF PLANE
            plt.text(converted_dates[i], cls[i] + offset, 'O', color='white', fontsize=10, ha='center', va='bottom')

    # Determine the maximum value to set the y-axis limit dynamically
    max_confidence = max(cls) + 0.7  # Add a little extra space above the highest point

    # Create custom legend handles for maneuver types
    legend_handles = [Line2D([0], [0], marker=markers[j], color='w', label=labels[j], 
                             markerfacecolor=colors[j], markersize=8) for j in range(len(labels))]

    # Create custom legend handles for the plane types with "O" and "I"
    legend_handles_plane = [
        Line2D([0], [0], marker='None', color='w', label='In Plane (I)', linestyle='None', markersize=8),
        Line2D([0], [0], marker='None', color='w', label='Out of Plane (O)', linestyle='None', markersize=8)
    ]

    # Combine both legends
    plt.legend(handles=legend_handles + legend_handles_plane, 
               title='Maneuver Types and Plane', loc='upper left', fontsize=10, 
               title_fontsize=12, facecolor='black', framealpha=0.7, edgecolor='white')

    # Format the date axis to only show ticks for the data points
    plt.gca().xaxis.set_major_locator(mdates.DayLocator())
    plt.gca().xaxis.set_major_formatter(mdates.DateFormatter('%d-%m-%Y'))  

    # Set x-ticks to only the dates corresponding to the data points
    plt.gca().set_xticks(converted_dates)

    # Rotate x labels for better readability
    plt.gcf().autofmt_xdate()
    plt.ylim(0, max_confidence)  # Set y-limits dynamically

    # Labels and title
    plt.xlabel('Date (Day-Year)', fontsize=14, color='white')
    plt.ylabel('Confidence Level', fontsize=14, color='white')

    # Add description box above the plot dynamically
    description_text = (
        "Description of Maneuver Types:\n"
        "1. ORBIT RAISING/LOWERING: Adjusting the altitude of the spacecraft's orbit.\n"
        "2. PLANE CHANGE: Changing the orientation of the spacecraft's orbit.\n"
        "3. STATION KEEPING: Maintaining a position in orbit relative to another object.\n"
        "4. PERIGEE/APOGEE CHANGE: Adjusting the closest or farthest point of the orbit.\n"
        "5. PHASING: Adjusting the timing of maneuvers to rendezvous with another object.\n"
        "6. DRAG COMPENSATION: Counteracting atmospheric drag on low orbits.\n"
        "7. In PLANE: A maneuver which does not change its orbital energy.\n"
        "8. OUT OF PLANE: A maneuver which changes its orbital energy.\n"
    )

    # Use figtext to create a text box above the plot
    plt.figtext(0.6, 0.6, description_text, wrap=True, horizontalalignment='center', 
                fontsize=10, color='white', bbox=dict(facecolor='black', alpha=0.8))

    # Show the plot
    plt.show()