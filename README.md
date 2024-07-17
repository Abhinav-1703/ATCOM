# ATCOM

This repository contains two ESP32 projects:

## AT Commands Loop

This project demonstrates how to run AT commands in a continuous loop on an ESP32. The code repeatedly sends AT commands to a connected module and processes the responses.

### Features
- Sends a predefined set of AT commands in a loop.
- Processes and prints responses from the connected module.

## GPS Data to ThingSpeak

This project collects latitude and longitude data from a GPS module connected to an ESP32 and sends the data to ThingSpeak for visualization.

### Features
- Collects GPS data (latitude and longitude) using a connected GPS module.
- Pushes the collected data to ThingSpeak at regular intervals.
- Allows real-time monitoring of GPS data on ThingSpeak.

## License
This repository is licensed under the MIT License. See the [LICENSE](LICENSE) file for more information.

