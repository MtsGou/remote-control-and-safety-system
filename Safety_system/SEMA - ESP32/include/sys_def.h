/* Packages and headers used */

#pragma once
#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <AsyncWebSocket.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <stdint.h>
#include <EEPROM.h>
#include <UART_handler.hpp>
#include <socket_handler.hpp>
#include <peripherals.h>
#include <emergency_break.hpp>
#include <data_transmission.hpp>


#define VERSION            "v1.1"
#define FIRMWARE_DATE      "08/12/2022"
#define COPYRIGHT          "COMPANY S/A"
#define SIGLA              "SS - Safety System"
#define MEANING_1          "Emergency and monitoring system"
#define MEANING_2          "Autonomous technology"
#define MCU_INFO           "ESP32 DO IT DEV KIT"

#define address_EEPROM      0 // flash address to save and get data
#define EMPTY_VALUE         0xFF // Erase flash location

#ifdef address_EEPROM // for future aplications
#define EEPROM_SIZE address_EEPROM + 512 //512 bytes
#endif

#define IN_EXTRA_CPU        23 // GPIO 23 - RECEBIMENTO DE COMANDOS DO CPU
#define OUT_EXTRA_CPU       22 // GPIO 22 - ENVIO DE FEEDBACK PARA CPU
#define IO_OUT_ESP_1        12 // GPIO 12 - RECEBIMENTO DE FEEDBACK DO STM32
#define IO_OUT_ESP_2        14 // GPIO 14 - ENVIO DE FEEDBACK PARA O STM32
#define RELE_FREIO_ESP       4 // GPIO 4 - SAIDA DIGITAL RELE ESP32
#define CONNECT_ESP_IN       3 // GPIO 3 - INFORMA ESTADO DA CONEXAO
#define ESP_IN_CLIENTE      15 // GPIO 15 - INDICA CLIENTES CONECTADOS

// PINOS ENTRADAS - RECEBER INFORMACAO DE STATUS STM32, CPU e UPA
#define OUT_PWM_ESP         19 // GPIO 19 - STM
#define IN_PWM_UPA          21 // GPIO 21 - UPA
#define IN_PWM_CPU          27 // GPIO 27 - CPU

// PINO SAIDA - ENVIAR INFORMACAO DE STATUS DO PROPRIO ESP32
#define IN_PWM_ESP           2 // GPIO 2 - ESP

// PWMs:
// FREIO: 
#define FE_ESP               5 // GPIO 5
#define FREQ_FE            500 // FREQUENCY (Hz)
#define CHANNEL_FE           0 // CHANNEL 0
// ACELERADOR:
#define AC_ESP              18 // GPIO 18
#define FREQ_AC            500 // FREQUENCY (Hz)
#define CHANNEL_AC           1 // CHANNEL 1
#define RESOLUTION_PWM      16 // Resolution PWM 16 bits

#define TCP_SERVER_PORT   8080 // Porta do servidor TCP
#define SOCKET_SERVER_PORT  80 // Porta do servidor socket

#define TIMEOUT_FAIL      3000 // TEMPO MAXIMO SEM RETORNO STM, UPA ou CPU (em ms)

// BAUD RATE PARA COMUNICACAO SERIAL
extern const unsigned int long BAUD_RATE;

// BITS DO PWM PARA USAR A FUNCAO LEDCWRITE
extern const uint32_t size_bits;

/* ================================= typedef =================================*/
typedef unsigned char byte;
typedef unsigned int uint;

/* ================== DECLARACAO DE VARIAVEIS GLOBAIS ========================*/

// ################## VARIAVEIS PARA GERACAO DOS SINAIS PWM ###################

// DUTY CYCLE DO FREIO E DO ACELERADOR

// FREIO
extern volatile float DUTYMAXFreio;    // limite maximo Duty Cycle Freio
extern volatile float DUTYMINFreio;    // limite minimo Duty Cycle Freio
extern volatile uint32_t PWMFreio;     // PWM efetivo para funcao ledcwrite
extern volatile float DUTYCYCLEFreio;  // Duty cycle efetivo
extern volatile uint8_t DUTYFREIO;     // % do duty cycle dentro dos limites

// ACELERADOR
extern volatile float DUTYMAXAcel;     // limite maximo Duty Cycle Acel
extern volatile float DUTYMINAcel;     // limite minimo Duty Cycle Freio
extern volatile uint32_t PWMAcel;      // PWM efetivo para funcao ledcwrite
extern volatile float DUTYCYCLEAcel;   // Duty cycle efetivo
extern volatile uint8_t DUTYACEL;      // % do dc dentro nos limites


// ########################## DADOS DA CONEXAO ################################

// PREENCHA COM O LOGIN DA REDE
extern String LOGIN;

// PREENCHA COM A SENHA DA REDE
extern String SENHA;

// ESPECIFICACOES REDE
extern uint8_t IP[4];
extern uint8_t GATE[4];
extern uint8_t SUBMASK[4];

// ########################### DADOS DE PROTOCOLO #############################

// IDENTIFICACAO DO CAMINHAO
extern String ID;
// Divisoria para capturar Checksum
extern String CHECKSUM_DIV;
// Start byte
extern String start_byte;
// End byte
extern String end_byte;

// ########################## VARIAVEIS DO SISTEMA ############################

// VARIAVEL DE ACIONAMENTO DO RELE
extern volatile bool relay;

// CONTADOR DE CLIENTES CONECTADOS
extern int cont_clientes;

// CONTADOR DE TEMPO DE RECONEXAO APOS PERDER CONEXAO
extern int cont_reconnect;

// TEMPO TIMEOUT DE RECONEXAO
extern unsigned int long TimeOut;

// CASO A CONEXAO FALHE E NAO CONSIGA RECONECTAR APOS TimeOut, Disconnect = 1.
extern bool Disconnect;

// VERIFICACAO DOS SINAIS QUE VEM DO STM, UPA e CPU
extern unsigned int long PERIODO_PWM_STATUS_STM;
extern bool VERIFY_STM; // STM OK - debug

extern unsigned int long PERIODO_PWM_STATUS_CPU;
extern bool VERIFY_CPU; // CPU OK - debug

extern unsigned int long PERIODO_PWM_STATUS_UPA;
extern bool VERIFY_UPA; // UPA OK - debug

// VARIAVEL PARA TOGGLE PIN STATUS DO PROPRIO ESP32
extern bool TogglePin_STATUS;

// NOTIFICACAO DE OCORRENCIA DE FALHA NO STM
extern bool OCORRENCIA_FALHA_STM;

// HISTORICO DE ACIONAMENTO PELO CPU
extern bool CPU_BREAK_ACT;

// HISTORICO DE FEEDBACK STM
extern bool STM_FEEDBACK;

// ##################### APLICACAO SERVIDOR-CLIENTE SOCKET ##################

// Servidor AsyncWebServer na porta 80
AsyncWebServer server(SOCKET_SERVER_PORT);
AsyncWebSocket ws("/ws");

// Servidor assincrono TCP porta 8080
AsyncServer *server_TCP = new AsyncServer(TCP_SERVER_PORT);

// Reservar lugar na memoria flash para parametros
extern char empty[EEPROM_SIZE];

/* Buffer para o relatorio */
extern String Buffer_report;