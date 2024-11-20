#include "moon.hpp"
#include "graphics.hpp"
#include "sd.hpp"

FlywheelGraphics graphics;
FlywheelSD sd;

void setup() {
  // display spalsh screen
	graphics.begin();
	graphics.clear(1);
	graphics.drawText(150, 100, "Flywheel", 2, 0);
	graphics.refresh();

  // setup lua interpreter
	lua_init_interpreter();

  // load and execute init file
	sd.begin();
	String init_file = sd.read_file("init.lua");
	lua_exec(init_file.c_str());
}

void loop() {
	delay(100);
}