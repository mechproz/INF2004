// Set up standard IO and initializes the CYW43 Wi-Fi driver for the Pico W
// Try to connect to the wi-fi nextwork using provided credentials
// Once connected, it prints the assigned IP address

#include "lwip/apps/httpd.h"
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "lwipopts.h"
#include "ssi.h"
#include "cgi.h"
#include "lwip/dhcp.h"
#include "lwip/prot/dhcp.h"


// Define the user input buffer
char user_input_buffer[100] = {0};
char port_range_buffer1[100] = {0};
char port_range_buffer2[100] = {0};

// Last known values of the variables for comparison
char last_ip[100] = {0};
char last_port1[100] = {0};
char last_port2[100] = {0};

//char user_input_buffer[100] = {0}; // Initialize with zeros

// WIFI Credentials, change to own wifi name and password
const char WIFI_SSID[] = "QQ";
const char WIFI_PASSWORD[] = "cheryl1234";

// If use hotspot, need to turn on "Maximise Compatability" -- On Iphone
// Makes the hotspot use the 2.4 GHz Wi-Fi Band, which is more compatible with a wider range of devices.

int main() {
    stdio_init_all();

    cyw43_arch_init();

    cyw43_arch_enable_sta_mode();

    int connection_attempts = 0;
    

    // Connect to the WiFI network - loop until connected
    while(cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 30000) != 0){
        printf("Attempt %d: Unable to connect to Wi-Fi network.\n", ++connection_attempts);
        if (connection_attempts >= 5) {
            printf("Failed to connect after %d attempts. Please check your SSID and PASSWORD. \n", connection_attempts);

        }
    }
    // Print a success message once connected
    printf("Connected! \n");

    // Fetch and print the IP address
    if (netif_default != NULL) {
        printf("IP Address: %s\n", ip4addr_ntoa(&netif_default->ip_addr));
    }
    
    // Initialise web server
    httpd_init();
    printf("Http server initialised\n");

    // Configure SSI and CGI handler
    ssi_init(); 
    printf("SSI Handler initialised\n");

    cgi_init();
    printf("CGI Handler initialised\n");
    

    // Infinite loop
    while(1) {
        tight_loop_contents();
        // Check if the global variables have been updated
        if (strcmp(last_ip, user_input_buffer) != 0 || strcmp(last_port1, port_range_buffer1) != 0 || strcmp(last_port2, port_range_buffer2) != 0) {
            printf("New Data Received:\n");
            printf("IP Address: %s\n", user_input_buffer);
            printf("Port Range: %s - %s\n", port_range_buffer1, port_range_buffer2);

            // Update the last known values
            strncpy(last_ip, user_input_buffer, sizeof(last_ip));
            strncpy(last_port1, port_range_buffer1, sizeof(last_port1));
            strncpy(last_port2, port_range_buffer2, sizeof(last_port2));
        }

        // Add a small delay to prevent flooding the console
        sleep_ms(1000);
    }
}
