import pygame
import sys
import socket
import threading
import time
import numpy as np
from pygame.locals import *
from datetime import datetime

# Initialize Pygame
WIDTH, HEIGHT = 1300, 700
pygame.init()
screen = pygame.display.set_mode((WIDTH, HEIGHT))
pygame.display.set_caption("Debugging Console")

# Constants
WINDOW_WIDTH = 1300
WINDOW_HEIGHT = 700
FPS = 60

# Colors
WHITE = (255, 255, 255)
BLACK = (0, 0, 0)
GRAY = (220, 220, 220)
GREEN = (0, 255, 0)
RED = (255, 0, 0)
DARK_GRAY = (100, 100, 100)
PRIMARY_BLUE = (41, 128, 185)
SECONDARY_BLUE = (52, 152, 219)
ACCENT_GREEN = (46, 204, 113)
ERROR_RED = (231, 76, 60)

# Network settings
UDP_IP = "192.168.1.8"
UDP_PORT = 12345

# Input storage
linear_velocities = []
rotational_velocities = []

def generate_velocity_values(num_values=500):
    linear = np.random.uniform(80, 100, num_values)
    rotational = np.random.uniform(80, 100, num_values)
    return list(linear), list(rotational)

def update_velocity_values(lin, rot):
    lin = lin[1:] + [50 + 20*np.sin(10*time.time())]
    rot = rot[1:] + [50 + 20*np.cos(10*time.time())]
    return lin, rot

