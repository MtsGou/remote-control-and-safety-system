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
  *  <h2><center>&copy;
  *  All rights reserved.</center></h2>
  *
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
  *        - Uma callback ocorre a cada overflow do timer 16 (1 ms).
  *        - Nessa callback, e feito o monitoramento e a
  *          identificacao de falhas.
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "string.h"
#include <stdio.h>
#include <stdlib.h>
#include <SEMA_definitions.h>
#include <FLASH.h>
#include <serial_radio.h>
#include <transmissao_dados.h>
#include <Menu_SEMA.h>

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM3_Init(void);
static void MX_TIM4_Init(void);
static void MX_UART4_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_TIM16_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

////////////////////////////////////////////////////////////////////////////////
///
/******************************************************************************
 ************************* Funcoes no escopo main *****************************
 ******************************************************************************/

/**------------------------------------------------------------------------------------
  * @brief STM Function to Get Time by timer.
  *
  *
  * @param  none
  *
  * @retval uwTick
  */
uint32_t HAL_GetTick(void){
  return uwTick;
}

/**------------------------------------------------------------------------------------
  * @brief Chave On/Off do Mux
  *
  * @param  MODE: ON or OFF.
  *
  * @retval void
  */
void SET_SELECTOR(int MODE){

	/* MUX On/Off
	 * 1 -> OFF
	 * 0 -> ON */
	HAL_GPIO_WritePin(ON_OFF_SWITCH_GPIO_Port, ON_OFF_SWITCH_Pin, !MODE);

}

/**------------------------------------------------------------------------------------
  * @brief Posicao Mux - L L - ESP
  *
  * @param  none
  *
  * @retval void
  */
void AB_Position_ESP(){

	HAL_GPIO_WritePin(A_SWITCH_GPIO_Port, A_SWITCH_Pin, LOW);
	HAL_GPIO_WritePin(B_SWITCH_GPIO_Port, B_SWITCH_Pin, LOW);

}

/**------------------------------------------------------------------------------------
  * @brief Posicao Mux - H L - STM
  *
  * @param  none
  *
  * @retval void
  */
void AB_Position_STM(){

	HAL_GPIO_WritePin(A_SWITCH_GPIO_Port, A_SWITCH_Pin, HIGH);
	HAL_GPIO_WritePin(B_SWITCH_GPIO_Port, B_SWITCH_Pin, LOW);

}

/**------------------------------------------------------------------------------------
  * @brief Posicao Mux - L H - UPA
  *
  * @param  none
  *
  * @retval void
  */
void AB_Position_UPA(){

	HAL_GPIO_WritePin(A_SWITCH_GPIO_Port, A_SWITCH_Pin, LOW);
	HAL_GPIO_WritePin(B_SWITCH_GPIO_Port, B_SWITCH_Pin, HIGH);

}

/**------------------------------------------------------------------------------------
  * @brief Inicializar pinos OUTPUT com valores padrao.
  *
  * @param  none
  *
  * @retval void
  */
void Init_Output_Pins(){

	HAL_GPIO_WritePin(PWM_OUT_CPU_GPIO_Port, PWM_OUT_CPU_Pin, LOW);
	HAL_GPIO_WritePin(PWM_OUT_UPA_GPIO_Port, PWM_OUT_UPA_Pin, LOW);
	HAL_GPIO_WritePin(OUT_PWM_ESP_GPIO_Port, OUT_PWM_ESP_Pin, LOW);

	HAL_GPIO_WritePin(RELE_FREIO_STM_GPIO_Port, RELE_FREIO_STM_Pin, LOW);
	HAL_GPIO_WritePin(FEEDBACK_CPU_GPIO_Port, FEEDBACK_CPU_Pin, LOW);
	HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, LOW);
	HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, LOW);

	HAL_GPIO_WritePin(IO_OUT_ESP_1_GPIO_Port, IO_OUT_ESP_1_Pin, LOW);

	/* Posicao padrao: MUX Ligado */
	SET_SELECTOR(ON);

	/* Posicao padrao: Sinais da UPA */
	AB_Position_UPA();

	/*	HAL_GPIO_WritePin(ON_OFF_SWITCH_GPIO_Port, ON_OFF_SWITCH_Pin, LOW); */
	HAL_GPIO_WritePin(CPU_OUT_A_GPIO_Port, CPU_OUT_A_Pin, LOW);
	HAL_GPIO_WritePin(CPU_OUT_B_GPIO_Port, CPU_OUT_B_Pin, LOW);
	HAL_GPIO_WritePin(CPU_OUT_C_GPIO_Port, CPU_OUT_C_Pin, LOW);

}

/**------------------------------------------------------------------------------------
  * @brief Atuacao do rele e envio de feedbacks.
  *
  * @OBS Futuramente deveria haver a verificacao
  *  da atuacao do rele (INPUT) para confirmar.
  *
  * @param  none
  *
  * @retval void
  */
