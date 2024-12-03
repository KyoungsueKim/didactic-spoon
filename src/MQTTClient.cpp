//
// Created by Administrator on 2024-12-03.
//

#include <ArduinoJson.h>
#include "MQTTClient.h"


MQTTClient::MQTTClient(const char* wifiSsid, const char* wifiPassword,
                       const char* mqttServer, int mqttPort,
                       const char* mqttUser, const char* mqttPassword)
        : _wifiSsid(wifiSsid)
        , _wifiPassword(wifiPassword)
        , _mqttServer(mqttServer)
        , _mqttPort(mqttPort)
        , _mqttUser(mqttUser)
        , _mqttPassword(mqttPassword)
        , _mqttClient(_wifiClient)
        , _isInitialized(false)
{

}

bool MQTTClient::begin() {
    if (!connectToWiFi()) {
        Serial.println("Failed to connect to WiFi");
        return false;
    }

    setupMQTT();

    if (!connectToMQTT()) {
        Serial.println("Failed to connect to MQTT");
        return false;
    }

    _isInitialized = true;
    return true;
}

bool MQTTClient::connectToWiFi() {
    Serial.print("Connecting to WiFi");
    WiFi.begin(_wifiSsid, _wifiPassword);

    int retry = 0;
    while (WiFi.status() != WL_CONNECTED && retry < MAX_RETRY) {
        delay(1000);
        Serial.print(".");
        retry++;
    }
    Serial.println();

    if (WiFi.status() == WL_CONNECTED) {
        Serial.print("Connected to WiFi. IP: ");
        Serial.println(WiFi.localIP());
        return true;
    }
    return false;
}

void MQTTClient::setupMQTT() {
    _mqttClient.setServer(_mqttServer, _mqttPort);
}

bool MQTTClient::connectToMQTT() {
    Serial.print("Connecting to MQTT");

    int retry = 0;
    while (!_mqttClient.connected() && retry < MAX_RETRY) {
        if (_mqttUser != nullptr) {
            if (_mqttClient.connect("PMS01Client", _mqttUser, _mqttPassword)) {
                break;
            }
        } else {
            if (_mqttClient.connect("PMS01Client")) {
                break;
            }
        }

        Serial.print(".");
        delay(RETRY_DELAY);
        retry++;
    }
    Serial.println();

    if (_mqttClient.connected()) {
        Serial.println("Connected to MQTT server");
        return true;
    }

    Serial.println("Failed to connect to MQTT");
    return false;
}

bool MQTTClient::publish(const char* topic, JsonDocument& doc) {
    if (!_isInitialized) return false;

    String jsonString;
    serializeJson(doc, jsonString);

    return _mqttClient.publish(topic, jsonString.c_str());
}

void MQTTClient::loop() {
    if (!_isInitialized) return;

    if (!_mqttClient.connected()) {
        if (!connectToMQTT()) {
            delay(RETRY_DELAY);
            return;
        }
    }

    _mqttClient.loop();
}

bool MQTTClient::isConnected() const {
    return _isInitialized && _mqttClient.connected();
}