import numpy as np
import matplotlib.pyplot as plt
import mne
import matplotlib.dates as mdates
from datetime import datetime, timedelta

def extract_flow_signal_robust(edf_file_path):
    """
    Extract the flow signal from an EDF file using MNE which is more robust
    for handling non-standard EDF files.
    
    Parameters:
    edf_file_path (str): Path to the EDF file
    
    Returns:
    tuple: (flow_signal, sampling_rate)
    """
    try:
        # Try loading with more permissive parameters
        raw = mne.io.read_raw_edf(edf_file_path, preload=True, verbose=False)
        
        # Get channel names
        ch_names = raw.ch_names
        print("Available channels:")
        for i, ch in enumerate(ch_names):
            print(f"{i}: {ch}")
        
        # Look for flow-related channels
        flow_terms = ['flow', 'nasal', 'resp', 'breathing', 'airflow', 'pressure']
        flow_channels = [ch for ch in ch_names 
                        if any(term in ch.lower() for term in flow_terms)]
        
        if not flow_channels:
            print("No flow signal channel found. Please specify a channel manually.")
            print("Available channels:", ch_names)
            return None, None
        
        # Use the first flow-related channel found
        flow_channel = flow_channels[0]
        print(f"Extracting flow signal from channel: {flow_channel}")
        
        # Extract the flow data
        flow_data, times = raw[flow_channel]
        flow_signal = flow_data.flatten()
        
        # Get sampling frequency
        sampling_freq = raw.info['sfreq']
        
        return flow_signal, sampling_freq
    
    except Exception as e:
        print(f"Error with MNE approach: {str(e)}")
        
        # Try an alternative approach using pyedflib with more forgiving settings
        try:
            print("Attempting alternative extraction method...")
            import pyedflib
            
            # Get header information first
            header = pyedflib.highlevel.read_edf_header(edf_file_path)
            signal_headers = header['signalparam']
            
            # Print all signals
            print("Available signals based on header:")
            for i, signal in enumerate(signal_headers):
                print(f"{i}: {signal['label']}")
            
            # Look for flow-related signals
            flow_indices = [i for i, sig in enumerate(signal_headers) 
                          if any(term in sig['label'].lower() 
                                for term in flow_terms)]
            
            if not flow_indices:
                print("No flow signal found in headers.")
                return None, None
                
            # Use the first flow-related signal found
            flow_idx = flow_indices[0]
            print(f"Trying to extract: {signal_headers[flow_idx]['label']}")
            
            # Try to open and read the signal
            signals, signal_headers, header = pyedflib.highlevel.read_edf(
                edf_file_path, 
                ch_nrs=flow_idx, 
                verbose=True,
                digital=False  # Try to read physical values
            )
            
            if isinstance(signals, list):
                flow_signal = signals[0]
            else:
                flow_signal = signals
                
            sampling_freq = signal_headers[0]['sample_rate']
            
            return flow_signal, sampling_freq
            
        except Exception as nested_e:
            print(f"Alternative extraction also failed: {str(nested_e)}")
            
            # Last resort: try to read raw binary data
            print("Attempting to read raw binary data from the file...")
            try:
                # This is a very basic approach that might work for some proprietary formats
                with open(edf_file_path, 'rb') as f:
                    # Read the entire file
                    data = f.read()
                    
                    # Look for data blocks after the header
                    # EDF header is typically 256 bytes per channel plus some metadata
                    # This is a rough estimation and may need adjustment
                    offset = 256  # Starting offset, adjust as needed
                    
                    # Extract what might be data
                    raw_data = data[offset:]
                    
                    # Convert to numpy array - assuming 16-bit integers
                    # This is a guess and might need adjusting based on the specific format
                    import struct
                    values = []
                    for i in range(0, len(raw_data), 2):
                        if i+1 < len(raw_data):
                            val = struct.unpack('<h', raw_data[i:i+2])[0]
                            values.append(val)
                    
                    flow_signal = np.array(values)
                    
                    # Assume a common sampling rate for respiratory signals
                    sampling_freq = 100.0  # Hz, this is a guess
                    
                    print(f"Extracted {len(flow_signal)} raw data points")
                    return flow_signal, sampling_freq
                    
            except Exception as raw_e:
                print(f"Raw data extraction failed: {str(raw_e)}")
                return None, None

from datetime import datetime, timedelta

def get_timestamp(flow_signal, sampling_freq, timestamp, duration=60):
    """
    Plot a portion of the flow signal with real timestamps on the x-axis, including apnea and hypopnea events.
    
    Args:
        flow_signal (array): The flow signal data
        sampling_freq (float): Sampling frequency in Hz
        timestamp (str): Timestamp string in format 'HHMMSS'
        duration (int): Duration to plot in seconds
        save_path (str, optional): Path to save the figure, if provided
        eventInfo (list): List of tuples with event information (time_index, event_type)
                          where event_type is 'A' for apnea or 'H' for hypopnea
    """
    if flow_signal is None:
        return
    
    # Parse the timestamp
    hours = int(timestamp[0:2])
    minutes = int(timestamp[2:4])
    seconds = int(timestamp[4:6])
    
    # Create a base datetime (using today's date as we only care about time)
    base_date = datetime.now().replace(hour=hours, minute=minutes, second=seconds, microsecond=0)
    
    # Generate timestamps for all data points
    total_samples = len(flow_signal)
    print(f"Generating timestamps for all {total_samples} data points...")
    
    # Create time array for all data points
    time_offsets = np.arange(total_samples) / sampling_freq
    time_points = [base_date + timedelta(seconds=float(offset)) for offset in time_offsets]
    
    print(f"Generated {len(time_points)} timestamps for {total_samples} data points")
    
    # Calculate number of samples to plot for the requested duration
    n_samples = min(int(duration * sampling_freq), total_samples)
    print(f"Plotting {n_samples} samples")
    
    return time_points