void Acionamento(){

	/* Ligar MUX */
	SET_SELECTOR(ON);

	/*Acionar por BREAK_STATUS */
	HAL_GPIO_WritePin(RELE_FREIO_STM_GPIO_Port,RELE_FREIO_STM_Pin, BREAK_STATUS);

	if (BREAK_STATUS){
		/* Mudar para a posicao STM */
		AB_Position_STM();
	}
	else {
		AB_Position_UPA();
	}

	/* Feedback para CPU */
	HAL_GPIO_WritePin(FEEDBACK_CPU_GPIO_Port, FEEDBACK_CPU_Pin, BREAK_STATUS);

	/* Feedback para ESP */
	HAL_GPIO_WritePin(IO_OUT_ESP_1_GPIO_Port, IO_OUT_ESP_1_Pin, BREAK_STATUS);

	/* Preencher buffer para enviar feedback ao transmissor */
	PreencherBuffer(BREAK_STATUS);

	/*Informar para monitor serial */
	strcat(BUFFER_SERIAL, "\n\rComando Efetuado. Enviando Feedback...\r\n");

	/* Transmitir feedback ao transmissor */

	//if (!feedback_multiple){
	//	HAL_UART_Transmit(&huart3, BUFFER_TRANSMIT, strlen(BUFFER_TRANSMIT), 100);
	//	//HAL_UART_Transmit_DMA(&huart3, BUFFER_TRANSMIT, strlen(BUFFER_TRANSMIT));
	//}

	//strcat(BUFFER_SERIAL, "\n\rFeedback enviado. \r\n");

}


/**------------------------------------------------------------------------------------
  * @brief Funcao callback para dados recebidos do radio.
  *
  * @OBS Interrupcao UART 3. Apos start byte preenche buffer,
  * 	 parando quando recebe o end byte. Apos buffer completo,
  * 	 verifica a mensagem.
  *
  * @param  *huart:		Objeto UART.
  *
  * @retval void
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){

	/* Acender LED de dado Serial recebido */
	HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, HIGH);

	/* contador_dados => index */
	static int contador_dados = 0;
	/* Preencher a partir do Start Byte */
	static int HABILITAR_PREENCHIMENTO = 0;

	if (Data[0] == end_byte[0]) {
		contador_dados = 0;
		HABILITAR_PREENCHIMENTO = 0;
		VerificarComando();
	}
	else {
		if (Data[0] == start_byte[0]){
			HABILITAR_PREENCHIMENTO = 1;
		}
		else if (HABILITAR_PREENCHIMENTO){
			FLAG_XBEE_PING = 0;
			BUFFER_RECEIVE[contador_dados] = Data[0];
			contador_dados++;
		}
	}
	/* Manter continuo recebimento */
	HAL_UART_Receive_IT(&huart3, Data, 1);
}

/**------------------------------------------------------------------------------------
  * @brief Funcao callback quando dados forem enviados.
  *
  * @OBS Interrupcao UART 3.
  *
  * @param  *huart:		Objeto UART.
  *
  * @retval void
  */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart){

	if (huart == &huart3){
		/* Acender LED de dado Serial enviado */
		HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, HIGH);
	}

}

/**------------------------------------------------------------------------------------
  * @brief Toggle Status Pins para indicar funcionamento.
  *
  *
  * @param  STATUS:		Digital H/L.
  *
  * @retval void
  */
void TOGGLE_STATUS_PINS(bool STATUS){

	HAL_GPIO_WritePin(PWM_OUT_CPU_GPIO_Port, PWM_OUT_CPU_Pin, STATUS);
	HAL_GPIO_WritePin(PWM_OUT_UPA_GPIO_Port, PWM_OUT_UPA_Pin, STATUS);
	HAL_GPIO_WritePin(OUT_PWM_ESP_GPIO_Port, OUT_PWM_ESP_Pin, STATUS);

}

/**------------------------------------------------------------------------------------
  * @brief Ajuste do Duty Cycle do Freio de Retardo pelo CPU.
  *
  * @param  none
  *
  * @retval void
  */
