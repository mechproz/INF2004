/*
 * Copyright (c) 2021 Sandeep Mistry
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "lwip/udp.h" //added udp header file with lwip
#include "hardware/clocks.h"

#include "lwip/dhcp.h"
#include "lwip/init.h"

#include "lwip/apps/httpd.h"

#include "lwip/sockets.h"
#include "lwip/inet.h"
#include "lwip/ip.h"
#include "lwip/icmp.h"
#include "lwip/netif.h"

#include "rmii_ethernet/netif.h"





void netif_link_callback(struct netif *netif)
{ 
    printf("netif link status changed %s\n", netif_is_link_up(netif) ? "up" : "down");
    
}

void netif_status_callback(struct netif *netif)
{
    printf("netif status changed %s\n", ip4addr_ntoa(netif_ip4_addr(netif)));
}


void send_udp_packet(struct netif *netif) {
    struct udp_pcb *udp;
    err_t err;

    // Allocate a UDP PCB (Protocol Control Block)
    udp = udp_new();
    if (!udp) {
        printf("Failed to create UDP PCB\n");
        return;
    }
    else {
        printf("UDP PCB created\n");
    }

    // Set the destination IP address and port
    ip_addr_t dest_ip;
    IP4_ADDR(&dest_ip, 192, 168, 100, 100); // Example destination IP address
    uint16_t dest_port = 80; // Example destination port

    // Connect to the remote host
    err = udp_connect(udp, &dest_ip, dest_port);
    if (err != ERR_OK) {
        printf("Failed to connect to the remote host\n");
        udp_remove(udp);
        return;
    }
    else
    {
        printf("Connected to the remote host\n");
    }

    // Set the network interface for sending the packet
    udp_bind_netif(udp, netif);

    // Create a buffer with the data to send
    uint8_t data_to_send[] = {0x01, 0x02, 0x03}; // Example data to send

    // Send the data in for loop
    for (int i = 0; i < 3; i++) {
        err = udp_send(udp, data_to_send);
        if (err != ERR_OK) {
            printf("Failed to send UDP packet\n");
            udp_remove(udp);
            return;
        }
        else {
            printf("UDP packet sent\n");
        }
    }

    // Clean up the UDP PCB
    udp_remove(udp);
}

//function to update netif


int main() {
    // LWIP network interface
    struct netif netif;
    ip4_addr_t ipaddr, netmask;
    // Define static ip variables
    IP4_ADDR(&ipaddr, 192, 168, 100, 2); //these are for static IP
    IP4_ADDR(&netmask, 255, 255, 255, 0);
    
    struct netif_rmii_ethernet_config netif_config = {
        pio0, // PIO:            0
        0,    // pio SM:         0 and 1
        6,    // rx pin start:   6, 7, 8    => RX0, RX1, CRS
        10,   // tx pin start:   10, 11, 12 => TX0, TX1, TX-EN
        14,   // mdio pin start: 14, 15   => ?MDIO, MDC
        NULL, // MAC address (optional - NULL generates one based on flash id) 
    };

    // change the system clock to use the RMII reference clock from pin 20
    clock_configure_gpin(clk_sys, 20, 50 * MHZ, 50 * MHZ);
    sleep_ms(100);

    // initialize stdio after the clock change
    stdio_init_all();
    sleep_ms(5000);
    
    
    
  
    lwip_init();
    // initialize the PIO base RMII Ethernet network interface
    netif_rmii_ethernet_init(&netif, &netif_config);
    
    // assign callbacks for link and status
    netif_set_link_callback(&netif, netif_link_callback);
    netif_set_status_callback(&netif, netif_status_callback);
    //netif_set_ipaddr(&netif, &ipaddr);
    // set the default interface and bring it up
    netif_set_default(&netif);
    netif_set_up(&netif);

    // Start DHCP client 
    

    dhcp_start(&netif);

    
    httpd_init(); //testing web page
    

   
   
   
    multicore_launch_core1(netif_rmii_ethernet_loop); //continously polls the ethernet interface for packets on second core
    sleep_ms(6000);
    while (1) {
         // First Core loop
        send_udp_packet(&netif);
        sleep_ms(2000); 
    
    }

    return 0;
}
