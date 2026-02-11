# ESP32 Water Monitoring System 💧

This is a simple IoT project built using ESP32 to monitor water level and temperature in real time.  
The idea was to create a small system that can give alerts when water becomes low and can also be monitored remotely.

## About the Project

In many homes and industries, people forget to check the water level in tanks.  
This project helps solve that problem by automatically monitoring water and giving alerts.

The system uses sensors to measure the water level and temperature, shows the data on an OLED screen, and also sends alerts through Blynk dashboard and Gmail notifications.

## What the System Does

• Measures water level using Ultrasonic Sensor  
• Reads temperature using DHT11  
• Shows live data on OLED display  
• Uses LEDs to indicate water status:
  - Blue → Water level high  
  - Yellow → Medium level  
  - Red → Low level  

• Buzzer turns ON when water becomes low or medium  
• Data can be viewed remotely using Blynk  
• Gmail warning notification is sent using Pipedream

## Components Used

- ESP32  
- Ultrasonic Sensor (HC-SR04)  
- DHT11 Temperature Sensor  
- OLED Display  
- LEDs (Red, Yellow, Blue)  
- Buzzer  

## Tools Used

Arduino IDE  
Blynk IoT Platform  
Pipedream (Email alerts)

## Future Improvements

- Store data in Firebase
- Create mobile app dashboard
- Add data logging and analytics


This project helped me learn more about IoT, sensors and real-time monitoring systems.
