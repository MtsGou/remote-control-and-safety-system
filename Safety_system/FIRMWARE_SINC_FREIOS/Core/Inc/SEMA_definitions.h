/*
 * SEMA_definitions.h
 *
 *  Created on: 30 de ago de 2022
 *      Author: matheus.gouveia
 */

/**
  ******************************************************************************
  * @file    SEMA_definitions.h
  * @author  Matheus Gouveia
  * @brief   Esse arquivo contem todas as declaracoes iniciais necessarias
  * 		 para  a aplicacao do SEMA.
  *
  ******************************************************************************
  * @attention
  *
  * Todos os direitos preservados.
  *
  *
  ******************************************************************************
  */

#ifndef INC_SEMA_DEFINITIONS_H_
#define INC_SEMA_DEFINITIONS_H_

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "stm32l4xx_hal.h"

typedef int bool;
#define true 1
#define false 0

#define VERSION            		   "v1.0"
#define FIRMWARE_DATE      		   "30/08/2022"
#define COPYRIGHT          		   "UEM MINERACAO E CONSTRUCAO S/A"
#define SIGLA              		   "SEMA"
#define MEANING_1          		   "Sistema de Emergencia e Monitoramento"
#define MEANING_2          		   "do Autonomo"
#define MCU_INFO           		   "STM32 L476RG"

#define LOW 						0x0
#define HIGH 						0x1
#define ON 							0x1
#define OFF 						0x0
#define END_INICIAL  				0x08080000UL /* Endereco salvar flash */
#define END_STG						0x08080000UL
#define PWM_FE_MAX					85.0f		 /* Duty Cycle maximo freio emergencia */
#define PWM_FE_MIN					17.4f		 /* Duty Cycle minimo freio emergencia */
#define	PWM_AC_MAX					84.8f		 /* Duty Cycle maximo acelerador */
#define PWM_AC_MIN					15.8f		 /* Duty Cycle minimo acelerador */
#define PWM_FR_MAX					88.0f		 /* Duty Cycle maximo freio retardo */
#define PWM_FR_MIN					10.5f		 /* Duty Cycle minimo freio retardo */
#define PERCENT_FE					100		 	 /* Percentual do DC freio emergencia */
#define PERCENT_AC					0			 /* Percentual do DC acelerador */
#define PERCENT_FR					0			 /* Percentual do DC freio retardo */
#define PING_TIMEOUT_STD			5000		 /* Timeout recebimento PING, em ms */
#define TIMEOUT_TIMER				3000		 /* Timeout recebimento STATUS, em ms */
#define FEEDBACK_NUM_STD			5		     /* Numero de multiplos feedbacks padrao */
#define MAX_LOOP					3			 /* Maximo de loops para enviar STATUS */

#define ESP_OK_UPA_OK_CPU_FAIL		0x0		 	 /* Caso falha do CPU */
#define ESP_OK_UPA_FAIL_CPU_OK		0x1		 	 /* Caso falha da UPA */
#define ESP_OK_UPA_FAIL_CPU_FAIL	0x2		 	 /* Caso falha CPU e UPA */
#define ESP_FAIL_UPA_OK_CPU_OK		0x3		 	 /* Caso falha ESP */
#define ESP_FAIL_UPA_OK_CPU_FAIL	0x4		 	 /* Caso falha ESP e CPU */
#define ESP_FAIL_UPA_FAIL_CPU_OK	0x5		 	 /* Caso falha ESP e UPA */
#define ALL_FAIL					0x6		 	 /* Caso falha CPU, UPA e ESP */
#define STM_ESP_CNCT				0x7		 	 /* Caso STM e ESP conectados */
#define STM_CNCT_ESP_N_CNCT			0x8		 	 /* Caso STM conectado, ESP desconectado */
#define STM_N_CNCT_ESP_CNCT			0x9		 	 /* Caso STM desconectado, ESP conectado */
#define STM_ESP_N_CNCT				0xA		 	 /* Caso STM e ESP desconectados */

#define FRAME_ACTUATE_RELAY			0x1		 	 /* Opcao preencher buffer feedback -> rele atuado */
#define FRAME_PING					0x2		 	 /* Opcao preencher buffer feedback -> caso PING */
#define FRAME_BATTERY_FAIL			0x3		 	 /* Opcao preencher buffer feedback -> falha bateria */
#define FRAME_BATTERY_RETURN		0x4		 	 /* Opcao preencher buffer feedback -> bateria ok */

/*typedef enum {

    IDLE,
    __LAST_STATE,
    __VALID_STATE,
    SYS_FAIL,
    FATAL_FAIL // break and check position
                // with GPS or referential
} FSM_t;

extern FSM_t sys_state; */

extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;
extern TIM_HandleTypeDef htim16;

extern UART_HandleTypeDef huart4;
extern UART_HandleTypeDef huart3;