void AJUSTAR_DUTY_CYCLE_FR(){

	/* Se GATE_FR for para nivel logico alto, o CPU
	 * esta solicitando o controle do FR. Em seguida
	 * o valor correspondente (4 bits) sera calculado
	 * para ajustar a largura do pulso do PWM. */

	/* Variaveis de controle do FR */
	CONTROLE_FR[3] = HAL_GPIO_ReadPin(FR_BIT_1_GPIO_Port, FR_BIT_1_Pin);
	CONTROLE_FR[2] = HAL_GPIO_ReadPin(FR_BIT_2_GPIO_Port, FR_BIT_2_Pin);
	CONTROLE_FR[1] = HAL_GPIO_ReadPin(FR_BIT_3_GPIO_Port, FR_BIT_3_Pin);
	CONTROLE_FR[0] = HAL_GPIO_ReadPin(FR_BIT_4_GPIO_Port, FR_BIT_4_Pin);

	int VALOR_FR = CONTROLE_FR[3] + 2*CONTROLE_FR[2] + 4*CONTROLE_FR[1] +
			8*CONTROLE_FR[0];

	static int VALOR_FR_ANTERIOR = 30 /*UM VALOR QUALQUER DIFERENTE DE FR*/;

	/* Desnecessario reajuste se o valor for o mesmo do anterior */
	if (VALOR_FR != VALOR_FR_ANTERIOR){
		int VALUE_INT = 0;

		/* TAXA = (VALOR_FR/VALOR_FR maximo), VALOR_FR maximo = 15 */
		float TAXA = (float) (VALOR_FR*100/15)/100;

		/* VALUE_INT = TAXA*(LARGURA DE PULSO MAXIMA) */
		VALUE_INT = (int) (TAXA*((DUTY_CYCLE_FR - DUTY_CYCLE_FR_MIN)*(200-1)/100));
		VALUE_INT = VALUE_INT + (int)(DUTY_CYCLE_FR_MIN*(200-1)/100);

		htim4.Instance -> CCR1 = VALUE_INT;

		/* Historico */
		VALOR_FR_ANTERIOR = VALOR_FR;
	}
}


/**------------------------------------------------------------------------------------
  * @brief Leitura dos pinos digitais de entrada. Informa sobre
  * 	   clientes conectados.
  *
  * @param  none
  *
  * @retval void
  */
void LEITURA_INPUT_PINS(){

	/* LEITURA HABILITAR CONTROLE FREIO RETARDO */
	GATE_FR = HAL_GPIO_ReadPin(GATE_FR_GPIO_Port, GATE_FR_Pin);

	/* Extras (Em aberto)
	HAL_GPIO_ReadPin(IN_EXTRA_1_GPIO_Port, IN_EXTRA_1_Pin);
	HAL_GPIO_ReadPin(IN_EXTRA_2_GPIO_Port, IN_EXTRA_2_Pin);
	HAL_GPIO_ReadPin(IN_EXTRA_3_GPIO_Port, IN_EXTRA_3_Pin);
	HAL_GPIO_ReadPin(IN_CPU_6_GPIO_Port, IN_CPU_6_Pin);
	HAL_GPIO_ReadPin(IN_CPU_7_GPIO_Port, IN_CPU_7_Pin);
	*/

	STATUS_CONNECTION_ESP = HAL_GPIO_ReadPin(CONNECT_ESP_IN_GPIO_Port, CONNECT_ESP_IN_Pin);
	STATUS_CLIENTS_ESP = HAL_GPIO_ReadPin(ESP_IN_CLIENT_GPIO_Port, ESP_IN_CLIENT_Pin);

	/* Armazenar leitura anterior clientes. */
	static int LEITURA_ANTERIOR_CLIENTES = 0;

	if (STATUS_CLIENTS_ESP && !LEITURA_ANTERIOR_CLIENTES){
		/* ADVERTER QUE PELO MENOS 1 CLIENTE CONECTOU */
		LEITURA_ANTERIOR_CLIENTES = 1;
		show_now();
		strcat(BUFFER_SERIAL, "\r\nPelo menos um cliente conectado ao servidor ESP.");
	}

	else if (!STATUS_CLIENTS_ESP && LEITURA_ANTERIOR_CLIENTES){
		/* ADVERTER QUE CLIENTES DESCONECTARAM */
		LEITURA_ANTERIOR_CLIENTES = 0;
		show_now();
		strcat(BUFFER_SERIAL, "\r\nNenhum cliente conectado ao servidor ESP.");
	}

}

/**------------------------------------------------------------------------------------
  * @brief  Callback periodica para identificar falhas e monitorar.
  *
  * @param  * htim: timer 16.
  *
  * @retval void
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim){

	/* Somar as flags. Se nao houver interrupcao externa, nao zera */
	FLAG_SOMA_ESP++;
	FLAG_SOMA_UPA++;
	FLAG_SOMA_CPU++;
	FLAG_XBEE_PING++;

	/* FALHA EM QUAL EQUIPAMENTO
	 * (1 -> ESP, 2 -> UPA, 5 -> CPU) */
	int status_identifier = 0;

	if (FLAG_SOMA_ESP > TIMEOUT_TIMER){
		status_identifier++;
	}
	if (FLAG_SOMA_UPA > TIMEOUT_TIMER){
		status_identifier = status_identifier + 2;
	}
	if (FLAG_SOMA_CPU > TIMEOUT_TIMER){
		status_identifier = status_identifier + 5;
	}

	MONITORAMENTO(status_identifier);
}

/**------------------------------------------------------------------------------------
  * @brief  Callback ao receber comandos do CPU. Atua apos
  *         estado do pino variar.
  *
  * @param  none
  *
  * @retval void
  */
