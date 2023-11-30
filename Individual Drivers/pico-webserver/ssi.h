#include "lwip/apps/httpd.h"
#include "pico/cyw43_arch.h"
#include "hardware/adc.h"

// SSI tags - tag length limited to 8 bytes by default
const char * ssi_tags[] = {"volt","temp","led", "userinput"};

// Somewhere globally accessible to both CGI and SSI handlers
extern char user_input_buffer[100];


u16_t ssi_handler(int iIndex, char *pcInsert, int iInsertLen) {
  size_t printed;
  switch (iIndex) {
  case 0: // volt
  {
    const float voltage = adc_read() * 3.3f / (1 << 12);
    printed = snprintf(pcInsert, iInsertLen, "%f", voltage);
  }
  break;
  case 1: // temp
  {
    const float voltage = adc_read() * 3.3f / (1 << 12);
    const float tempC = 27.0f - (voltage - 0.706f) / 0.001721f;
    printed = snprintf(pcInsert, iInsertLen, "%f", tempC);
  }
  break;
  case 2: // led
  {
    bool led_status = cyw43_arch_gpio_get(CYW43_WL_GPIO_LED_PIN);
    if(led_status == true){
      printed = snprintf(pcInsert, iInsertLen, "ON");
    }
    else{
      printed = snprintf(pcInsert, iInsertLen, "OFF");
    }
  }
  break;
  case 3: 
  {
    printf("SSI Handler for user input called.\r\n"); // Debugging
    printed = snprintf(pcInsert, iInsertLen, "%s", user_input_buffer);
    break;
  }
  // case 3:
  //   printf("SSI Handler for user input called with buffer: %s\n", user_input_buffer); // Debug print
  //   printed = snprintf(pcInsert, iInsertLen, "%s", user_input_buffer);
  //   break;
  default:
    break;
  }

  return (u16_t)printed;
}

// Initialise the SSI handler
void ssi_init() {
  // Initialise ADC (internal pin)
  adc_init();
  adc_set_temp_sensor_enabled(true);
  adc_select_input(4);

  http_set_ssi_handler(ssi_handler, ssi_tags, LWIP_ARRAYSIZE(ssi_tags));
}
