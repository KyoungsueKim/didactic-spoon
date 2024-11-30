//
// Created by Administrator on 2024-11-30.
//

#include "PowerMonitor.h"
#include <Wire.h>

PowerMonitor::PowerMonitor() :
        _serial0(0),      // Only channel 0
        _pzem0(_serial0), // Only one PZEM
        _isActive(false),
        _isInitialized(false),
        _voltage(0),
        _current(0),
        _power(0),
        _energy(0),
        _frequency(0),
        _powerFactor(0) {
}

bool PowerMonitor::begin() {
    Serial.println("\n=== Starting PowerMonitor Initialization ===");

    bool success = true;
    success &= initializeI2C();
    success &= resetSC16IS752();
    success &= initializeSerial();
    success &= initializePZEM();

    _isInitialized = success;

    Serial.printf("Power Monitor Initialization %s\n", success ? "SUCCESSFUL" : "FAILED");
    Serial.println("=========================================\n");

    return success;
}

bool PowerMonitor::initializeI2C() {
    Serial.println("Initializing I2C...");

    Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN, 400000);
    Wire.setClock(400000);

    // Test I2C by attempting to write to SC16IS752
    Wire.beginTransmission(SC16IS752_SADDR0);
    bool i2cOK = (Wire.endTransmission() == 0);

    Serial.printf("I2C Initialization %s\n", i2cOK ? "SUCCESSFUL" : "FAILED");
    if (!i2cOK) {
        Serial.println("Error: Could not communicate with SC16IS752 via I2C");
    }

    return i2cOK;
}

bool PowerMonitor::resetSC16IS752() {
    Serial.println("Resetting SC16IS752...");

    pinMode(SC16IS752_RST_PIN, OUTPUT);
    digitalWrite(SC16IS752_RST_PIN, LOW);
    delay(100);
    digitalWrite(SC16IS752_RST_PIN, HIGH);
    delay(100);

    // Verify reset by attempting to read a register
    Wire.beginTransmission(SC16IS752_SADDR0);
    Wire.write((SC16IS7XX_LCR_REG << 3) | (SC16IS752_CHANNEL_A << 1));
    Wire.endTransmission();

    Wire.requestFrom((uint8_t) SC16IS752_SADDR0, (uint8_t) 1);
    bool resetOK = Wire.available();

    Serial.printf("SC16IS752 Reset %s\n", resetOK ? "SUCCESSFUL" : "FAILED");
    if (!resetOK) {
        Serial.println("Error: Could not verify SC16IS752 reset");
    }

    return resetOK;
}

bool PowerMonitor::initializeSerial() {
    Serial.println("Initializing Serial...");

    _serial0.begin(SC_REF_BAUDRATE);

    // Verify serial initialization by checking if the FIFO is accessible
    uint8_t fifoStatus = _serial0.getFIFOStatus();
    bool serialOK = (fifoStatus != 0xFF);  // 0xFF typically indicates read failure

    Serial.printf("Serial Initialization %s\n", serialOK ? "SUCCESSFUL" : "FAILED");
    if (!serialOK) {
        Serial.println("Error: Could not verify serial FIFO status");
    }

    return serialOK;
}

bool PowerMonitor::initializePZEM() {
    Serial.println("Initializing PZEM...");

    _pzem0.init(&_serial0, false, PZEM_DEFAULT_ADDR);

    // Try to read the address to verify initialization
    uint8_t addr = _pzem0.readAddress();
    bool pzemOK = (addr != 0);

    Serial.printf("PZEM Initialization %s\n", pzemOK ? "SUCCESSFUL" : "FAILED");
    if (!pzemOK) {
        Serial.println("Error: Could not verify PZEM address");
    }
    if (pzemOK) {
        Serial.printf("PZEM Address: 0x%02X\n", addr);
    }

    return pzemOK;
}

void PowerMonitor::update() {
    if (!_isInitialized) {
        Serial.println("Warning: PowerMonitor not properly initialized");
        return;
    }

    uint8_t addr = _pzem0.readAddress();
    _isActive = (addr != 0);

    if (_isActive) {
        readMeasurements();
    }
}

void PowerMonitor::readMeasurements() {
    _voltage = _pzem0.voltage();
    _current = _pzem0.current();
    _power = _pzem0.power();
    _energy = _pzem0.energy();
    _frequency = _pzem0.frequency();
    _powerFactor = _pzem0.pf();

    // Add debug information for measurements
    if (!isnan(_voltage)) {
        Serial.println("\n=== PZEM Measurements ===");
        Serial.printf("Voltage: %.1fV\n", _voltage);
        Serial.printf("Current: %.3fA\n", _current);
        Serial.printf("Power: %.1fW\n", _power);
        Serial.printf("Energy: %.3fkWh\n", _energy);
        Serial.printf("Frequency: %.1fHz\n", _frequency);
        Serial.printf("Power Factor: %.2f\n", _powerFactor);
        Serial.println("========================\n");
    } else {
        Serial.println("Warning: Invalid measurements received from PZEM");
    }
}