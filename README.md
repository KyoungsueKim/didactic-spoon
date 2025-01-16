# PMS01 Power Monitor Demo Guide
<img width="916" alt="image" src="https://github.com/user-attachments/assets/778c7cbd-cac6-4dd8-a6ea-f943715503e4" />

This document guides you through quickly setting up and running a simple test and demo environment for Dignsys' PMS01 power monitoring board. Previously, manual installation and configuration of Arduino IDE, Node-RED, MQTT broker (Mosquitto), and InfluxDB were required. However, this updated guide allows you to conveniently configure all backend environments using **Docker Compose**. Additionally, the firmware uploaded to the PMS01 board is now built and uploaded using **PlatformIO**.

By following this guide, you can configure the PMS01 board to send power data to a local MQTT server and visualize real-time power measurements via the Node-RED dashboard.

---

## Key Features Overview
<img width="471" alt="image" src="https://github.com/user-attachments/assets/fe8a859e-0f4b-4be7-9a66-9b16900112ff" />

- **PMS01 Board**: Power measurement (voltage, current, power, energy, frequency, power factor) using the PZEM-004T module.
- **MQTT Communication**: Periodic data transmission to a local Mosquitto MQTT broker.
- **Node-RED Dashboard**: Visualization of real-time and historical data with InfluxDB as the backend.
- **Docker Compose Environment**: Convenient single-command deployment of Node-RED, InfluxDB, and Mosquitto services.

---

## Prerequisites

1. Prepare the **PMS01 Board and PZEM-004T Module**.
2. Install **PlatformIO**:
   - Refer to the [PlatformIO official site](https://platformio.org/).
   - Available as a VSCode extension or CLI tool.
3. Install **Docker & Docker Compose**:
   - Refer to the [Docker official site](https://docs.docker.com/get-docker/).
   - See [Docker Compose Installation Guide](https://docs.docker.com/compose/install/).
4. Ensure a **Wi-Fi Network**:
   - The PMS01 board and the test PC (or laptop) must be connected to the same network (Wi-Fi).
5. Supply **12V power to the PMS01 board** and **220V power to the PZEM-004T module**.  
   **Caution:** The PZEM-004T module requires 220V power supply to function properly, independent of the PMS01 board.

---

## Project Structure

```
.
├── README.md                # Project overview and usage guide
├── docker-compose.yml       # Docker Compose file for Node-RED, InfluxDB, and Mosquitto services
├── include
│   └── README               # Documentation for the include directory
├── init-influxdb
│   └── init-influxdb.sh     # Initialization script for InfluxDB
├── lib
│   ├── PZEM004Tv30-pms      # PZEM-004T v3.0 sensor library (for PMS01 board)
│   ├── README               # Documentation for the lib directory
│   └── SC16IS752Serial      # Library for serial communication extension
├── mosquitto
│   └── config               # Mosquitto MQTT broker configuration files
├── platformio.ini           # Configuration for building and uploading PMS01 firmware (PlatformIO)
├── src
│   ├── MQTTClient.cpp       # MQTT client logic
│   ├── MQTTClient.h
│   ├── PowerMonitor.cpp     # Power monitoring logic
│   ├── PowerMonitor.h
│   └── main.cpp             # Main entry point for the PMS01 firmware
├── test
│   └── README               # Documentation for testing
└── volumes
    ├── influxdb-data        # InfluxDB data volume
    ├── mosquitto-data       # Mosquitto data volume
    ├── mosquitto-log        # Mosquitto log volume
    └── nodered-data         # Node-RED data volume
```

---

## Configuring PMS01 Firmware (PlatformIO)

1. Modify the `platformio.ini` file to set the serial port, Wi-Fi, and MQTT settings for the PMS01 board.

   ```cpp
   // Example settings in main.cpp
   const char* WIFI_SSID = "YOUR_WIFI_SSID";
   const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";
   const char* MQTT_SERVER = "192.168.0.10"; // IP of the PC running Docker Compose
   const int MQTT_PORT = 1883;
   ```

   Ensure the PMS01 board and PC are on the same network. Set the `MQTT_SERVER` to the IP address of the host (PC) running the Mosquitto container.

2. Connect the PMS01 board to the PC via USB and use PlatformIO to build and upload the firmware.

   ```bash
   pio run -t upload
   ```

   After the upload is complete, the PMS01 board will connect to the Wi-Fi and begin sending data to the MQTT server.

---

## Running Backend Services with Docker Compose

The `docker-compose.yml` file is located in the root directory of this project. Use the following commands to run backend services like Node-RED, InfluxDB, and Mosquitto.

1. Start Docker Compose:
   ```bash
   docker-compose up -d
   ```

   The following services will run as containers:
   - **Node-RED**: [http://localhost:1880](http://localhost:1880)
   - **InfluxDB**: [http://localhost:8086](http://localhost:8086)
   - **Mosquitto (MQTT Broker)**: mqtt://localhost:1883
   - **InfluxDB Admin UI**: [http://localhost:8083](http://localhost:8083) (optional)

2. Check the infrastructure status:
   ```bash
   docker ps
   ```

   Use `docker logs <container_name>` to troubleshoot any issues.

---

## Setting Up the Node-RED Dashboard

1. Access the Node-RED UI:
   - Open your browser and go to [http://localhost:1880](http://localhost:1880).
2. Import the provided Node-RED flow JSON:
   - In the Node-RED editor, select Import from the top-right menu.
   - Paste the JSON code and click Import.
3. Deploy the flow:
   - Click the Deploy button to activate the flow.
4. View the dashboard:
   - Open [http://localhost:1880/ui](http://localhost:1880/ui) in your browser to view the power monitoring dashboard.

---

## Data Flow

1. **PMS01 Board** -> **Mosquitto (MQTT Broker)**  
   The PMS01 board connects to the configured Wi-Fi and sends power data to the `pms01/power` topic.

2. **Mosquitto** -> **Node-RED**  
   Node-RED subscribes to the `pms01/power` topic and receives data in real-time.

3. **Node-RED** -> **InfluxDB**  
   Data is parsed within Node-RED and stored in InfluxDB.

4. **Node-RED Dashboard**  
   The dashboard retrieves data from InfluxDB for real-time and historical visualization.

---

## Troubleshooting Guide

1. **MQTT Connection Issues**:
   - Check if the PMS01 board is connected to the same LAN as the host PC.
   - Verify that the Mosquitto container is running using `docker logs mosquitto`.
   - Ensure firewall settings allow communication.

2. **Missing Data**:
   - Add a debug node in Node-RED to verify data output from the MQTT-In node.
   - Use PlatformIO Monitor to view logs from the PMS01 board.

3. **InfluxDB Query Issues**:
   - Access the InfluxDB Admin UI at [http://localhost:8083](http://localhost:8083) to check the database and measurements.
   - Verify the InfluxDB node configuration in Node-RED.

---

## Maintenance and Extension

- Modify the PMS01 firmware using PlatformIO for updates.
- Back up and manage InfluxDB data.
- Extend Node-RED flows for additional features (e.g., alerts, email notifications).

---

## License and Contributions

- This project is licensed under the MIT License.
- Bug reports, feature suggestions, and Pull Requests are welcome.

---
