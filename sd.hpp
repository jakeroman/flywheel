#ifndef FLYWHEEL_SD_HPP
#define FLYWHEEL_SD_HPP

#include <SPI.h>
#include <SdFat.h>

// SD card pins (adjusted for your wiring)
#define SD_CS 10    // Chip Select
#define SD_SCK 12   // Clock
#define SD_MISO 13  // Data In (Master In, Slave Out)
#define SD_MOSI 11  // Data Out (Master Out, Slave In)

// Create custom SPI object for the SD card
SPIClass sdSPI(HSPI); // Use HSPI for the SD card

class FlywheelSD {
private:
    SdFat sd;
    bool initialized = false;

public:
    // Initialize the SD card
    bool begin() {
        sdSPI.begin(SD_SCK, SD_MISO, SD_MOSI, SD_CS);
        SdSpiConfig spiConfig(SD_CS, SHARED_SPI, SD_SCK_MHZ(25), &sdSPI);
        initialized = sd.begin(spiConfig);
        return initialized;
    }

    // Check if the SD card is initialized
    bool is_initialized() const {
        return initialized;
    }

    // Check if a file or directory exists
    bool exists(const char* path) {
        if (!initialized) {
            Serial.println("SD card not initialized");
            return false;
        }
        return sd.exists(path);
    }

    // Read a file from the SD card
    String read_file(const char* filePath) {
        if (!initialized) return "SD card not initialized";

        File file = sd.open(filePath, O_READ);
        if (!file) {
            return "Failed to open file";
        }

        String content;
        while (file.available()) {
            content += (char)file.read();
        }
        file.close();
        return content;
    }

    // Write data to a file on the SD card
    bool write_file(const char* filePath, const char* data) {
        if (!initialized) return false;

        File file = sd.open(filePath, O_WRITE | O_CREAT);
        if (!file) {
            return false;
        }

        file.print(data);
        file.close();
        return true;
    }

    // List files and directories in a given path
    void list_directory(const char* dirPath) {
        if (!initialized) {
            Serial.println("SD card not initialized");
            return;
        }

        File dir = sd.open(dirPath);
        if (!dir || !dir.isDir()) {
            Serial.println("Failed to open directory");
            return;
        }

        File file;
        while ((file = dir.openNextFile())) {
            if (file.isDir()) {
                Serial.print("DIR : ");
            } else {
                Serial.print("FILE: ");
            }
            Serial.println(file.name());
            file.close();
        }
    }
};

#endif