void CPU_Handler(){

	static int count_cpu_orders = 0;
	static uint32_t time_last_order = 0;
	uint32_t time_interval = 0;
	static uint32_t sum_intervals = 0;

	/* Secao para evitar freio loop - bug do cpu */
	if (count_cpu_orders > 5 && sum_intervals < 20000){
		show_now();
		strcat(BUFFER_SERIAL, "\r\nABORTAR COMANDO CPU. FREIO ACIONADO.\r\n");
		BREAK_STATUS = 1;
		feedback_multiple = 0;
		Acionamento();
		return;
	}

	if (time_last_order != 0){
		time_interval = HAL_GetTick() - time_last_order;
		sum_intervals += time_interval;
	}
	if (sum_intervals > 30000){
		sum_intervals = 0;
		count_cpu_orders = 0;
	}
	time_last_order = HAL_GetTick();

	if (HAL_GPIO_ReadPin(IN_CPU_6_GPIO_Port, IN_CPU_6_Pin) && !BREAK_STATUS){
		show_now();
		strcat(BUFFER_SERIAL, "\r\nFREIO ACIONADO PELA CPU.\r\n");
		BREAK_STATUS = 1;
		count_cpu_orders++;
	}
	else if (!HAL_GPIO_ReadPin(IN_CPU_6_GPIO_Port, IN_CPU_6_Pin) && BREAK_STATUS){
		show_now();
		strcat(BUFFER_SERIAL, "\r\nFREIO DESATIVADO PELA CPU.\r\n");
		BREAK_STATUS = 0;
		count_cpu_orders++;
	}

	/* Ativar multiplos feedbacks para radio */
	feedback_multiple = 1;

	Acionamento();

}

/**------------------------------------------------------------------------------------
  * @brief  Callback ao receber feedbacks do ESP.
  * 		Informa no serial que houve atuacao do ESP.
  *
  * @param  none
  *
  * @retval void
  */
void ESP_Handler(){

	static bool initialized = false;

	if (HAL_GPIO_ReadPin(IO_OUT_ESP_2_GPIO_Port, IO_OUT_ESP_2_Pin)){
		show_now();
		strcat(BUFFER_SERIAL, "\r\nFREIO ACIONADO PELO ESP32.\r\n");
	}

	else {
		if (!initialized){
			/* Verificar inicializao do ESP */
			initialized = true;
		}
		else {
			show_now();
			strcat(BUFFER_SERIAL, "\r\nFREIO DESATIVADO PELO ESP32. \r\n");
		}
	}
}

/**------------------------------------------------------------------------------------
  * @brief  Callback para interrupcoes externas.
  *
  * @param  GPIO_Pin: Pino em que houve interrupcao.
  *
  * @retval void
  */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){

	/* STATUS - Verificar apenas borda de subida */
	if (GPIO_Pin == IN_PWM_ESP_Pin){
		FLAG_SOMA_ESP = 0;
	}
	else if (GPIO_Pin == IN_PWM_UPA_Pin){
		FLAG_SOMA_UPA = 0;
	}
	else if (GPIO_Pin == IN_PWM_CPU_Pin){
		FLAG_SOMA_CPU = 0;
	}
	else if (GPIO_Pin == IN_CPU_6_Pin){
		/*borda de subida ou descida */
		CPU_Handler();
	}
	else if (GPIO_Pin == IO_OUT_ESP_2_Pin){
		/*borda de subida ou descida */
		ESP_Handler();
	}
	else if (GPIO_Pin == CHECK_FONTE_Pin){

		/* Queda ou recuperacao da alimentacao. Informar radio. */
		if (HAL_GPIO_ReadPin(CHECK_FONTE_GPIO_Port, CHECK_FONTE_Pin)){
			PreencherBuffer(FRAME_BATTERY_FAIL);
		}
		if (!HAL_GPIO_ReadPin(CHECK_FONTE_GPIO_Port, CHECK_FONTE_Pin)){
			PreencherBuffer(FRAME_BATTERY_RETURN);
		}

		HAL_UART_Transmit_IT(&huart3, BUFFER_TRANSMIT, strlen(BUFFER_TRANSMIT));
	}
}

/**------------------------------------------------------------------------------------
  * @brief  Enviar multiplos feedbacks para radio.
  *
  * @param  none
  *
  * @retval void
  */
void Multiple_Feedback(){

	int num_of_tries = NUMBER_OF_FEEDBACKS;
	static int counter = 0;
	char num_of_tries_STR[16];

	if (counter < num_of_tries){
		strcat(BUFFER_SERIAL, "\r\nTentativa de feedback: ");
		IntParaString(counter + 1, num_of_tries_STR, 10);
		strcat(BUFFER_SERIAL, num_of_tries_STR);
		strcat(BUFFER_SERIAL, "\n\r");
		counter++;
	}
	else {
		feedback_multiple = 0;
		counter = 0;
	}
}

/**------------------------------------------------------------------------------------
  * @brief  Ajuste dos PWMs: freio emergencia, acelerador e freio retardo.
  *
  * @OBS	PWM ajustado por interpolacao linear.
  *
  * @param  none
  *
  * @retval void
  */
