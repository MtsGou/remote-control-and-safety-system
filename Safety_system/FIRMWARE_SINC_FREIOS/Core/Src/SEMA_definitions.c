/*
 * SEMA_definitions.c
 *
 *  Created on: 30 de ago de 2022
 *      Author: matheus.gouveia
 */

/**
  ******************************************************************************
  * @file    SEMA_definitions.c
  * @author  Matheus Gouveia
  * @brief   Esse arquivo define as variaveis globais, alem de funcoes,
  * 		 para  a aplicacao do SEMA.
  *
  ******************************************************************************
  * @attention
  *
  * Todos os direitos preservados.
  *
  *
  ==============================================================================
               ##### VARIAVEIS GLOBAIS E FUNCOES DO SEMA #####
  ==============================================================================
  [..]
    (+) Funcao MONITORAMENTO(int IDENTIFIER):
        (++) Verifica se esta tudo OK (IDENTIFIER = 0).
        (++) Armazena historico -> IDENTIFIER_ANTERIOR.
        (++) Adverte se houve mudancas (se difere do estado anterior).
        (++) Se houve falha UPA ou CPU, freia (IDENTIFIER > 1).
        (++) Informa status do sistema geral para CPU por GPIO.

    (+) Funcao FIRMWARE_BEGIN(void):
    	(++) Mostra informacoes do firmware na inicializacao.

    (+) Funcao SEND_STATUS_TO_CPU(int which):
    	(++) Define saidas GPIO para informar status do sistema.
    	(++) GPIO output -> CPU input GPIO.

  ******************************************************************************
  */

#include <SEMA_definitions.h>
#include <transmissao_dados.h>
#include "main.h"

TIM_HandleTypeDef htim3;		/* Timer PWMs freio emergencie e acelerador */
TIM_HandleTypeDef htim4;		/* Timer PWM freio retardo */
TIM_HandleTypeDef htim16;		/* Timer para funcao callback overflow */

UART_HandleTypeDef huart4;		/* UART 4 - USB e comunicacao serial monitor */
UART_HandleTypeDef huart3;		/* UART 3 - comunicacao serial com radio */

//////////////////////////////////////////////////////////////////////////////////////
////////////////////// VARIAVEIS GLOBAIS - GLOBAL VARIABLES //////////////////////////
///

/*******************************************************************************
 ************************** Dados do protocolo *********************************
 *******************************************************************************/

/* Start Byte */
const uint8_t start_byte[2] = "@";

/* Byte comando - RELE DE ACIONAMENTO DO FREIO DE EMERGENCIA (ON) */
const uint8_t ON_COMMAND[2] = "1";

/* Byte comando - RELE DE ACIONAMENTO DO FREIO DE EMERGENCIA (OFF) */
const uint8_t OFF_COMMAND[2] = "0";

/* Divisoria Split */
const uint8_t DIV[2] = ",";

/* Confirmacao Feedback */
const uint8_t Confirmation[3] = "OK";

/* Divisoria para capturar Checksum */
const uint8_t CHECKSUM_DIV[2] = "*";

/* End Byte */
const uint8_t end_byte[2] = "?";

/* Dado mensagem PING/PONG */
const uint8_t PING_DATA[2] = "X";

/* Identificacao Equipamento */
uint8_t ID[7] = "CR2100";

/* Comando - Freio de Emergencia (FE)*/
uint8_t ID_COMMAND[3] = "FE";

/**************************************************************************
 ***************** Buffers para transmissao de dados **********************
 **************************************************************************/

/* Frames de envio */
uint8_t FrameOn_A[30];
uint8_t FrameOff_A[30];
uint8_t Frame_PING_A[30];

/* Frames de feedback */
uint8_t FrameOn_B[30];
uint8_t FrameOff_B[30];
uint8_t Frame_PING_B[30];

/* Buffer para envio de dados ao radio transmissor */
uint8_t BUFFER_TRANSMIT[255];

/* Receber um dado por vez do radio transmissor */
uint8_t Data[1];

/* Buffer de recebimento de dados do radio transmissor */
uint8_t BUFFER_RECEIVE[255];

