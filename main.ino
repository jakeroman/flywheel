#define CONFIG_ESP32S3_SPIRAM_SUPPORT 1
#define CONFIG_SPIRAM_SPEED_40M 1

#include "moon.hpp"
#include "graphics.hpp"
#include "sd.hpp"
#include "input.hpp"
#include <esp_heap_caps.h> // Required for PSRAM functions

FlywheelGraphics graphics;
FlywheelSD sd;
FlywheelInput input;

void setup() {
  input.begin(); // configure inputs
  delay(1000);

  // display splash screen
	graphics.begin();
	graphics.clear(1);
	graphics.drawText(150, 100, "Flywheel", 2, 0);
  if (psramInit()) {
	  graphics.drawText(100, 140, "Loaded PSRAM", 2, 0);
  }
  else {
	  graphics.drawText(50, 140, "No PSRAM Detected", 2, 0);
  }
	graphics.refresh();
  delay(500);

  size_t free_psram = heap_caps_get_free_size(MALLOC_CAP_SPIRAM);
  char buffer[50];  // Buffer to hold the free PSRAM message
  snprintf(buffer, sizeof(buffer), "Free PSRAM: %u bytes", free_psram);
  graphics.drawText(100, 40, buffer, 2, 0);
  graphics.refresh();
  delay(200);

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