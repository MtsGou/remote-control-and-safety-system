/*
 * serial_radio.c
 *
 *  Created on: 16 de ago de 2022
 *      Author: matheus.gouveia
 */

/**
  ******************************************************************************
  * @file    serial_radio.c
  * @author  Matheus Gouveia
  * @brief   Esse arquivo contem as definicoes das funcoes para a
  * 		 comunicacao serial com o radio.
  *
  ******************************************************************************
  */

#include <SEMA_definitions.h>
#include <serial_radio.h>
#include "stm32l4xx_hal_conf.h"
#include "stm32l4xx_it.h"


/**-----------------------------------------------------------------------------------------------------------------------------------
  * @brief Gera frames a serem enviados por serial.
  *
  * @OBS   NESSA APLICACAO EM ESPECIFICO, OS FRAMES SERAO SEMPRE OS MESMOS. MAS ESSA E UMA FORMA MAIS AMPLA
  *  DE GERAR QUALQUER FRAME DE ACORDO COM A MENSAGEM QUE SE QUEIRA ENVIAR, A QUALQUER MOMENTO.
  *
  * @param  none
  *
  * @retval void
  */
void GerarFrames(){

	memset(FrameOn_A, 0, strlen(FrameOn_A));
	memset(FrameOff_A, 0, strlen(FrameOff_A));
	memset(FrameOn_B, 0, strlen(FrameOn_B));
	memset(FrameOff_B, 0, strlen(FrameOff_B));
	memset(Frame_PING_A, 0, strlen(Frame_PING_A));
	memset(Frame_PING_B, 0, strlen(Frame_PING_B));

	/* FRAME COMANDO ON */
	strcat(FrameOn_A, ID);
	strcat(FrameOn_A, DIV);
	strcat(FrameOn_A, ID_COMMAND);
	strcat(FrameOn_A, DIV);
	strcat(FrameOn_A, ON_COMMAND);

	/* FRAME COMANDO OFF */
	strcat(FrameOff_A, ID);
	strcat(FrameOff_A, DIV);
	strcat(FrameOff_A, ID_COMMAND);
	strcat(FrameOff_A, DIV);
	strcat(FrameOff_A, OFF_COMMAND);

	/* FRAME COMING PING */
	strcat(Frame_PING_A, ID);
	strcat(Frame_PING_A, DIV);
	strcat(Frame_PING_A, PING_DATA);

	/* FRAME FEEDBACK ON */
	strcat(FrameOn_B, start_byte);
	strcat(FrameOn_B, ID);
	strcat(FrameOn_B, DIV);
	strcat(FrameOn_B, ID_COMMAND);
	strcat(FrameOn_B, DIV);
	strcat(FrameOn_B, Confirmation);
	strcat(FrameOn_B, ON_COMMAND);
	strcat(FrameOn_B, CHECKSUM_DIV);

	/* FRAME FEEDBACK OFF */
	strcat(FrameOff_B, start_byte);
	strcat(FrameOff_B, ID);
	strcat(FrameOff_B, DIV);
	strcat(FrameOff_B, ID_COMMAND);
	strcat(FrameOff_B, DIV);
	strcat(FrameOff_B, Confirmation);
	strcat(FrameOff_B, OFF_COMMAND);
	strcat(FrameOff_B, CHECKSUM_DIV);

	/* FRAME FEEDBACK PING (PONG) */
	strcat(Frame_PING_B, start_byte);
	strcat(Frame_PING_B, ID);
	strcat(Frame_PING_B, DIV);
	strcat(Frame_PING_B, Confirmation);
	strcat(Frame_PING_B, PING_DATA);
	strcat(Frame_PING_B, CHECKSUM_DIV);

	/* Fim da funcao */
}
/*-----------------------------------------------------------------------------------------------------------------------------------*/


/**-----------------------------------------------------------------------------------------------------------------------------------
  * @brief PREENCHE O BUFFER E CALCULA O RESPECTIVO CHECKSUM, QUE SERA ENVIADO POR SERIAL
  *
  * @param  MODE: Define qual mensagem enviar, qual frame usar.
  *
  * @retval void
  */
void PreencherBuffer(int MODE){

	/* APAGAR BUFFER e CRC_STR_X */
	strcpy(BUFFER_TRANSMIT, "");
	strcpy(CRC_STR_X, "");

	/* Inicializar Checksum em 0 */
	CRC_checksum = 0;

	/* PREENCHER BUFFER de acordo com comando
	 *  0 -> OFF
	 *  1 -> ON
	 *  2-> PING
	 *  3-> BATTERY FAIL
	 *  4-> BATTERY RETURN
	 *  */

	if (MODE == FRAME_ACTUATE_RELAY){
		strcat(BUFFER_TRANSMIT, FrameOn_B);
	}
	else if (!MODE){
		strcat(BUFFER_TRANSMIT, FrameOff_B);
	}
	else if (MODE == FRAME_PING){
		strcat(BUFFER_TRANSMIT, Frame_PING_B);
	}
	else if (MODE == FRAME_BATTERY_FAIL || MODE == FRAME_BATTERY_RETURN){
		strcat(BUFFER_TRANSMIT, start_byte);
		strcat(BUFFER_TRANSMIT, ID);
		strcat(BUFFER_TRANSMIT, DIV);
		strcat(BUFFER_TRANSMIT, "BT");
		strcat(BUFFER_TRANSMIT, DIV);
		if (MODE == FRAME_BATTERY_FAIL){
			strcat(BUFFER_TRANSMIT, OFF_COMMAND);
		}
		else if (MODE == FRAME_BATTERY_RETURN){
			strcat(BUFFER_TRANSMIT, ON_COMMAND);
		}
		strcat(BUFFER_TRANSMIT, CHECKSUM_DIV);
	}

	/* CALCULO DO CHECKSUM */
	for (int index = 1; index < strlen(BUFFER_TRANSMIT)-1; index++){
		CRC_checksum = CRC_checksum ^ BUFFER_TRANSMIT[index];
	}

	/* PASSAR A VARIAVEL INT PARA CHAR */
	sprintf(CRC_STR_X, "%X", CRC_checksum);

	/* ADICIONAR CRC e END BYTE PARA ENVIAR */
	strcat(BUFFER_TRANSMIT, CRC_STR_X);
	strcat(BUFFER_TRANSMIT, end_byte);

	/* Fim da funcao */
}
/*-----------------------------------------------------------------------------------------------------------------------------------*/