/* BUFFER PARA MONITOR SERIAL E DEBUG */
uint16_t BUFFER_SERIAL[255];

/* BUFFER PARA RECEBER DADOS USB */
char BUFFER_RECEIVE_USB[1];

/***************************************************************************
 ******************    Buffers para salvar na Flash   **********************
 ***************************************************************************/

/* Buffer Flash - escrever e ler */
char BUFFER_FLASH[300];
char BUFFER_FLASH_L[300];

/* Dados configuraveis em formato CHAR
 * para concatenar buffer flash */
char DUTY_CYCLE_FE_CHAR[8];
char DUTY_CYCLE_FE_MIN_CHAR[8];

char DUTY_CYCLE_AC_CHAR[8];
char DUTY_CYCLE_AC_MAX_CHAR[8];

char DUTY_CYCLE_FR_CHAR[8];
char DUTY_CYCLE_FR_MIN_CHAR[8];

char DUTYFE_CHAR[4], DUTYACEL_CHAR[4]
, DUTYFR_CHAR[4];

char TIMEOUT_PING_CHAR[8];

char NUMBER_OF_FEEDBACKS_CHAR[4];

/*************************************************************************
 ********************** Demais variaveis Globais *************************
 *************************************************************************/

/* Duty Cycle Maximo do Freio Emergencia */
volatile float DUTY_CYCLE_FE = PWM_FE_MAX;

/* Duty Cycle Minimo do Freio Emergencia */
volatile float DUTY_CYCLE_FE_MIN = PWM_FE_MIN;

/* PWM Freio de emergencia - atuacao timer */
volatile int PWM_FE;

/* Duty Cycle Minimo do acelerador */
volatile float DUTY_CYCLE_AC = PWM_AC_MIN;

/* Duty Cycle Maximo do acelerador */
volatile float DUTY_CYCLE_AC_MAX = PWM_AC_MAX;

/* PWM Acelerador - atuacao timer */
volatile int PWM_AC;

/* Duty Cycle Maximo do Freio de retardo */
volatile float DUTY_CYCLE_FR = PWM_FR_MAX;

/* Duty Cycle Minimo do Freio de retardo */
volatile float DUTY_CYCLE_FR_MIN = PWM_FR_MIN;

/* PWM Freio de retardo - atuacao timer */
volatile int PWM_FR;

/* Duty cycle efetivos */
volatile float DUTYCYCLE_FE, DUTYCYCLE_AC, DUTYCYCLE_FR;

/* Percentual do duty cycle dentro dos limites */
volatile uint8_t DUTYFE = PERCENT_FE;
volatile uint8_t DUTYACEL = PERCENT_AC;
volatile uint8_t DUTYFR = PERCENT_FR;

/* CRC/checksum a ser calculado e enviado */
uint8_t CRC_checksum = 0;

/* String que armazena o CRC/checksum calculado */
char CRC_STR_X[8];

/* Variavel responsavel pelo acionamento
 * e que informa o status atual do FE */
volatile int BREAK_STATUS = 0;

/* Contador para TOGGLE_PWM_STATUS() */
/*unsigned long*/
int CONT_LOOP = 0;

/* Valor maximo do CONT_LOOP para enviar status */
/* OBS.: created as 'unsigned long' without menu
 * because without menu the value could be very big*/
/*unsigned long MAX_LOOP*/

/* Variavel para Toggle_Pins TOGGLE_STATUS_PINS(bool STATUS)*/
bool Toggle_Pins = false;

/* Contadores FLAG funcao callback timer 16*/
uint32_t FLAG_SOMA_UPA = 0;
uint32_t FLAG_SOMA_ESP = 0;
uint32_t FLAG_SOMA_CPU = 0;
uint32_t FLAG_XBEE_PING = 0;

/* Limite TIMEOUT dos contadores FLAG */
uint32_t TIMEOUT_PING = PING_TIMEOUT_STD;

/* INPUT VINDO DO CPU PARA AJUSTE DO SINAL DO FREIO DE RETARDO */
bool GATE_FR = 0;

