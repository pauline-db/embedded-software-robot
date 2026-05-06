# embedded-software-robot
# Robotic Car System

This project implements a complete control pipeline for a line-following robotic car. It covers obstacle detection, line detection, obstacle avoidance, and distance-based stopping for sub-assignments (a)-(d) of CSE2425.

**Author:** Pauline De Baets  
**Framework:** Pico VS Code Extension
**Date:** Spring 2026

## Project Overview

The robot uses a Raspberry Pi Pico-based setup with separate source files for each assignment. The main file selects the active assignment, while shared support files handle initialization, motor control, ultrasonic sensing, infrared sensing, GPIO definitions, and PIO-based echo timing.

The implementation is organized around a finite-state machine with five assignment states. Each state runs its own control loop until a transition condition is met. Before changing states, the motors are stopped.

## Main Components

- **Main control:** selects which assignment is active.
- **Assignment files:** contain the assignment-specific control loops.
- **Initialization files:** configure required peripherals and GPIOs.
- **Motor functions:** control robot movement, including forward motion and turning.
- **HC-SR04 functions:** measure distances using ultrasonic sensors.
- **TCRT5000 functions:** read infrared sensors for line detection.
- **PIO echo timing file:** measures how long the ultrasonic echo pin remains high.
- **Definitions file:** stores GPIO pin assignments and pin-name conventions.

## Hardware and Sensors

### Motors

- `A1A`, `A1B`, `B1A`, `B1B`

### TCRT5000 Infrared Sensors

- `IR1_3`: digital pin for left sensor
- `IR1_4`: analog pin for left sensor
- `IR2_3`: digital pin for right sensor
- `IR2_4`: analog pin for right sensor

### Encoders

- `ENC1_1`: left encoder output pin
- `ENC1_2`: right encoder output pin

### HC-SR04 Ultrasonic Sensors

- `SRF1_E`: echo pin of front sensor
- `SRF1_T`: trigger pin of front sensor
- `SRF2_E`: echo pin of side sensor
- `SRF2_T`: trigger pin of side sensor

## Implemented Algorithms

### (a) Obstacle Detection

The robot detects an object in front of it using an HC-SR04 ultrasonic sensor. A PIO program measures the echo pulse duration, which is converted into distance. An object is considered detected when it is closer than 15 cm.

Key parameters:

- Distance threshold: 15 cm
- Sensor timeout: 3 s
- Window size: N

### (b) Line Detection

The robot uses two TCRT5000 infrared sensors to detect the ground under the robot and follow a black line. The goal is to keep the line centered between the sensors.

Basic steering logic:

- Left sensor clear and right sensor clear: move forward
- Left sensor clear and right sensor detects line: turn right
- Left sensor detects line and right sensor clear: turn left
- Both sensors detect black: continue previous motion to handle sharp or irregular turns

Key parameter:

- Analog threshold: 1000

### (c) Obstacle Avoidance

When the front ultrasonic sensor detects an obstacle within 15 cm, the robot starts an avoidance maneuver. It turns around the object while using the side ultrasonic sensor to maintain an approximate distance. Once the line is found again using the infrared sensors, the robot returns to line-following mode.

Key parameters:

- Stabilizer: 4 consecutive reads
- Turn right when side sensor distance is less than 15 cm
- Turn left when side sensor distance is greater than 20 cm and front sensor distance is less than 50 cm
- Zero distance readings are treated as infinite distance

### (d) Traveling Distance Calculation

The robot estimates the distance traveled using wheel encoder interrupts. Rising edges on each encoder signal increment left and right tick counters. The robot uses the average of both tick counts to estimate traveled distance.

Distance model:

```text
d_tick = 2 * pi * R / PPR
D ~= n_avg * d_tick
