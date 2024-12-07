import serial
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
import numpy as np
import argparse
from datetime import datetime
import os
from scipy import stats

class MotorCalibrationPlotter:
    def __init__(self, port='COM3', baudrate=115200, log_dir='motor_logs'):
        # Initialize data storage
        self.percentages = []
        self.lb_rps = []
        self.rb_rps = []
        self.lf_rps = []
        self.rf_rps = []
        
        # Setup logging
        self.log_dir = log_dir
        os.makedirs(log_dir, exist_ok=True)
        self.log_file = os.path.join(log_dir, 
            f'motor_calibration_{datetime.now().strftime("%Y%m%d_%H%M%S")}.txt')
        
        # Setup serial connection
        self.serial = serial.Serial(port, baudrate)
        print(f"Connected to {port} at {baudrate} baud")
        print(f"Logging data to: {self.log_file}")
        
        # Setup the real-time plot
        self.fig, (self.ax1, self.ax2) = plt.subplots(2, 1, figsize=(10, 10))
        self.fig.suptitle('Motor Calibration Results')
        
        # Initialize lines for positive and negative percentages
        self.lines_pos = []
        self.lines_neg = []
        
        # Setup both plots with initial ranges
        for ax in [self.ax1, self.ax2]:
            lines = ax.plot([], [], 'b-', [], [], 'r-', [], [], 'g-', [], [], 'y-')
            if ax == self.ax1:
                self.lines_pos = lines
                ax.set_xlim(0, 100)
                ax.set_title('Positive Motor Percentages')
            else:
                self.lines_neg = lines
                ax.set_xlim(-100, 0)
                ax.set_title('Negative Motor Percentages')
                ax.set_xlabel('Motor Percentage')
            ax.set_ylabel('RPS (Rotations Per Second)')
            ax.set_ylim(0, 10)
            ax.grid(True)
            ax.legend(['Left Back', 'Right Back', 'Left Front', 'Right Front'])

    def log_data(self, line, is_header=False):
        with open(self.log_file, 'a') as f:
            if is_header:
                f.write(f"\n{'-'*50}\n{line}\n{'-'*50}\n")
            else:
                f.write(f"{line}\n")

    def calculate_linear_approximation(self):
        # Separate positive and negative data
        pos_mask = np.array(self.percentages) >= 0
        neg_mask = np.array(self.percentages) < 0
        
        results = {
            'positive': {'x': [], 'y': [], 'labels': []},
            'negative': {'x': [], 'y': [], 'labels': []}
        }
        
        motor_data = [
            (self.lb_rps, 'Left Back'),
            (self.rb_rps, 'Right Back'),
            (self.lf_rps, 'Left Front'),
            (self.rf_rps, 'Right Front')
        ]
        
        # Calculate linear approximations
        approx_results = []
        for rps_data, motor_name in motor_data:
            # Positive percentages
            pos_x = np.array(self.percentages)[pos_mask]
            pos_y = np.array(rps_data)[pos_mask]
            if len(pos_x) > 1:
                slope, intercept, r_value, p_value, std_err = stats.linregress(pos_x, pos_y)
                approx_results.append({
                    'motor': motor_name,
                    'direction': 'positive',
                    'slope': slope,
                    'intercept': intercept,
                    'r_squared': r_value**2
                })
                results['positive']['x'].append(pos_x)
                results['positive']['y'].append(pos_y)
                results['positive']['labels'].append(motor_name)
            
            # Negative percentages
            neg_x = np.array(self.percentages)[neg_mask]
            neg_y = np.array(rps_data)[neg_mask]
            if len(neg_x) > 1:
                slope, intercept, r_value, p_value, std_err = stats.linregress(neg_x, neg_y)
                approx_results.append({
                    'motor': motor_name,
                    'direction': 'negative',
                    'slope': slope,
                    'intercept': intercept,
                    'r_squared': r_value**2
                })
                results['negative']['x'].append(neg_x)
                results['negative']['y'].append(neg_y)
                results['negative']['labels'].append(motor_name)
        
        # Log approximation results
        self.log_data("Linear Approximation Results:", True)
        for result in approx_results:
            log_line = (f"Motor: {result['motor']}, Direction: {result['direction']}\n"
                       f"    y = {result['slope']:.4f}x + {result['intercept']:.4f}\n"
                       f"    R² = {result['r_squared']:.4f}")
            self.log_data(log_line)
        
        # Create approximation plot
        fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(10, 10))
        fig.suptitle('Motor Response Linear Approximation')
        
        colors = ['b', 'r', 'g', 'y']
        
        # Plot positive percentages
        ax1.set_title('Positive Motor Percentages')
        for i in range(len(results['positive']['labels'])):
            x = results['positive']['x'][i]
            y = results['positive']['y'][i]
            label = results['positive']['labels'][i]
            ax1.scatter(x, y, c=colors[i], alpha=0.5, label=f'{label} (Data)')
            
            # Plot approximation line
            if len(x) > 1:
                slope, intercept, r_value, p_value, std_err = stats.linregress(x, y)
                x_line = np.array([min(x), max(x)])
                y_line = slope * x_line + intercept
                ax1.plot(x_line, y_line, c=colors[i], linestyle='--', 
                        label=f'{label} (Fit: y={slope:.3f}x+{intercept:.3f})')
        
        # Plot negative percentages
        ax2.set_title('Negative Motor Percentages')
        for i in range(len(results['negative']['labels'])):
            x = results['negative']['x'][i]
            y = results['negative']['y'][i]
            label = results['negative']['labels'][i]
            ax2.scatter(x, y, c=colors[i], alpha=0.5, label=f'{label} (Data)')
            
            # Plot approximation line
            if len(x) > 1:
                slope, intercept, r_value, p_value, std_err = stats.linregress(x, y)
                x_line = np.array([min(x), max(x)])
                y_line = slope * x_line + intercept
                ax2.plot(x_line, y_line, c=colors[i], linestyle='--', 
                        label=f'{label} (Fit: y={slope:.3f}x+{intercept:.3f})')
        
        for ax in [ax1, ax2]:
            ax.grid(True)
            ax.legend()
            ax.set_xlabel('Motor Percentage')
            ax.set_ylabel('RPS (Rotations Per Second)')
        
        plt.tight_layout()
        
        # Save the approximation plot
        plot_file = os.path.join(self.log_dir, 
            f'motor_approximation_{datetime.now().strftime("%Y%m%d_%H%M%S")}.png')
        fig.savefig(plot_file)
        self.log_data(f"\nApproximation plot saved to: {plot_file}", True)

        self.calculate_inverse_functions(approx_results)

    def parse_line(self, line):
        try:
            data = [float(x.strip()) for x in line.split(',')]
            if len(data) == 5:
                return data
        except:
            return None
        return None

    def update_plot(self, frame):
        if self.serial.in_waiting:
            line = self.serial.readline().decode('utf-8').strip()
            
            # Log raw data
            self.log_data(line)
            
            # Skip header lines
            if line.startswith("Starting") or line.startswith("Percentage"):
                return self.lines_pos + self.lines_neg
            
            if line.startswith("Calibration complete"):
                self.calculate_linear_approximation()
                return self.lines_pos + self.lines_neg
            
            # Parse the data
            data = self.parse_line(line)
            if data:
                percentage, lb, rb, lf, rf = data
                
                # Store the data
                self.percentages.append(percentage)
                self.lb_rps.append(abs(lb))
                self.rb_rps.append(abs(rb))
                self.lf_rps.append(abs(lf))
                self.rf_rps.append(abs(rf))
                
                # Update plots (same as before)
                pos_mask = np.array(self.percentages) >= 0
                neg_mask = np.array(self.percentages) < 0
                
                for i, (line, data) in enumerate(zip(self.lines_pos, 
                    [self.lb_rps, self.rb_rps, self.lf_rps, self.rf_rps])):
                    pos_x = np.array(self.percentages)[pos_mask]
                    pos_y = np.array(data)[pos_mask]
                    if len(pos_x) > 0:
                        line.set_data(pos_x, pos_y)
                
                for i, (line, data) in enumerate(zip(self.lines_neg,
                    [self.lb_rps, self.rb_rps, self.lf_rps, self.rf_rps])):
                    neg_x = np.array(self.percentages)[neg_mask]
                    neg_y = np.array(data)[neg_mask]
                    if len(neg_x) > 0:
                        line.set_data(neg_x, neg_y)
                
                # Update axis limits
                all_rps = self.lb_rps + self.rb_rps + self.lf_rps + self.rf_rps
                if all_rps:
                    max_rps = max(all_rps)
                    y_padding = max_rps * 0.1
                    for ax in [self.ax1, self.ax2]:
                        ax.set_ylim(0, max_rps + y_padding)
                    
                    if any(pos_mask):
                        max_pos = max(np.array(self.percentages)[pos_mask])
                        self.ax1.set_xlim(0, max_pos + 2)
                    
                    if any(neg_mask):
                        min_neg = min(np.array(self.percentages)[neg_mask])
                        self.ax2.set_xlim(min_neg - 2, 0)
                
                self.fig.canvas.draw_idle()
                
        return self.lines_pos + self.lines_neg

    def start(self):
        max_motor_percentage = 100
        percentage_step = 2.0
        expected_points = int((2 * max_motor_percentage / percentage_step) + 2)
        
        self.ani = FuncAnimation(
            self.fig,
            self.update_plot,
            interval=50,
            save_count=expected_points,
            cache_frame_data=True,
            blit=True
        )
        plt.show()
        
    def cleanup(self):
        self.serial.close()
    
    def calculate_inverse_functions(self, approx_results):
        # Create inverse function plot
        fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(10, 10))
        fig.suptitle('Motor Inverse Functions (RPS to Percentage)')
        
        colors = ['b', 'r', 'g', 'y']
        
        # Separate positive and negative results
        pos_results = [r for r in approx_results if r['direction'] == 'positive']
        neg_results = [r for r in approx_results if r['direction'] == 'negative']
        
        # Calculate and log inverse functions
        self.log_data("\nSimplified Motor Equations (y = mx + c format):", True)
        
        # Function to calculate inverse function from slope and intercept
        def get_inverse_function(slope, intercept):
            return lambda rps: (rps - intercept)/slope
        
        # Plot positive direction inverse functions
        ax1.set_title('Positive RPS to Percentage')
        for i, result in enumerate(pos_results):
            motor = result['motor']
            slope = result['slope']
            intercept = result['intercept']
            
            # Calculate inverse function
            inverse_func = get_inverse_function(slope, intercept)
            
            # Get RPS range from data
            rps_data = {
                'Left Back': self.lb_rps,
                'Right Back': self.rb_rps,
                'Left Front': self.lf_rps,
                'Right Front': self.rf_rps
            }[motor]
            rps_data = np.array(rps_data)
            
            pos_mask = np.array(self.percentages) >= 0
            rps_data = rps_data[pos_mask]
            
            if len(rps_data) > 0:
                rps_range = np.linspace(0, max(rps_data), 100)
                percentage_range = [inverse_func(rps) for rps in rps_range]
                
                # Plot inverse function with simplified equation
                ax1.plot(rps_range, percentage_range, c=colors[i], linestyle='--',
                        label=f'{motor}: y = {1/slope:.4f}x + {-intercept/slope:.4f}')
                
                # Plot original data points
                ax1.scatter(rps_data, np.array(self.percentages)[pos_mask], 
                        c=colors[i], alpha=0.5)
                
                # Log simplified equation
                self.log_data(f"{motor} (Positive):")
                self.log_data(f"    y = {1/slope:.4f}x + {-intercept/slope:.4f}")
        
        # Plot negative direction inverse functions
        ax2.set_title('Negative RPS to Percentage')
        for i, result in enumerate(neg_results):
            motor = result['motor']
            slope = result['slope']
            intercept = result['intercept']
            
            # Calculate inverse function
            inverse_func = get_inverse_function(slope, intercept)
            
            # Get RPS range from data
            rps_data = {
                'Left Back': self.lb_rps,
                'Right Back': self.rb_rps,
                'Left Front': self.lf_rps,
                'Right Front': self.rf_rps
            }[motor]
            rps_data = np.array(rps_data)
            
            neg_mask = np.array(self.percentages) < 0
            rps_data = rps_data[neg_mask]
            
            if len(rps_data) > 0:
                rps_range = np.linspace(0, max(rps_data), 100)
                percentage_range = [inverse_func(rps) for rps in rps_range]
                
                # Plot inverse function with simplified equation
                ax2.plot(rps_range, percentage_range, c=colors[i], linestyle='--',
                        label=f'{motor}: y = {1/slope:.4f}x + {-intercept/slope:.4f}')
                
                # Plot original data points
                ax2.scatter(rps_data, np.array(self.percentages)[neg_mask], 
                        c=colors[i], alpha=0.5)
                
                # Log simplified equation
                self.log_data(f"{motor} (Negative):")
                self.log_data(f"    y = {1/slope:.4f}x + {-intercept/slope:.4f}")
        
        # Configure plot axes and labels
        for ax in [ax1, ax2]:
            ax.grid(True)
            ax.legend()
            ax.set_xlabel('RPS (Rotations Per Second)')
            ax.set_ylabel('Motor Percentage')
        
        ax1.set_ylim(0, 100)
        ax2.set_ylim(-100, 0)
        
        plt.tight_layout()
        
        # Save the inverse function plot
        plot_file = os.path.join(self.log_dir, 
            f'motor_inverse_functions_{datetime.now().strftime("%Y%m%d_%H%M%S")}.png')
        fig.savefig(plot_file)
        self.log_data(f"\nInverse function plot saved to: {plot_file}", True)
    

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Plot motor calibration data from serial port')
    parser.add_argument('--port', default='COM3', help='Serial port to use')
    parser.add_argument('--baud', type=int, default=115200, help='Baud rate')
    parser.add_argument('--log-dir', default='./motor_logs', help='Directory to save logs and plots')
    args = parser.parse_args()
    
    plotter = MotorCalibrationPlotter(port=args.port, baudrate=args.baud, log_dir=args.log_dir)
    try:
        plotter.start()
    finally:
        plotter.cleanup()