#include <esp_now.h>
#include <WiFi.h>

// REPLACE WITH THE MAC ADDRESS OF YOUR ESP01S RECEIVER
uint8_t receiverMac[] = {0xCC, 0x50, 0xE3, 0X41, 0x1C, 0xD6};

// Structure to hold the data to be sent
typedef struct sendPacket
{
  float fwdKp1;
  float fwdKd1;
  float rotKp1;
  float rotKd1;

  float fwdKp2;
  float fwdKd2;
  float rotKp2;
  float rotKd2;

  float steeringKp;
  float steeringKd;
} sendPacket;

typedef struct receivePacket
{
  int cs1;
  int cs2;
  int cs3;
  int cs4;
  int cs5;

  float tof1;
  float tof2;
  float tof3;
  float tof4;
  float tof5;

  float fwdKp1;
  float fwdKd1;
  float rotKp1;
  float rotKd1;

  float fwdKp2;
  float fwdKd2;
  float rotKp2;
  float rotKd2;

  float steeringKp;
  float steeringKd;

  bool servoGripper;
  bool servoLift;
} receivePacket;

sendPacket sendData;
receivePacket receiveData;

esp_now_peer_info_t peerInfo;

void setup()
{
  Serial.begin(115200);

  // Set the device in station mode
  WiFi.mode(WIFI_STA);

  // Initalize ESP-NOW
  if (esp_now_init() != ESP_OK)
  {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Register the send callback
  esp_now_register_send_cb(OnDataSent);

  // Register the receive callback
  esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));

  memcpy(peerInfo.peer_addr, receiverMac, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK)
  {
    Serial.println("Failed to add peer");
    return;
  }
}

void loop()
{

  if (Serial.available() > 0)
  {
    // Read the data from the serial port
    sendData.fwdKp1 = Serial.parseFloat();
    sendData.fwdKp1 = Serial.parseFloat();

    sendData.fwdKd1= Serial.parseFloat();
    sendData.fwdKd1= Serial.parseFloat();

    sendData.rotKp1 = Serial.parseFloat();
    sendData.rotKp1 = Serial.parseFloat();

    sendData.rotKd2 = Serial.parseFloat();
    sendData.rotKd2 = Serial.parseFloat();

    sendData.steeringKp = Serial.parseFloat();
    sendData.steeringKd = Serial.parseFloat();


    // Send the data via ESP-NOW
    esp_now_send(receiverMac, (uint8_t *)&sendData, sizeof(sendData));
  }
}

// Callback function for sent data
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
  if (status == ESP_NOW_SEND_SUCCESS)
  {
    Serial.println("SEND_OK");
  }
  else
  {
    Serial.println("SEND_FAIL");
  }
}

// Callback function for received data
void OnDataRecv(const uint8_t *mac_addr, const uint8_t *data, int data_len)
{
  memcpy(&receiveData, data, sizeof(receiveData));

  //these are colour sensor serial prints, integer corresponds to a colour
  //0 white , 1 Red , 2 blue, 3 black , 4 unkonwn 
  Serial.printf("%d %d %d %d %d", receiveData.cs1, receiveData.cs2, receiveData.cs3, receiveData.cs4, receiveData.cs5);

  //these are tof distance data
  Serial.printf("%f %f %f %f %f", receiveData.tof1, receiveData.tof2, receiveData.tof3, receiveData.tof4, receiveData.tof5);

  //odometry data, each value are in pairs as these are calculated and processed pairwise (front pair, back pair)
  Serial.printf("%f %f %f %f", receiveData.distanceBack, receiveData.distanceFront, receiveData.angleBack, receiveData.angleFront);

  //speeds and omegas of the two pairs
  Serial.printf("%f %f %f %f", receiveData.speedBack, receiveData.speedFront, receiveData.omegaBack, receiveData.omegaFront);

  //indicate 
  wheteher servo is on or off
  Serial.printf("%b %b", receiveData.servoGripper, receiveData.servoLift);

}

import tkinter as tk
from tkinter import ttk
import serial
import threading

# Replace with your COM port and baud rate
SERIAL_PORT = "COM3"
BAUD_RATE = 115200

# Initialize serial communication
ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)

# Input Variables
input_vars = [tk.StringVar(value="0.0") for _ in range(10)]  # For input fields

# Output Variables
color_vars = [tk.StringVar(value="Unknown") for _ in range(5)]  # For color sensors
tof_vars = [tk.StringVar(value="0.0") for _ in range(5)]  # For TOF sensors
servo_vars = [tk.StringVar(value="Off") for _ in range(2)]  # For servo states

# Odometry and speed variables
odometry_vars = [tk.StringVar(value="0.0") for _ in range(4)]  # Back distance, Front distance, Back angle, Front angle
speed_vars = [tk.StringVar(value="0.0") for _ in range(4)]  # Back speed, Front speed, Back omega, Front omega

# Function to send input data
def send_input_data():
    try:
        # Read input values
        values = [float(var.get()) for var in input_vars]
        data = " ".join(map(str, values)) + "\n"
        ser.write(data.encode("utf-8"))
    except ValueError:
        print("Invalid input. Please enter valid float values.")

