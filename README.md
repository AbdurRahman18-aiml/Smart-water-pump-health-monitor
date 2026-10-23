# Smart Water Pump Health Monitor

An IoT-based prototype for monitoring water pump health using an ESP32, MPU6050 sensor, LCD display, and fault alerts.

## Project Overview

The Smart Water Pump Health Monitor demonstrates how embedded systems and IoT concepts can be used to monitor the operating condition of a water pump.

The ESP32 processes vibration readings, displays pump status and simulated water-flow information, and activates LED and buzzer alerts when a warning or fault condition is detected.

## Features

- Vibration-status display using the MPU6050.
- Vibration RMS value shown on the LCD.
- Simulated water-flow rate and total water volume.
- Green and red LED status indicators.
- Buzzer alerts for warning and fault conditions.
- Pushbutton for manually simulating a pump fault.
- Circuit simulation using Wokwi.

## Components and Technologies

- ESP32 DevKit
- MPU6050 accelerometer and gyroscope
- I2C LCD display
- LEDs and buzzer
- Pushbutton
- Arduino C++
- Wokwi simulator

## Working Principle

1. The MPU6050 provides acceleration readings to the ESP32.
2. The program calculates a vibration RMS value and assigns a vibration status.
3. The LCD displays vibration information and simulated water-flow data.
4. LEDs and the buzzer indicate warning and fault conditions.
5. A pushbutton allows a pump fault to be simulated during testing.

## Simulation

Try the project on Wokwi:

https://wokwi.com/projects/478483576238468097

## Current Limitations

- Water flow is simulated in software rather than measured using a physical flow sensor.
- Vibration status currently uses preset test values to demonstrate the warning and alarm logic.
- Wi-Fi dashboard integration and machine-learning-based fault prediction are not yet implemented.

## Future Improvements

- Replace preset vibration values with measured vibration data.
- Add a real water-flow sensor.
- Build an IoT dashboard for remote monitoring.
- Explore machine-learning-based anomaly detection.
- Add data logging and historical graphs.

## Project Status

Working simulation prototype — developed as a learning project exploring IoT, embedded systems, sensor monitoring, and future AI/ML extensions.

