import matplotlib.pyplot as plt
import numpy as np
from matplotlib import style
from scipy import signal
from _Lib import _LibExtractWaveform as extractWave
from _Lib import _libReadData as readD
import matplotlib.dates as mdates
from datetime import datetime, timedelta
import os
style.use('ggplot')

timeStampData,flowData = readD.ReadDataFromPath("ResmedFlow.txt", False)
timeStampData = timeStampData[-6:]
workspace_folder = os.path.dirname(os.path.abspath(__file__))

path_var = os.path.join(workspace_folder, "Debug/print.txt")
filteredFlow = readD.ReadDataPath(path_var, False)

path_var = os.path.join(workspace_folder, "Debug/NegAvg.csv")
negAvgPath = readD.ReadEventFromCSV(path_var, 2)

path_var = os.path.join(workspace_folder, "Debug/PeakPos.csv")
peakPosPath = readD.ReadEventFromCSV(path_var, 2)

path_var = os.path.join(workspace_folder, "ComparisionFolder/ApneaTime.csv")
eventInfoApn = readD.ReadEventFromCSV(path_var, 3)

path_var = os.path.join(workspace_folder, "ComparisionFolder/_events.csv")
resmedEventInfoApn = readD.ReadEventFromCSV(path_var, 4, True)


samplingFreq = 25
eventInfoX = np.array(eventInfoApn)
resmedEventInfoX = np.array(resmedEventInfoApn)
negAvgPathX = np.array(negAvgPath)
peakPosPathX = np.array(peakPosPath)

flowData = flowData[:len(filteredFlow)]
getXaxisTimePoints = extractWave.get_timestamp(flowData, samplingFreq, timeStampData,duration=60)


fig, ax = plt.subplots(2,figsize=(15, 5),sharex=True)
# fig.suptitle('Main Title for Both Subplots', fontsize=24)
 
ax[0].plot(getXaxisTimePoints, flowData,color = 'violet', label='Raw Flow')
ax[0].grid(True)
ax[0].legend()

# Plot the flow signal
ax[1].plot(getXaxisTimePoints, filteredFlow,color = 'seagreen', label='Filtered Signal')

# Format the x-axis to show appropriate time format
fig.autofmt_xdate()  # Auto-format the date labels
time_format = '%H:%M:%S'

total_samples = len(flowData)

n_samples = min(int(60 * samplingFreq), total_samples)
# Set up the x-axis formatter
ax[1].xaxis.set_major_formatter(mdates.DateFormatter(time_format))

# Process event information if provided
if eventInfoX is not None and isinstance(eventInfoX, (list, np.ndarray)) and len(eventInfoX) > 0:
    apnea_times = []
    hypopnea_times = []
    
    # Process each event
    for event in eventInfoX:
        if isinstance(event, (list, tuple, np.ndarray)) and len(event) >= 2:
            event_index = int(event[0])
            event_type = event[1]
            
            # Verify the event index is within range
            if event_index < total_samples:
                event_time = getXaxisTimePoints[event_index]
                
                # Categorize events
                if event_type == '-Apnea-':
                    apnea_times.append(event_time)
                elif event_type == '-Hypopnea-':
                    hypopnea_times.append(event_time)
    
    # Plot apnea events
    if apnea_times:
        # Find y-value at the top of the current plot
        y_max = max(flowData[:n_samples]) * 1
        
        for apnea_time in apnea_times:
            ax[1].stem([apnea_time], [y_max], linefmt='r-', markerfmt='ro', basefmt=' ')
            ax[1].annotate('A', xy=(apnea_time, y_max), 
                        xytext=(0, 5), textcoords='offset points',
                        ha='center', color='red', fontweight='bold')
        
    # Plot hypopnea events
    if hypopnea_times:
        # Find y-value at the top of the current plot
        y_max = max(flowData[:n_samples]) * 1.1
        
        for hypopnea_time in hypopnea_times:
            ax[1].stem([hypopnea_time], [y_max], linefmt='b-', markerfmt='bo', basefmt=' ')
            ax[1].annotate('H', xy=(hypopnea_time, y_max), 
                        xytext=(0, 5), textcoords='offset points',
                        ha='center', color='blue', fontweight='bold')
    
    # Add legend entries for events
    if apnea_times:
        ax[1].plot([], [], 'ro', label='Apnea')
    if hypopnea_times:
        ax[1].plot([], [], 'bo', label='Hypopnea')
    ax[1].set_title(f'vREM Apnea - {len(apnea_times)} : Hypopnea - {len(hypopnea_times)}')

