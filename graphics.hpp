#ifndef FLYWHEEL_GRAPHICS_HPP
#define FLYWHEEL_GRAPHICS_HPP

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SharpMem.h>

// Pin configuration for the Sharp Memory Display
#define SHARP_SCK 18
#define SHARP_MOSI 17
#define SHARP_CS 8

#define DISPLAY_WIDTH 400
#define DISPLAY_HEIGHT 240

class FlywheelGraphics {
public:
    FlywheelGraphics(): display(&SPI, SHARP_CS, DISPLAY_WIDTH, DISPLAY_HEIGHT, 8000000) {}

    void begin() {
        SPI.begin(SHARP_SCK, -1, SHARP_MOSI, SHARP_CS);
        display.begin();
        clear(1);
        display.refresh();

        // Optional: log framebuffer address
        Serial.printf("📺 SharpMem initialized. Display buffer likely in internal RAM.\n");
    }

    void clear(uint8_t color) {
        display.fillScreen(color);
    }

    void drawPixel(int16_t x, int16_t y, uint8_t color) {
        display.drawPixel(x, y, color);
    }

    void drawBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint8_t color) {
        display.drawBitmap(x, y, bitmap, w, h, color);
    }

    void refresh() {
        display.refresh();
    }

    void drawText(int16_t x, int16_t y, const char *text, uint8_t size, uint8_t color) {
        display.setCursor(x, y);
        display.setTextSize(size);
        display.setTextColor(color);
        display.print(text);
    }

    void update() {
        refresh();
    }

private:
    Adafruit_SharpMem display;
};

#endif