/* BITS DE AJUSTE DO FR PELO CPU */
int CONTROLE_FR[4] = {0,0,0,0};

/* STATUS DA CONEXAO A REDE DO ESP */
bool STATUS_CONNECTION_ESP = 1;

/* STATUS DOS CLIENTES CONECTADOS AO SERVIDOR SOCKET */
bool STATUS_CLIENTS_ESP = 0;

/* DADOS A SEREM ENVIADOS AO CPU SOBRE O ESP, O STM E A UPA */
int STATUS_PARA_CPU[3] = {0,0,0};

/* VARIAVEL QUE INFORMA O TEMPO DE EXECUCAO EM MILISSEGUNDOS */
uint32_t millis = 0;

/* VARIAVEL PARA ATIVAR ENVIO DE MULTIPLOS FEEDBACKs */
bool feedback_multiple = 0;

/* Numero de feedbacks RF desejado */
uint8_t NUMBER_OF_FEEDBACKS = FEEDBACK_NUM_STD;

/* Maquina de estados */
/* FSM_t sys_state; */

//////////////////////////////////////////////////////////////////////////////////////
////////////////////// FUNCOES DO SISTEMA - SYSTEM FUNCTIONS /////////////////////////
///

/**------------------------------------------------------------------------------------
  * @brief Apresentacao serial para monitor, info firmware version.
  *
  * @param  none
  *
  * @retval void
  */
void FIRMWARE_BEGIN(){
  escreve_USB("\r\nRESET\r\n");
  escreve_USB("\n\rWELCOME to ");
  escreve_USB(SIGLA);
  escreve_USB("!\n\r \n\r");
  escreve_USB(MEANING_1);
  escreve_USB("\n\r");
  escreve_USB(MEANING_2);
  escreve_USB("\n\r\n\r");
  escreve_USB("MCU info:  ");
  escreve_USB(MCU_INFO);
  escreve_USB("\n\rEquipment ID: ");
  escreve_USB(ID);
  escreve_USB("\n\rVersao:  ");
  escreve_USB(VERSION);
  escreve_USB("          FIRMWARE DATE: ");
  escreve_USB(FIRMWARE_DATE);
  escreve_USB("\n\r\n\r");
  escreve_USB(COPYRIGHT);
  escreve_USB("\n\r");
}

/**------------------------------------------------------------------------------------
  * @brief  SEMA: Monitoramento do sistema. Identificacao de falhas, monitorar rede,
  * 			 informar o CPU - status, atuar freio e informar
  * 			 por serial as ocorrencias.
  *
  * @param  IDENTIFIER:	Identificador do status do sistema. 0 -> Sem falhas.
  *
  * @retval void
  */
