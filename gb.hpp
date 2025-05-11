#ifndef FLYWHEEL_GB_HPP
#define FLYWHEEL_GB_HPP

#include <peanut_gb.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp32-hal-psram.h> // Safer PSRAM helper API for Arduino

#include "sd.hpp"
#include "graphics.hpp"

extern FlywheelSD sd;

class FlywheelGB {
private:
    struct gb_s gb;                  // PeanutGB state
    uint8_t* romBuffer = nullptr;    // Pointer to ROM buffer
    size_t romSize = 0;              // Size of the loaded ROM
    bool emulatorRunning = false;    // Status of the emulator
    TaskHandle_t emulatorTaskHandle = nullptr; // Task handle for the emulator

    static constexpr uint32_t FRAME_DURATION_MS = 1000 / 60; // 60 FPS
    uint8_t framebuffer[160 * 144]; // Framebuffer for the Game Boy display (160x144 pixels)

    static FlywheelGB* instance; // Static instance pointer

    // Task to run the emulator loop
    static void emulator_task(void* parameter) {
        FlywheelGB* inst = static_cast<FlywheelGB*>(parameter);
        while (inst->emulatorRunning) {
            uint32_t start = millis();

            inst->run_frame();

            uint32_t frameTime = millis() - start;
            if (frameTime < FRAME_DURATION_MS) {
                vTaskDelay(pdMS_TO_TICKS(FRAME_DURATION_MS - frameTime));
            }
        }
        vTaskDelete(nullptr);
    }

    // Run a single frame of the emulator
    void run_frame() {
        gb_run_frame(&gb);
    }

    // PeanutGB callbacks
    static uint8_t gb_rom_read(struct gb_s* gb, uint_fast32_t addr) {
        if (instance && instance->romBuffer && addr < instance->romSize) {
            return instance->romBuffer[addr];
        }
        return 0xFF;
    }

    static uint8_t gb_ram_read(struct gb_s* gb, uint_fast32_t addr) {
        // No crash here since it doesn't access memory — just keep as-is or add checks if RAM support is added later
        return 0xFF;
    }

    static void gb_ram_write(struct gb_s* gb, uint_fast32_t addr, uint8_t val) {
        // Same as above
    }

    static void gb_error(struct gb_s* gb, enum gb_error_e gb_err, uint16_t val) {
        if (instance) {
            Serial.printf("PeanutGB Error: %d (Value: %d)\n", gb_err, val);
        }
    }

    // Custom draw line
    static void custom_draw_line(struct gb_s* gb, const uint8_t* pixels, const uint_fast8_t line) {
        if (!instance || !pixels || line >= 144) return;

        uint8_t* framebufferLine = &instance->framebuffer[line * 160];
        for (int x = 0; x < 160; x++) {
            framebufferLine[x] = pixels[x] & 0x03;
        }
    }


public:
    // Load ROM from SD card
    String load_rom(const char* path) {
        if (!sd.is_initialized()) {
            return "SD card not initialized.";
        }

        romSize = sd.get_file_size(path);
        if (romSize == 0) {
            return "Failed to get ROM size or file not found.";
        }
        if (romSize > (2 * 1024 * 1024)) {
            return "Invalid ROM size.";
        }


        if (romBuffer) {
            free(romBuffer);
        }

        if (!psramFound()) {
            return "PSRAM not available.";
        }

        romBuffer = static_cast<uint8_t*>(ps_malloc(romSize));
        if (!romBuffer) {
            return "Failed to allocate memory for ROM.";
        }

        size_t bytesRead;
        if (!sd.read_binary_file(path, romBuffer, romSize, bytesRead) || bytesRead != romSize) {
            free(romBuffer);
            romBuffer = nullptr;
            romSize = 0;
            return "Failed to load ROM from SD card.";
        }

        return "success";
    }

    // Start the emulator in a separate task
    bool start_emulator() {
        if (emulatorRunning) {
            Serial.println("Emulator already running.");
            return false;
        }

        if (!romBuffer || romSize == 0) {
            Serial.println("No ROM loaded. Please load a ROM before starting the emulator.");
            return false;
        }

        instance = this; // 🔁 moved up for safety

        gb_init_error_e init_result = gb_init(&gb, gb_rom_read, gb_ram_read, gb_ram_write, gb_error, nullptr);
        if (init_result != GB_INIT_NO_ERROR) {
            Serial.println("Failed to initialize PeanutGB.");
            instance = nullptr;
            return false;
        }

        gb.display.lcd_draw_line = custom_draw_line;

        emulatorRunning = true;

        // 🔁 Increase stack size from 8192 → 16384
        xTaskCreatePinnedToCore(emulator_task, "EmulatorTask", 16384, this, 1, &emulatorTaskHandle, 0);
        Serial.println("Emulator started.");
        return true;
    }


    // Stop the emulator and clean up
    void stop_emulator() {
        if (!emulatorRunning) {
            Serial.println("Emulator is not running.");
            return;
        }

        emulatorRunning = false;

        // Wait for the task to terminate
        if (emulatorTaskHandle) {
            vTaskDelete(emulatorTaskHandle);
            emulatorTaskHandle = nullptr;
        }

        instance = nullptr; // Clear the static pointer
        Serial.println("Emulator stopped.");
    }

    // Destructor to clean up resources
    ~FlywheelGB() {
        stop_emulator();
        if (romBuffer) {
            free(romBuffer);
            romBuffer = nullptr;
        }
    }

    // Retrieve the framebuffer for rendering
    const uint8_t* get_framebuffer() const {
        return framebuffer;
    }

    // Draw framebuffer to screen
    void draw_framebuffer() {
        const float scale = 1.66f;
        const int src_w = 160;
        const int src_h = 144;
        const int dst_w = int(src_w * scale);  // 266
        const int dst_h = int(src_h * scale);  // 239
        const int screen_w = 400;
        const int screen_h = 240;
        const int offset_x = (screen_w - dst_w) / 2;
        const int offset_y = (screen_h - dst_h) / 2;

        for (int y = 0; y < dst_h; ++y) {
            int src_y = y / scale;
            for (int x = 0; x < dst_w; ++x) {
                int src_x = x / scale;
                uint8_t pixel = framebuffer[src_y * src_w + src_x] & 0x03;

                uint8_t mono = (pixel > 2) ? 0 : 1;

                graphics.drawPixel(offset_x + x, offset_y + y, mono);
            }
        }

        graphics.refresh();
    }

};

// Define the static instance pointer
FlywheelGB* FlywheelGB::instance = nullptr;

#endif