void PWM_DEFINITION() {

  DUTYCYCLE_FE = (float)DUTYFE*(DUTY_CYCLE_FE - DUTY_CYCLE_FE_MIN);
  DUTYCYCLE_FE += DUTY_CYCLE_FE_MIN*100;
  DUTYCYCLE_FE = DUTYCYCLE_FE/100;

  DUTYCYCLE_AC = (float)DUTYACEL*(DUTY_CYCLE_AC_MAX - DUTY_CYCLE_AC);
  DUTYCYCLE_AC += DUTY_CYCLE_AC*100;
  DUTYCYCLE_AC = DUTYCYCLE_AC/100;

  DUTYCYCLE_FR = (float)DUTYFR*(DUTY_CYCLE_FR - DUTY_CYCLE_FR_MIN);
  DUTYCYCLE_FR += DUTY_CYCLE_FR_MIN*100;
  DUTYCYCLE_FR = DUTYCYCLE_FR/100;

    /* FREIO DE EMERGENCIA */
  PWM_FE = (int) (DUTYCYCLE_FE*(200-1)/100);

  /* ACELERADOR */
  PWM_AC = (int)(DUTYCYCLE_AC*(200-1)/100);

  /* FREIO DE RETARDO */
  PWM_FR = (int) (DUTYCYCLE_FR*(200-1)/100);

}

/**------------------------------------------------------------------------------------
  * @brief  Contador para variar pinos de status. Envio de multiplos feedbacks.
  *
  * @param  none
  *
  * @retval void
  */
void TOGGLE_PWM_STATUS(){

	CONT_LOOP++;
	if (CONT_LOOP > MAX_LOOP){
		Toggle_Pins = !Toggle_Pins;
		TOGGLE_STATUS_PINS(Toggle_Pins);

		/* Apagar LEDs de indicacao de transmissao */
		HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, LOW);
		HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, LOW);

		CONT_LOOP = 0;
		if (feedback_multiple){
			Multiple_Feedback();
		}
	}
}

/**------------------------------------------------------------------------------------
  * @brief  Inicializacao de parametros, verificar memoria flash.
  *
  * @param  none
  *
  * @retval void
  */
