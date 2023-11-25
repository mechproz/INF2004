#include <stdio.h>
#include "pico/stdlib.h"
#include "sd_card.h"
#include "ff.h"

void SD_init() {
    // Initialize chosen serial port
    stdio_init_all();

    // Sleep for 10 seconds before the program starts
    printf("\r\nProg Started. Wait for 5 seconds\r\n");
    sleep_ms(10000);
    printf("\r\nMain Prog Starting\r\n");

    // Initialize SD card
    if (!sd_init_driver()) {
        printf("ERROR: Could not initialize SD card\r\n");
        while (true);
    }
}

void list_directory(const char *path) {
    DIR dir;
    FRESULT result;
    static FILINFO fInfo;

    result = f_opendir(&dir, path); // Open the directory

    if (result == FR_OK) {
        for (;;) {
            result = f_readdir(&dir, &fInfo); // Read directory contents
            if (result != FR_OK || fInfo.fname[0] == 0) {
                break; // Break if there's an error or no more files
            }

            if (fInfo.fattrib & AM_DIR) {
                if (fInfo.fname[0] != '.') {
                    printf("Directory: %s\n", fInfo.fname); // Print directory name
                }
            } else {
                printf("File in SD card: %s\n", fInfo.fname); // Print file name
            }
        }
        f_closedir(&dir); // Close the directory
    } else {
        // Handle error opening the directory
        printf("Error opening directory: %s\n", path);
    }
}

void deleteFile(const char *fname) {
    FRESULT fr = f_unlink(fname);
    if (fr != FR_OK) {
        printf("ERROR: Could not delete file '%s' (%d)\r\n", fname, fr);
        while (true);
    }
    printf("File '%s' deleted\r\n", fname);
}

int fileChecker(const char *fname) {
    FILINFO fInfo;
    FRESULT res = f_stat(fname, &fInfo);
    return (res == FR_OK && (fInfo.fattrib & AM_DIR) == 0); // Returns 1 if the file exists, 0 otherwise
}

// Function to write content to a file
void writeContentToFile(FIL *file, const char *filename, const char *content) {
    FRESULT fr;
    int ret;

    // Open file for writing or create if it doesn't exist
    fr = f_open(file, filename, FA_WRITE | FA_CREATE_ALWAYS);
    if (fr != FR_OK) {
        printf("ERROR: Could not open file for writing (%d)\r\n", fr);
        while (true);
    }

    // Write content to the file
    ret = f_printf(file, content);
    if (ret < 0) {
        printf("ERROR: Could not write to file (%d)\r\n", ret);
        f_close(file);
        while (true);
    }
    printf("Data written to file successfully.\r\n");
}

void readContentFromFile(FIL *file, const char *filename){
    FRESULT fr;
    char buf[100];
    printf("File '%s' exists\n", filename);
    // Open file for reading
    fr = f_open(file, filename, FA_READ);
    if (fr != FR_OK) {
        printf("ERROR: Could not open file for reading (%d)\r\n", fr);
        while (true);
    }

    // Read file content
    printf("Reading information from the file '%s':\r\n", filename);
    printf("---\r\n");
    while (f_gets(buf, sizeof(buf), file)) {
        printf(buf);
    }
    printf("\r\n---\r\n");
}

void closeFile(FIL *file){
    FRESULT fr;
    // Close file
    fr = f_close(file);
    if (fr != FR_OK) {
        printf("ERROR: Could not close file (%d)\r\n", fr);
        while (true);
    }
}

void unmountFile(){
    FRESULT fr;
    // Unmount drive
    fr = f_unmount("0:");
    if (fr != FR_OK) {
        printf("ERROR: Could not unmount filesystem (%d)\r\n", fr);
        while (true);
    }

    printf("File Unmounted\r\n");
}

void mountFile(FATFS *fs){
    FRESULT fr;
    // Mount drive
    fr = f_mount(fs, "0:", 1);
    if (fr != FR_OK) {
        printf("ERROR: Could not mount filesystem (%d)\r\n", fr);
        while (true);
    }
}

int main() {
    FIL fil;  
    FATFS fs;  
    const char filename[] = "test02.txt";

    SD_init();
    mountFile(&fs);
    list_directory("/");

    if (fileChecker(filename)) {
        readContentFromFile(&fil, filename);
    } else {
        const char content[] = "This is another test of writing to an SD card.\r\n";
        writeContentToFile(&fil, filename, content);
    }

    closeFile(&fil);

    //deleteFile(filename);

    unmountFile();

    // Loop forever doing nothing
    while (true) {
        sleep_ms(1000);
    }
}
