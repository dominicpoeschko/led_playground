cmake_minimum_required(VERSION 3.18)

include(kvasir/cmake/kvasir.cmake)

project("led_playground" VERSION 0.1.0)

include(kvasir/cmake/util.cmake)

pioasm_generate(ws2812Pio INPUT_FILE chip/src/chip/pio/ws2812.pio)

add_executable(debug src/main.cpp)
target_configure_kvasir(debug OPTIMIZATION_STRATEGY size USE_LOG)
target_link_libraries(debug ws2812Pio)

add_executable(release src/main.cpp)
target_configure_kvasir(release OPTIMIZATION_STRATEGY size)
target_link_libraries(release ws2812Pio)

