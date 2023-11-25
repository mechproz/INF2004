include(FreeRTOS_Kernel_import.cmake)
include(pico_sdk_import.cmake)

add_subdirectory(FatFs_SPI2)

message("Running makefsdata python script")
execute_process(COMMAND
    py makefsdata.py
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
)

add_executable(nmap-lite
    proj.c
    hw_config.c
    ${PICO_LWIP_CONTRIB_PATH}/apps/socket_examples/socket_examples.c
)
target_compile_definitions(nmap-lite PRIVATE
    WIFI_SSID=\"${WIFI_SSID}\"
    WIFI_PASSWORD=\"${WIFI_PASSWORD}\"
    NO_SYS=0            # don't want NO_SYS (generally this would be in your lwipopts.h)
    LWIP_SOCKET=1       # we need the socket API (generally this would be in your lwipopts.h)
    PING_USE_SOCKETS=1
)
target_include_directories(nmap-lite PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/../.. # for our common lwipopts
    ${PICO_LWIP_CONTRIB_PATH}/apps/socket_examples
)
target_link_libraries(nmap-lite
    pico_cyw43_arch_lwip_sys_freertos
    pico_stdlib
    pico_lwip_iperf
    FreeRTOS-Kernel-Heap4 # FreeRTOS kernel and dynamic heap
    pico_lwip_http
    FatFs_SPI2
)
pico_enable_stdio_usb(nmap-lite 1)

pico_add_extra_outputs(nmap-lite)