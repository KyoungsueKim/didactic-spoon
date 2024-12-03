//
// Created by Administrator on 2024-12-03.
//

#ifndef DIDACTIC_SPOON_MQTTCLIENT_H
#define DIDACTIC_SPOON_MQTTCLIENT_H

#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

class MQTTClient {
public:
    MQTTClient(const char* wifiSsid, const char* wifiPassword,
               const char* mqttServer, int mqttPort = 1883,
               const char* mqttUser = nullptr, const char* mqttPassword = nullptr);

    bool begin();
    bool publish(const char* topic, JsonDocument& doc);
    void loop();
    bool isConnected() const;

private:
    bool connectToWiFi();
    bool connectToMQTT();
    void setupMQTT();

    const char* _wifiSsid;
    const char* _wifiPassword;
    const char* _mqttServer;
    const char* _mqttUser;
    const char* _mqttPassword;
    int _mqttPort;

    WiFiClient _wifiClient;
    PubSubClient _mqttClient;
    bool _isInitialized;

    static const int MAX_RETRY = 5;
    static const unsigned long RETRY_DELAY = 5000; // 5 seconds
};

#endif //DIDACTIC_SPOON_MQTTCLIENT_H