# Function to draw the velocity graphs in the bottom right corner
def draw_graphs():
    offset_x = WIDTH // 2
    offset_y = HEIGHT - 25

    max_value = max(max(linear_velocities, default=0), max(rotational_velocities, default=0), 100)

    # Draw axes and linear velocity graph
    pygame.draw.line(screen, BLACK, (offset_x + 50, offset_y), (WIDTH - 50, offset_y), 2)  # X-axis
    pygame.draw.line(screen, BLACK, (offset_x + 50, offset_y), (offset_x + 50, HEIGHT // 2+50), 2)  # Y-axis

    # Linear Velocity Points
    linear_points = [(offset_x + 50 + i * (WIDTH // 2 - 100) // (len(linear_velocities) - 1),
                      offset_y - (lv / max_value * (HEIGHT // 2 - 100))) for i, lv in enumerate(linear_velocities)]
    pygame.draw.lines(screen, GREEN, False, linear_points, 2)

    # Rotational Velocity Points
    rotational_points = [(offset_x + 50 + i * (WIDTH // 2 - 100) // (len(rotational_velocities) - 1),
                          offset_y - (rv / max_value * (HEIGHT // 2 - 100))) for i, rv in enumerate(rotational_velocities)]
    pygame.draw.lines(screen, RED, False, rotational_points, 2)

    # Label for Velocity section
    velocity_label = pygame.font.SysFont('Courier', 32).render("Velocity", True, BLACK)
    screen.blit(velocity_label, (offset_x + 250, HEIGHT//2 + 50))

# Add labels and values in the right corner
def draw_labels(robo):
    # Define the labels and some placeholder values for each metric
    metrics = [
        ("Robot Speed ", str(robo.fwd_speed) + " mm/s"),
        ("Robot Omega ", str(robo.rot_speed)+" rad/s"),
        ("Distance ", "No Data" + " mm"),
        ("Angle ", "No Data"+"°"),
        ("Mission Status ", str(robo.mission_state)),
        ("Motion Status ", str(robo.motion_state)),
        ("Line Status ", "Not Data"),
        ("Junction Detected ", "No Data"),
        ("Object Detected ", "No Data"),
        ("Height ", str(robo.height)+ " cm"),
    ]

    # Starting position for the labels
    x = WIDTH - 260
    y = 75
    label_font = pygame.font.Font(None, 22)
    value_font = pygame.font.Font(None, 22)

    # Loop through each metric and render its label and value
    for label, value in metrics:
        # Render label
        label_text = label_font.render(label + ":", True, BLACK)
        screen.blit(label_text, (x, y))
        
        # Render value aligned to the right
        value_text = value_font.render(value, True, DARK_GRAY)
        screen.blit(value_text, (x + 150, y))
        
        y += 30  # Move down for the next metric

class Robot:
    def __init__(self):
        self.target_fwd_s = 100.0
        self.fwd_speed = 100.0
        self.target_rot_s = 0.0
        self.rot_speed = 0.0
        self.IRSENSORS = (0, 5, 2, 1, 0, 0, 0, 0, 0, 0)
        self.motion_state = ""
        self.mission_state = ""
        self.height = 0.0
        self.obstacle_distance = 0.0
    
    def update_velocity(self, target_fwd_s, fwd_speed, target_rot_s, rot_speed):
        self.target_fwd_s = target_fwd_s
        self.fwd_speed = fwd_speed
        self.target_rot_s = target_rot_s
        self.rot_speed = rot_speed
    
    def update_irsensors(self, *sensors):
        if len(sensors) == 10:
            self.IRSENSORS = sensors
    
    def update_state(self, motion_state, mission_state):
        self.motion_state = motion_state
        self.mission_state = mission_state
    
    def update_distsensors(self, height, obstacle_distance):
        self.height = height
        self.obstacle_distance = obstacle_distance

    def update_robot(self, data):
            data_type = data[0]
            values = data[1:]
            
            if data_type == "VELOCITY":
                self.update_velocity(*values)
            elif data_type == "IRSENSORS":
                self.update_irsensors(*values)
            elif data_type == "STATE":
                self.update_state(*values)
            elif data_type == "DISTSENSORS":
                self.update_distsensors(*values)


    def get_robot_data(self):
        return {
            'target_fwd_s': self.target_fwd_s,
            'fwd_speed': self.fwd_speed,
            'target_rot_s': self.target_rot_s,
            'rot_speed': self.rot_speed,
            'IRSENSORS': self.IRSENSORS,
            'motion_state': self.motion_state,
            'mission_state': self.mission_state,
            'height': self.height,
            'obstacle_distance': self.obstacle_distance
        }

class NetworkManager:
    def __init__(self):
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.sock.bind(('', UDP_PORT))
        self.messages = []
        self.max_messages = 15

    def send_message(self, label, value):
        try:
            message = f"{label}={value}".encode()
            self.sock.sendto(message, (UDP_IP, UDP_PORT))
            return True
        except Exception as e:
            self.add_message(f"Send error: {str(e)}", is_error=True)
            return False

    def add_message(self, message, is_error=False):
        timestamp = datetime.now().strftime("%H:%M:%S")
        self.messages.append({
            "text": f"[{timestamp}] {message}",
            "is_error": is_error
        })
        if len(self.messages) > self.max_messages:
            self.messages.pop(0)

class TextInputBox:
    def __init__(self, x, y, label_text, function, default_value=""):
        self.label_text = label_text
        self.rect = pygame.Rect(x + 120, y, 100, 32)
        self.button = pygame.Rect(x + 230, y, 80, 32)
        self.text = default_value
        self.selected = False
        self.function = function
        self.value = 0
        self.last_update = time.time()
        self.update_interval = 0.1
        self.font = pygame.font.Font(None, 24)
        self.label = self.font.render(label_text, True, BLACK)
        self.update_text_surface()

    def update_text_surface(self):
        self.text_surface = self.font.render(self.text, True, BLACK)

    def handle_event(self, event):
        if event.type == MOUSEBUTTONDOWN and event.button == 1:
            self.selected = self.rect.collidepoint(event.pos)
            if self.button.collidepoint(event.pos):
                self.send_value()
        elif event.type == KEYDOWN and self.selected:
            if event.key == K_RETURN:
                self.send_value()
            elif event.key == K_BACKSPACE:
                self.text = self.text[:-1]
            else:
                if event.unicode.replace(".", "").isnumeric() or event.unicode == ".":
                    self.text += event.unicode
            self.update_text_surface()

    def send_value(self):
        if time.time() - self.last_update >= self.update_interval:
            try:
                value = float(self.text)
                self.value = value
                self.function(self.label_text, self.text)
                self.last_update = time.time()
            except ValueError:
                pass

    def draw(self, screen):
        screen.blit(self.label, (self.rect.x - 110, self.rect.y + 6))
        border_color = PRIMARY_BLUE if self.selected else DARK_GRAY
        pygame.draw.rect(screen, WHITE, self.rect, border_radius=5)
        pygame.draw.rect(screen, border_color, self.rect, 2, border_radius=5)
        screen.blit(self.text_surface, (self.rect.x + 5, self.rect.y + 6))
        pygame.draw.rect(screen, ACCENT_GREEN, self.button, border_radius=5)
        button_text = self.font.render("Update", True, WHITE)
        screen.blit(button_text, (self.button.x + 10, self.button.y + 5))

class BarGraph:
    def __init__(self, x, y, width, height):
        self.rect = pygame.Rect(x, y, width, height)
        self.bar_width = 20
        self.spacing = 10
        self.max_height = height - 40
        self.font = pygame.font.Font(None, 20)

    def draw(self, screen, data):
        pygame.draw.rect(screen, GRAY, self.rect, border_radius=10)
        title_font = pygame.font.Font(None, 24)
        title = title_font.render("Sensor Readings", True, BLACK)
        screen.blit(title, (self.rect.x + 10, self.rect.y - 30))
        x = self.rect.x + 20
        for i, value in enumerate(data):
            height = (value / 100) * self.max_height
            bar_rect = pygame.Rect(x, self.rect.y + self.max_height - height, self.bar_width, height)
            pygame.draw.rect(screen, SECONDARY_BLUE, bar_rect, border_radius=3)
            value_text = self.font.render(f"{value:.1f}", True, BLACK)
            text_rect = value_text.get_rect(centerx=x + self.bar_width / 2, bottom=self.rect.bottom - 5)
            screen.blit(value_text, text_rect)
            x += self.bar_width + self.spacing

class Application:
    def __init__(self):
        self.screen = pygame.display.set_mode((WINDOW_WIDTH, WINDOW_HEIGHT))
        pygame.display.set_caption('Robot Control Panel')
        self.clock = pygame.time.Clock()
        self.network = NetworkManager()
        self.robot = Robot()
        self.bar_data = [0] * 10
        self.create_input_boxes()
        self.bar_graph = BarGraph(WIDTH//2+50, 75, 325, 300)
        self.start_udp_listener()

    def create_input_boxes(self):
        labels = ["FWD_KP", "FWD_KD", "FWD_KI", "ROT_KP", "ROT_KD", "ROT_KI", "STR_KP", "STR_KD", "STR_KI", "SPEED"]
        self.input_boxes = []
        for i, label in enumerate(labels):
            row = i % 5
            col = i // 5
            x = 20 + col * 320
            y = 80 + row * 50
            self.input_boxes.append(TextInputBox(x, y, label, self.network.send_message))

    def classify_robot_data(self, decoded_data):
        robot_data_list = []
        #print(decoded_data)
        data_parts = decoded_data.split(':')

        if len(data_parts) == 2:
            data_type = data_parts[0].strip()
            values = data_parts[1].strip().split(',')

            if data_type == "VELOCITY" and len(values) == 4:
                velocity_data = [float(values[0]), float(values[1]), float(values[2]), float(values[3])]
                #print(velocity_data)
                robot_data_list.append('VELOCITY')
                robot_data_list+= velocity_data
            elif data_type == "IRSENSORS" and len(values) == 10:
                ir_data = list(map(float, values))
                robot_data_list.append('IRSENSORS')
                robot_data_list += ir_data
            elif data_type == "STATE" and len(values) == 2:
                state_data = [values[0].strip(), values[1].strip()]
                robot_data_list.append('STATE')
                robot_data_list += state_data
            elif data_type == "DISTSENSORS" and len(values) == 2:
                dist_data = [float(values[0]), float(values[1])]
                robot_data_list.append('DISTSENSORS')
                robot_data_list += dist_data

        #print(robot_data_list)
        return robot_data_list


    def start_udp_listener(self):
        def udp_listen():
            while True:
                try:
                    self.network.sock.settimeout(0.02)
                    data, addr = self.network.sock.recvfrom(1024)
                    message = data.decode()
                    #print(message)
                    self.robot.update_robot(self.classify_robot_data(message))
                    values = message.split(":")[1].split(",")
                    for i, val in enumerate(values):
                        self.bar_data[i] = float(val)
                    self.network.add_message(message)
                except socket.timeout:
                    pass
                except Exception as e:
                    self.network.add_message(f"Receive error: {str(e)}", is_error=True)
                time.sleep(0.02)
        threading.Thread(target=udp_listen, daemon=True).start()

    def run(self):
        global linear_velocities, rotational_velocities
        linear_velocities, rotational_velocities = generate_velocity_values()

        running = True
        while running:
            for event in pygame.event.get():
                if event.type == QUIT:
                    running = False
                for box in self.input_boxes:
                    box.handle_event(event)

            linear_velocities, rotational_velocities = update_velocity_values(linear_velocities, rotational_velocities)
            self.screen.fill(WHITE)

            # Draw "Control Panel" label
            control_panel_label = pygame.font.Font(None, 32).render("Control Panel", True, BLACK)
            self.screen.blit(control_panel_label, (20, 30))
            
            for box in self.input_boxes:
                box.draw(self.screen)

            # Draw "Sensor Readings" label within the bar graph
            self.bar_graph.draw(self.screen, self.robot.IRSENSORS)

            # Draw "Velocity" graphs and label
            draw_graphs()

            # Draw "Messages" label and messages
            messages_label = pygame.font.Font(None, 32).render("Messages", True, BLACK)
            self.screen.blit(messages_label, (20, HEIGHT//2))
            msg_font = pygame.font.Font(None, 24)
            for i, msg in enumerate(self.network.messages):
                text = msg_font.render(msg["text"], True, ERROR_RED if msg["is_error"] else BLACK)
                self.screen.blit(text, (20, HEIGHT//2 + (i+2) * 20))

            draw_labels(self.robot)


            pygame.display.flip()
            self.clock.tick(FPS)

        pygame.quit()
        sys.exit()

if __name__ == '__main__':
    app = Application()
    app.run()
