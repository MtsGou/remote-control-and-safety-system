/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : SEMA Main program body
  * @author			: Matheus Gouveia
  * @brief   		: Sistema de Emergencia e Monitoramento do autonomo (SEMA)
  *          		  This software is for Microcontroller STM32 L476RG
  *          		  Comunicacao com CPU, UPA e ESP32
  *          		  Monitora o sistema e atua caso CPU ou UPA falhem
  *         		  Atua remotamente por botao de emergencia do controlador
  *          		  Retorna feedbacks ao controlador, ao CPU e ao ESP
  *          		  Atua em conjunto com o microcontrolador ESP32
  *          		  Linguagem de programacao utilizada: C
  *
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  *  Sistema de Emergencia e Monitoramento do Autonomo (SEMA)
  *  Created on: 30/08/2022
  *  Os direitos autorais devem ser incluidos
  *  em todas as copias ou partes substanciais do Software.
  *      Author: Matheus Gouveia.
  *
  * ===============================================================================
  *                      ##### EXPLICACAO DO SOFTWARE #####
  *  ===============================================================================
  * [..]
  *
  *  PINOUT:
  *
  *  (#) CPU communication PINS:
  *    (*) IN_CPU_6: Recebimento de comandos do CPU para frear ou desativar freio.
  *                        1 -> Freio atuado. 0-> Freio nao atuado.
  *    (*) FEEDBACK_CPU: Envio de feedbacks para CPU apos atuacao.
  *                        1 -> Freio atuado. 0-> Freio nao atuado.
  *    (*) IN_PWM_CPU: Recebe o PWM indicativo do estado de funcionamento do CPU.
  *    (*) PWM_OUT_CPU: Envia o PWM indicativo do estado de funcionamento do STM32.
  *    (*) GATE_FR: Habilita o ajuste do freio de retardo pelo CPU.
  *    (*) FR_BIT_1: Bit 1 do ajuste do freio de retardo pelo CPU.
  *    (*) FR_BIT_2: Bit 2 "									"
  *    (*) FR_BIT_3: Bit 3 "									"
  *    (*) FR_BIT_4: Bit 4 "									"
  *    (*) CPU_OUT_A: Primeiro pino para informar status do sistema para CPU.
  *    (*) CPU_OUT_B: Segundo pino "									"
  *    (*) CPU_OUT_C: Terceiro pino "									"
  *
  *
  *  (#) UPA communication PINS:
  *    (*) IN_PWM_UPA: Recebe o PWM indicativo do estado de funcionamento da UPA.
  *    (*) PWM_OUT_UPA: Envia o PWM indicativo do estado de funcionamento do STM32.
  *
  *  (#) ESP communication PINS:
  *    (*) IO_OUT_ESP_2: Recebimento de feedbacks do ESP apos atuacao deste.
  *                        1 -> Freio atuado. 0-> Freio nao atuado.
  *    (*) IO_OUT_ESP_1: Envio de feedbacks para ESP apos atuacao.
  *                        1 -> Freio atuado. 0-> Freio nao atuado.
  *    (*) OUT_PWM_ESP: Envia um PWM indicativo do estado de funcionamento do STM32.
  *    (*) IN_PWM_ESP: Recebe o PWM indicativo do estado de funcionamento do ESP.
  *    (*) CONNECT_ESP_IN: Recebe um sinal digital para informar a conexao do ESP.
  *                        1 -> conectado. 0-> nao conectado.
  *    (*) ESP_IN_CLIENTE: Recebe sinal digital do ESP indicando conexao dos clientes.
  *                      1 -> ao menos 1 cliente conectado. 0-> nao conectado.
  *
  *  (#) Actuation pins:
  *    (*) RELE_FREIO_STM: Saida digital responsavel por atuar o rele de freio.
  *    (*) FE_STM: Geracao do PWM do freio de emergencia para a ECM do caminhao.
  *    (*) AC_STM: Geracao do PWM do acelerador para a ECM do caminhao.
  *    (*) FR_STM: Geracao do PWM do freio de retardo para a ECM do caminhao.
  *    (*) ON_OFF_SWITCH: Pino para ligar o MUX
  *    (*) A_SWITCH: Pino A para trocar posicao do MUX.
  *    (*) B_SWITCH: Pino B "						   "
  *
  *  (#) Indication pins:
  *    (*) LED_GREEN: Led para indicar recebimento RX do radio (UART 3)
  *    (*) LED_RED: Led para indicar envio TX para radio (UART 3)
  *    (*) CHECK_FONTE: Indica o estado da bateria do caminhao.
  *    					1 -> Bateria OK. 0-> Falha na alimentacao.
  *
  *  (#) RESUMO:
  *      (+) Em condicoes normais, os reles nao estao atuados.
  *      (+) Posicao do MUX padrao: sinais da UPA.
  *      (+) Recebimento de PWM indicativo de estado de funcionamento:
  *        - Feito por meio de pinos de interrupcao externa.
  *        - A interrupcao zera contadores.
  *        - Se os contadores ultrapassarem um limite (timeout),
  *          e entendido como falha.
  *        - Falha ESP: +1, Falha UPA: +2, Falha CPU: +5.
  *          Ex.: Falha de numero 7 = Falha UPA e CPU (5 + 2)
  *
  *      (+) Condicoes de atuacao:
  *        - Falha da UPA
  *        - Falha do CPU
  *        - Falha de ambos
  *        - Ordem direta do controlador (botao de emergencia)
  *        - Com ordem direta do CPU
  *      (+) Na atuacao, o rele atua e envia para ECM os PWM gerados,
  *      	 em conjunto com a troca da posicao do MUX (STM).
  *      (+) Um menu de configuracao pode ser acessado.
  *        - Os parametros configurados podem ser salvos na flash.
  *        - Os parametros configurados podem ser apagados da flash.
  *      (+) Eventualidades sao informadas a qualquer momento por UART4,
  *          contendo timestamp (tempo apos inicio do funcionamento).
  *      (+) Conexao:
  *        - Recebe PING do transmissor na torre de controle.
  *        - Responde PONG ao transmissor.
  *        - Se parar de receber PING, indica perda de conexao do STM.
  *      (+) A placa possui sistema preventivo de queda de energia.
  *        - Se o sistema perder a alimentacao que vem da bateria
  *      	 do caminhao, uma bateria reserva em paralelo, protegida
  *      	 por um diodo, passa a alimentar o sistema, para mante-lo
  *      	 em devido funcionamento.
  *        - Se perder a alimentacao do caminhao, uma mensagem sera
  *        	 enviada remotamente ao controlador na torre de controle.
  *        - Com falha dos demais componentes do autonomo (UPA e CPU)
  *          o STM32 ira frear de imediato.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define CHECK_FONTE_Pin GPIO_PIN_0
#define CHECK_FONTE_GPIO_Port GPIOH
#define CHECK_FONTE_EXTI_IRQn EXTI0_IRQn
#define CPU_OUT_C_Pin GPIO_PIN_0
#define CPU_OUT_C_GPIO_Port GPIOC
#define ON_OFF_SWITCH_Pin GPIO_PIN_1
#define ON_OFF_SWITCH_GPIO_Port GPIOC
#define B_SWITCH_Pin GPIO_PIN_2
#define B_SWITCH_GPIO_Port GPIOC
#define A_SWITCH_Pin GPIO_PIN_3
#define A_SWITCH_GPIO_Port GPIOC
#define IN_PWM_ESP_Pin GPIO_PIN_3
#define IN_PWM_ESP_GPIO_Port GPIOA
#define IN_PWM_ESP_EXTI_IRQn EXTI3_IRQn
#define OUT_PWM_ESP_Pin GPIO_PIN_5
#define OUT_PWM_ESP_GPIO_Port GPIOA
#define RELE_FREIO_STM_Pin GPIO_PIN_6
#define RELE_FREIO_STM_GPIO_Port GPIOA
#define IO_OUT_ESP_1_Pin GPIO_PIN_0
#define IO_OUT_ESP_1_GPIO_Port GPIOB
#define IO_OUT_ESP_2_Pin GPIO_PIN_1
#define IO_OUT_ESP_2_GPIO_Port GPIOB
#define IO_OUT_ESP_2_EXTI_IRQn EXTI1_IRQn
#define ESP_IN_CLIENT_Pin GPIO_PIN_2
#define ESP_IN_CLIENT_GPIO_Port GPIOB
#define FEEDBACK_CPU_Pin GPIO_PIN_10
#define FEEDBACK_CPU_GPIO_Port GPIOB
#define LED_GREEN_Pin GPIO_PIN_12
#define LED_GREEN_GPIO_Port GPIOB
#define LED_RED_Pin GPIO_PIN_13
#define LED_RED_GPIO_Port GPIOB
#define IN_EXTRA_3_Pin GPIO_PIN_14
#define IN_EXTRA_3_GPIO_Port GPIOB
#define IN_EXTRA_2_Pin GPIO_PIN_15
#define IN_EXTRA_2_GPIO_Port GPIOB
#define IN_EXTRA_1_Pin GPIO_PIN_6
#define IN_EXTRA_1_GPIO_Port GPIOC
#define IN_PWM_UPA_Pin GPIO_PIN_7
#define IN_PWM_UPA_GPIO_Port GPIOC
#define IN_PWM_UPA_EXTI_IRQn EXTI9_5_IRQn
#define IN_CPU_7_Pin GPIO_PIN_8
#define IN_CPU_7_GPIO_Port GPIOC
#define IN_CPU_6_Pin GPIO_PIN_9
#define IN_CPU_6_GPIO_Port GPIOC
#define IN_CPU_6_EXTI_IRQn EXTI9_5_IRQn
#define FR_BIT_4_Pin GPIO_PIN_8
#define FR_BIT_4_GPIO_Port GPIOA
#define FR_BIT_3_Pin GPIO_PIN_9
#define FR_BIT_3_GPIO_Port GPIOA
#define CPU_OUT_B_Pin GPIO_PIN_10
#define CPU_OUT_B_GPIO_Port GPIOA
#define PWM_OUT_UPA_Pin GPIO_PIN_11
#define PWM_OUT_UPA_GPIO_Port GPIOA
#define CPU_OUT_A_Pin GPIO_PIN_12
#define CPU_OUT_A_GPIO_Port GPIOA
#define PWM_OUT_CPU_Pin GPIO_PIN_15
#define PWM_OUT_CPU_GPIO_Port GPIOA
#define FR_BIT_2_Pin GPIO_PIN_10
#define FR_BIT_2_GPIO_Port GPIOC
#define FR_BIT_1_Pin GPIO_PIN_11
#define FR_BIT_1_GPIO_Port GPIOC
#define GATE_FR_Pin GPIO_PIN_12
#define GATE_FR_GPIO_Port GPIOC
#define IN_PWM_CPU_Pin GPIO_PIN_2
#define IN_PWM_CPU_GPIO_Port GPIOD
#define IN_PWM_CPU_EXTI_IRQn EXTI2_IRQn
#define FE_STM_Pin GPIO_PIN_4
#define FE_STM_GPIO_Port GPIOB
#define AC_STM_Pin GPIO_PIN_5
#define AC_STM_GPIO_Port GPIOB
#define FR_STM_Pin GPIO_PIN_6
#define FR_STM_GPIO_Port GPIOB
#define CONNECT_ESP_IN_Pin GPIO_PIN_8
#define CONNECT_ESP_IN_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
