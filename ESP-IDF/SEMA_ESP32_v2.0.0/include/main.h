#ifndef __MAIN_H
#define __MAIN_H

#include <iostream>
#include <stdio.h>
#include <string>
#include <string.h>
#include <stdint.h>
#include "driver/ledc.h"
#include "esp_err.h"

/* ========================= DEFINICAO DE CONSTANTES ======================== */

#define VERSION            "v1.1"
#define FIRMWARE_DATE      "08/12/2022"
#define COPYRIGHT          "SS copyright"
#define SIGLA              "SEMA"
#define MEANING_1          "Sistema de Emergencia e Monitoramento"
#define MEANING_2          "do Autonomo"
#define MCU_INFO           "ESP32 DO IT DEV KIT"
#define ON 					0x1
#define OFF 				0x0
#define LOW                 0x0
#define HIGH                0x1

#define address_EEPROM      0                           // flash address to save and get data
#define EMPTY_VALUE         0xFF                        // Erase flash location

#ifdef address_EEPROM                                   // for future aplications
#define EEPROM_SIZE address_EEPROM + 512                //512 bytes
#endif

#define IN_EXTRA_CPU        GPIO_NUM_23                 // GPIO 23 - RECEBIMENTO DE COMANDOS DO CPU
#define OUT_EXTRA_CPU       GPIO_NUM_22                 // GPIO 22 - ENVIO DE FEEDBACK PARA CPU
#define IO_OUT_ESP_1        GPIO_NUM_12                 // GPIO 12 - RECEBIMENTO DE FEEDBACK DO STM32
#define IO_OUT_ESP_2        GPIO_NUM_14                 // GPIO 14 - ENVIO DE FEEDBACK PARA O STM32
#define RELE_FREIO_ESP      GPIO_NUM_4                  // GPIO 4 - SAIDA DIGITAL RELE ESP32
#define CONNECT_ESP_IN      GPIO_NUM_3                  // GPIO 3 - INFORMA ESTADO DA CONEXAO
#define ESP_IN_CLIENTE      GPIO_NUM_15                 // GPIO 15 - INDICA CLIENTES CONECTADOS

// PINOS ENTRADAS - RECEBER INFORMACAO DE STATUS STM32, CPU e UPA
#define OUT_PWM_ESP         GPIO_NUM_19                 // GPIO 19 - STM
#define IN_PWM_UPA          GPIO_NUM_21                 // GPIO 21 - UPA
#define IN_PWM_CPU          GPIO_NUM_27                 // GPIO 27 - CPU

// PINO SAIDA - ENVIAR INFORMACAO DE STATUS DO PROPRIO ESP32
#define IN_PWM_ESP          GPIO_NUM_2                  // GPIO 2 - ESP

// PWMs:
// FREIO: 
#define FE_ESP              GPIO_NUM_5                  // GPIO 5
#define FREQ_FE             500                         // FREQUENCY (Hz)
#define CHANNEL_FE          LEDC_CHANNEL_0              // CHANNEL 0
// ACELERADOR:
#define AC_ESP              GPIO_NUM_18                 // GPIO 18
#define FREQ_AC             500                         // FREQUENCY (Hz)
#define CHANNEL_AC          LEDC_CHANNEL_1              // CHANNEL 1

#define RESOLUTION_PWM      LEDC_TIMER_16_BIT           // Resolution PWM 16 bits

#define TCP_SERVER_PORT     8080                        // Porta do servidor TCP
#define SOCKET_SERVER_PORT  80                          // Porta do servidor socket

#define TIMEOUT_FAIL        3000                        // TEMPO MAXIMO SEM RETORNO STM, UPA ou CPU (em ms)

#endif /* __MAIN_H */