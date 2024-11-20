#ifndef FLYWHEEL_LUA_HPP
#define FLYWHEEL_LUA_HPP

// Libraries
extern "C" {
	#include <lua.h>
	#include <lauxlib.h>
	#include <lualib.h>
}
#include <esp_heap_caps.h>


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


// Lua Supporters
lua_State *L; // Global lua state
void *lua_psram_allocator(void *ud, void *ptr, size_t osize, size_t nsize) {
    (void)ud;  // Unused userdata parameter
    (void)osize;  // Original size, not used here

    // Debug: Log the allocation request
    Serial.printf("Lua allocator: ptr=%p, osize=%zu, nsize=%zu\n", ptr, osize, nsize);

    // Free memory if nsize is 0
    if (nsize == 0) {
        if (ptr != NULL) {
            heap_caps_free(ptr);
            Serial.println("Lua allocator: memory freed.");
        }
        return NULL;
    }

    // Allocate or reallocate memory
    void *new_ptr = NULL;

    if (ptr == NULL) {
        // Allocate new memory in PSRAM
        new_ptr = heap_caps_malloc(nsize, MALLOC_CAP_SPIRAM);
    } else {
        // Reallocate existing memory in PSRAM
        new_ptr = heap_caps_realloc(ptr, nsize, MALLOC_CAP_SPIRAM);
    }

    // Fallback to SRAM if PSRAM allocation fails
    if (new_ptr == NULL) {
        Serial.println("Lua allocator: PSRAM allocation failed, falling back to SRAM.");
        if (ptr == NULL) {
            new_ptr = heap_caps_malloc(nsize, MALLOC_CAP_8BIT);  // Allocate in SRAM
        } else {
            new_ptr = heap_caps_realloc(ptr, nsize, MALLOC_CAP_8BIT);  // Reallocate in SRAM
        }
    }

    // Check if allocation succeeded
    if (new_ptr == NULL) {
        Serial.printf("Lua allocator: Allocation failed. Requested size: %zu bytes.\n", nsize);
    } else {
        Serial.printf("Lua allocator: Memory allocated at %p (size: %zu bytes).\n", new_ptr, nsize);
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
    luaL_openlibs(L);  // Load Lua standard libraries

    // Register FlywheelGraphics library
    luaL_requiref(L, "graphics", luaopen_FlywheelGraphics, 1);
    lua_pop(L, 1);  // Remove library table from stack after registration

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