# Function to update color boxes
def update_color_boxes():
    for i in range(5):
        color = "white"  # Default background
        if color_vars[i].get() == "1":
            color = "red"
        elif color_vars[i].get() == "2":
            color = "blue"
        elif color_vars[i].get() == "3":
            color = "black"
        elif color_vars[i].get() == "0":
            color = "white"
        color_boxes[i].configure(bg=color)

# Function to receive and parse data
def receive_serial_data():
    while True:
        try:
            line = ser.readline().decode('utf-8').strip()
            if line:
                data = line.split()  # Assuming space-separated data
                # Update color sensors
                for i in range(5):
                    color_vars[i].set(data[i])
                # Update TOF sensors
                for i in range(5):
                    tof_vars[i].set(data[5 + i])
                # Update odometry data
                for i in range(4):
                    odometry_vars[i].set(data[10 + i])
                # Update speed and omega
                for i in range(4):
                    speed_vars[i].set(data[14 + i])
                # Update servos
                servo_vars[0].set("On" if data[18] == "1" else "Off")
                servo_vars[1].set("On" if data[19] == "1" else "Off")
                update_color_boxes()
        except Exception as e:
            print(f"Error: {e}")

# Create main GUI window
root = tk.Tk()
root.title("ESP-NOW Interface")

# Input Section
frame_inputs = ttk.LabelFrame(root, text="Input Parameters")
frame_inputs.pack(padx=10, pady=10, fill="x")

inputs = [
    "fwdKp1", "fwdKd1", "rotKp1", "rotKd1",
    "fwdKp2", "fwdKd2", "rotKp2", "rotKd2",
    "steeringKp", "steeringKd"
]

for i, label in enumerate(inputs):
    ttk.Label(frame_inputs, text=label).grid(row=i, column=0, padx=5, pady=5, sticky="e")
    ttk.Entry(frame_inputs, textvariable=input_vars[i], width=20).grid(row=i, column=1, padx=5, pady=5)

ttk.Button(frame_inputs, text="Send Data", command=send_input_data).grid(row=len(inputs), column=0, columnspan=2, pady=10)

# Color Sensors Section
frame_colors = ttk.LabelFrame(root, text="Color Sensors")
frame_colors.pack(padx=10, pady=10, fill="x")

color_boxes = []
for i in range(5):
    label = ttk.Label(frame_colors, text=f"CS{i+1}:")
    label.grid(row=i, column=0, padx=5, pady=5, sticky="e")
    color_box = tk.Label(frame_colors, textvariable=color_vars[i], width=20, relief="solid")
    color_box.grid(row=i, column=1, padx=5, pady=5, sticky="w")
    color_boxes.append(color_box)

# TOF Sensors Section
frame_tof = ttk.LabelFrame(root, text="TOF Sensors")
frame_tof.pack(padx=10, pady=10, fill="x")

for i in range(5):
    ttk.Label(frame_tof, text=f"TOF{i+1}:").grid(row=i, column=0, padx=5, pady=5, sticky="e")
    ttk.Label(frame_tof, textvariable=tof_vars[i], width=20).grid(row=i, column=1, padx=5, pady=5, sticky="w")

# Odometry Section
frame_odometry = ttk.LabelFrame(root, text="Odometry Data")
frame_odometry.pack(padx=10, pady=10, fill="x")

odometry_labels = ["Distance Back", "Distance Front", "Angle Back", "Angle Front"]
for i, label in enumerate(odometry_labels):
    ttk.Label(frame_odometry, text=label).grid(row=i, column=0, padx=5, pady=5, sticky="e")
    ttk.Label(frame_odometry, textvariable=odometry_vars[i], width=20).grid(row=i, column=1, padx=5, pady=5, sticky="w")

# Speed Section
frame_speed = ttk.LabelFrame(root, text="Speed and Omega")
frame_speed.pack(padx=10, pady=10, fill="x")

speed_labels = ["Speed Back", "Speed Front", "Omega Back", "Omega Front"]
for i, label in enumerate(speed_labels):
    ttk.Label(frame_speed, text=label).grid(row=i, column=0, padx=5, pady=5, sticky="e")
    ttk.Label(frame_speed, textvariable=speed_vars[i], width=20).grid(row=i, column=1, padx=5, pady=5, sticky="w")

# Servo States Section
frame_servos = ttk.LabelFrame(root, text="Servo States")
frame_servos.pack(padx=10, pady=10, fill="x")

for i in range(2):
    ttk.Label(frame_servos, text=f"Servo{i+1}:").grid(row=i, column=0, padx=5, pady=5, sticky="e")
    ttk.Label(frame_servos, textvariable=servo_vars[i], width=20).grid(row=i, column=1, padx=5, pady=5, sticky="w")

# Start serial data reading in a separate thread
thread = threading.Thread(target=receive_serial_data, daemon=True)
thread.start()

# Run the Tkinter main loop
root.mainloop()