void MONITORAMENTO(int IDENTIFIER){

	/* ESSA VARIAVEL ADQUIRE O VALOR DE 1 QUANDO OCORRE UMA FALHA */
	static int NOTIFICADOR_FALHA = 0;

	/* ESSA VARIAVEL INFORMA SE ESP E UPA (AMBOS) ESTAO OK */
	static int ESP_UPA_OK = 1;

	/* VALOR ANTERIOR IDENTIFIER */
	static int IDENTIFIER_ANTERIOR = 0;

	/* Verificar estado do ESP e da UPA */
	switch (IDENTIFIER){
	case 0:		/* Sistema sem falhas */
		NOTIFICADOR_FALHA = 0;
		ESP_UPA_OK = 1;
		if (!IDENTIFIER_ANTERIOR){}
		else if (IDENTIFIER_ANTERIOR > 0){
			show_now();
			SYS_OK_RECOVERED(IDENTIFIER_ANTERIOR);
		}
		/* else {
			strcat(BUFFER_SERIAL, "\r\nERRO! IDENTIFIER returned -1 or NaN\r\n");
			break;
		}*/
		break;
	case 5:
		/* Apenas CPU falhou */
		ESP_UPA_OK = 1;
		break;
	default:
		ESP_UPA_OK = 0;
		break;
	}

	/* Se nao houver falhas, monitorar a rede
	 * ou se falhar, verificar apenas mudancas do IDENTIFIER */
	if (!NOTIFICADOR_FALHA || IDENTIFIER_ANTERIOR ^ IDENTIFIER){

		/*A SEGUINTE VERIFICACAO DEVE SER ARBITRARIA: DE ACORDO COM O NIVEL
		 * DE GRAVIDADE DO IDENTIFIER, DECIDIR SE ACIONA OU NAO O FREIO.
		 * VERIFICACAO PADRAO: SE **PELO MENOS UPA** APRESENTAR FALHA (IDENTIFIER > 1).*/
		bool VERIFICACAO_FREIO = (IDENTIFIER > 1);

		if (VERIFICACAO_FREIO == true && !BREAK_STATUS){
			BREAK_STATUS = 1;
			feedback_multiple = 1;
			show_now();
			strcat(BUFFER_SERIAL, "\r\nFREIO ACIONADO.\r\n");
			Acionamento();
		}

		/* Se qualquer falha ocorrer, NOTIFICADOR_FALHA vai para 1 */
		if (IDENTIFIER && !NOTIFICADOR_FALHA){
			NOTIFICADOR_FALHA = 1;
		}

		/* Identificar a falha */
		switch (IDENTIFIER){
		case 0:
			break;
		case 1:
			/* FALHA ESP */
			show_now();
			FAIL_ESP(IDENTIFIER_ANTERIOR);
			break;
		case 2:
			/* FALHA UPA */
			show_now();
			FAIL_UPA(IDENTIFIER_ANTERIOR);
			break;
		case 3:
			/* FALHA ESP e UPA */
			show_now();
			FAIL_ESP_UPA(IDENTIFIER_ANTERIOR);
			break;
		case 5:
			/* FALHA CPU APENAS */
			show_now();
			FAIL_CPU(IDENTIFIER_ANTERIOR);
			break;
		default:
			/* FALHA GENERALIZADA, ja que CPU falhou */
			show_now();
			FAIL_GENERAL(IDENTIFIER);
			break;
		}

		/* VERIFICACOES DE CONEXAO
		 * ESP -> REDE DESPACHO
		 * STM -> RESPOSTA PING ZIGBEE
		 * */

		if (ESP_UPA_OK && IDENTIFIER <= 5) {
			MONITORAR_REDE();
		}
		/* Armazenar ao historico o valor de IDENTIFIER atual */
		if (IDENTIFIER_ANTERIOR ^ IDENTIFIER){
			IDENTIFIER_ANTERIOR = IDENTIFIER;
		}

		/* Atualizar estados logicos do pinos */
		HAL_GPIO_WritePin(CPU_OUT_A_GPIO_Port, CPU_OUT_A_Pin, STATUS_PARA_CPU[0]);
		HAL_GPIO_WritePin(CPU_OUT_B_GPIO_Port, CPU_OUT_B_Pin, STATUS_PARA_CPU[1]);
		HAL_GPIO_WritePin(CPU_OUT_C_GPIO_Port, CPU_OUT_C_Pin, STATUS_PARA_CPU[2]);
	}

	/* DEBUG COMUNICACAO SERIAL */
	COMUNICACAO_SERIAL_DEBUG();

	/* Fim da funcao */
}

/**------------------------------------------------------------------------------------
  * @brief  Definir STATUS para enviar estados logicos ao CPU.
  *
  * @param  which:	Identificador do status do sistema.
  *
  * @retval void
  */
