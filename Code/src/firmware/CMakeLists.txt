# Code/src/firmware/CMakeLists.txt

# add source files
add_executable(firmware
  main.c
  ssd1306.c
)

# include submodules



# link in libraries (almost every hardware/<blah>.h or pico/<blah>.h is hardware_blah or pico_blah)
target_link_libraries(firmware PRIVATE
    pico_stdlib
    hardware_gpio
    hardware_i2c

    # ioLibrary
)

# enable usb/uart stdio
pico_enable_stdio_usb(firmware 1)
pico_enable_stdio_uart(firmware 0) 






# compiler flags
target_compile_options(firmware PRIVATE 
    $<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>
    -fno-exceptions 
    -fno-check-new 
    $<$<COMPILE_LANGUAGE:CXX>:-fno-enforce-eh-specs>
    -g 
    -Os
    -ffunction-sections 
    -fdata-sections 
    -O3
    -funroll-loops 
    -Werror 
    -Wall
)

# preprocessor macros
target_compile_definitions(firmware PRIVATE 
    PICO_HEAP_SIZE=0
    PICO_XOSC_STARTUP_DELAY_MULTIPLIER=64
)

# select linker script
#pico_set_binary_type(firmware copy_to_ram)


# create map/bin/hex file etc.
pico_add_extra_outputs(firmware)
  

# linker options
add_link_options(-s, -Wl)
target_link_options(firmware
    PRIVATE "LINKER:--print-memory-usage"
)
