#ifndef FLYWHEEL_GRAPHICS_HPP
#define FLYWHEEL_GRAPHICS_HPP

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SharpMem.h>

// Pin configuration for the Sharp Memory Display
#define SHARP_SCK 36    // Clock pin
#define SHARP_MOSI 35   // MOSI pin
#define SHARP_CS 39     // Chip Select (CS) pin

// Display resolution for the Sharp Memory Display
#define DISPLAY_WIDTH 400
#define DISPLAY_HEIGHT 240

class FlywheelGraphics {
public:
    // Constructor
    FlywheelGraphics()
        : display(&SPI, SHARP_CS, DISPLAY_WIDTH, DISPLAY_HEIGHT, 8000000) { }

    // Initialization method
    void begin() {
        SPI.begin(SHARP_SCK, -1, SHARP_MOSI, SHARP_CS);
        display.begin();
        clear(1); // Clear the display to white
        display.refresh();
    }

    // Clear the display to a specified color (0 = black, 1 = white)
    void clear(uint8_t color) {
        display.fillScreen(color);
    }

    // Draw a pixel at the specified position
    void drawPixel(int16_t x, int16_t y, uint8_t color) {
        display.drawPixel(x, y, color);
    }

    // Draw a bitmap to the display
    void drawBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint8_t color) {
        display.drawBitmap(x, y, bitmap, w, h, color);
    }

    // Refresh the display
    void refresh() {
        display.refresh();
    }

    // Draw text at the specified position
    void drawText(int16_t x, int16_t y, const char *text, uint8_t size, uint8_t color) {
        display.setCursor(x, y);
        display.setTextSize(size);
        display.setTextColor(color);
        display.print(text);
    }

    // Update (optional if you want periodic updates)
    void update() {
        refresh();
    }

private:
    Adafruit_SharpMem display;
};

#endif