# Process event information if provided
if resmedEventInfoX is not None and isinstance(resmedEventInfoX, (list, np.ndarray)) and len(resmedEventInfoX) > 0:
    res_apnea_times = []
    res_hypopnea_times = []
    res_central_apnea_times = []
    res_unclassified_apnea_times = []
    
    # Process each event
    for event in resmedEventInfoX:
        if isinstance(event, (list, tuple, np.ndarray)) and len(event) >= 3:
            event_index = int(int(event[0]) * samplingFreq)
            event_type = event[2]
            
            # Verify the event index is within range
            if event_index < total_samples:
                event_time = getXaxisTimePoints[event_index]
                
                # Categorize events
                if event_type == "Obstructive Apnea":
                    res_apnea_times.append(event_time)
                elif event_type == "Hypopnea":
                    res_hypopnea_times.append(event_time)
                elif event_type == "Central Apnea":
                    res_central_apnea_times.append(event_time)
                elif event_type == "Apnea":
                    res_unclassified_apnea_times.append(event_time)
    
    # Plot apnea events
    if res_apnea_times:
        # Find y-value at the top of the current plot
        y_max = max(flowData[:n_samples]) * 1
        
        for apnea_time in res_apnea_times:
            ax[0].stem([apnea_time], [y_max], linefmt='r-', markerfmt='ro', basefmt=' ')
            ax[0].annotate('A', xy=(apnea_time, y_max), 
                        xytext=(0, 5), textcoords='offset points',
                        ha='center', color='red', fontweight='bold')
        
    # Plot hypopnea events
    if res_hypopnea_times:
        # Find y-value at the top of the current plot
        y_max = max(flowData[:n_samples]) * 1.1
        
        for hypopnea_time in res_hypopnea_times:
            ax[0].stem([hypopnea_time], [y_max], linefmt='b-', markerfmt='bo', basefmt=' ')
            ax[0].annotate('H', xy=(hypopnea_time, y_max), 
                        xytext=(0, 5), textcoords='offset points',
                        ha='center', color='blue', fontweight='bold')
    
    if res_central_apnea_times:
        # Find y-value at the top of the current plot
        y_max = max(flowData[:n_samples]) * 1
        
        for cent_apnea_time in res_central_apnea_times:
            ax[0].stem([cent_apnea_time], [y_max], linefmt='r-', markerfmt='ro', basefmt=' ')
            ax[0].annotate('CA', xy=(cent_apnea_time, y_max), 
                        xytext=(0, 5), textcoords='offset points',
                        ha='center', color='red', fontweight='bold')
    
    if res_unclassified_apnea_times:
        # Find y-value at the top of the current plot
        y_max = max(flowData[:n_samples]) * 1
        
        for uncl_apnea_time in res_unclassified_apnea_times:
            ax[0].stem([uncl_apnea_time], [y_max], linefmt='r-', markerfmt='ro', basefmt=' ')
            ax[0].annotate('UA', xy=(uncl_apnea_time, y_max), 
                        xytext=(0, 5), textcoords='offset points',
                        ha='center', color='red', fontweight='bold')
    

    # Add legend entries for events
    if res_apnea_times:
        ax[0].plot([], [], 'ro', label='Apnea')
    if res_hypopnea_times:
        ax[0].plot([], [], 'bo', label='Hypopnea')
    if res_central_apnea_times:
        ax[0].plot([], [], 'ro', label='Apnea')
    if res_unclassified_apnea_times:
        ax[0].plot([], [], 'bo', label='Hypopnea')
    ax[0].set_title(f'Resmed Apnea - {len(res_apnea_times) + len(res_unclassified_apnea_times) + len(res_central_apnea_times)} : Hypopnea - {len(res_hypopnea_times)}')

    # Process event information if provided
    if negAvgPathX is not None and isinstance(negAvgPathX, (list, np.ndarray)) and len(negAvgPathX) > 0:
        neg_avg_time = []
        neg_avg_value = []
        # Process each event
        for eventNeg in negAvgPathX:
            if isinstance(event, (list, tuple, np.ndarray)) and len(eventNeg) >= 2:
                event_index = int(eventNeg[0])
                event_neg = eventNeg[1]
                
                # Verify the event index is within range
                if event_index < total_samples:
                    event_time = getXaxisTimePoints[event_index]
                    neg_avg_time.append(event_time)
                    neg_avg_value.append(float(event_neg))
                    
        
        ax[1].plot(neg_avg_time, neg_avg_value, 'm-', label='neg')
    # Process event information if provided
    if peakPosPathX is not None and isinstance(peakPosPathX, (list, np.ndarray)) and len(peakPosPathX) > 0:
        peak_times = []
        peaks_data = []
        
        # Process each event
        for eventPeak in peakPosPathX:
            if isinstance(eventPeak, (list, tuple, np.ndarray)) and len(eventPeak) >= 2:
                peak_event_index = int(eventPeak[0])
                peak_event_type = eventPeak[1]
                
                # Verify the event index is within range
                if peak_event_index < total_samples:
                    peak_event_time = getXaxisTimePoints[peak_event_index]
                    peak_times.append(peak_event_time)
                    peaks_data.append(float(peak_event_type))
    
        ax[1].plot(peak_times, peaks_data, 'kx', label='peaks')

# Add title and labels
ax[1].set_xlabel('Time')
ax[1].set_ylabel('Amplitude')
ax[1].grid(True)
ax[1].legend()



# Adjust the layout
plt.tight_layout()

plt.show()
