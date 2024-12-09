#include <Arduino.h>
#include "PowerMonitor.h"
#include "MQTTClient.h"

// WiFi and MQTT Configuration
const char* WIFI_SSID = "iPhone 15 Pro";
const char* WIFI_PASSWORD = "000000madrid";
const char* MQTT_SERVER = "172.20.10.6";  // Node-RED server IP
const int MQTT_PORT = 1883;
const char* MQTT_TOPIC = "pms01/power";

// NTP Configuration
const char* NTP_SERVER = "pool.ntp.org";
const long GMT_OFFSET_SEC = 3600 * 9;  // KST (UTC+9)
const int DAYLIGHT_OFFSET_SEC = 0;

MQTTClient mqttClient(WIFI_SSID, WIFI_PASSWORD, MQTT_SERVER, MQTT_PORT);
StaticJsonDocument<200> doc;

PowerMonitor powerMonitor;
const unsigned long UPDATE_INTERVAL = 1000; // 1 second
unsigned long lastUpdateTime = 0;

void setup() {
    Serial.begin(115200);
    while (!Serial) { delay(100); }

    Serial.println("\n=== PMS01 Power Monitor with MQTT Starting ===");

    // Initialize power monitor
    if (!powerMonitor.begin()) {
        Serial.println("ERROR: Failed to initialize PowerMonitor!");
        Serial.println("System halted. Please check connections and restart.");
        while (1) { delay(1000); }
    }

    // Initialize MQTT client
    if (!mqttClient.begin()) {
        Serial.println("ERROR: Failed to initialize MQTT Client!");
        Serial.println("System halted. Please check connections and restart.");
        while (1) { delay(1000); }
    }

    // Initialize time
    configTime(GMT_OFFSET_SEC, DAYLIGHT_OFFSET_SEC, NTP_SERVER);
    while (!time(nullptr)) {
        Serial.print(".");
        delay(1000);
    }
    Serial.println("\nTime synchronized");
}

void loop() {
    mqttClient.loop();

    unsigned long currentTime = millis();
    if (currentTime - lastUpdateTime >= UPDATE_INTERVAL) {
        powerMonitor.update();

        if (powerMonitor.isActive()) {
            time_t now = time(nullptr);
            struct tm* timeinfo = localtime(&now);
            char timestamp[24];
            strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", timeinfo);

            doc.clear();
            doc["timestamp"] = timestamp;
            doc["voltage"] = powerMonitor.getVoltage();
            doc["current"] = powerMonitor.getCurrent();
            doc["power"] = powerMonitor.getPower();
            doc["energy"] = powerMonitor.getEnergy();
            doc["frequency"] = powerMonitor.getFrequency();
            doc["powerFactor"] = powerMonitor.getPowerFactor();

            if (mqttClient.publish(MQTT_TOPIC, doc)) {
                Serial.println("Data published to MQTT");
            } else {
                Serial.println("Failed to publish data");
            }
        } else {
            Serial.println("PZEM device not detected!");
        }

        lastUpdateTime = currentTime;
    }
}