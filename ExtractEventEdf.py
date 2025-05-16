import os
import re
import sys
import pandas as pd
from datetime import datetime, timedelta
from PyQt5.QtWidgets import (QApplication, QMainWindow, QWidget, QVBoxLayout, 
                             QHBoxLayout, QPushButton, QLabel, QLineEdit, 
                             QFileDialog, QMessageBox, QTextEdit, QTableWidget,
                             QTableWidgetItem, QHeaderView)
from PyQt5.QtCore import Qt


def extract_start_datetime_from_filename(filename):
    match = re.search(r'(\d{8})_(\d{6})', filename)
    if not match:
        raise ValueError("Filename does not contain a valid datetime pattern.")
    date_str, time_str = match.groups()
    return datetime.strptime(date_str + time_str, "%Y%m%d%H%M%S")


def parse_format_1(content, base_time):
    pattern = r'\+(\d+)\x15(\d+)\x14([^\x00\x15\x14]+)'
    matches = re.findall(pattern, content)
    records = []
    for onset_str, dur_str, desc in matches:
        onset = int(onset_str)
        duration = int(dur_str)
        onset_time = base_time + timedelta(seconds=onset)
        records.append({
            "onset_seconds": onset,
            "duration": duration,
            "description": desc.strip(),
            "onset_time": onset_time.strftime("%Y-%m-%d %H:%M:%S.%f")[:-3]
        })
    return records


def parse_format_2(content, base_time):
    pattern = r'\+0\s*\+(\d+)([A-Za-z\s]+)'
    matches = re.findall(pattern, content)
    records = [{
        "onset_seconds": 0,
        "duration": "",
        "description": "Recording starts",
        "onset_time": base_time.strftime("%Y-%m-%d %H:%M:%S")
    }]
    for onset_cs, desc in matches:
        if desc.strip() == "Recording starts":
            continue
        try:
            onset = int(onset_cs) / 100.0
            description = desc.strip()
            if "Apnea" in description and not description.startswith("Obstructive"):
                description = "Obstructive " + description
            onset_time = base_time + timedelta(seconds=onset)
            records.append({
                "onset_seconds": onset,
                "duration": "",
                "description": description,
                "onset_time": onset_time.strftime("%Y-%m-%d %H:%M:%S.%f")[:-3]
            })
        except ValueError:
            continue
    return records


def detect_format(content):
    if re.search(r'\+\d+\x15\d+\x14', content):
        return 'format1'
    elif re.search(r'\+0\s*\+\d+[A-Za-z\s]+', content):
        return 'format2'
    return None


def parse_edf_file(filepath):
    filename = os.path.basename(filepath)
    base_time = extract_start_datetime_from_filename(filename)

    with open(filepath, 'rb') as f:
        raw = f.read()

    content = raw.decode('latin-1', errors='replace')
    fmt = detect_format(content)

    if not fmt:
        raise ValueError("Unknown EDF annotation format.")

    if fmt == 'format1':
        print("Detected Format 1")
        records = parse_format_1(content, base_time)
    else:
        print("Detected Format 2")
        records = parse_format_2(content, base_time)

    return pd.DataFrame(records)


class EventsTableDialog(QWidget):
    def __init__(self, df, parent=None):
        super().__init__(parent)
        self.df = df
        self.setWindowTitle("First 10 Events")
        self.setMinimumSize(800, 400)
        
        # Create table
        self.table = QTableWidget(self)
        self.table.setColumnCount(len(df.columns))
        self.table.setRowCount(min(10, len(df)))
        self.table.setHorizontalHeaderLabels(df.columns)
        
        # Fill the table
        for i in range(min(10, len(df))):
            for j, col in enumerate(df.columns):
                item = QTableWidgetItem(str(df.iloc[i][col]))
                self.table.setItem(i, j, item)
        
        # Adjust column widths
        self.table.horizontalHeader().setSectionResizeMode(QHeaderView.Stretch)
        
        # Layout
        layout = QVBoxLayout()
        layout.addWidget(self.table)
        
        close_button = QPushButton("Close")
        close_button.clicked.connect(self.close)
        layout.addWidget(close_button)
        
        self.setLayout(layout)


class EventCountsDialog(QWidget):
    def __init__(self, df, parent=None):
        super().__init__(parent)
        self.df = df
        self.setWindowTitle("Event Types and Counts")
        self.setMinimumSize(500, 300)
        
        # Count event types
        event_counts = df['description'].value_counts()
        
        # Create text display
        self.text_edit = QTextEdit()
        self.text_edit.setReadOnly(True)
        
        # Format the text
        content = "Event Types and Counts:\n\n"
        for event_type, count in event_counts.items():
            content += f"{event_type}: {count}\n"
        
        self.text_edit.setText(content)
        
        # Layout
        layout = QVBoxLayout()
        layout.addWidget(self.text_edit)
        
        close_button = QPushButton("Close")
        close_button.clicked.connect(self.close)
        layout.addWidget(close_button)
        
        self.setLayout(layout)


class EDFEventExtractor(QMainWindow):
    def __init__(self):
        super().__init__()
        
        self.setWindowTitle("EDF Event Extractor")
        self.setGeometry(100, 100, 700, 150)
        
        # Central widget
        central_widget = QWidget()
        self.setCentralWidget(central_widget)
        
        # Main layout
        main_layout = QVBoxLayout(central_widget)
        
        # File selection row
        file_layout = QHBoxLayout()
        file_label = QLabel("Select an EDF File:")
        self.file_path = QLineEdit()
        self.file_path.setMinimumWidth(400)
        browse_button = QPushButton("Browse...")
        browse_button.clicked.connect(self.browse_file)
        
        file_layout.addWidget(file_label)
        file_layout.addWidget(self.file_path)
        file_layout.addWidget(browse_button)
        main_layout.addLayout(file_layout)
        
        # Button row
        button_layout = QHBoxLayout()
        process_button = QPushButton("Process")
        process_button.clicked.connect(self.process_file)
        exit_button = QPushButton("Exit")
        exit_button.clicked.connect(self.close)
        
        button_layout.addStretch(1)
        button_layout.addWidget(process_button)
        button_layout.addWidget(exit_button)
        main_layout.addLayout(button_layout)
    
    def browse_file(self):
        options = QFileDialog.Options()
        file_path, _ = QFileDialog.getOpenFileName(
            self, "Select EDF File", "", "EDF Files (*.edf);;All Files (*)", 
            options=options
        )
        if file_path:
            self.file_path.setText(file_path)
    
    def process_file(self):
        filepath = self.file_path.text().strip()
        if not filepath or not os.path.exists(filepath):
            QMessageBox.critical(self, "Error", "Please select a valid EDF file.")
            return
        
        try:
            workspace_folder = os.path.dirname(os.path.abspath(__file__))
            output_csv = os.path.join(workspace_folder, "ComparisionFolder/_events.csv")

            df = parse_edf_file(filepath)
            df.to_csv(output_csv, index=False)
            
            # Success message
            QMessageBox.information(
                self, "Success", 
                f"{len(df)} events extracted.\nSaved to:\n{output_csv}"
            )
            
            # Show first 10 events
            events_dialog = EventsTableDialog(df, self)
            events_dialog.show()
            
            # Show event counts
            counts_dialog = EventCountsDialog(df, self)
            counts_dialog.show()
            
        except Exception as e:
            QMessageBox.critical(self, "Error", f"Error: {str(e)}")


if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = EDFEventExtractor()
    window.show()
    sys.exit(app.exec_())
