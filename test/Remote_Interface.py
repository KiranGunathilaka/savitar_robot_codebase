import sys
from PyQt5.QtWidgets import (
    QApplication, QMainWindow, QVBoxLayout, QWidget, QTextEdit, QPushButton,
    QComboBox, QLabel, QHBoxLayout, QCheckBox, QLineEdit, QSplitter
)
from PyQt5.QtCore import QThread, pyqtSignal, Qt
import pyqtgraph as pg
import socket


class UDPReaderThread(QThread):
    data_received = pyqtSignal(str)

    def __init__(self, ip, port):
        super().__init__()
        self.ip = ip
        self.port = port
        self.running = True

    def run(self):
        try:
            sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
            sock.bind((self.ip, self.port))
            while self.running:
                data, _ = sock.recvfrom(1024)  # Buffer size 1024
                try:
                    message = data.decode('utf-8').strip()
                except UnicodeDecodeError:
                    message = f"Received non-UTF-8 data: {data}"
                self.data_received.emit(message)
        except Exception as e:
            self.data_received.emit(f"UDP Error: {e}")
        finally:
            sock.close()

    def stop(self):
        self.running = False
        self.wait()


class SerialMonitor(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("Serial/UDP Monitor")
        self.setGeometry(100, 100, 1200, 800)  # Increased default window size

        self.udp_thread = None
        self.data_series = {
            "SPD": [],
            "ROT_SPD": [],
            "EXP_SPD": [],
            "EXP_ROT": [],
            "LFT_FDSPD": [],
            "RT_FDSPD": [],
            "LR_SPD": [],
            "RR_SPD": [],
        }
        self.current_values = {
            "Current Speed": 0.0,
            "Omega": 0.0,
            "Expected Speed": 0.0,
            "Expected Omega": 0.0,
            "Left Front Speed": 0.0,
            "Right Front Speed": 0.0,
            "Left Rear Speed": 0.0,
            "Right Rear Speed": 0.0,
        }
        self.plot_items = {}
        self.initUI()

    def initUI(self):
        # Main widget
        main_widget = QWidget(self)
        self.setCentralWidget(main_widget)
        
        # Create main horizontal layout
        main_layout = QHBoxLayout()
        
        # Create left panel for controls
        left_panel = QWidget()
        left_layout = QVBoxLayout()
        left_panel.setLayout(left_layout)
        left_panel.setMaximumWidth(400)  # Limit width of control panel
        
        # Create right panel for graph
        right_panel = QWidget()
        right_layout = QVBoxLayout()
        right_panel.setLayout(right_layout)

        # Add UDP Controls to left panel
        udp_group = QWidget()
        udp_layout = QVBoxLayout()
        
        # Read UDP controls
        read_udp = QHBoxLayout()
        self.ip_label = QLabel("Read IP:")
        self.ip_input = QComboBox()
        self.ip_input.addItems(["192.168.1.157", "192.168.1.131", "192.168.1.6", "192.168.1.8", "0.0.0.0", "192.168.8.157"])
        self.port_label = QLabel("Port:")
        self.port_input = QComboBox()
        self.port_input.addItems(["3333", "5005"])
        read_udp.addWidget(self.ip_label)
        read_udp.addWidget(self.ip_input)
        read_udp.addWidget(self.port_label)
        read_udp.addWidget(self.port_input)
        udp_layout.addLayout(read_udp)

        # Send UDP controls
        send_udp = QHBoxLayout()
        self.send_ip_label = QLabel("Send IP:")
        self.send_ip_input = QLineEdit("192.168.1.189")
        self.send_port_label = QLabel("Port:")
        self.send_port_input = QLineEdit("12345")
        send_udp.addWidget(self.send_ip_label)
        send_udp.addWidget(self.send_ip_input)
        send_udp.addWidget(self.send_port_label)
        send_udp.addWidget(self.send_port_input)
        udp_layout.addLayout(send_udp)

        # Connect/Stop buttons
        buttons = QHBoxLayout()
        self.connect_button = QPushButton("Connect")
        self.connect_button.clicked.connect(self.connect_to_udp)
        self.stop_button = QPushButton("Stop")
        self.stop_button.setEnabled(False)
        self.stop_button.clicked.connect(self.stop_reading)
        buttons.addWidget(self.connect_button)
        buttons.addWidget(self.stop_button)
        udp_layout.addLayout(buttons)
        
        udp_group.setLayout(udp_layout)
        left_layout.addWidget(udp_group)

        # Add real-time values display to left panel
        values_group = QWidget()
        value_layout = QVBoxLayout()
        self.value_labels = {}
        for key in self.current_values:
            label = QLabel(f"{key}: 0.0")
            self.value_labels[key] = label
            value_layout.addWidget(label)
        values_group.setLayout(value_layout)
        left_layout.addWidget(values_group)

        # Add parameter inputs to left panel
        params_group = QWidget()
        param_layout = QVBoxLayout()
        self.params = {}
        for param in ["FWD_KP", "FWD_KD", "FWD_KI", "ROT_KP", "ROT_KD", "ROT_KI"]:
            row_layout = QHBoxLayout()
            label = QLabel(f"{param}:")
            input_field = QLineEdit()
            input_field.setPlaceholderText("Enter value")
            button = QPushButton("Send")
            button.clicked.connect(lambda _, p=param, i=input_field: self.send_param_udp(p, i))
            row_layout.addWidget(label)
            row_layout.addWidget(input_field)
            row_layout.addWidget(button)
            param_layout.addLayout(row_layout)
            self.params[param] = input_field
        params_group.setLayout(param_layout)
        left_layout.addWidget(params_group)

        # Add other output to left panel
        self.other_output = QTextEdit()
        self.other_output.setReadOnly(True)
        self.other_output.setPlaceholderText("Other Messages")
        self.other_output.setMaximumHeight(100)  # Limit height of output area
        left_layout.addWidget(QLabel("Messages:"))
        left_layout.addWidget(self.other_output)

        # Add stretchable space at the bottom of left panel
        left_layout.addStretch()

        # Setup plot in right panel
        self.plot_widget = pg.PlotWidget()
        self.plot_widget.setBackground('w')
        self.plot_widget.setTitle("Real-Time Data Plot", color='b', size="12pt")
        self.plot_widget.setLabel("left", "Value", color='blue')
        self.plot_widget.setLabel("bottom", "Sample", color='blue')
        self.plot_widget.showGrid(x=True, y=True)
        right_layout.addWidget(self.plot_widget)

        # Add plot controls to right panel
        plot_controls = QHBoxLayout()
        self.header_checkboxes = {}
        colors = {
            "SPD": "r", 
            "ROT_SPD": "g", 
            "EXP_SPD": "b", 
            "EXP_ROT": "m",
            "LFT_FDSPD": "y", 
            "RT_FDSPD": "c", 
            "LR_SPD": (255, 128, 0),  # Orange
            "RR_SPD": (128, 0, 128)   # Purple
        }
        for header, color in colors.items():
            checkbox = QCheckBox(header)
            checkbox.setChecked(True)
            checkbox.stateChanged.connect(self.update_plot_visibility)
            self.header_checkboxes[header] = checkbox
            self.plot_items[header] = self.plot_widget.plot(
                pen=pg.mkPen(color=color, width=2), name=header
            )
            plot_controls.addWidget(checkbox)
        right_layout.addLayout(plot_controls)

        # Add panels to main layout
        main_layout.addWidget(left_panel)
        main_layout.addWidget(right_panel, stretch=2)  # Give right panel more space
        
        # Set the main layout
        main_widget.setLayout(main_layout)

    # [Rest of the methods remain unchanged]
    def update_output(self, data):
        if data.startswith("VELOCITY:"):
            try:
                parts = data.split(":")[1].split(",")
                exp_spd, spd, exp_rot, rot_spd, lft_fdspd, rt_fdspd, lr_spd, rr_spd = map(float, parts)

                # Update data series
                self.data_series["EXP_SPD"].append(exp_spd)
                self.data_series["SPD"].append(spd)
                self.data_series["EXP_ROT"].append(exp_rot)
                self.data_series["ROT_SPD"].append(rot_spd)
                self.data_series["LFT_FDSPD"].append(lft_fdspd)
                self.data_series["RT_FDSPD"].append(rt_fdspd)
                self.data_series["LR_SPD"].append(lr_spd)
                self.data_series["RR_SPD"].append(rr_spd)

                # Trim series to avoid memory overflow
                for key in self.data_series:
                    if len(self.data_series[key]) > 2000:
                        self.data_series[key].pop(0)

                # Update real-time display
                self.current_values["Expected Speed"] = exp_spd
                self.value_labels["Expected Speed"].setText(f"Expected Speed: {exp_spd}")
                self.current_values["Current Speed"] = spd
                self.value_labels["Current Speed"].setText(f"Current Speed: {spd}")
                self.current_values["Expected Omega"] = exp_rot
                self.value_labels["Expected Omega"].setText(f"Expected Omega: {exp_rot}")
                self.current_values["Omega"] = rot_spd
                self.value_labels["Omega"].setText(f"Omega: {rot_spd}")
                self.current_values["Left Front Speed"] = lft_fdspd
                self.value_labels["Left Front Speed"].setText(f"Left Front Speed: {lft_fdspd}")
                self.current_values["Right Front Speed"] = rt_fdspd
                self.value_labels["Right Front Speed"].setText(f"Right Front Speed: {rt_fdspd}")
                self.current_values["Left Rear Speed"] = lr_spd
                self.value_labels["Left Rear Speed"].setText(f"Left Rear Speed: {lr_spd}")
                self.current_values["Right Rear Speed"] = rr_spd
                self.value_labels["Right Rear Speed"].setText(f"Right Rear Speed: {rr_spd}")

                # Update plots
                for header in self.data_series:
                    self.update_plot(header)
            except (IndexError, ValueError):
                self.other_output.append(f"Invalid VELOCITY format: {data}")
        else:
            self.other_output.append(data)

    def connect_to_udp(self):
        ip = self.ip_input.currentText()
        port = int(self.port_input.currentText())

        try:
            self.udp_thread = UDPReaderThread(ip, port)
            self.udp_thread.data_received.connect(self.update_output)
            self.udp_thread.start()

            self.other_output.append(f"Listening on {ip}:{port} for UDP messages.")
            self.connect_button.setEnabled(False)
            self.stop_button.setEnabled(True)
        except Exception as e:
            self.other_output.append(f"UDP Connection Error: {e}")

    def stop_reading(self):
        if self.udp_thread:
            self.udp_thread.stop()
            self.udp_thread = None
        self.other_output.append("Stopped reading.")
        self.connect_button.setEnabled(True)
        self.stop_button.setEnabled(False)

    def update_plot(self, header):
        if self.header_checkboxes[header].isChecked():
            self.plot_items[header].setData(self.data_series[header])
        else:
            self.plot_items[header].clear()

    def update_plot_visibility(self):
        for header, checkbox in self.header_checkboxes.items():
            if not checkbox.isChecked():
                self.plot_items[header].clear()
            elif len(self.data_series[header]) > 0:
                self.plot_items[header].setData(self.data_series[header])

    def send_param_udp(self, param, input_field):
        value = input_field.text()
        if value:
            try:
                sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
                send_ip = self.send_ip_input.text()
                send_port = int(self.send_port_input.text())
                message = f"{param}={value}"
                sock.sendto(message.encode('utf-8'), (send_ip, send_port))
                self.other_output.append(f"Sent: {message} to {send_ip}:{send_port}")
            except Exception as e:
                self.other_output.append(f"Error sending {param}: {e}")
        else:
            self.other_output.append(f"Error: No value entered for {param}")


def main():
    app = QApplication(sys.argv)
    window = SerialMonitor()
    window.show()
    sys.exit(app.exec_())


if __name__ == "__main__":
    main()