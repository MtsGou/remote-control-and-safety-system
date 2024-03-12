#include "driver/uart.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"
#include <string>

#ifndef __MAIN_H
    #include <main.h>
#endif

#define TXD_PIN (GPIO_NUM_17)
#define RXD_PIN (GPIO_NUM_16)

#define UART UART_NUM_2

void init_uart(void);

void send_uart(const char *data);

bool serial_available(void);

std::string serial_read_char(void);

std::string serial_read_string_until(std::string stop_str);