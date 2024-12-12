import tkinter as tk
from tkinter import ttk, messagebox
import serial
import threading
import matplotlib.pyplot as plt
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
import numpy as np
import queue

class SavitarRobotGUI:
    def __init__(self, master, serial_port, baud_rate):
        self.root = master
        self.root.title("Savitar Robot Interface")
        self.root.geometry("1200x900")
        self.root.configure(bg="#f0f0f0")

        # Modern style
        self.style = ttk.Style()
        self.style.theme_use('clam')  # Use a modern theme
        self.style.configure("TLabel", font=('Helvetica', 10))
        self.style.configure("TButton", font=('Helvetica', 10))
        self.style.configure("TFrame", background="#f0f0f0")
        self.style.configure("TNotebook", background="#f0f0f0")
        self.style.configure("TNotebook.Tab", font=('Helvetica', 10, 'bold'))

        # Serial Communication Setup
        self.ser = serial.Serial(serial_port, baud_rate, timeout=1)

        # Graph Data Storage
        self.speed_data = {
            'back': [],
            'front': []
        }
        self.omega_data = {
            'back': [],
            'front': []
        }
        self.max_graph_points = 50  # Number of points to keep in graph

        # Initialize Variables
        self.setup_variables()

        # Create UI Components
        self.create_ui()

        # Start Serial Data Thread
        self.start_serial_thread()
        
        # Schedule periodic queue checking
        self.root.after(50, self.process_serial_queue)

    def start_serial_thread(self):
        # Create a thread for receiving serial data
        self.serial_thread = threading.Thread(target=self.receive_serial_data, daemon=True)
        self.serial_thread.start()

    def process_serial_queue(self):
        try:
            while not self.data_queue.empty():
                data = self.data_queue.get_nowait()
                self.update_ui_with_data(data)
        except queue.Empty:
            pass
        
        # Reschedule the queue checking
        self.root.after(50, self.process_serial_queue)

    def update_color_boxes(self):
        # Add method to update color boxes based on sensor data
        for i, var in enumerate(self.color_vars):
            color = var.get().lower()
            # Map color strings to actual colors or use a default
            color_map = {
                '0': 'white',
                '1': 'red', 
                '2': 'blue', 
                '3': 'black', 
                '4': 'unknown'
            }
            box_color = color_map.get(color, 'gray')

    def setup_variables(self):
        # Input Variables
        self.input_vars = [
            tk.StringVar(value="0.0") for _ in range(10)
        ]
        
        # Output Variables
        self.color_vars = [tk.StringVar(value="Unknown") for _ in range(5)]
        self.tof_vars = [tk.StringVar(value="0.0") for _ in range(5)]
        self.servo_vars = [tk.StringVar(value="Off") for _ in range(2)]
        self.odometry_vars = [tk.StringVar(value="0.0") for _ in range(4)]
        self.speed_vars = [tk.StringVar(value="0.0") for _ in range(4)]

    def create_ui(self):
        # Main Frame
        main_frame = ttk.Frame(self.root, padding="15")
        main_frame.grid(row=0, column=0, sticky=(tk.W, tk.E, tk.N, tk.S))
        self.root.columnconfigure(0, weight=1)
        self.root.rowconfigure(0, weight=1)

        # Create Notebook (Tabbed Interface)
        notebook = ttk.Notebook(main_frame)
        notebook.grid(row=0, column=0, sticky=(tk.W, tk.E, tk.N, tk.S))
        main_frame.columnconfigure(0, weight=1)
        main_frame.rowconfigure(0, weight=1)

        # Input Parameters Tab
        input_frame = ttk.Frame(notebook, padding="15")
        notebook.add(input_frame, text="Input Parameters")
        self.create_input_section(input_frame)

        # Sensor Data Tab
        sensor_frame = ttk.Frame(notebook, padding="15")
        notebook.add(sensor_frame, text="Sensor Data")
        self.create_sensor_section(sensor_frame)

        # Graphs Tab
        graphs_frame = ttk.Frame(notebook, padding="15")
        notebook.add(graphs_frame, text="Speed & Omega Graphs")
        self.create_graphs_section(graphs_frame)

    def create_graphs_section(self, parent):
        # Speed Graph
        speed_frame = ttk.LabelFrame(parent, text="Speed Graph")
        speed_frame.grid(row=0, column=0, padx=10, pady=10, sticky=tk.NSEW)
        
        self.speed_fig, self.speed_ax = plt.subplots(figsize=(6, 4), dpi=100)
        self.speed_canvas = FigureCanvasTkAgg(self.speed_fig, master=speed_frame)
        self.speed_canvas_widget = self.speed_canvas.get_tk_widget()
        self.speed_canvas_widget.pack(fill=tk.BOTH, expand=True)
        self.speed_ax.set_title('Robot Speed')
        self.speed_ax.set_xlabel('Time')
        self.speed_ax.set_ylabel('Speed')

        # Omega Graph
        omega_frame = ttk.LabelFrame(parent, text="Omega Graph")
        omega_frame.grid(row=1, column=0, padx=10, pady=10, sticky=tk.NSEW)
        
        self.omega_fig, self.omega_ax = plt.subplots(figsize=(6, 4), dpi=100)
        self.omega_canvas = FigureCanvasTkAgg(self.omega_fig, master=omega_frame)
        self.omega_canvas_widget = self.omega_canvas.get_tk_widget()
        self.omega_canvas_widget.pack(fill=tk.BOTH, expand=True)
        self.omega_ax.set_title('Robot Omega')
        self.omega_ax.set_xlabel('Time')
        self.omega_ax.set_ylabel('Omega')

        # Configure grid weights
        parent.columnconfigure(0, weight=1)
        parent.rowconfigure(0, weight=1)
        parent.rowconfigure(1, weight=1)

    def update_graphs(self):
        # Update Speed Graph
        self.speed_ax.clear()
        self.speed_ax.plot(self.speed_data['back'], label='Back Speed', color='blue')
        self.speed_ax.plot(self.speed_data['front'], label='Front Speed', color='red')
        self.speed_ax.legend()
        self.speed_ax.set_title('Robot Speed')
        self.speed_ax.set_xlabel('Time')
        self.speed_ax.set_ylabel('Speed')
        self.speed_canvas.draw()

        # Update Omega Graph
        self.omega_ax.clear()
        self.omega_ax.plot(self.omega_data['back'], label='Back Omega', color='green')
        self.omega_ax.plot(self.omega_data['front'], label='Front Omega', color='orange')
        self.omega_ax.legend()
        self.omega_ax.set_title('Robot Omega')
        self.omega_ax.set_xlabel('Time')
        self.omega_ax.set_ylabel('Omega')
        self.omega_canvas.draw()

    def create_input_section(self, parent):
        input_labels = [
            "fwdKp1", "fwdKd1", "rotKp1", "rotKd1",
            "fwdKp2", "fwdKd2", "rotKp2", "rotKd2",
            "steeringKp", "steeringKd"
        ]

        for i, label in enumerate(input_labels):
            row = i // 2
            col = i % 2
            
            ttk.Label(parent, text=label, font=('Helvetica', 12)).grid(
                row=row, column=col*2, padx=10, pady=10, sticky=tk.E
            )
            ttk.Entry(
                parent, 
                textvariable=self.input_vars[i], 
                width=20,
                font=('Helvetica', 12)
            ).grid(row=row, column=col*2+1, padx=10, pady=10)

        # Send Data Button
        ttk.Button(
            parent, 
            text="Send Data", 
            command=self.send_input_data,
            style='TButton'
        ).grid(row=len(input_labels)//2, column=0, columnspan=4, pady=20)

    def create_sensor_section(self, parent):
        # Color Sensors (Horizontal)
        color_frame = ttk.LabelFrame(parent, text="Color Sensors")
        color_frame.grid(row=0, column=0, padx=10, pady=10, sticky=tk.EW)
        
        color_boxes = []
        for i in range(5):
            ttk.Label(color_frame, text=f"CS{i+1}:", font=('Helvetica', 12)).grid(row=0, column=i*2, padx=5, pady=5)
            box = tk.Label(
                color_frame, 
                textvariable=self.color_vars[i], 
                width=10, 
                relief="solid",
                font=('Helvetica', 12)
            )
            box.grid(row=0, column=i*2+1, padx=5, pady=5)
            color_boxes.append(box)

        # TOF Sensors
        tof_frame = ttk.LabelFrame(parent, text="TOF Sensors")
        tof_frame.grid(row=1, column=0, padx=10, pady=10, sticky=tk.EW)
        
        tofArr = ["Left ToF", "Front ToF", "Right ToF", "Gripper ToF" , "Lift ToF"]
        for i in range(5):
            ttk.Label(tof_frame, text= tofArr[i], font=('Helvetica', 12)).grid(row=i, column=0, padx=10, pady=5)
            ttk.Label(
                tof_frame, 
                textvariable=self.tof_vars[i], 
                width=15,
                font=('Helvetica', 12)
            ).grid(row=i, column=1, padx=10, pady=5)

        # Odometry Data
        odometry_frame = ttk.LabelFrame(parent, text="Odometry")
        odometry_frame.grid(row=2, column=0, padx=10, pady=10, sticky=tk.EW)
        
        odometry_labels = ["Distance Back", "Distance Front", "Angle Back", "Angle Front"]
        for i, label in enumerate(odometry_labels):
            ttk.Label(odometry_frame, text=label, font=('Helvetica', 12)).grid(row=i, column=0, padx=10, pady=5)
            ttk.Label(
                odometry_frame, 
                textvariable=self.odometry_vars[i], 
                width=15,
                font=('Helvetica', 12)
            ).grid(row=i, column=1, padx=10, pady=5)

        # Speed and Omega
        speed_frame = ttk.LabelFrame(parent, text="Speed & Omega")
        speed_frame.grid(row=3, column=0, padx=10, pady=10, sticky=tk.EW)
        
        speed_labels = ["Speed Back", "Speed Front", "Omega Back", "Omega Front"]
        for i, label in enumerate(speed_labels):
            ttk.Label(speed_frame, text=label, font=('Helvetica', 12)).grid(row=i, column=0, padx=10, pady=5)
            ttk.Label(
                speed_frame, 
                textvariable=self.speed_vars[i], 
                width=15,
                font=('Helvetica', 12)
            ).grid(row=i, column=1, padx=10, pady=5)

        # Servo States
        servo_frame = ttk.LabelFrame(parent, text="Servo States")
        servo_frame.grid(row=4, column=0, padx=10, pady=10, sticky=tk.EW)
        
        for i in range(2):
            ttk.Label(servo_frame, text=f"Servo{i+1}:", font=('Helvetica', 12)).grid(row=0, column=i*2, padx=10, pady=5)
            ttk.Label(
                servo_frame, 
                textvariable=self.servo_vars[i], 
                width=10,
                font=('Helvetica', 12)
            ).grid(row=0, column=i*2+1, padx=10, pady=5)
            
    def send_input_data(self):
        try:
            values = [float(var.get()) for var in self.input_vars]
            data = " ".join(map(str, values)) + "\n"
            self.ser.write(data.encode("utf-8"))
        except ValueError:
            messagebox.showerror("Input Error", "Please enter valid float values.")

    def receive_serial_data(self):
        while True:
            try:
                line = self.ser.readline().decode('utf-8').strip()
                if line:
                    data = line.split()
                    
                    # Update color sensors
                    for i in range(5):
                        self.color_vars[i].set(data[i])
                    
                    # Update TOF sensors
                    for i in range(5):
                        self.tof_vars[i].set(data[5 + i])
                    
                    # Update odometry data
                    for i in range(4):
                        self.odometry_vars[i].set(data[10 + i])
                    
                    # Update speed and omega
                    back_speed = float(data[14])
                    front_speed = float(data[15])
                    back_omega = float(data[16])
                    front_omega = float(data[17])
                    
                    # Update speed data for graphs
                    self.speed_data['back'].append(back_speed)
                    self.speed_data['front'].append(front_speed)
                    self.omega_data['back'].append(back_omega)
                    self.omega_data['front'].append(front_omega)

                    # Limit graph points
                    for dataset in [self.speed_data['back'], self.speed_data['front'], 
                                    self.omega_data['back'], self.omega_data['front']]:
                        if len(dataset) > self.max_graph_points:
                            dataset.pop(0)

                    # Update graph
                    self.update_graphs()
                    
                    # Update speed vars
                    for i in range(4):
                        self.speed_vars[i].set(data[14 + i])
                    
                    # Update servos
                    self.servo_vars[0].set("On" if data[18] == "1" else "Off")
                    self.servo_vars[1].set("On" if data[19] == "1" else "Off")
                    
                    self.update_color_boxes()
            except Exception as e:
                print(f"Error: {e}")

def main():
    root = tk.Tk()
    app = SavitarRobotGUI(root, "COM4", 115200)
    root.mainloop()

if __name__ == "__main__":
    main()