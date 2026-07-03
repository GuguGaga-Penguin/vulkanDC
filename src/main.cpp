#include <emscripten/emscripten.h>
#include <cstdint>
#include <cstring>

// Hard allocations representing exact internal physical hardware bounds
alignas(16) uint8_t system_ram[16 * 1024 * 1024]; // 16 MB Main RAM
alignas(16) uint8_t video_ram[8 * 1024 * 1024];  // 8 MB VRAM (PVR Chip)

uint32_t controller_state = 0;
uint32_t program_counter = 0;

extern "C" {

    EMSCRIPTEN_KEEPALIVE
    void init_system() {
        std::memset(system_ram, 0, sizeof(system_ram));
        std::memset(video_ram, 0, sizeof(video_ram));
        controller_state = 0;
        program_counter = 0x00000000; // Point to start of Virtual System ROM
    }

    EMSCRIPTEN_KEEPALIVE
    uint8_t* get_vram_ptr() {
        return video_ram; // Expose native buffer pointer up to WebGPU
    }

    EMSCRIPTEN_KEEPALIVE
    void update_controllers(uint32_t button_bits) {
        controller_state = button_bits;
    }

    EMSCRIPTEN_KEEPALIVE
    void run_cycles(int cycle_count) {
        int cycles_spent = 0;
        while (cycles_spent < cycle_count) {
            // Emulate SH-4 CPU Fetch, Decode, and Execution pipelines
            // Write visual test markers inside virtual VRAM space for the renderer
            video_ram[cycles_spent % (8 * 1024 * 1024)] = static_cast<uint8_t>(controller_state & 0xFF);
            cycles_spent += 4; // Assume a generic base block cycle footprint
        }
    }
}
