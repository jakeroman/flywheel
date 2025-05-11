#ifndef FLYWHEEL_LUA_HPP
#define FLYWHEEL_LUA_HPP

// Libraries
extern "C" {
	#include <lua.h>
	#include <lauxlib.h>
	#include <lualib.h>
}
#include "esp_heap_caps.h"


// Flywheel Graphics
#include "graphics.hpp"
extern FlywheelGraphics graphics;

int lua_FlywheelGraphics_clear(lua_State *L) {
    int color = luaL_checkinteger(L, 1);  // First argument: color
    graphics.clear(color);
    return 0;  // No return values
}

int lua_FlywheelGraphics_drawPixel(lua_State *L) {
    int x = luaL_checkinteger(L, 1);  // First argument: x coordinate
    int y = luaL_checkinteger(L, 2);  // Second argument: y coordinate
    int color = luaL_checkinteger(L, 3);  // Third argument: color
    graphics.drawPixel(x, y, color);
    return 0;  // No return values
}

int lua_FlywheelGraphics_drawBitmap(lua_State *L) {
    int x = luaL_checkinteger(L, 1);  // First argument: x coordinate
    int y = luaL_checkinteger(L, 2);  // Second argument: y coordinate
    size_t bitmapSize;
    const uint8_t *bitmap = (const uint8_t *)luaL_checklstring(L, 3, &bitmapSize);  // Third argument: bitmap data
    int w = luaL_checkinteger(L, 4);  // Fourth argument: width
    int h = luaL_checkinteger(L, 5);  // Fifth argument: height
    int color = luaL_checkinteger(L, 6);  // Sixth argument: color
    graphics.drawBitmap(x, y, bitmap, w, h, color);
    return 0;  // No return values
}

int lua_FlywheelGraphics_refresh(lua_State *L) {
    graphics.refresh();
    return 0;  // No return values
}

int lua_FlywheelGraphics_drawText(lua_State *L) {
    int x = luaL_checkinteger(L, 1);  // First argument: x coordinate
    int y = luaL_checkinteger(L, 2);  // Second argument: y coordinate
    const char *text = luaL_checkstring(L, 3);  // Third argument: text
    int size = luaL_checkinteger(L, 4);  // Fourth argument: text size
    int color = luaL_checkinteger(L, 5);  // Fifth argument: text color
    graphics.drawText(x, y, text, size, color);
    return 0;  // No return values
}

int lua_FlywheelGraphics_update(lua_State *L) {
    graphics.update();
    return 0;  // No return values
}

static const luaL_Reg FlywheelGraphicsLib[] = {
    {"clear", lua_FlywheelGraphics_clear},
    {"drawPixel", lua_FlywheelGraphics_drawPixel},
    {"drawBitmap", lua_FlywheelGraphics_drawBitmap},
    {"refresh", lua_FlywheelGraphics_refresh},
    {"drawText", lua_FlywheelGraphics_drawText},
    {"update", lua_FlywheelGraphics_update},
    {NULL, NULL}  // Sentinel to mark the end of the array
};

int luaopen_FlywheelGraphics(lua_State *L) {
    luaL_newlib(L, FlywheelGraphicsLib);  // Create a new Lua table with the functions
    return 1;  // Return the table on the Lua stack
}


// Flywheel SD Card
#include "sd.hpp"
extern FlywheelSD sd;


// Flywheel Input
#include "input.hpp"
extern FlywheelInput input;

int lua_FlywheelInput_checkUp(lua_State *L) {
    lua_pushboolean(L, input.check_up());  // Push the result as a boolean to the Lua stack
    return 1;  // Return 1 value to Lua (the boolean)
}

int lua_FlywheelInput_checkDown(lua_State *L) {
    lua_pushboolean(L, input.check_down());
    return 1;
}

int lua_FlywheelInput_checkLeft(lua_State *L) {
    lua_pushboolean(L, input.check_left());
    return 1;
}

int lua_FlywheelInput_checkRight(lua_State *L) {
    lua_pushboolean(L, input.check_right());
    return 1;
}

int lua_FlywheelInput_checkA(lua_State *L) {
    lua_pushboolean(L, input.check_a());
    return 1;
}

int lua_FlywheelInput_checkB(lua_State *L) {
    lua_pushboolean(L, input.check_b());
    return 1;
}

static const luaL_Reg FlywheelInputLib[] = {
    {"checkUp", lua_FlywheelInput_checkUp},
    {"checkDown", lua_FlywheelInput_checkDown},
    {"checkLeft", lua_FlywheelInput_checkLeft},
    {"checkRight", lua_FlywheelInput_checkRight},
    {"checkA", lua_FlywheelInput_checkA},
    {"checkB", lua_FlywheelInput_checkB},
    {NULL, NULL}  // Sentinel to mark the end of the array
};

int luaopen_FlywheelInput(lua_State *L) {
    luaL_newlib(L, FlywheelInputLib);  // Create a new Lua table with the functions
    return 1;  // Return the table on the Lua stack
}


// Flywheel GB
#include "gb.hpp"
FlywheelGB emulator; // Global FlywheelGB instance

int lua_FlywheelGB_loadROM(lua_State *L) {
    const char* path = luaL_checkstring(L, 1); // Get ROM file path from Lua
    lua_pushstring(L, emulator.load_rom(path).c_str());
    return 1;
}

