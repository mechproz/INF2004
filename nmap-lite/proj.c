#include "FreeRTOS.h"
#include "task.h"
#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"
#include "lwip/tcp.h"
#include "lwip/udp.h"
#include "lwip/init.h"
#include "lwip/ip_addr.h"
#include "ssi.h"
#include "cgi.h"
#include "lwip/apps/httpd.h"
#include "semphr.h"
#include "ff.h"
#include <stdio.h>

// 18.142.229.102
#define MY_STACK_SIZE 2000
#define THREAD_COUNT 4 // Number of scanning threads

#define MAX_OPEN_PORTS 100 // Define the maximum number of open ports to 

u16_t open_ports[MAX_OPEN_PORTS]; // Array to store open ports
int open_ports_count = 0; // Counter for open ports

int completedTasks = 0;
SemaphoreHandle_t completedTasksMutex;

// Define the user input buffer
char user_input_buffer[100] = {0};
char port_range_buffer1[100] = {0};
char port_range_buffer2[100] = {0};

// Last known values of the variables for comparison
char last_ip[100] = {0};
char last_port1[100] = {0};
char last_port2[100] = {0};

int flag = 0;

// Structure to hold scanning parameters
typedef struct {
    ip_addr_t target_ip;
    u16_t start_port;
    u16_t end_port;
    int thread_id; // Identifier for the thread
} ScanParams;

void closeFile(FIL *file) {
    FRESULT fr = f_close(file);
    if (fr != FR_OK) {
        printf("ERROR: Could not close file (%d)\r\n", fr);
    } else {
        printf("Unmounted\n");
    }
}

void SD_init() {
    // Sleep for 10 seconds before the program starts
    printf("\r\nProg Started. Wait for 10 seconds\r\n");
    sleep_ms(10000);
    printf("\r\nMain Prog Starting\r\n");

    // Initialize SD card
    if (!sd_init_driver()) {
        printf("ERROR: Could not initialize SD card\r\n");
        while (true);
    }
}

void mountFile(FATFS *fs) {
    // Mount drive
    FRESULT fr;
    fr = f_mount(fs, "0:", 1);
    if (fr != FR_OK) {
        printf("ERROR: Could not mount filesystem (%d)\r\n", fr);
        while (true);
    }
}

// Callback function for connection result
static err_t tcp_result_cb(void *arg, struct tcp_pcb *tpcb, err_t err) {
    u16_t port = (u16_t)(uintptr_t)arg;

    if (err == ERR_OK) {
        printf("Port %u is open\n", port);

        // Store the open port into the array if space is available
        if (open_ports_count < MAX_OPEN_PORTS) {
            open_ports[open_ports_count] = port;
            open_ports_count++;

        } else {
            printf("Reached maximum open ports limit\n");
            // You might want to handle this condition based on your requirements
        }

        return ERR_OK;
    } else {
        printf("Port %u is closed or filtered\n", port);
        return ERR_TIMEOUT;
    }
}

