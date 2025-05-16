import sys
import os
import re
from PyQt5.QtWidgets import (QApplication, QMainWindow, QWidget, QVBoxLayout, 
                            QHBoxLayout, QLabel, QLineEdit, QPushButton, 
                            QFileDialog, QMessageBox, QFrame)
from _Lib import _LibExtractWaveform as extractWave


def extract_date_and_time(file_path):
    """
    Extract both date (YYYYMMDD) and time (HHMMSS) from a ResMed EDF file path.
    
    Parameters:
    file_path (str): Path to the EDF file (e.g., "20250428_155252_EVE.edf")
    
    Returns:
    tuple: A tuple containing (date_str, time_str) if found, or (None, None) if not found
    """
    filename = os.path.basename(file_path)
    
    # Pattern to match YYYYMMDD_HHMMSS structure
    match = re.search(r'(\d{8})_(\d{6})_', filename)
    
    if match:
        date_str = match.group(1)  # YYYYMMDD
        time_str = match.group(2)  # HHMMSS
        return date_str, time_str
    else:
        # Try alternative patterns if the first one fails
        # For example, some files might use different separators or formats
        alt_match = re.search(r'(\d{8})[^\d]*(\d{6})', filename)
        if alt_match:
            return alt_match.group(1), alt_match.group(2)
        
        return None, None


class EDFFileSelector(QMainWindow):
    def __init__(self):
        super().__init__()
        
        # Set up the main window
        self.setWindowTitle("EDF File Selector")
        self.setGeometry(100, 100, 600, 150)
        self.file_path = None
        
        # Create central widget and main layout
        central_widget = QWidget()
        self.setCentralWidget(central_widget)
        main_layout = QVBoxLayout(central_widget)
        
        # Create file selection frame
        file_frame = QFrame()
        file_layout = QHBoxLayout(file_frame)
        
        # Add file selection widgets
        file_label = QLabel("Select EDF File:")
        self.file_entry = QLineEdit()
        browse_button = QPushButton("Browse")
        browse_button.clicked.connect(self.browse_file)
        
        file_layout.addWidget(file_label)
        file_layout.addWidget(self.file_entry, 1)  # 1 is the stretch factor
        file_layout.addWidget(browse_button)
        
        # Create button frame
        button_frame = QFrame()
        button_layout = QHBoxLayout(button_frame)
        button_layout.addStretch(1)  # Push buttons to the right
        
        # Add action buttons
        cancel_button = QPushButton("Cancel")
        cancel_button.clicked.connect(self.on_cancel)
        ok_button = QPushButton("OK")
        ok_button.clicked.connect(self.on_ok)
        
        button_layout.addWidget(cancel_button)
        button_layout.addWidget(ok_button)
        
        # Add frames to main layout
        main_layout.addWidget(file_frame)
        main_layout.addWidget(button_frame)
        
        # Check command line arguments
        if len(sys.argv) > 1 and os.path.exists(sys.argv[1]) and sys.argv[1].lower().endswith('.edf'):
            self.file_entry.setText(sys.argv[1])
    
    def browse_file(self):
        file_path, _ = QFileDialog.getOpenFileName(
            self,
            "Select EDF File",
            "",
            "EDF Files (*.edf)"
        )
        if file_path:
            self.file_entry.setText(file_path)
    
    def on_ok(self):
        file_path = self.file_entry.text().strip()
        
        if not file_path:
            QMessageBox.critical(self, "Error", "Please select an EDF file first")
            return
        
        if not os.path.exists(file_path):
            QMessageBox.critical(self, "Error", "Selected file does not exist")
            return
        
        self.file_path = file_path
        self.close()
    
    def on_cancel(self):
        self.close()


def select_edf_file():
    """
    PyQt5 GUI to select an EDF file path.
    
    Returns:
        str: The selected file path, or None if cancelled
    """
    app = QApplication(sys.argv)
    window = EDFFileSelector()
    window.show()
    app.exec_()
    return window.file_path


# Main execution
if __name__ == "__main__":
    edf_file_path = select_edf_file()
    
    if edf_file_path:
        print(f"Selected file: {edf_file_path}")
        # Here you would typically call your existing code to process the file
        workspace_folder = os.path.dirname(os.path.abspath(__file__))
        output_file = os.path.join(workspace_folder, "ResmedFlow.txt")
        
        # Try to extract the flow signal
        flow_signal, sampling_freq = extractWave.extract_flow_signal_robust(edf_file_path)
        print("Total Len", len(flow_signal), sampling_freq)
        
        dateStampStr, timestampstr = extract_date_and_time(edf_file_path)
        if timestampstr:
            # Format as time if needed
            hours = timestampstr[0:2]
            minutes = timestampstr[2:4]
            seconds = timestampstr[4:6]
            formatted_time = f"{hours}:{minutes}:{seconds}"
            print("Time ->", formatted_time, timestampstr)

        flow_signal = flow_signal * 60
        if flow_signal is not None:
            # Plot the first minute of data
            extractWave.plot_flow_signal_with_timestamp(flow_signal, sampling_freq, timestampstr, duration=60)
            extractWave.save_flow_signal(flow_signal, sampling_freq, output_file, dateStampStr+'_'+timestampstr)
        else:
            print("Failed to extract flow signal.")
            
            # Suggest examining the file format
            print("\nSuggestions:")
            print("1. The file may be in ResMed's proprietary format rather than standard EDF")
            print("2. Try using ResMed's own software to convert this file to a standard format")
            print("3. Contact ResMed support for information on the file format")
    else:
        print("No file selected or operation cancelled")