def plot_flow_signal_with_timestamp(flow_signal, sampling_freq, timestamp, duration=60, save_path=None, eventInfo=None):
    """
    Plot a portion of the flow signal with real timestamps on the x-axis, including apnea and hypopnea events.
    
    Args:
        flow_signal (array): The flow signal data
        sampling_freq (float): Sampling frequency in Hz
        timestamp (str): Timestamp string in format 'HHMMSS'
        duration (int): Duration to plot in seconds
        save_path (str, optional): Path to save the figure, if provided
        eventInfo (list): List of tuples with event information (time_index, event_type)
                          where event_type is 'A' for apnea or 'H' for hypopnea
    """
    if flow_signal is None:
        return
    
    # Parse the timestamp
    hours = int(timestamp[0:2])
    minutes = int(timestamp[2:4])
    seconds = int(timestamp[4:6])
    
    # Create a base datetime (using today's date as we only care about time)
    base_date = datetime.now().replace(hour=hours, minute=minutes, second=seconds, microsecond=0)
    
    # Generate timestamps for all data points
    total_samples = len(flow_signal)
    print(f"Generating timestamps for all {total_samples} data points...")
    
    # Create time array for all data points
    time_offsets = np.arange(total_samples) / sampling_freq
    time_points = [base_date + timedelta(seconds=float(offset)) for offset in time_offsets]
    
    print(f"Generated {len(time_points)} timestamps for {total_samples} data points")
    
    # Calculate number of samples to plot for the requested duration
    n_samples = min(int(duration * sampling_freq), total_samples)
    print(f"Plotting {n_samples} samples")
    
    # Create the figure
    fig, ax = plt.subplots(figsize=(15, 5))
    
    # Plot the flow signal
    ax.plot(time_points, flow_signal,color = 'seagreen', label='Flow Signal')
    
    # Format the x-axis to show appropriate time format
    fig.autofmt_xdate()  # Auto-format the date labels
    time_format = '%H:%M:%S'
    if duration <= 10:  # Show milliseconds for shorter durations
        time_format = '%H:%M:%S.%f'
    
    # Set up the x-axis formatter
    ax.xaxis.set_major_formatter(mdates.DateFormatter(time_format))
    
    
    # Process event information if provided
    if eventInfo is not None and isinstance(eventInfo, (list, np.ndarray)) and len(eventInfo) > 0:
        apnea_times = []
        hypopnea_times = []
        
        # Process each event
        for event in eventInfo:
            if isinstance(event, (list, tuple, np.ndarray)) and len(event) >= 2:
                event_index = int(event[0])
                event_type = event[1]
                
                # Verify the event index is within range
                if event_index < total_samples:
                    event_time = time_points[event_index]
                    
                    # Categorize events
                    if event_type == '-Apnea-':
                        apnea_times.append(event_time)
                    elif event_type == '-Hypopnea-':
                        hypopnea_times.append(event_time)
        
        # Plot apnea events
        if apnea_times:
            # Find y-value at the top of the current plot
            y_max = max(flow_signal[:n_samples]) * 1.2
            
            for apnea_time in apnea_times:
                ax.stem([apnea_time], [y_max], linefmt='r-', markerfmt='ro', basefmt=' ')
                ax.annotate('A', xy=(apnea_time, y_max), 
                            xytext=(0, 5), textcoords='offset points',
                            ha='center', color='red', fontweight='bold')
            
        # Plot hypopnea events
        if hypopnea_times:
            # Find y-value at the top of the current plot
            y_max = max(flow_signal[:n_samples]) * 1.1
            
            for hypopnea_time in hypopnea_times:
                ax.stem([hypopnea_time], [y_max], linefmt='b-', markerfmt='bo', basefmt=' ')
                ax.annotate('H', xy=(hypopnea_time, y_max), 
                            xytext=(0, 5), textcoords='offset points',
                            ha='center', color='blue', fontweight='bold')
        
        # Add legend entries for events
        if apnea_times:
            ax.plot([], [], 'ro', label='Apnea')
        if hypopnea_times:
            ax.plot([], [], 'bo', label='Hypopnea')
        ax.set_title(f'vREM Apnea - {len(apnea_times)} : Hypopnea - {len(hypopnea_times)}')
    # Add title and labels
    ax.set_xlabel('Time')
    ax.set_ylabel('Amplitude')
    ax.grid(True)
    ax.legend()
    
    # Adjust the layout
    plt.tight_layout()
    
    # Save the figure if a path is provided
    if save_path:
        plt.savefig(save_path)
    
    plt.show()
    
    # Return the full time array for all data points
    return time_points


def save_flow_signal(flow_signal, sampling_freq, output_file,headerArg):
    """
    Save only the flow signal data to a plain text file,
    with each value on a separate line.
    
    Parameters:
    flow_signal (array): The extracted flow signal
    sampling_freq (float): Sampling frequency in Hz (not used in this version)
    output_file (str): Output file path
    """
    if flow_signal is None:
        return
    
    # Save just the flow signal values (one value per line)
    np.savetxt(output_file, flow_signal, 
               fmt='%.6f',  # Use 6 decimal places for precision
               delimiter='\n',  # Each value on a new line
               header=str(headerArg),   # Header
               comments='')     # No comment marker for the header
    
    print(f"Flow signal saved to {output_file}")