extern const uint8_t start_byte[2];
extern const uint8_t ON_COMMAND[2];
extern const uint8_t OFF_COMMAND[2];
extern const uint8_t DIV[2];
extern const uint8_t Confirmation[3];
extern const uint8_t CHECKSUM_DIV[2];
extern const uint8_t end_byte[2];
extern const uint8_t PING_DATA[2];
extern uint8_t ID[7];
extern uint8_t ID_COMMAND[3];
extern uint8_t FrameOn_A[30];
extern uint8_t FrameOff_A[30];
extern uint8_t Frame_PING_A[30];
extern uint8_t FrameOn_B[30];
extern uint8_t FrameOff_B[30];
extern uint8_t Frame_PING_B[30];
extern uint8_t BUFFER_TRANSMIT[255];
extern uint8_t Data[1];
extern uint8_t BUFFER_RECEIVE[255];
extern uint16_t BUFFER_SERIAL[255];
extern char BUFFER_RECEIVE_USB[1];
extern char BUFFER_FLASH[300];
extern char BUFFER_FLASH_L[300];
extern char DUTY_CYCLE_FE_CHAR[8];
extern char DUTY_CYCLE_FE_MIN_CHAR[8];
extern char DUTY_CYCLE_AC_CHAR[8];
extern char DUTY_CYCLE_AC_MAX_CHAR[8];
extern char DUTY_CYCLE_FR_CHAR[8];
extern char DUTY_CYCLE_FR_MIN_CHAR[8];
extern char DUTYFE_CHAR[4];
extern char DUTYACEL_CHAR[4];
extern char DUTYFR_CHAR[4];
extern char TIMEOUT_PING_CHAR[8];
extern char NUMBER_OF_FEEDBACKS_CHAR[4];
extern volatile float DUTY_CYCLE_FE;
extern volatile float DUTY_CYCLE_FE_MIN;
extern volatile int PWM_FE;
extern volatile float DUTY_CYCLE_AC;
extern volatile float DUTY_CYCLE_AC_MAX;
extern volatile int PWM_AC;
extern volatile float DUTY_CYCLE_FR;
extern volatile float DUTY_CYCLE_FR_MIN;
extern volatile int PWM_FR;
extern volatile float DUTYCYCLE_FE;
extern volatile float DUTYCYCLE_AC;
extern volatile float DUTYCYCLE_FR;
extern volatile uint8_t DUTYFE;
extern volatile uint8_t DUTYACEL;
extern volatile uint8_t DUTYFR;
extern uint8_t CRC_checksum;
extern char CRC_STR_X[8];
extern volatile int BREAK_STATUS;
extern int CONT_LOOP;
extern bool Toggle_Pins;
extern uint32_t FLAG_SOMA_UPA;
extern uint32_t FLAG_SOMA_ESP;
extern uint32_t FLAG_SOMA_CPU;
extern uint32_t FLAG_XBEE_PING;
extern uint32_t TIMEOUT_PING;
extern bool GATE_FR;
extern int CONTROLE_FR[4];
extern bool STATUS_CONNECTION_ESP;
extern bool STATUS_CLIENTS_ESP;
extern int STATUS_PARA_CPU[3];
extern uint32_t millis;
extern bool feedback_multiple;
extern uint8_t NUMBER_OF_FEEDBACKS;

/* Chave On/Off do Mux */
void SET_SELECTOR(int MODE);

/* Posicao Mux - ESP */
void AB_Position_ESP(void);

/* Posicao Mux - STM */
void AB_Position_STM(void);

/* Posicao Mux - UPA */
void AB_Position_UPA(void);

/* Inicializacao dos pinos digitais de saida */
void Init_Output_Pins(void);

/* Atuacao do rele e envio de feedbacks. */
void Acionamento(void);

/* Toggle Status Pins para geracao de PWM*/
void TOGGLE_STATUS_PINS(bool STATUS);

/* Ajuste do Duty Cycle do Freio de Retardo pelo CPU */
void AJUSTAR_DUTY_CYCLE_FR(void);

/* Leitura dos pinos digitais de entrada */
void LEITURA_INPUT_PINS(void);

/* Funcao para receber comandos do CPU e atuar */
void CPU_Handler(void);

/* Funcao para receber feedbacks do ESP */
void ESP_Handler(void);

/* Funcao para enviar multiplos feedbacks */
void Multiple_Feedback(void);

/* Ajuste dos PWMs: freio emergencia,
 * acelerador e freio retardo. */
void PWM_DEFINITION(void);

/* Sinal de STATUS para CPU, UPA e ESP */
void TOGGLE_PWM_STATUS(void);

/* Inicializacao de parametros, verificar
 * memoria flash. */
void inicializar_parametros(void);

/* Atuacao dos PWMs: freio emergencia,
 * acelerador e freio retardo.*/
void atuar_PWMs(void);

/* Enviar por serial o horario de agora, desde
 * que o sistema foi ligado */
void show_now(void);

/* Apresentacao monitor serial, info firmware version */
void FIRMWARE_BEGIN(void);

/* Monitoramento do sistema */
void MONITORAMENTO(int IDENTIFIER);

/* Definir STATUS para enviar para CPU */
void SEND_STATUS_TO_CPU(int which);

/* Monitorar a rede e informar CPU */
void MONITORAR_REDE(void);

/* Informar por serial falha ESP e enviar estado para CPU */
void FAIL_ESP(uint8_t __ESTADO_ANTERIOR);

/* Informar por serial falha UPA e enviar estado para CPU */
void FAIL_UPA(uint8_t __ESTADO_ANTERIOR);

/* Informar por serial falha ESP e UPA e enviar estado para CPU */
void FAIL_ESP_UPA(uint8_t _ESTADO_ANTERIOR);

/* Informar por serial falha CPU e enviar estado para CPU */
void FAIL_CPU(uint8_t ESTADO_ANTERIOR);

/* Informar por serial falha CPU + outro componente e enviar estado para CPU */
void FAIL_GENERAL(uint8_t __ESTADO_ATUAL);

/* Informar por serial caso tenha recuperado comunicacao com componentes. */
void SYS_OK_RECOVERED(uint8_t FALHA_ANTERIOR);

#ifdef __cplusplus
}
#endif

#endif /* INC_SEMA_DEFINITIONS_H_ */