void SEND_STATUS_TO_CPU(int which){

	switch(which){
	case ESP_OK_UPA_OK_CPU_FAIL: 	/*ESP OK, UPA OK, CPU FAIL*/
		break;

	case ESP_OK_UPA_FAIL_CPU_OK:	/*ESP OK, UPA FAIL, CPU OK*/
		STATUS_PARA_CPU[0] = 1;
		STATUS_PARA_CPU[1] = 1;
		STATUS_PARA_CPU[2] = 0;
		break;

	case ESP_OK_UPA_FAIL_CPU_FAIL: 	/*ESP OK, UPA FAIL, CPU FAIL*/
		STATUS_PARA_CPU[0] = 1;
		STATUS_PARA_CPU[1] = 1;
		STATUS_PARA_CPU[2] = 0;
		break;

	case ESP_FAIL_UPA_OK_CPU_OK: 	/*ESP FAIL, UPA OK, CPU OK*/
		STATUS_PARA_CPU[0] = 1;
		STATUS_PARA_CPU[1] = 0;
		STATUS_PARA_CPU[2] = 1;
		break;

	case ESP_FAIL_UPA_OK_CPU_FAIL: 	/*ESP FAIL, UPA OK, CPU FAIL*/
		STATUS_PARA_CPU[0] = 1;
		STATUS_PARA_CPU[1] = 0;
		STATUS_PARA_CPU[2] = 1;
		break;

	case ESP_FAIL_UPA_FAIL_CPU_OK: 	/*ESP FAIL, UPA FAIL, CPU OK*/
		STATUS_PARA_CPU[0] = 1;
		STATUS_PARA_CPU[1] = 1;
		STATUS_PARA_CPU[2] = 1;
		break;

	case ALL_FAIL: 					/*ESP FAIL, UPA FAIL, CPU FAIL*/
		STATUS_PARA_CPU[0] = 1;
		STATUS_PARA_CPU[1] = 1;
		STATUS_PARA_CPU[2] = 1;
		break;

	case STM_ESP_CNCT: 				/*ESP CONNECTED, STM CONNECTED*/
		STATUS_PARA_CPU[0] = 0;
		STATUS_PARA_CPU[1] = 0;
		STATUS_PARA_CPU[2] = 1;
		break;

	case STM_CNCT_ESP_N_CNCT: 		/*ESP NOT CONNECTED, STM CONNECTED*/
		STATUS_PARA_CPU[0] = 0;
		STATUS_PARA_CPU[1] = 1;
		STATUS_PARA_CPU[2] = 0;
		break;

	case STM_N_CNCT_ESP_CNCT: 		/*ESP CONNECTED, STM NOT CONNECTED*/
		STATUS_PARA_CPU[0] = 0;
		STATUS_PARA_CPU[1] = 1;
		STATUS_PARA_CPU[2] = 1;
		break;

	case STM_ESP_N_CNCT: 			/*ESP NOT CONNECTED, STM NOT CONNECTED*/
		STATUS_PARA_CPU[0] = 1;
		STATUS_PARA_CPU[1] = 0;
		STATUS_PARA_CPU[2] = 0;
		break;
	}
}

/**------------------------------------------------------------------------------------
  * @brief  Monitorar a rede e informar CPU.
  *
  * @param  none
  *
  * @retval void
  */
void MONITORAR_REDE(){

	/* STM CONECTADO E ESP CONECTADO */
	if (FLAG_XBEE_PING <= TIMEOUT_PING && STATUS_CONNECTION_ESP){
		SEND_STATUS_TO_CPU(STM_ESP_CNCT);
	}

	/* STM CONECTADO E ESP NAO CONECTADO */
	if (FLAG_XBEE_PING <= TIMEOUT_PING && !STATUS_CONNECTION_ESP){
		SEND_STATUS_TO_CPU(STM_CNCT_ESP_N_CNCT);
	}

	/* STM NAO CONECTADO E ESP CONECTADO */
	if (FLAG_XBEE_PING > TIMEOUT_PING && STATUS_CONNECTION_ESP){
		SEND_STATUS_TO_CPU(STM_N_CNCT_ESP_CNCT);
	}

	/* STM NAO CONECTADO E ESP NAO CONECTADO */
	if (FLAG_XBEE_PING > TIMEOUT_PING && !STATUS_CONNECTION_ESP){
		SEND_STATUS_TO_CPU(STM_ESP_N_CNCT);
	}
}


/**------------------------------------------------------------------------------------
  * @brief  Informar por serial falha ESP e enviar estado para CPU.
  *
  * @param  __ESTADO_ANTERIOR estado anterior
  *
  * @retval void
  */
