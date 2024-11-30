//
// Created by Administrator on 2024-11-30.
//

#ifndef UNTITLED_POWERMONITOR_H
#define UNTITLED_POWERMONITOR_H

#include <Arduino.h>
#include <SC16IS752Serial.h>
#include <PZEM004Tv30-pms.h>

class PowerMonitor {
public:
    PowerMonitor();

    bool begin();  // Changed return type to bool for error checking
    void update();

    bool isActive() const { return _isActive; }

    float getVoltage() const { return _voltage; }

    float getCurrent() const { return _current; }

    float getPower() const { return _power; }

    float getEnergy() const { return _energy; }

    float getFrequency() const { return _frequency; }

    float getPowerFactor() const { return _powerFactor; }

private:
    bool initializeI2C();

    bool resetSC16IS752();

    bool initializeSerial();

    bool initializePZEM();

    void readMeasurements();

    SC16IS752Serial _serial0;  // Only using channel 0 for PMS01
    PZEM004Tv30 _pzem0;       // Only using one PZEM for PMS01
    bool _isActive;
    bool _isInitialized;

    float _voltage;
    float _current;
    float _power;
    float _energy;
    float _frequency;
    float _powerFactor;

    static const uint8_t SC16IS752_RST_PIN = 15;
    static const uint8_t I2C_SDA_PIN = 23;
    static const uint8_t I2C_SCL_PIN = 22;
};

#endif //UNTITLED_POWERMONITOR_H