// TCP Scan Task
void tcp_scan_task(void *params) {
    ScanParams *scanParams = (ScanParams *)params;

    for(u16_t port = scanParams->start_port; port <= scanParams->end_port; port++) {
        struct tcp_pcb *pcb = tcp_new();
        if (pcb == NULL) {
            continue;
        }

        err_t err = tcp_bind(pcb, IP_ADDR_ANY, 0);
        if (err != ERR_OK) {
            tcp_abort(pcb);
            continue;
        }

        tcp_arg(pcb, (void*)(uintptr_t)port);
        printf("Thread %d: Trying Port %d\n", scanParams->thread_id, port);
        err = tcp_connect(pcb, &scanParams->target_ip, port, tcp_result_cb);
        vTaskDelay(pdMS_TO_TICKS(100));
        if (err != ERR_OK) {
            tcp_abort(pcb);
        } else {
            tcp_abort(pcb);
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    vTaskDelay(pdMS_TO_TICKS(2000));
    vPortFree(scanParams);
    // Increment the completed task counter
    xSemaphoreTake(completedTasksMutex, portMAX_DELAY);
    completedTasks++;
    
    
    if (completedTasks == THREAD_COUNT) {
        printf("All scanning tasks done\n");
        FIL file;
        const char filename[] = "ip.txt";
        
        FRESULT fr = f_open(&file, filename, FA_WRITE | FA_OPEN_ALWAYS);
        if (fr != FR_OK) {
            printf("ERROR: Could not open file for writing (%d)\r\n", fr);
            return;
        }

        fr = f_lseek(&file, f_size(&file));
        if (fr != FR_OK) {
            printf("ERROR: Could not move to the end of the file (%d)\r\n", fr);
            closeFile(&file);
            return;
        }

        int ret = 0;
        completedTasks = 0; // Reset the counter for next time
        // All tasks have completed, print the open ports directly
        if (open_ports_count == 0) {
            printf("No open ports\n");
        }else {
            ret = f_printf(&file, "%s: ", user_input_buffer);
            printf("Open ports: \n");
            for (int i = 0; i < open_ports_count; i++) {
                printf("%u ", open_ports[i]);
                if(i == 0){
                    ret = f_printf(&file, "%u", open_ports[0]);
                }else{
                    ret = f_printf(&file, ",%u", open_ports[i]);
                }
            }
            printf("\n");
            open_ports_count = 0;
        }
        ret = f_printf(&file, "\n");
        if (ret < 0) {
            printf("ERROR: Could not write to file (%d)\r\n", ret);
        } else {
            printf("File written Successfully\n");
        }
        
        fr = f_close(&file);
        if (fr != FR_OK) {
            printf("ERROR: Could not close file (%d)\r\n", fr);
        }
    }

    xSemaphoreGive(completedTasksMutex);
    vTaskDelete(NULL);
}


// Main Task
void main_task(void *params) {
    stdio_init_all();

    if (cyw43_arch_init()) {
        printf("Failed to initialize\n");
        vTaskDelete(NULL);
    }

    FATFS fs;

    SD_init();
    mountFile(&fs);

    cyw43_arch_enable_sta_mode();
    printf("Connecting to Wi-Fi...\n");
    if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 30000)) {
        printf("Failed to connect.\n");
        vTaskDelete(NULL);
    } else {
        printf("Connected.\n");
    }

    // Fetch and print the IP address
    if (netif_default != NULL) {
        printf("IP Address: %s\n", ip4addr_ntoa(&netif_default->ip_addr));
    }

    
    
    // Webserver stuff
    httpd_init();
    printf("Http server initialised\n");
    ssi_init(); 
    printf("SSI Handler initialised\n");
    cgi_init();
    printf("CGI Handler initialised\n");

    while (1) {
        // Check if the global variables have been updated
        if (strcmp(last_ip, user_input_buffer) != 0 || strcmp(last_port1, port_range_buffer1) != 0 || strcmp(last_port2, port_range_buffer2) != 0) {
            printf("New Data Received:\n");
            printf("IP Address: %s\n", user_input_buffer);
            printf("Port Range: %s - %s\n", port_range_buffer1, port_range_buffer2);

            // Update the last known values
            strncpy(last_ip, user_input_buffer, sizeof(last_ip));
            strncpy(last_port1, port_range_buffer1, sizeof(last_port1));
            strncpy(last_port2, port_range_buffer2, sizeof(last_port2));
            flag = 1;
        }

        if (flag == 1) {
            flag = 0;
            int start_port_int = atoi(last_port1);
            int end_port_int = atoi(last_port2);
            // Calculate port range per thread
            int totalPorts = end_port_int - start_port_int + 1;
            int basePortRange = totalPorts / THREAD_COUNT;

            for (int i = 0; i < THREAD_COUNT; i++) {
                // Dynamically allocate a ScanParams structure for each task
                ScanParams *scanParams = pvPortMalloc(sizeof(ScanParams));
                if (scanParams != NULL) {
                    ip4addr_aton(last_ip, &scanParams->target_ip);
                    scanParams->start_port = start_port_int + (i * basePortRange);
                    
                    if (i < THREAD_COUNT - 1) {
                        // Distribute base range to all but the last thread
                        scanParams->end_port = scanParams->start_port + basePortRange - 1;
                    } else {
                        // Assign any remaining ports to the last thread
                        scanParams->end_port = end_port_int;
                    }

                    scanParams->thread_id = i;
                    xTaskCreate(tcp_scan_task, "TCP Scan Task", MY_STACK_SIZE, scanParams, tskIDLE_PRIORITY + 1, NULL);
                }
            }
        }
        vTaskDelay(pdMS_TO_TICKS(1000)); // Main task loop delay
    }
}

int main(void) {
    completedTasksMutex = xSemaphoreCreateMutex();
    xTaskCreate(main_task, "Main Task", MY_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);
    vTaskStartScheduler();
    for (;;);
    return 0;
}
