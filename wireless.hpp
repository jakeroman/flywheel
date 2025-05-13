#ifndef FLYWHEEL_WIRELESS_HPP
#define FLYWHEEL_WIRELESS_HPP

#include <WiFi.h>

class FlywheelWireless {
public:
    void begin() {
        // Optional: do nothing until explicitly enabled
        disable();
    }

    void enable(const char* ssid, const char* password) {
        WiFi.mode(WIFI_STA);
        WiFi.begin(ssid, password);
        while (WiFi.status() != WL_CONNECTED) {
            delay(100); // Wait for connection
        }
    }

    void disable() {
        WiFi.disconnect(true);   // Disconnect and erase saved network
        WiFi.mode(WIFI_OFF);     // Fully shut down Wi-Fi
        btStop();                // Disable Bluetooth
    }

    bool isConnected() {
        return WiFi.status() == WL_CONNECTED;
    }
};

#endif
