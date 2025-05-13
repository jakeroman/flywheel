#define CONFIG_ESP32S3_SPIRAM_SUPPORT 1
#define CONFIG_SPIRAM_SPEED_40M 1

#include "moon.hpp"
#include "graphics.hpp"
#include "sd.hpp"
#include "input.hpp"
#include "wireless.hpp"
#include <esp_heap_caps.h>

FlywheelGraphics graphics;
FlywheelSD sd;
FlywheelInput input;
FlywheelWireless wireless;

void setup() {
  psramInit();

  // Config power pins
  analogReadResolution(12); // Optional: sets ADC resolution (ESP32 default is 12 bits)
  analogSetPinAttenuation(9, ADC_11db); // Critical!
  pinMode(9, INPUT);   // Stored power ADC pin
  pinMode(3, INPUT);   // Positive charge input ADC pin
  pinMode(10, INPUT);  // Negative charge input ADC pin (or however you're measuring discharge)

  // Setup input
  input.begin(); // configure inputs
  if (input.check_a()) {
    while (true) {
      delay(100);
    }
  }
  delay(1000);

  // display splash screen
	graphics.begin();
	graphics.clear(1);
	graphics.drawText(150, 100, "Flywheel", 2, 0);
	graphics.refresh();
  delay(500);

  // prepare wifi stack & sleep
  wireless.begin();

  // setup lua interpreter
	lua_init_interpreter();
  graphics.drawText(100, 10, "Lua initted", 2, 0);
  graphics.refresh();
  delay(200);

  // load and execute init file
	sd.begin();
	String init_file = sd.read_file("init.lua");
  graphics.drawText(100, 170, "SD Begun and loaded", 2, 0);
  graphics.refresh();
  delay(200);
	lua_exec(init_file.c_str());
}

void loop() {
	delay(100);
}