void inicializar_parametros(){

	char* token1;

	char ID_READ[30];
	char TIMEOUT_PING_CHAR_READ[8];
	char DUTY_CYCLE_FE_CHAR_READ[8];
	char DUTY_CYCLE_FE_MIN_CHAR_READ[8];
	char DUTY_CYCLE_FR_CHAR_READ[8];
	char DUTY_CYCLE_FR_MIN_CHAR_READ[8];
	char DUTY_CYCLE_AC_CHAR_READ[8];
	char DUTY_CYCLE_AC_MAX_CHAR_READ[8];
	char DUTYFE_CHAR_READ[4];
	char DUTYACEL_CHAR_READ[4];
	char DUTYFR_CHAR_READ[4];
	char NUMBER_FEEDBACKS_CHAR_READ[4];

	uint32_t TIMEOUT_PING_INT_READ;
	float DUTY_CYCLE_FE_INT_READ;
	float DUTY_CYCLE_FE_MIN_INT_READ;
	float DUTY_CYCLE_FR_INT_READ;
	float DUTY_CYCLE_FR_MIN_INT_READ;
	float DUTY_CYCLE_AC_INT_READ;
	float DUTY_CYCLE_AC_MAX_INT_READ;
	uint8_t DUTYFE_INT_READ;
	uint8_t DUTYACEL_INT_READ;
	uint8_t DUTYFR_INT_READ;
	uint8_t NUMBER_FEEDBACKS_INT_READ;

	FLASH_le_string_0xFF(END_STG, BUFFER_FLASH_L);

	if (strlen(BUFFER_FLASH_L)){

		/* Se ha dados na memoria flash */

		escreve_USB("\n\r\n\rDados armazenados na Flash:\n\r");
		escreve_USB(BUFFER_FLASH_L);

		/* Capturar dados */
		token1 = strtok(BUFFER_FLASH_L, ","); strcpy(ID_READ, token1);
		token1 = strtok(NULL, ","); strcpy(TIMEOUT_PING_CHAR_READ, token1);
		token1 = strtok(NULL, ","); strcpy(DUTY_CYCLE_FE_CHAR_READ, token1);
		token1 = strtok(NULL, ","); strcpy(DUTY_CYCLE_FE_MIN_CHAR_READ, token1);
		token1 = strtok(NULL, ","); strcpy(DUTY_CYCLE_FR_CHAR_READ, token1);
		token1 = strtok(NULL, ","); strcpy(DUTY_CYCLE_FR_MIN_CHAR_READ, token1);
		token1 = strtok(NULL, ","); strcpy(DUTY_CYCLE_AC_CHAR_READ, token1);
		token1 = strtok(NULL, ","); strcpy(DUTY_CYCLE_AC_MAX_CHAR_READ, token1);
		token1 = strtok(NULL, ","); strcpy(DUTYFE_CHAR_READ, token1);
		token1 = strtok(NULL, ","); strcpy(DUTYACEL_CHAR_READ, token1);
		token1 = strtok(NULL, ","); strcpy(DUTYFR_CHAR_READ, token1);
		token1 = strtok(NULL, ","); strcpy(NUMBER_FEEDBACKS_CHAR_READ, token1);

		TIMEOUT_PING_INT_READ = atoi(TIMEOUT_PING_CHAR_READ);
		DUTY_CYCLE_FE_INT_READ = atof(DUTY_CYCLE_FE_CHAR_READ);
		DUTY_CYCLE_FE_MIN_INT_READ = atof(DUTY_CYCLE_FE_MIN_CHAR_READ);
		DUTY_CYCLE_FR_INT_READ = atof(DUTY_CYCLE_FR_CHAR_READ);
		DUTY_CYCLE_FR_MIN_INT_READ = atof(DUTY_CYCLE_FR_MIN_CHAR_READ);
		DUTY_CYCLE_AC_INT_READ = atof(DUTY_CYCLE_AC_CHAR_READ);
		DUTY_CYCLE_AC_MAX_INT_READ = atof(DUTY_CYCLE_AC_MAX_CHAR_READ);
		DUTYFE_INT_READ = atoi(DUTYFE_CHAR_READ);
		DUTYACEL_INT_READ = atoi(DUTYACEL_CHAR_READ);
		DUTYFR_INT_READ = atoi(DUTYFR_CHAR_READ);
		NUMBER_FEEDBACKS_INT_READ = atoi(NUMBER_FEEDBACKS_CHAR_READ);

		/* Verificar se dados nao estao corrompidos */
		if(strlen(ID_READ) < 20 && strlen(ID_READ) > 0){ memset(ID,0,strlen(ID)); strcat(ID, ID_READ); }
		if(!((TIMEOUT_PING_INT_READ > 10000) || (TIMEOUT_PING_INT_READ < 1000))){ TIMEOUT_PING = TIMEOUT_PING_INT_READ;}
		if(!((DUTY_CYCLE_FE_INT_READ > 100) || (DUTY_CYCLE_FE_INT_READ < 60))){ DUTY_CYCLE_FE = DUTY_CYCLE_FE_INT_READ;}
		if(!((DUTY_CYCLE_FE_MIN_INT_READ > 40) || (DUTY_CYCLE_FE_MIN_INT_READ <= 0))){ DUTY_CYCLE_FE_MIN = DUTY_CYCLE_FE_MIN_INT_READ;}
		if(!((DUTY_CYCLE_FR_INT_READ > 100) || (DUTY_CYCLE_FR_INT_READ < 50))){ DUTY_CYCLE_FR = DUTY_CYCLE_FR_INT_READ;}
		if(!((DUTY_CYCLE_FR_MIN_INT_READ > 50) || (DUTY_CYCLE_FR_MIN_INT_READ <= 0))){ DUTY_CYCLE_FR_MIN = DUTY_CYCLE_FR_MIN_INT_READ;}
		if(!((DUTY_CYCLE_AC_INT_READ > 40) || (DUTY_CYCLE_AC_INT_READ <= 0))){ DUTY_CYCLE_AC = DUTY_CYCLE_AC_INT_READ;}
		if(!((DUTY_CYCLE_AC_MAX_INT_READ > 100) || (DUTY_CYCLE_AC_MAX_INT_READ < 30))){ DUTY_CYCLE_AC_MAX = DUTY_CYCLE_AC_MAX_INT_READ;}
		if(!((DUTYFE_INT_READ > 100) || (DUTYFE_INT_READ < 50))){ DUTYFE = DUTYFE_INT_READ;}
		if(!((DUTYACEL_INT_READ > 50) || (DUTYACEL_INT_READ < 0))){ DUTYACEL = DUTYACEL_INT_READ;}
		if(!((DUTYFR_INT_READ > 100) || (DUTYFR_INT_READ < 50))){ DUTYFR = DUTYFR_INT_READ;}
		if(!((NUMBER_FEEDBACKS_INT_READ > 30) || (NUMBER_FEEDBACKS_INT_READ <= 0))){ NUMBER_OF_FEEDBACKS = NUMBER_FEEDBACKS_INT_READ;}

	}

	/* Apagar as variaveis CHAR atuais */
	memset(TIMEOUT_PING_CHAR, 0, strlen(TIMEOUT_PING_CHAR));
	memset(DUTY_CYCLE_FE_CHAR, 0, strlen(DUTY_CYCLE_FE_CHAR));
	memset(DUTY_CYCLE_FE_MIN_CHAR, 0, strlen(DUTY_CYCLE_FE_MIN_CHAR));
	memset(DUTY_CYCLE_FR_CHAR, 0, strlen(DUTY_CYCLE_FR_CHAR));
	memset(DUTY_CYCLE_FR_MIN_CHAR, 0, strlen(DUTY_CYCLE_FR_MIN_CHAR));
	memset(DUTY_CYCLE_AC_CHAR, 0, strlen(DUTY_CYCLE_AC_CHAR));
	memset(DUTY_CYCLE_AC_MAX_CHAR, 0, strlen(DUTY_CYCLE_AC_MAX_CHAR));
	memset(DUTYFE_CHAR, 0, strlen(DUTYFE_CHAR));
	memset(DUTYACEL_CHAR, 0, strlen(DUTYACEL_CHAR));
	memset(DUTYFR_CHAR, 0, strlen(DUTYFR_CHAR));
	memset(NUMBER_OF_FEEDBACKS_CHAR, 0, strlen(NUMBER_OF_FEEDBACKS_CHAR));

	/* Atualizar as variaveis CHAR */
	sprintf(TIMEOUT_PING_CHAR, "%d", TIMEOUT_PING);
	sprintf(DUTYFE_CHAR, "%d", DUTYFE);
	sprintf(DUTYACEL_CHAR, "%d", DUTYACEL);
	sprintf(DUTYFR_CHAR, "%d", DUTYFR);
	sprintf(NUMBER_OF_FEEDBACKS_CHAR, "%d", NUMBER_OF_FEEDBACKS);

	ftoa(DUTY_CYCLE_FE, DUTY_CYCLE_FE_CHAR, 2);
	ftoa(DUTY_CYCLE_FE_MIN, DUTY_CYCLE_FE_MIN_CHAR, 2);
	ftoa(DUTY_CYCLE_FR, DUTY_CYCLE_FR_CHAR, 2);
	ftoa(DUTY_CYCLE_FR_MIN, DUTY_CYCLE_FR_MIN_CHAR, 2);
	ftoa(DUTY_CYCLE_AC, DUTY_CYCLE_AC_CHAR, 2);
	ftoa(DUTY_CYCLE_AC_MAX, DUTY_CYCLE_AC_MAX_CHAR, 2);

}

