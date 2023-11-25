#include "lwip/apps/httpd.h"
#include "pico/cyw43_arch.h"

// SSI tags - tag length limited to 8 bytes by default
const char * ssi_tags[] = {"volt","temp","led"};

// Somewhere globally accessible to both CGI and SSI handlers
extern char user_input_buffer[100];


u16_t ssi_handler(int iIndex, char *pcInsert, int iInsertLen) {
  size_t printed;
  printed=0;
  return (u16_t)printed;
}

// Initialise the SSI handler
void ssi_init() {

  http_set_ssi_handler(ssi_handler, ssi_tags, LWIP_ARRAYSIZE(ssi_tags));
}