void FAIL_ESP(uint8_t __ESTADO_ANTERIOR){

	if (!__ESTADO_ANTERIOR){
		strcat(BUFFER_SERIAL, "\r\nFALHA ESP.\r\n");
	}
	else if (!(__ESTADO_ANTERIOR ^ 3)){
		strcat(BUFFER_SERIAL, "\r\nCOMUNICACAO COM UPA RECUPERADA.\r\n");
	}
	else if (!(__ESTADO_ANTERIOR ^ 6)){
		strcat(BUFFER_SERIAL, "\r\nCOMUNICACAO COM CPU RECUPERADA.\r\n");
	}
	else if (!(__ESTADO_ANTERIOR ^ 8)){
		strcat(BUFFER_SERIAL, "\r\nCOMUNICACAO COM CPU E UPA RECUPERADA.\r\n");
	}
	strcat(BUFFER_SERIAL, "\r\n=========== !!! SYSTEM STATUS: ESP...FAIL,"
			" UPA...OK, CPU...OK. !!! ===========\r\n");
	SEND_STATUS_TO_CPU(ESP_FAIL_UPA_OK_CPU_OK);
}

/**------------------------------------------------------------------------------------
  * @brief  Informar por serial falha UPA e enviar estado para CPU.
  *
  * @param  __ESTADO_ANTERIOR estado anterior
  *
  * @retval void
  */
void FAIL_UPA(uint8_t __ESTADO_ANTERIOR){

	if (__ESTADO_ANTERIOR < 2){
		strcat(BUFFER_SERIAL, "\r\nFALHA UPA.\r\n");
	}
	else if (!(__ESTADO_ANTERIOR ^ 3)){
		strcat(BUFFER_SERIAL, "\r\nCOMUNICACAO COM ESP RECUPERADA.\r\n");
	}
	else if (!(__ESTADO_ANTERIOR ^ 7)){
		strcat(BUFFER_SERIAL, "\r\nCOMUNICACAO COM CPU RECUPERADA.\r\n");
	}
	else if (!(__ESTADO_ANTERIOR ^ 8)){
		strcat(BUFFER_SERIAL, "\r\nCOMUNICACAO COM CPU E ESP RECUPERADA.\r\n");
	}
	strcat(BUFFER_SERIAL, "\r\n=========== !!! SYSTEM STATUS: ESP...OK,"
			" UPA...FAIL, CPU...OK. !!! ===========\r\n");
	SEND_STATUS_TO_CPU(ESP_OK_UPA_FAIL_CPU_OK);

}

/**------------------------------------------------------------------------------------
  * @brief  Informar por serial falha ESP e UPA e enviar estado para CPU.
  *
  * @param  _ESTADO_ANTERIOR estado anterior
  *
  * @retval void
  */
void FAIL_ESP_UPA(uint8_t _ESTADO_ANTERIOR){

	if (_ESTADO_ANTERIOR < 3){
		strcat(BUFFER_SERIAL, "\r\nFALHA ESP e UPA.\r\n");
	}
	else if (!(_ESTADO_ANTERIOR ^ 8)){
		strcat(BUFFER_SERIAL, "\r\nCOMUNICACAO COM CPU RECUPERADA.\r\n");
	}
	strcat(BUFFER_SERIAL, "\r\n=========== !!! SYSTEM STATUS: ESP...FAIL,"
			" UPA...FAIL, CPU...OK. !!! ===========\r\n");
	SEND_STATUS_TO_CPU(ESP_FAIL_UPA_FAIL_CPU_OK);
}

/**------------------------------------------------------------------------------------
  * @brief  Informar por serial falha CPU e enviar estado para CPU.
  *
  * @param  ESTADO_ANTERIOR estado anterior
  *
  * @retval void
  */
