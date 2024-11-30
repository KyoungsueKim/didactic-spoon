#include <Arduino.h>
#include "PowerMonitor.h"


PowerMonitor powerMonitor;
const unsigned long UPDATE_INTERVAL = 1000; // 1 second
unsigned long lastUpdateTime = 0;

void setup() {
    Serial.begin(115200);
    while (!Serial) { delay(100); }  // Wait for serial connection

    Serial.println("\n=== PMS01 Power Monitor Starting ===");

    // Initialize power monitor with error checking
    if (!powerMonitor.begin()) {
        Serial.println("ERROR: Failed to initialize PowerMonitor!");
        Serial.println("System halted. Please check connections and restart.");
        while (1) { delay(1000); }  // Halt execution
    }
}

void loop() {
    unsigned long currentTime = millis();

    if (currentTime - lastUpdateTime >= UPDATE_INTERVAL) {
        powerMonitor.update();

        if (!powerMonitor.isActive()) {
            Serial.println("Warning: PZEM device not detected!");
        }

        lastUpdateTime = currentTime;
    }
}