/**------------------------------------------------------------------------------------
  * @brief  Atuacao dos PWMs: freio emergencia, acelerador e freio retardo.
  *
  * @param  none
  *
  * @retval void
  */
void atuar_PWMs(){

	htim3.Instance -> CCR1 = PWM_FE;
	htim3.Instance -> CCR2 = PWM_AC;
	htim4.Instance -> CCR1 = PWM_FR;

}

/**------------------------------------------------------------------------------------
  * @brief  Enviar por serial o horario de agora, desde que o sistema foi ligado.
  *
  * @param  none
  *
  * @retval void
  */
void show_now(){
	millis = HAL_GetTick();
	MILLIS_INFORMAR_HORARIO(millis);
}


/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  /* Aguardar inicializacao do CPU */
  while(HAL_GetTick() < 27000);
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  MX_UART4_Init();
  MX_USART3_UART_Init();
  MX_TIM16_Init();
  /* USER CODE BEGIN 2 */

  /* INICIALIZAR SAIDAS GPIO DIGITAIS */
  Init_Output_Pins();

  /* Verificar dados salvos na flash */
  inicializar_parametros();

  /* PRESENT FIRMWARE INFORMATION */
  FIRMWARE_BEGIN();

  /* GERAR FRAMES DE COMUNICACAO COM O RADIO */
  GerarFrames();

  /* Largura de pulso dos PWMs */
  PWM_DEFINITION();

  /* Indicacao Reset */
  HAL_UART_Transmit_IT(&huart4, "\n\r=========== !!! SYSTEM STATUS: ESP...OK,"
		  " UPA...OK, CPU...OK. !!! ===========\r\n", 78);

  /* Inicializar PWMs */
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
  HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);

  atuar_PWMs();

  /* DAR INICIO AO TIMER 16 para monitoramento */
  HAL_TIM_Base_Start_IT(&htim16);

  /* Receber dados do radio por serial */
  HAL_UART_Receive_IT(&huart3, Data, 1);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  /* LEITURA DOS PINOS DIGITAIS */
	  LEITURA_INPUT_PINS();

	  /* GERACAO DO SINAL STATUS */
	  TOGGLE_PWM_STATUS();

	  /* AJUSTE FREIO RETARDO */
	  if (GATE_FR){
		  AJUSTAR_DUTY_CYCLE_FR();
	  }

	  /* Aguardar solicitacoes do usuario */
	  Handle_user(100 /*time ms*/);

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 36;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 720-1;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 200-1;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */
  HAL_TIM_MspPostInit(&htim3);

}

/**
  * @brief TIM4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM4_Init(void)
{

  /* USER CODE BEGIN TIM4_Init 0 */

  /* USER CODE END TIM4_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM4_Init 1 */

  /* USER CODE END TIM4_Init 1 */
  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 720-1;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 200-1;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim4, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM4_Init 2 */

  /* USER CODE END TIM4_Init 2 */
  HAL_TIM_MspPostInit(&htim4);

}

