cmake_minimum_required(VERSION 3.18)
project(DreamcastWasmCore CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Force aggressive build optimization and optimize mathematical structures
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -flto -fno-rtti -fno-exceptions")

# Configure target flags specifically for asynchronous web workloads
set(EMSCRIPTEN_LINK_FLAGS
    "-s WASM=1"
    "-s INITIAL_MEMORY=67108864" # Allocate 64MB initial base memory array
    "-s ALLOW_MEMORY_GROWTH=0"   # Lock limits to secure native vector pointers
    "-s WASM_BIGINT=1"           # Expose fast native 64-bit integer processing
    "-s EXPORTED_FUNCTIONS=['_init_system','_run_cycles','_update_controllers','_get_vram_ptr']"
    "-s EXPORTED_RUNTIME_METHODS=['ccall','cwrap','WebAssembly']"
)

string(REPLACE ";" " " EMSCRIPTEN_LINK_FLAGS_STR "${EMSCRIPTEN_LINK_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${EMSCRIPTEN_LINK_FLAGS_STR}")
set(CMAKE_EXECUTABLE_SUFFIX ".js")

add_executable(dreamcast_core 
    src/main.cpp
)
