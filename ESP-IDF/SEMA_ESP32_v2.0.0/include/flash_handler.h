#include "nvs_flash.h"
#include "nvs.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#ifndef __MAIN_H
    #include <main.h>
#endif

#include "esp_system.h"

void flash_init(void);

char* read_data_flash(void);

void write_data_flash(char* data);

void erase_all_data_flash(void);