int lua_FlywheelGB_startEmulator(lua_State *L) {
    if (emulator.start_emulator()) {
        lua_pushboolean(L, 1); // Success
    } else {
        lua_pushboolean(L, 0); // Failure
    }
    return 1; // Return boolean
}

int lua_FlywheelGB_stopEmulator(lua_State *L) {
    emulator.stop_emulator();
    return 0; // No return values
}

int lua_FlywheelGB_getFramebuffer(lua_State *L) {
    const uint8_t* framebuffer = emulator.get_framebuffer();
    if (!framebuffer) {
        lua_pushnil(L); // Push nil if framebuffer is unavailable
        return 1;
    }

    // Create a Lua string containing the framebuffer data
    lua_pushlstring(L, reinterpret_cast<const char*>(framebuffer), 160 * 144);
    return 1; // Return the string
}

int lua_FlywheelGB_drawFramebuffer(lua_State *L) {
    emulator.draw_framebuffer();
    return 0; // No return values
}

static const luaL_Reg FlywheelGBLib[] = {
    {"loadROM", lua_FlywheelGB_loadROM},
    {"startEmulator", lua_FlywheelGB_startEmulator},
    {"stopEmulator", lua_FlywheelGB_stopEmulator},
    {"getFramebuffer", lua_FlywheelGB_getFramebuffer},
    {"drawFramebuffer", lua_FlywheelGB_drawFramebuffer},
    {NULL, NULL} // Sentinel to mark the end of the array
};

int luaopen_FlywheelGB(lua_State *L) {
    luaL_newlib(L, FlywheelGBLib); // Create a new Lua table with the functions
    return 1; // Return the table on the Lua stack
}


// General Lua Passthrough Methods
int lua_sleep(lua_State *L) {
    int duration = luaL_checkinteger(L, 1);  // Get the duration (in milliseconds) from Lua
    delay(duration);  // Sleep for the specified duration
    return 0;  // No return values
}


// Lua Core
lua_State *L; // Global lua state

// Memory Allocation
void* lua_internal_allocator(void* ud, void* ptr, size_t osize, size_t nsize) {
    (void)ud; (void)osize;
    if (nsize == 0) {
        free(ptr);
        return nullptr;
    }
    return realloc(ptr, nsize);
}

void* lua_psram_allocator(void* ud, void* ptr, size_t osize, size_t nsize) {
    (void)ud; (void)osize;

    // Freeing memory
    if (nsize == 0) {
        free(ptr);
        return nullptr;
    }

    // Reallocate memory using the appropriate allocator
    void* new_ptr;

    if (nsize >= 2048 && psramFound()) {
        // Large allocation -> use PSRAM
        new_ptr = heap_caps_realloc(ptr, nsize, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    } else {
        // Small allocation -> use internal RAM
        new_ptr = realloc(ptr, nsize);
    }

    return new_ptr;
}



int lua_load_from_sd(lua_State *L) {
    const char* filename = luaL_checkstring(L, 1);  // Get the module name from the stack
    String filepath = String(filename);
    // Check if ".lua" should be appended
    if (!filepath.endsWith(".lua")) {
        filepath += ".lua";
    }

    // Read file from SD card
    if (sd.exists(filepath.c_str())) {
        String fileData = sd.read_file(filepath.c_str());
        if (luaL_loadstring(L, fileData.c_str()) == LUA_OK) {
            return 1;  // Return the loaded chunk
        } else {
            return luaL_error(L, "Error loading module from SD");
        }
    }

    return luaL_error(L, "File not found: %s", filepath.c_str());
}



// Main Control Methods
bool lua_init_interpreter() {
    // Initialize Lua interpreter
    L = lua_newstate(lua_psram_allocator, NULL);
    if (L == NULL) {
        return false;
    }
    lua_checkstack(L, 64);  // Enough for most scripts but prevents deep recursion
    luaL_openlibs(L);  // Load Lua standard libraries

    // Register FlywheelGraphics library
    luaL_requiref(L, "graphics", luaopen_FlywheelGraphics, 1);
    lua_pop(L, 1);  // Remove library table from stack after registration

    // Register FlywheelInput library
    luaL_requiref(L, "input", luaopen_FlywheelInput, 1);
    lua_pop(L, 1);  // Remove library table from stack after registration

    // Register FlywheelGB library
    luaL_requiref(L, "emulator", luaopen_FlywheelGB, 1);
    lua_pop(L, 1); // Remove library table from stack after registration

    // Register the global sleep function
    lua_pushcfunction(L, lua_sleep);
    lua_setglobal(L, "sleep");  // Make it accessible globally as "sleep"

    // Register custom SD card loader
    lua_getglobal(L, "package");
    lua_getfield(L, -1, "searchers");
    int numLoaders = luaL_len(L, -1);
    lua_pushinteger(L, numLoaders + 1);  // Index for new loader
    lua_pushcfunction(L, lua_load_from_sd);  // Push your custom loader
    lua_settable(L, -3);  // package.searchers[new index] = lua_load_from_sd
    lua_pop(L, 2);  // Clean the stack (package and searchers)

	// Lua init complete!
	return true;
}

void lua_exec(const char *script) {
    // Load and run the Lua script
    if (luaL_loadstring(L, script) || lua_pcall(L, 0, LUA_MULTRET, 0)) {
        // Print the error message from the Lua stack
        Serial.println("Error running Lua script:");
        Serial.println(lua_tostring(L, -1));  
        lua_pop(L, 1);  // Remove the error message from the stack
    }
}

#endif