include(Pico-DMX/interfaceLibForPicoSDK.cmake)
add_subdirectory(pico-ssd1306)

add_library(ioLibrary STATIC
    ioLibrary_Driver/Ethernet/socket.c
    ioLibrary_Driver/Ethernet/W5500/w5500.c
    ioLibrary_Driver/Ethernet/wizchip_conf.c
)

target_include_directories(ioLibrary PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/ioLibrary_Driver/Ethernet
)

target_compile_definitions(ioLibrary PUBLIC
    _WIZCHIP_=W5500
)
