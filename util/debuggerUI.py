import serial
import threading

class SerialPortHandler:
    def __init__(self, port='/dev/ttyUSB0', baudrate=9600, timeout=1):
        """
        Initialize serial port communication.
        
        :param port: Serial port name (e.g., '/dev/ttyUSB0' for Linux, 'COM3' for Windows)
        :param baudrate: Communication speed (default 9600)
        :param timeout: Read timeout in seconds
        """
        self.port = port
        self.baudrate = baudrate
        self.timeout = timeout
        self.serial_connection = None
        self.is_reading = False
        self.read_thread = None

    def connect(self):
        """
        Establish connection to the serial port.
        
        :return: True if connection successful, False otherwise
        """
        try:
            self.serial_connection = serial.Serial(
                port=self.port, 
                baudrate=self.baudrate, 
                timeout=self.timeout
            )
            return True
        except serial.SerialException as e:
            print(f"Error connecting to serial port: {e}")
            return False

    def disconnect(self):
        """
        Close the serial port connection.
        """
        if self.serial_connection and self.serial_connection.is_open:
            self.stop_reading()
            self.serial_connection.close()
            print("Serial port disconnected.")

    def write_data(self, data):
        """
        Write data to the serial port.
        
        :param data: Data to write (string or bytes)
        :return: True if write successful, False otherwise
        """
        if not self.serial_connection or not self.serial_connection.is_open:
            print("Serial port not connected.")
            return False

        try:
            # Ensure data is in bytes
            if isinstance(data, str):
                data = data.encode('utf-8')
            
            self.serial_connection.write(data)
            return True
        except Exception as e:
            print(f"Error writing to serial port: {e}")
            return False

    def _read_data(self):
        """
        Internal method to continuously read data from the serial port.
        """
        while self.is_reading:
            try:
                if self.serial_connection.in_waiting:
                    data = self.serial_connection.readline().decode('utf-8').strip()
                    if data:
                        print(f"Received data: {data}")
                        # You can modify this to handle data as needed
                        # For example, call a callback function or store in a queue
            except Exception as e:
                print(f"Error reading from serial port: {e}")

    def start_reading(self):
        """
        Start a thread to continuously read from the serial port.
        """
        if not self.serial_connection or not self.serial_connection.is_open:
            print("Serial port not connected.")
            return False

        if not self.is_reading:
            self.is_reading = True
            self.read_thread = threading.Thread(target=self._read_data)
            self.read_thread.daemon = True  # Allow thread to be killed when main program exits
            self.read_thread.start()
            print("Started reading from serial port.")
            return True
        
        return False

    def stop_reading(self):
        """
        Stop the reading thread.
        """
        self.is_reading = False
        if self.read_thread:
            self.read_thread.join()
            print("Stopped reading from serial port.")

# Example usage
def main():
    # Create a serial port handler
    serial_handler = SerialPortHandler(port='COM3', baudrate=9600)
    
    # Connect to the serial port
    if serial_handler.connect():
        # Start reading data in the background
        serial_handler.start_reading()
        
        # Write some data
        serial_handler.write_data("Hello, Serial Port!")
        
        # Keep the program running (in a real scenario, you might have other logic here)
        import time
        time.sleep(10)
        
        # Clean up
        serial_handler.stop_reading()
        serial_handler.disconnect()

if __name__ == "__main__":
    main()
