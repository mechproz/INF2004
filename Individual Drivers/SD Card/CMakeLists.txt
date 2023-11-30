#Add Subdirectory (uncomment this if this is your first time adding)
#add_subdirectory(FatFS_SPI2)

add_executable(SD_Card 
    main.c
    hw_config.c
)

# Add the standard library and FatFS/SPI to the build
target_link_libraries(SD_Card  
    pico_stdlib
    FatFs_SPI2
)

#Enable USB IO
pico_enable_stdio_usb(SD_Card  1)

#UF2 File
pico_add_extra_outputs(SD_Card)

