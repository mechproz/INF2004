#include "lwip/apps/httpd.h"
#include "pico/cyw43_arch.h"

extern char user_input_buffer[100];
extern char port_range_buffer1[100];
extern char port_range_buffer2[100];

// CGI handler which is run when a request for /led.cgi is detected
const char * cgi_led_handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[])
{
    // Debugging: Log the number of parameters
    printf("CGI Handler called with %d parameters.\r\n", iNumParams);

    int handledIP = 0;
    int handledPortRange = 0;

    for (int i = 0; i < iNumParams; i++) {
        if (strcmp(pcParam[i], "ip") == 0) {
            // Copy the IP address input into the user_input_buffer
            snprintf(user_input_buffer, sizeof(user_input_buffer), "%s", pcValue[i]);
            printf("Received IP Address: %s \r\n", pcValue[i]); // For debugging
            handledIP = 1;
        } else if (strcmp(pcParam[i], "portRange1") == 0) {
            // Copy the port range input into the port_range_buffer
            snprintf(port_range_buffer1, sizeof(port_range_buffer1), "%s", pcValue[i]);
            printf("Received Port Range1: %s \r\n", pcValue[i]); // For debugging
            handledPortRange = 1;
        }
        else if (strcmp(pcParam[i], "portRange2") == 0) {
            // Copy the port range input into the port_range_buffer
            snprintf(port_range_buffer2, sizeof(port_range_buffer2), "%s", pcValue[i]);
            printf("Received Port Range2: %s \r\n", pcValue[i]); // For debugging
            handledPortRange = 1;
        }
    }

    if (!handledIP) {
        printf("No valid 'ip' parameter found.\r\n");
    }

    if (!handledPortRange) {
        printf("No valid 'portRange' parameter found.\r\n");
    }

    // // Check if a request for user input has been made
    // if (iNumParams > 0 && strcmp(pcParam[0], "ip") == 0) {
    //     // Copy the user input into the user_input_buffer
    //     snprintf(user_input_buffer, sizeof(user_input_buffer), "%s", pcValue[0]);
    //     printf("Received User Input: %s \r\n", user_input_buffer); // For debugging
    // } else {
    //     printf("No valid 'ip' parameter found.\r\n");
    // }
    
    // // Check if an request for LED has been made (/led.cgi?led=x)
    // if (strcmp(pcParam[0] , "ip") == 0 && iNumParams > 0){
    //     // Copy the user input into the user_input_buffer
    //     snprintf(user_input_buffer, sizeof(user_input_buffer), "%s", pcValue[0]);
    //     // printf("Received User Input: %s \r\n", pcValue[0]);
    //     printf("Received User Input: %s \r\n", user_input_buffer);
    
    // }
    
    // Send the index page back to the use2r
    return "/index.shtml";
}

// tCGI Struct
// Fill this with all of the CGI requests and their respective handlers
static const tCGI cgi_handlers[] = {
    {
        // Html request for "/led.cgi" triggers cgi_handler
        "/index.cgi", cgi_led_handler
    },
};

void cgi_init(void)
{
    http_set_cgi_handlers(cgi_handlers, 1);
}
