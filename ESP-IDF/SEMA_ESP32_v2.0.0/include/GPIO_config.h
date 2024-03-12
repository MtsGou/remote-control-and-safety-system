#include "driver/gpio.h"
#include "esp_rom_gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"


#ifndef __MAIN_H
    #include <main.h>
#endif

void init_Digital_GPIO_pins(void);

void init_PWM_GPIO_pins(void);

void init_ISR_GPIO_pins(void);

