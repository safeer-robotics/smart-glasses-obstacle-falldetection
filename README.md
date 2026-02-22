# smart-glasses-obstacle-falldetection
IoT-enabled smart glasses with obstacle avoidance and fall detection using MPU-6050, ESP-32 and C++.

## Project Summary:
Assistive smart glasses concept for visually impaired users using ESP32 + ultrasonic sensing + MPU6050 fall detection, with real-time alerts via Blynk and Firebase. Built and tested in Wokwi simulation, designed to be transferable to a physical prototype.

This project is a **Smart Glasses system** designed to assist visually impaired users by providing real-time Obstacles detection, fall detection, and manual emergency alerts. It integrates sensors, Wi-Fi connectivity, Firebase database, and the Blynk app for remote monitoring and alerting. This project is completely made in wokwi online simulation software, but with all these hardware and code that i have added it can be easily made in real life prototype.

---

## Features

- **Obstacles Detection:** Uses three ultrasonic sensors to detect objects on the left, center, and right sides. When an object is detected within 100 cm, the system alerts the user with a buzzer and sends an update to Firebase.
- **Fall Detection:** Uses an MPU6050 gyroscope sensor to detect sudden falls by measuring angular velocity. When a fall is detected, the system sends alerts via Blynk and Firebase, displaying location information.
- **Manual Emergency Alert:** A button allows users to manually trigger an emergency alert. The alert is sent to Firebase and Blynk, along with the preset GPS location.
- **Real-time Remote Monitoring:** Integration with Firebase Realtime Database and Blynk app for live updates and remote alert notifications.
- **LCD Display:** Two Blynk LCD widgets display fall alerts and manual emergency alerts on the app.

---

## Hardware Components

- ESP32 Development Board
- MPU6050 Gyroscope and Accelerometer Sensor
- Ultrasonic Sensors (HC-SR04 or equivalent) - 3 units
- Buzzer
- Push Button (with internal pull-up resistor)
- Wi-Fi Network
- Optional: OLED or LCD displays for local alerts (via Blynk app)

---

## Software Libraries Used

- **Wire.h** – For I2C communication with MPU6050
- **MPU6050.h** – For MPU6050 sensor control and data reading
- **WiFi.h** – To connect ESP32 to Wi-Fi
- **BlynkSimpleEsp32.h** – For connecting to Blynk IoT platform
- **FirebaseESP32.h** – For Firebase Realtime Database connectivity

---

## How It Works

1. **Initialization:** The ESP32 connects to the specified Wi-Fi network and initializes Firebase and Blynk connections.
2. **Obstacles Detection:** Ultrasonic sensors continuously measure distances. If an object is detected within a threshold (100 cm), the buzzer sounds and a Firebase update is triggered.
3. **Fall Detection:** The MPU6050 gyroscope readings are monitored. When the angular velocity exceeds a threshold, it triggers a fall alert with location data.
4. **Manual Alert:** Pressing the emergency button sends an immediate alert with GPS coordinates, lasting for 10 seconds.
5. **Remote Monitoring:** Alerts and sensor data are pushed to Firebase and displayed on the Blynk app in real time.

---
## Demo (Wokwi Simulation)
- Wokwi project link: https://wokwi.com/projects/430975671899423745
## Usage Instructions

1. Connect all hardware components as per pin definitions in the code.
2. Replace Wi-Fi credentials (`WIFI_SSID`, `WIFI_PASS`) with your network details.
3. Insert your Firebase project credentials (`FIREBASE_HOST`, `FIREBASE_AUTH`).
4. Set the GPS location in the `LOCATION` variable (latitude,longitude).
5. Upload the code to your ESP32 using the Arduino IDE.
6. Open the Blynk app and use the provided template ID and auth token to connect.
7. Monitor sensor data and alerts through the Blynk app and Firebase Realtime Database.

---

## Pin Configuration

| Component          | ESP32 Pin    |
|--------------------|--------------|
| Ultrasonic Sensor Left - Trigger | 12           |
| Ultrasonic Sensor Left - Echo    | 13           |
| Ultrasonic Sensor Centre - Trigger | 5           |
| Ultrasonic Sensor Centre - Echo  | 18           |
| Ultrasonic Sensor Right - Trigger | 14           |
| Ultrasonic Sensor Right - Echo   | 27           |
| Buzzer              | 26           |
| Emergency Button    | 32 (with internal pull-up) |

---

## Firebase Database Paths

- `/object_detection` - Updates with the detected object’s position or "Cleared" when no object is detected.
- `/fall_alert` - Updates when a fall is detected or cleared.
- `/manual_alert` - Updates when manual emergency alert is active or cleared.

---

## Dependencies & Setup

Make sure you install the following Arduino libraries before uploading the code:

- **MPU6050**
- **Blynk**
- **Firebase ESP32**
- **Wire**

---

## License

This project is open-source and available under the MIT License.

---

## Author

Safeer Ahmed  
Contact: safeerkhan5471@gmail.com

---

## Acknowledgements

- Blynk IoT platform for remote monitoring.
- Firebase Realtime Database for cloud data storage.
- MPU6050 sensor library contributors.

---

Feel free to contribute, raise issues, or suggest improvements!