void FAIL_CPU(uint8_t ESTADO_ANTERIOR){

	if (ESTADO_ANTERIOR < 5){
		strcat(BUFFER_SERIAL, "\r\nFALHA CPU.\r\n");
	}
	if (ESTADO_ANTERIOR == 7 || ESTADO_ANTERIOR == 8){
		strcat(BUFFER_SERIAL, "\r\nCOMUNICACAO COM UPA RECUPERADA.\r\n");
	}
	if (ESTADO_ANTERIOR == 6 || ESTADO_ANTERIOR == 8){
		strcat(BUFFER_SERIAL, "\r\nCOMUNICACAO COM ESP RECUPERADA.\r\n");
	}
	strcat(BUFFER_SERIAL, "\r\n=========== !!! SYSTEM STATUS: ESP...OK,"
			" UPA...OK, CPU...FAIL. !!! ===========\r\n");

}

/**------------------------------------------------------------------------------------
  * @brief  Informar por serial falha CPU + outro componente e enviar estado para CPU.
  *
  * @param  __ESTADO_ATUAL estado atual de falha
  *
  * @retval void
  */
void FAIL_GENERAL(uint8_t __ESTADO_ATUAL){

	strcat(BUFFER_SERIAL, "\r\nFALHA GENERALIZADA. CPU FALHOU.\r\n");
	if (!(__ESTADO_ATUAL ^ 8)){
		strcat(BUFFER_SERIAL, "\r\nTODOS OS COMPONENTES FALHARAM.\r\n");
		strcat(BUFFER_SERIAL, "\r\n=========== !!! SYSTEM STATUS: ESP..."
				"FAIL, UPA...FAIL, CPU...FAIL. !!! ===========\r\n");
		SEND_STATUS_TO_CPU(ALL_FAIL);
	}
	if (!(__ESTADO_ATUAL ^ 6)){
		strcat(BUFFER_SERIAL, "\r\nUPA ESTA OK.\r\n");
		strcat(BUFFER_SERIAL, "\r\n=========== !!! SYSTEM STATUS: ESP..."
				"FAIL, UPA...OK, CPU...FAIL. !!! ===========\r\n");
		SEND_STATUS_TO_CPU(ESP_FAIL_UPA_OK_CPU_FAIL);
	}
	if (!(__ESTADO_ATUAL ^ 7)){
		strcat(BUFFER_SERIAL, "\r\nESP ESTA OK.\r\n");
		strcat(BUFFER_SERIAL, "\r\n=========== !!! SYSTEM STATUS: ESP..."
				"OK, UPA...FAIL, CPU...FAIL. !!! =========== \r\n");
		SEND_STATUS_TO_CPU(ESP_OK_UPA_FAIL_CPU_FAIL);
	}

}

/**------------------------------------------------------------------------------------
  * @brief  Informar por serial caso tenha recuperado comunicacao com componentes.
  *
  * @param  FALHA_ANTERIOR falha anterior ao estado ok
  *
  * @retval void
  */
void SYS_OK_RECOVERED(uint8_t FALHA_ANTERIOR){

	switch (FALHA_ANTERIOR){
	case 8:
		strcat(BUFFER_SERIAL, "\r\nCOMUNICACAO "
				"RECUPERADA COM TODOS OS COMPONENTES.\r\n");
		break;
	case 7:
		strcat(BUFFER_SERIAL, "\r\nCOMUNICACAO COM CPU E "
				"UPA RECUPERADA.\r\n");
		break;
	case 6:
		strcat(BUFFER_SERIAL, "\r\nCOMUNICACAO COM CPU E"
				" ESP RECUPERADA.\r\n");
		break;
	case 5:
		strcat(BUFFER_SERIAL, "\r\nCOMUNICACAO COM CPU"
				" RECUPERADA.\r\n");
		break;
	case 3:
		strcat(BUFFER_SERIAL, "\r\nCOMUNICACAO COM UPA"
				" E ESP RECUPERADA.\r\n");
		break;
	case 2:
		strcat(BUFFER_SERIAL, "\r\nCOMUNICACAO COM"
				" UPA RECUPERADA.\r\n");
		break;
	case 1:
		strcat(BUFFER_SERIAL, "\r\nCOMUNICACAO COM"
				" ESP RECUPERADA.\r\n");
		break;
	default:
		break;
	}
	strcat(BUFFER_SERIAL, "\r\n=========== !!! SYSTEM STATUS: ESP...OK, UPA"
			"...OK, CPU...OK. !!! ============\r\n");
}