/**-----------------------------------------------------------------------------------------------------------------------------------
  * @brief RECEBE O COMANDO DO TRANSMISSOR E VERIFICA A MENSAGEM E O CHECKSUM. Envia feedbacks apos atuar.
  *
  * @param  none
  *
  * @retval void
  */
void VerificarComando(){

	/* BUFFER_COMPARE: A ser comparado com
	 * os frames para identificar o comando.
	 * COMPARE_CHECKSUM: Retorna "0" apenas
	 * se as strings forem iguais.
	 * CRC_RECEIVE_INT: Corresponde ao CHECKSUM
	 * da mensagem recebida (calculado),
	 * variavel inteira.
	 * CRC_RECEIVED: Corresponde ao CRC
	 * recebido do transmissor.
	 * CRC_RECEIVE_STR: CRC_RECEIVE_INT
	 * em formato string.
	 * ACTIVATE_PreencherBuffer: Ativa a
	 * chamada da funcao PrencherBuffer
	 * apenas quando necessario.
	 * */

	if (feedback_multiple){
		HAL_UART_Transmit(&huart3, BUFFER_TRANSMIT, strlen(BUFFER_TRANSMIT),20);
		return;
	}

	char* BUFFER_COMPARE;
	int COMPARE_CHECKSUM = 0;
	int CRC_RECEIVE_INT = 0;
	char* CRC_RECEIVED;
	char CRC_RECEIVE_STR[8];
	static int ACTIVATE_PreencherBuffer = 1;

	BUFFER_COMPARE = BUFFER_RECEIVE;

	/* CAPTURAR BUFFER ATE "*" */
	BUFFER_COMPARE = strtok(BUFFER_COMPARE, CHECKSUM_DIV);

	/* Se estiver vazio, apagar */
	if (BUFFER_COMPARE == NULL){
		BUFFER_COMPARE = "";
	}

	/* CAPTURAR DADOS APOS "*" */
	CRC_RECEIVED = strtok(NULL, "");

	if (CRC_RECEIVED == NULL){
		CRC_RECEIVED = "";
	}

	/* SE O PRIMEIRO CARACTERE E ZERO, O SEGUNDO PASSA A SER O PRIMEIRO */
	if(strncmp(BUFFER_COMPARE, "0", 1) == 0){
		strcpy(BUFFER_COMPARE, &(BUFFER_COMPARE[1]));
	}

	/* CALCULO DO CHECKSUM - CRC */
	for (int index = 0; index < strlen(BUFFER_COMPARE); index++){
		CRC_RECEIVE_INT = CRC_RECEIVE_INT ^ BUFFER_COMPARE[index];
	}

	/* INT PARA CHAR */
	sprintf(CRC_RECEIVE_STR, "%X", CRC_RECEIVE_INT);

	/* COMPARAR O CHECKSUM RECEBIDO COM O CALCULADO */
	COMPARE_CHECKSUM = strcmp(CRC_RECEIVED, CRC_RECEIVE_STR);

	if (COMPARE_CHECKSUM == 0 && CRC_RECEIVED != ""){
		/* Apenas valida se os Checksum forem iguais */
		if (strcmp(BUFFER_COMPARE, FrameOff_A) == 0){
			/* Frame Off - Comando desacionar rele */
			BREAK_STATUS = 0;
			Acionamento();
			ACTIVATE_PreencherBuffer = 1;
		}
		else if (strcmp(BUFFER_COMPARE, FrameOn_A) == 0){
			/* Frame On - Comando acionar rele */
			BREAK_STATUS = 1;
			Acionamento();
			ACTIVATE_PreencherBuffer = 1;
		}
		else if (strcmp(BUFFER_COMPARE, Frame_PING_A) == 0){
			FLAG_XBEE_PING = 0;
			if (ACTIVATE_PreencherBuffer){
				PreencherBuffer(FRAME_PING);

				/* Nao chamar PreencherBuffer novamente para PING */
				ACTIVATE_PreencherBuffer = 0;
			}

		}
		/* Enviar para Transmissor */
		//HAL_UART_Transmit_IT(&huart3, BUFFER_TRANSMIT, strlen(BUFFER_TRANSMIT));

		if (!feedback_multiple){
			if (HAL_UART_Transmit(&huart3, BUFFER_TRANSMIT, strlen(BUFFER_TRANSMIT),20) == HAL_OK)
				if (ACTIVATE_PreencherBuffer == 1){
					strcat(BUFFER_SERIAL, "\n\rFeedback enviado. \r\n");
				}
		}

		//HAL_UART_Transmit_DMA(&huart3, BUFFER_TRANSMIT, strlen(BUFFER_TRANSMIT));
	}
	/* LIMPAR TODOS CHAR E POINTERS (redundancia) */
	memset(BUFFER_RECEIVE, 0, strlen(BUFFER_RECEIVE));
	memset(CRC_RECEIVED, 0, strlen(CRC_RECEIVED));
	memset(BUFFER_COMPARE, 0, strlen(BUFFER_COMPARE));

	/* Fim da funcao */
}

