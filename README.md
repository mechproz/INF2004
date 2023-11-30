# INF2004
This is the repository for ICT2201/INF2001, Group 46

Brief Introduction/description:
- This project seeks to implement a lightweight nmap scan utility hosted on the raspberry Pico W. It offers 2 main advantages

- Plug and Play: With our nmap lite, it only requires 2 steps, connect to our device to your Wifi, and type in a IP address in our website and we will scan for you the TCP ports and let you know about what ports are open!

- Portable - comparing to actual port scanners, you would require some form of operating systems to run those port scanning services. Our solution only requires a Raspberry Pico W and a USB cable to power it

Project Objective:
- The main objective is to serve as a helper for network penetration testers or network administrators to have a portable version of nmap, revealing open ports when scanning a given device's IP address and port range.
Currently this project supports a TCP scan and returns only open ports, however it could potentially be expanded to detect closed or filtered ports, and perhaps even version detection of services hosted on open ports.





Requirements to run: 
- Raspberry Pi
- Maker Pico 
- USB cable
- SD card
- SD card reader 
- FreeRTOS
- LWIP
- FatFS_SPI


How to run the program:
Clone the github repository to Pico Vscode. The repository also includes individual drivers components as such Ethernet (scrapped), SD card, and web server. This facilitates a clear dive into each individual component if necessary. 

The nmap-lite folder contains the completed program. Compile and run the files. Upon successful compilation, a uf2 file will be created. Copy the uf2 file into your Raspberry Pi.

Run program, wifi will be initialized and webserver will be run on the IP allocated to the pico.
Navigate to the webserver using your ip, then input the desired values into the input fields.
Scanning will begin and results will be sent to the console and sd card.

Task Allocation:
Member 1: Ong Jun Heng, James (Leader)
Task:
1. Overall project coordination and management.
2. Handles the integration of the scanner module and web server module for the drivers and components for the project
3. Handles output of the scan from the program to be displayed on the website

Member 2: Koh Leng Siang, Aloysus
Task:
1. Working on the SD card and FAT file system integration
2. Handles the reading, writing and storage of the SD card

Member 3: Keegan Tang Sze Kang
Task:
1. Creation of TCP connection scans via WiFi
2. Testing and validation of the network scanning code and configurations.

Member 4: Lan Yiling Elizabeth
Task:
1. Supervision of project milestones and deadlines.
2. Setup and integration of scanner module with FreeRTOS
3. Handles the output of scan stored in SD card to be displayed on the website

Member 5: Cheryl Tay Ching
Task:
1. Set up the web server in Pico W
2. Set up the Wifi component to allow the client to connect to the web server via WiFi

Future Implementations: 
- To display scanning results on webpage successfully 
- Add outcomes of closed and filtered port

# Flow Chart
![FlowChart](https://github.com/mechproz/INF2004/assets/121922214/1619dbeb-5bac-41e3-bd0f-ee0ec8c99851)

# Block Diagram
![BlockDiagram](https://github.com/mechproz/INF2004/assets/121922214/ecf3556b-ee19-4fbe-8826-b6488fb7b348)