/**
  * @brief TIM16 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM16_Init(void)
{

  /* USER CODE BEGIN TIM16_Init 0 */

  /* USER CODE END TIM16_Init 0 */

  /* USER CODE BEGIN TIM16_Init 1 */

  /* USER CODE END TIM16_Init 1 */
  htim16.Instance = TIM16;
  htim16.Init.Prescaler = 720-1;
  htim16.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim16.Init.Period = 100-1;
  htim16.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim16.Init.RepetitionCounter = 0;
  htim16.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim16) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM16_Init 2 */

  /* USER CODE END TIM16_Init 2 */

}

/**
  * @brief UART4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_UART4_Init(void)
{

  /* USER CODE BEGIN UART4_Init 0 */

  /* USER CODE END UART4_Init 0 */

  /* USER CODE BEGIN UART4_Init 1 */

  /* USER CODE END UART4_Init 1 */
  huart4.Instance = UART4;
  huart4.Init.BaudRate = 9600;
  huart4.Init.WordLength = UART_WORDLENGTH_8B;
  huart4.Init.StopBits = UART_STOPBITS_1;
  huart4.Init.Parity = UART_PARITY_NONE;
  huart4.Init.Mode = UART_MODE_TX_RX;
  huart4.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart4.Init.OverSampling = UART_OVERSAMPLING_16;
  huart4.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart4.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN UART4_Init 2 */

  /* USER CODE END UART4_Init 2 */

}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 9600;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  huart3.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart3.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, CPU_OUT_C_Pin|ON_OFF_SWITCH_Pin|B_SWITCH_Pin|A_SWITCH_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, OUT_PWM_ESP_Pin|RELE_FREIO_STM_Pin|CPU_OUT_B_Pin|PWM_OUT_UPA_Pin
                          |CPU_OUT_A_Pin|PWM_OUT_CPU_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, IO_OUT_ESP_1_Pin|FEEDBACK_CPU_Pin|LED_GREEN_Pin|LED_RED_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : CHECK_FONTE_Pin */
  GPIO_InitStruct.Pin = CHECK_FONTE_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(CHECK_FONTE_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : CPU_OUT_C_Pin ON_OFF_SWITCH_Pin B_SWITCH_Pin A_SWITCH_Pin */
  GPIO_InitStruct.Pin = CPU_OUT_C_Pin|ON_OFF_SWITCH_Pin|B_SWITCH_Pin|A_SWITCH_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : IN_PWM_ESP_Pin */
  GPIO_InitStruct.Pin = IN_PWM_ESP_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(IN_PWM_ESP_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : OUT_PWM_ESP_Pin RELE_FREIO_STM_Pin CPU_OUT_B_Pin PWM_OUT_UPA_Pin
                           CPU_OUT_A_Pin PWM_OUT_CPU_Pin */
  GPIO_InitStruct.Pin = OUT_PWM_ESP_Pin|RELE_FREIO_STM_Pin|CPU_OUT_B_Pin|PWM_OUT_UPA_Pin
                          |CPU_OUT_A_Pin|PWM_OUT_CPU_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : IO_OUT_ESP_1_Pin FEEDBACK_CPU_Pin LED_GREEN_Pin LED_RED_Pin */
  GPIO_InitStruct.Pin = IO_OUT_ESP_1_Pin|FEEDBACK_CPU_Pin|LED_GREEN_Pin|LED_RED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : IO_OUT_ESP_2_Pin */
  GPIO_InitStruct.Pin = IO_OUT_ESP_2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(IO_OUT_ESP_2_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : ESP_IN_CLIENT_Pin IN_EXTRA_3_Pin IN_EXTRA_2_Pin CONNECT_ESP_IN_Pin */
  GPIO_InitStruct.Pin = ESP_IN_CLIENT_Pin|IN_EXTRA_3_Pin|IN_EXTRA_2_Pin|CONNECT_ESP_IN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : IN_EXTRA_1_Pin IN_CPU_7_Pin FR_BIT_2_Pin FR_BIT_1_Pin
                           GATE_FR_Pin */
  GPIO_InitStruct.Pin = IN_EXTRA_1_Pin|IN_CPU_7_Pin|FR_BIT_2_Pin|FR_BIT_1_Pin
                          |GATE_FR_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : IN_PWM_UPA_Pin */
  GPIO_InitStruct.Pin = IN_PWM_UPA_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(IN_PWM_UPA_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : IN_CPU_6_Pin */
  GPIO_InitStruct.Pin = IN_CPU_6_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(IN_CPU_6_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : FR_BIT_4_Pin FR_BIT_3_Pin */
  GPIO_InitStruct.Pin = FR_BIT_4_Pin|FR_BIT_3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : IN_PWM_CPU_Pin */
  GPIO_InitStruct.Pin = IN_PWM_CPU_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(IN_PWM_CPU_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);

  HAL_NVIC_SetPriority(EXTI1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI1_IRQn);

  HAL_NVIC_SetPriority(EXTI2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI2_IRQn);

  HAL_NVIC_SetPriority(EXTI3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI3_IRQn);

  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
