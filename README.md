# Group Project(NTC) - Wind Speed and Direction Monitoring System (Arduino-Based)

## Overview

This project implements an Arduino-based system to measure wind speed and direction, display the collected data on a 16x2 LCD, and transmit it to a remote server using the MQTT protocol over an Ethernet connection. The system also features a 4x4 keypad that allows users to switch between different display modes on the LCD.

## Features

* **Wind Speed Measurement:** Measures wind speed using an analog sensor and calculates it based on the frequency of voltage transitions.
* **Wind Direction Measurement:** Determines wind direction in degrees (0-360°) from an analog sensor and converts it to cardinal directions (N, NE, E, SW, etc.).
* **LCD Display:** Shows sensor data and device information on a 16x2 LCD with four selectable modes:
    * Wind speed only
    * Wind direction only
    * Both wind speed and direction
    * Device IP address
* **MQTT Communication:** Publishes wind speed and direction data to an MQTT broker in JSON format under a specified topic.
* **Configurable Display Modes:** Allows users to toggle between the four display modes using a 4x4 keypad.
* **Ethernet Connectivity:** Utilizes an Ethernet shield for network communication.

## Hardware Requirements

* Arduino Board (e.g., Arduino Uno, Arduino Mega)
* Ethernet Shield (W5100 compatible)
* 16x2 LCD Display (direct pin connection - I2C adapter not used in this code)
* 4x4 Keypad
* Wind Speed Sensor (analog output, 0-5V)
* Wind Direction Sensor (analog output, 0-5V)
* Breadboard
* Jumper Wires

## Pin Configuration

| Component        | Pins Used         |
| ---------------- | ----------------- |
| LCD RS           | 6                 |
| LCD EN           | 7                 |
| LCD D4           | 5                 |
| LCD D5           | 4                 |
| LCD D6           | 3                 |
| LCD D7           | 8                 |
| Keypad Rows      | 9, A4, 2, A5      |
| Keypad Columns   | A0, A1, A2, A3      |
| Wind Speed       | A7                |
| Wind Direction   | A6                |
| Ethernet CS      | 10 (default)      |

## Software Requirements

* Arduino IDE
* **Libraries:**
    * `LiquidCrystal.h` (for LCD control)
    * `Ethernet.h` (for Ethernet communication)
    * `Keypad.h` (for keypad input)
    * `PubSubClient.h` (for MQTT communication)

### Installing Libraries

You can install these libraries using the Arduino Library Manager (Sketch > Include Library > Manage Libraries...) or by downloading them as ZIP files and installing them manually (Sketch > Include Library > Add .ZIP Library...).

## Setup Instructions

### Hardware Setup

1.  Connect the 16x2 LCD to the Arduino using jumper wires according to the pin configuration.
2.  Connect the 4x4 keypad to the Arduino using jumper wires according to the pin configuration.
3.  Connect the analog output of the Wind Speed Sensor to analog pin A7 on the Arduino.
4.  Connect the analog output of the Wind Direction Sensor to analog pin A6 on the Arduino.
5.  Attach the Ethernet Shield to your Arduino board.
6.  Connect your Arduino to your local network using an Ethernet cable.
7.  Ensure all connections are secure and correct.

### Software Setup

1.  Open the Arduino IDE.
2.  Create a new sketch (File > New).
3.  Copy the provided Arduino code into the new sketch.
4.  Install the required libraries as mentioned in the [Software Requirements](#software-requirements) section.

### Configuration

Within the Arduino sketch, locate the following configuration parameters and modify them according to your setup:

* **MQTT Broker:**
    ```arduino
    byte server[] = { 10, 6, 0, 23 }; // IP address of your MQTT broker
    int Port = 1883;                  // Port number of the MQTT broker
    ```
    Update the `server` array with the IP address of your MQTT broker. If your broker uses a different port, change the `Port` variable.

* **MQTT Client ID:**
    ```arduino
    char clientId[] = "a731fsd4"; // Unique client ID for MQTT
    ```
    Set a unique `clientId` for your Arduino to identify itself to the MQTT broker.

* **Ethernet MAC Address (Optional):**
    ```arduino
    byte mymac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
    ```
    The Ethernet shield will usually obtain an IP address via DHCP. If you need to assign a specific MAC address, modify the `mymac` array.

* **MQTT Topic:**
    ```arduino
    const char* topic = "ICT4_out_2020"; // MQTT topic to publish data to
    ```
    Ensure this matches the topic your MQTT broker is configured to listen to.

* **Sampling Period:**
    ```arduino
    const long SAMPLE_PERIOD = 5000; // Time in milliseconds between data updates (5 seconds)
    ```
    Adjust the `SAMPLE_PERIOD` to change how frequently the system reads sensors, updates the LCD, and publishes data.

### Upload

1.  Connect your Arduino board to your computer using a USB cable.
2.  In the Arduino IDE, go to Tools > Board and select the type of your Arduino board.
3.  Go to Tools > Port and select the serial port that corresponds to your Arduino.
4.  Click the "Upload" button (the right-arrow icon) to upload the sketch to your Arduino.

## Usage

### Power On

Once the code is uploaded and the hardware is connected, power on your Arduino. The system will initialize the LCD, establish an Ethernet connection, and attempt to connect to the configured MQTT broker.

### Keypad Controls

Pressing the 'A' key on the 4x4 keypad will cycle through the four different display modes on the LCD:

1.  Wind speed only
2.  Wind direction only
3.  Both wind speed and direction
4.  Device IP address

Other keys on the keypad are currently logged to the Serial Monitor but do not have any other functionality in this version. You can open the Serial Monitor in the Arduino IDE (Tools > Serial Monitor) to see the output when other keys are pressed.

### Data Display

The LCD will update the displayed information every 5 seconds (or according to the `SAMPLE_PERIOD` you configured). The content displayed depends on the currently selected mode.

### MQTT

The system will publish sensor data to the MQTT broker under the topic `ICT4_out_2020`. The data is formatted as a JSON object with the following structure:

```json
{"S_name1":"NTC_windSpeed","S_value1":"XX.XX"}
{"S_name1":"NTC_windDirection","S_value1":"YY"}
