# Smart Trash Can Monitoring System

This project is an Arduino-based system for monitoring the fill level of a trash can. It utilizes an ultrasonic sensor (HC-SR04) to measure the distance to the trash, and a touch sensor to detect when the trash can is opened. The fill level is then displayed using RGB LEDs, and the data is transmitted via LoRaWAN protocol.

## Components

- Arduino UNO
- Ultrasonic Sensor HC-SR04
- RGB LED (Red, Green, Blue)
- Touch Sensor

## Features

1. **Trash Fill Level Monitoring**: The ultrasonic sensor measures the distance to the trash, and the fill level is displayed using the RGB LEDs. The LEDs indicate the fill level as follows:
   - Blue LED: Low fill level
   - Green LED: Medium fill level
   - Red LED: High fill level

2. **Touch Sensor Integration**: The touch sensor is used to detect when the trash can is opened. When the trash can is opened, the system will send a message indicating that the trash can is open.

3. **LoRaWAN Connectivity**: The system uses LoRaWAN protocol to transmit the fill level data to a remote server or monitoring system. This allows for remote monitoring and tracking of the trash can's status.

## Usage

1. Connect the components to the Arduino UNO as follows:
   - Ultrasonic sensor:
     - Trigger pin to Arduino pin 5
     - Echo pin to Arduino pin 6
   - RGB LED:
     - Blue LED to Arduino pin 2
     - Green LED to Arduino pin 8
     - Red LED to Arduino pin 4
   - Touch sensor to Arduino pin 3

2. Upload the provided Arduino code to the board.

3. The system will start monitoring the trash can's fill level and transmitting the data via LoRaWAN. The RGB LEDs will display the current fill level.

4. When the trash can is opened, the system will send a message indicating that the trash can is open.

## Configuration

The Arduino code includes the following configurable parameters:

- `thresh`: An array of three threshold distances (in cm) that define the fill level ranges for the RGB LEDs.
- LoRaWAN configuration:
  - `AppSKey`: Application Session Key
  - `NetSKey`: Network Session Key
  - `DevAddress`: Device Address

Make sure to update these parameters with the appropriate values for your LoRaWAN network.

## Contribution

If you have any suggestions or improvements, feel free to contribute to the project by submitting a pull request.
