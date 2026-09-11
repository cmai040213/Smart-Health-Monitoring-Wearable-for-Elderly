# Smart Health Monitoring Wearable for the Elderly

An Arduino-based wearable prototype developed independently by Chi Nhan Mai through the Dallas College ARISE research program in Fall 2025.

## Project Purpose

This project explores how wearable sensors and a web dashboard can support health monitoring for older adults. The prototype measures heart rate and acceleration, flags potential falls, and displays monitoring information in one place.

## Features

* Heart rate measurement and averaging.
* Acceleration-based alerts for potential falls.
* Web dashboard displaying heart rate, data history, a location map, and fall alerts.
* Arduino HTTP server providing sensor readings as JSON.
* Custom PCB designed in KiCad and assembled to reduce wiring.
* Custom enclosure designed in Autodesk Fusion and 3D-printed.

## Technologies

* **Firmware:** Arduino C++
* **Web:** HTML, CSS, JavaScript
* **Hardware:** Arduino UNO R4 WiFi, MPU6050 motion sensor, optical heart rate sensor, NEO-6M GPS module
* **Design:** KiCad, Autodesk Fusion
* **Libraries:** Adafruit MPU6050, Adafruit Unified Sensor, WiFiS3, Arduino_JSON, SoftwareSerial, TinyGPS++, SparkFun MAX3010x Sensor Library

## My Contributions

I independently integrated the hardware, wrote the firmware, built the dashboard, designed and assembled the PCB, and created the enclosure. I also tested the prototype, adjusted the detection threshold, and presented the project at the ARISE Symposium.

## Detection and Testing

The firmware calculates total acceleration magnitude and triggers a potential-fall alert when it exceeds **2.5 g**. The alert resets after approximately **2 seconds**.

I refined the threshold through repeated comparisons of normal movement readings with device drops and staged high-impact movements, including a wearer jumping onto a bed. These tests triggered detection, and the dashboard displayed “Fall detected.”

This is threshold-based detection, not a trained machine learning model. The tests do not establish accuracy for real-world falls.

## Repository Contents

* `Final.ino`: Arduino firmware for sensor readings, detection logic, Wi-Fi connectivity, and JSON responses.
* `index.html`, `product.html`, `aboutMe.html`: Website pages.
* CSS files: Website styling.
* `script.js`: Website JavaScript.
* `images/`: Website image assets.

## Running the Prototype

1. Open `Final.ino` in Arduino IDE. If prompted, place it in a sketch folder named `Final`.
2. Install the board support and libraries required by the firmware.
3. Connect the sensors according to the pin assignments in the sketch and your hardware wiring.
4. Set your Wi-Fi credentials locally. Do not commit actual credentials.
5. Update the static IP configuration to match your network, or adapt the sketch to use DHCP.
6. Upload the sketch and open Serial Monitor at **115200 baud**.
7. Configure the dashboard’s Arduino endpoint to match the device address.
8. Run the dashboard on a device that can reach the Arduino on the local network.

The hosted website alone does not provide remote access to the Arduino. Browser security settings may also block requests from an HTTPS page to the Arduino’s local HTTP server.

## Known Limitations

* GPS reception was unreliable indoors.
* The uploaded firmware currently sends fixed demonstration coordinates in its JSON response, rather than live GPS coordinates.
* Heart rate measurements require steady sensor contact and time to stabilize.
* High-impact movements can trigger alerts without a real fall.
* Heart rate categories are experimental labels and have not been clinically validated.
* The prototype was not clinically validated and is not intended for medical diagnosis or emergency response.

## Author

**Chi Nhan Mai**
Dallas College ARISE Research Program — Fall 2025
