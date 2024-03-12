/*
 * Menu_SEMA.c
 *
 *  Created on: 16 de ago de 2022
 *      Author: matheus.gouveia
 */

/**
  ******************************************************************************
  * @file    Menu_SEMA.c
  * @author  Matheus Gouveia
  * @brief   Esse arquivo contem as definicoes das funcoes utilizadas
  * 		 para o menu do SEMA.
  *
  ******************************************************************************
  * @attention
  *
  * Todos os direitos preservados.
  *
  *
  ******************************************************************************
  */

#include <SEMA_definitions.h>
#include <Menu_SEMA.h>
#include <transmissao_dados.h>
#include "stm32l4xx_hal_conf.h"
#include "stm32l4xx_it.h"

/*-----------------------------------------------------------------------------------------------------------------------------------*/


/**-----------------------------------------------------------------------------------------------------------------------------------
  * @brief  Aguarda um dado serial do teclado pelo tempo especificado. Se chegar um dado, verifica o dado.
  * 	    Se for 'M', entra no Menu. Apos entrar no Menu, atende as requisicoes do usuario.
  *
  * @param  ms_time:  tempo para aguardar dados do usuario (em ms).
  *
  * @retval void
  */
void Handle_user(int ms_time){

	static uint8_t MENU_INIT = 0; // Apos entrar no menu, MENU_INIT = 1

	memset(BUFFER_RECEIVE_USB, 0, strlen(BUFFER_RECEIVE_USB));
	if (HAL_UART_Receive(&huart4, &BUFFER_RECEIVE_USB, 1, ms_time) == HAL_OK){

		if(strcmp(BUFFER_RECEIVE_USB, "M") == 0){
			MENU_INIT = 1;
			HAL_UART_AbortReceive_IT(&huart3); //ABORTA OUTROS RECEBIMENTOS
			Print_Menu();
		}

		else if (!MENU_INIT){}

		else if(strcmp(BUFFER_RECEIVE_USB, "A") == 0){
			edit_string();
			Print_Menu();
			escreve_USB("\n\r\n\rID Salvo: ");
			escreve_USB((char *) ID);
		}

		else if(strcmp(BUFFER_RECEIVE_USB, "B") == 0){
			TIMEOUT_PING = edit_int(TIMEOUT_PING, 2);
			memset(TIMEOUT_PING_CHAR, 0, strlen(TIMEOUT_PING_CHAR));
			sprintf(TIMEOUT_PING_CHAR, "%d", TIMEOUT_PING);
			Print_Menu();
		}

		else if(strcmp(BUFFER_RECEIVE_USB, "C") == 0){
			DUTY_CYCLE_FE = edit_float(DUTY_CYCLE_FE);
			if (DUTY_CYCLE_FE < 60){DUTY_CYCLE_FE = 60;}
			memset(DUTY_CYCLE_FE_CHAR, 0, strlen(DUTY_CYCLE_FE_CHAR));
			ftoa(DUTY_CYCLE_FE, DUTY_CYCLE_FE_CHAR, 2);
			Print_Menu();
		}

		else if(strcmp(BUFFER_RECEIVE_USB, "D") == 0){
			DUTY_CYCLE_FE_MIN = edit_float(DUTY_CYCLE_FE_MIN);
			if (DUTY_CYCLE_FE_MIN > 40){DUTY_CYCLE_FE_MIN = 40;}
			memset(DUTY_CYCLE_FE_MIN_CHAR, 0, strlen(DUTY_CYCLE_FE_MIN_CHAR));
			ftoa(DUTY_CYCLE_FE_MIN, DUTY_CYCLE_FE_MIN_CHAR, 2);
			Print_Menu();
		}

		else if(strcmp(BUFFER_RECEIVE_USB, "E") == 0){
			DUTY_CYCLE_AC_MAX = edit_float(DUTY_CYCLE_AC_MAX);
			if (DUTY_CYCLE_AC_MAX < 40){DUTY_CYCLE_AC_MAX = 40;}
			memset(DUTY_CYCLE_AC_MAX_CHAR, 0, strlen(DUTY_CYCLE_AC_MAX_CHAR));
			ftoa(DUTY_CYCLE_AC_MAX, DUTY_CYCLE_AC_MAX_CHAR, 2);
			Print_Menu();
		}

		else if(strcmp(BUFFER_RECEIVE_USB, "F") == 0){
			DUTY_CYCLE_AC = edit_float(DUTY_CYCLE_AC);
			if (DUTY_CYCLE_AC > 40){DUTY_CYCLE_AC = 40;}
			memset(DUTY_CYCLE_AC_CHAR, 0, strlen(DUTY_CYCLE_AC_CHAR));
			ftoa(DUTY_CYCLE_AC, DUTY_CYCLE_AC_CHAR, 2);
			Print_Menu();
		}

		else if(strcmp(BUFFER_RECEIVE_USB, "G") == 0){
			DUTY_CYCLE_FR = edit_float(DUTY_CYCLE_FR);
			if (DUTY_CYCLE_FR < 50){DUTY_CYCLE_FR = 50;}
			memset(DUTY_CYCLE_FR_CHAR, 0, strlen(DUTY_CYCLE_FR_CHAR));
			ftoa(DUTY_CYCLE_FR, DUTY_CYCLE_FR_CHAR, 2);
			Print_Menu();
		}

		else if(strcmp(BUFFER_RECEIVE_USB, "H") == 0){
			DUTY_CYCLE_FR_MIN = edit_float(DUTY_CYCLE_FR_MIN);
			if (DUTY_CYCLE_FR_MIN > 50){DUTY_CYCLE_FR_MIN = 50;}
			memset(DUTY_CYCLE_FR_MIN_CHAR, 0, strlen(DUTY_CYCLE_FR_MIN_CHAR));
			ftoa(DUTY_CYCLE_FR_MIN, DUTY_CYCLE_FR_MIN_CHAR, 2);
			Print_Menu();
		}

		else if(strcmp(BUFFER_RECEIVE_USB, "I") == 0){
			DUTYFE = edit_int(DUTYFE,1);
			memset(DUTYFE_CHAR, 0, strlen(DUTYFE_CHAR));
			sprintf(DUTYFE_CHAR, "%d", DUTYFE);
			Print_Menu();
		}

		else if(strcmp(BUFFER_RECEIVE_USB, "J") == 0){
			DUTYACEL = edit_int(DUTYACEL,1);
			memset(DUTYACEL_CHAR, 0, strlen(DUTYACEL_CHAR));
			sprintf(DUTYACEL_CHAR, "%d", DUTYACEL);
			Print_Menu();
		}

		else if(strcmp(BUFFER_RECEIVE_USB, "K") == 0){
			DUTYFR = edit_int(DUTYFR,1);
			memset(DUTYFR_CHAR, 0, strlen(DUTYFR_CHAR));
			sprintf(DUTYFR_CHAR, "%d", DUTYFR);
			Print_Menu();
		}

		else if (strcmp(BUFFER_RECEIVE_USB, "L") == 0){
			NUMBER_OF_FEEDBACKS = edit_int(NUMBER_OF_FEEDBACKS, 3);
			memset(NUMBER_OF_FEEDBACKS_CHAR, 0, strlen(NUMBER_OF_FEEDBACKS_CHAR));
			sprintf(NUMBER_OF_FEEDBACKS_CHAR, "%d", NUMBER_OF_FEEDBACKS);
			Print_Menu();
		}

		else if(strcmp(BUFFER_RECEIVE_USB, "V") == 0){
			ver_parametros();
		}

		else if(strcmp(BUFFER_RECEIVE_USB, "S") == 0){
			gravar_parametros();
			escreve_USB("\n\rValores salvos na memoria.\n\r");
		}

		else if(strcmp(BUFFER_RECEIVE_USB, "Y") == 0){
			reset_dados();
		}

		else if(strcmp(BUFFER_RECEIVE_USB, "X") == 0){
			apagar_tela();
			escreve_USB("\r");
			PWM_DEFINITION();
			atuar_PWMs();
			MENU_INIT = 0;
			//RETORNA RECEBIMENTOS
			HAL_UART_Receive_IT(&huart3, Data, 1);
		}
	}

	/* Fim da funcao */
}

/*-----------------------------------------------------------------------------------------------------------------------------------*/


/**-----------------------------------------------------------------------------------------------------------------------------------
  * @brief  Edita um valor inteiro de forma interativa.
  *
  * @param  value:  valor a ser editado.
  * @param  type:   Define o tipo de variavel que esta sendo editada.
  *
  * @retval value
  */
uint32_t edit_int(uint32_t value, int type){

	char data_rcv[10];
	char* command;
	int change = 1;
	uint32_t Standard = value;
	char value_CHAR[10];

	int MIN = 0;
	int MAX = 100;

	if (type == 2){
		MIN = 1000;
		MAX = 9999;
		change = 100;
	}

	if (type == 3){
		MIN = 1;
		MAX = 30;
	}

	escreve_USB("\r\n");
	escreve_USB("\r\nw - AUMENTAR\r\n");
	escreve_USB("\r\ns - DIMINUIR\r\n");
	escreve_USB("\r\nA - SALVAR\r\n");
	escreve_USB("\r\nX - RETORNAR MENU INICIAL\r\n\r\n");

	IntParaString(value, value_CHAR, 10);
	escreve_USB(value_CHAR);
	escreve_USB("\r");

	while(1){

		TOGGLE_PWM_STATUS(); // Manter envio do status de funcionamento

		memset(data_rcv, 0, strlen(data_rcv));
		memset(command, 0, strlen(command));

		if(HAL_UART_Receive(&huart4, &data_rcv, 1,100) == HAL_OK){

			command = data_rcv;
			command = eliminar_nao_char(command);


			if(strcmp(command, "w") == 0){
				value += change;

				if(value > MAX){value = MAX;}
				if(value < MIN){value = MIN;}

				memset(value_CHAR, 0, strlen(value_CHAR));

				IntParaString(value, value_CHAR, 10);
				escreve_USB(value_CHAR);
				escreve_USB("\r");

			}

			else if (strcmp(command, "s") == 0){
				value -= change;

				if(value < MIN){value = MIN;}
				if(value > MAX){value = MAX;}

				memset(value_CHAR, 0, strlen(value_CHAR));

				IntParaString(value, value_CHAR, 10);

				escreve_USB(value_CHAR);
				escreve_USB("\r");
			}

			else if(strcmp(command, "A") == 0){
				escreve_USB("\n\rSalvo! ");
				break;
			}

			else if(strcmp(command, "X") == 0){
				value = Standard;
				break;
			}

		}
	}
	memset(command, 0, strlen(command));
	return value;
}

/*-----------------------------------------------------------------------------------------------------------------------------------*/


/**-----------------------------------------------------------------------------------------------------------------------------------
  * @brief  Edita um valor flutuante de forma interativa.
  *
  * @param  value:  valor a ser editado.
  *
  * @retval value
  */
float edit_float(float value){

	char rcvd_data[10];
	char* command;
	float change = 0.01;
	float Standard = value;
	char value_CHAR[6];

	int MIN = 0;
	int MAX = 100;

	escreve_USB("\r\n\r\n");
	escreve_USB("\r\nw - AUMENTAR\r\n");
	escreve_USB("\r\ns - DIMINUIR\r\n");
	escreve_USB("\r\na - ALGARISMO <\r\n");
	escreve_USB("\r\nd - ALGARISMO >\r\n");
	escreve_USB("\r\nA - SALVAR\r\n");
	escreve_USB("\r\nX - RETORNAR MENU INICIAL\r\n");
	escreve_USB("\r\n");

	ftoa(value, value_CHAR, 2);
	escreve_USB(value_CHAR);
	escreve_USB("\r");

	while(1){

		TOGGLE_PWM_STATUS(); // Manter envio do status de funcionamento

		memset(rcvd_data, 0, strlen(rcvd_data));
		memset(command, 0, strlen(command));

		if(HAL_UART_Receive(&huart4, &rcvd_data, 1,100) == HAL_OK){

			command = rcvd_data;
			command = eliminar_nao_char(command);

			if(strcmp(command, "w") == 0){
				value += change;

				if(value > MAX){value = MAX;}
				if(value < MIN){value = MIN;}
				memset(value_CHAR, 0, strlen(value_CHAR));
				ftoa(value, value_CHAR, 2);

				escreve_USB(value_CHAR);
				escreve_USB("\r");
			}

			else if (strcmp(command, "s") == 0){
				value -= change;

				if(value < MIN){value = MIN;}
				if(value > MAX){value = MAX;}
				memset(value_CHAR, 0, strlen(value_CHAR));
				ftoa(value, value_CHAR, 2);

				escreve_USB(value_CHAR);
				escreve_USB("\r");
			}

			else if (strcmp(command, "d") == 0){
				change = change/10;
				if (change < 0.01){ change = 0.01;}
			}

			else if (strcmp(command, "a") == 0){
				change = change*10;
				if (change > 10){ change = 10;}
			}

			else if(strcmp(command, "A") == 0){
				escreve_USB("\n\rSalvo! ");
				break;
			}

			else if(strcmp(command, "X") == 0){
				value = Standard;
				break;
			}
		}

	}
	memset(command, 0, strlen(command));
	return value;
}

/*-----------------------------------------------------------------------------------------------------------------------------------*/


/**-----------------------------------------------------------------------------------------------------------------------------------
  * @brief  Edita uma string, aguardando o usuario digitar.
  *
  * @param  none
  *
  * @retval value
  */
void edit_string(){

	char data_received[10];
	char* command;
	uint8_t BUFFER_ID[20];
	int EXIT = 0;

	escreve_USB("\n\r");
	escreve_USB("\r\nW - SALVAR\r\n");
	escreve_USB("X - RETORNAR MENU INICIAL\r\n");
	escreve_USB("\r\n");
	escreve_USB("\n\rDIGITE O NOVO ID: ");
	memset(BUFFER_ID, 0, strlen(BUFFER_ID));

	while (!EXIT){

		TOGGLE_PWM_STATUS(); // Manter envio do status de funcionamento
		memset(data_received, 0, strlen(data_received));
		memset(command, 0, strlen(command));

		if(HAL_UART_Receive(&huart4, &data_received, 1,100) == HAL_OK){


			command = data_received;
			command = eliminar_nao_char(command);

			if (strcmp(command, "W") == 0){
				memset(ID, 0, strlen(ID));
				strcat(ID, BUFFER_ID);
				GerarFrames();
				EXIT = 1;//break;
			}

			else if (strcmp(command, "X") == 0){
				EXIT = 1;//break;
			}

			else if (strlen(command)){
				escreve_USB(command);
				strcat(BUFFER_ID, command);
			}
		}

	}
	memset(command, 0, strlen(command));
}

/*-----------------------------------------------------------------------------------------------------------------------------------*/


/**-----------------------------------------------------------------------------------------------------------------------------------
  * @brief  Apaga dados da memoria flash.
  *
  * @param  none
  *
  * @retval value
  */
void reset_dados(){

	char rcvd_data[10];
	char *command;
	int FLAG_EXIT = 0;

	escreve_USB("\n\r");
	escreve_USB("\r\nTEM CERTEZA QUE DESEJA LIMPAR OS DADOS?\r\n");
	escreve_USB("S - SIM\r\nN - NAO\r\n");

	while(!FLAG_EXIT){

		TOGGLE_PWM_STATUS(); // Manter envio do status de funcionamento

		memset(rcvd_data, 0, strlen(rcvd_data));
		memset(command, 0, strlen(command));

		if (HAL_UART_Receive(&huart4, &rcvd_data, 1,100) == HAL_OK){

			command = rcvd_data;
			command = eliminar_nao_char(command);

			if(strcmp(command, "S") == 0){

				/* Apaga todas as variaveis CHAR */
				memset(&DUTY_CYCLE_FE_CHAR, 0, strlen(DUTY_CYCLE_FE_CHAR));
				memset(&DUTY_CYCLE_FE_MIN_CHAR, 0, strlen(DUTY_CYCLE_FE_MIN_CHAR));
				memset(&DUTY_CYCLE_AC_CHAR, 0, strlen(DUTY_CYCLE_AC_CHAR));
				memset(&DUTY_CYCLE_AC_MAX_CHAR, 0, strlen(DUTY_CYCLE_AC_MAX_CHAR));
				memset(&DUTY_CYCLE_FR_CHAR, 0, strlen(DUTY_CYCLE_FR_CHAR));
				memset(&DUTY_CYCLE_FR_MIN_CHAR, 0, strlen(DUTY_CYCLE_FR_MIN_CHAR));
				memset(&TIMEOUT_PING_CHAR, 0, strlen(TIMEOUT_PING_CHAR));
				memset(&DUTYFE_CHAR, 0, strlen(DUTYFE_CHAR));
				memset(&DUTYACEL_CHAR, 0, strlen(DUTYACEL_CHAR));
				memset(&DUTYFR_CHAR, 0, strlen(DUTYFR_CHAR));
				memset(&NUMBER_OF_FEEDBACKS_CHAR, 0, strlen(NUMBER_OF_FEEDBACKS_CHAR));

				/* Apaga os buffers de dados da flash */
				memset(BUFFER_FLASH_L, 0, strlen(BUFFER_FLASH_L));
				memset(BUFFER_FLASH, 0, strlen(BUFFER_FLASH));

				/* Retornar valores padrao */
				usar_valores_padrao();

				/* Apaga flash */
				FLASH_apaga(END_INICIAL, 2);

				/* Inicializa novamente os parametros */
				inicializar_parametros();

				/* Gera novamente os frames */
				GerarFrames();

				memset(rcvd_data, 0, strlen(rcvd_data));
				FLAG_EXIT = 1;

				escreve_USB("\n\rMemoria Apagada\n\r");
			}

			if(strcmp(command, "N") == 0){
				FLAG_EXIT = 1;

				escreve_USB("\r\nEXIT CANCEL\r\n");

			}
		}

	}

}

/*-----------------------------------------------------------------------------------------------------------------------------------*/


/**-----------------------------------------------------------------------------------------------------------------------------------
  * @brief  Recebe uma string e elimina dela qualquer dado vazio. Retorna a nova string.
  *
  * @param  *string: String a ser alterada.
  *
  * @retval *string
  */
char* eliminar_nao_char(char* string){

	int error_index = 1;

	while (error_index < strlen(string)){
		error_index = strcspn(string, "\r\n\t  ");
		memmove(&string[error_index], &string[error_index + 1], strlen(string) - error_index);
	}

	return string;
}

/*-----------------------------------------------------------------------------------------------------------------------------------*/


/**-----------------------------------------------------------------------------------------------------------------------------------
  * @brief  Apaga a tela do monitor serial.
  *
  * @param  none
  *
  * @retval void
  */
void apagar_tela(){
	for (int times = 0; times < 60; times++){
		escreve_USB("\n\r");
	}
}

/*-----------------------------------------------------------------------------------------------------------------------------------*/


/**-----------------------------------------------------------------------------------------------------------------------------------
  * @brief  Atribui aos parametros os valores padrao.
  *
  * @param  none
  *
  * @retval void
  */
void usar_valores_padrao(){

	memset(ID, 0, strlen(ID));
	strcat(ID, "CR2100");
	TIMEOUT_PING = PING_TIMEOUT_STD;
	DUTY_CYCLE_FE = PWM_FE_MAX;
	DUTY_CYCLE_FE_MIN = PWM_FE_MIN;
	DUTY_CYCLE_AC = PWM_AC_MIN;
	DUTY_CYCLE_AC_MAX = PWM_AC_MAX;
	DUTY_CYCLE_FR = PWM_FR_MAX;
	DUTY_CYCLE_FR_MIN = PWM_FR_MIN;
	DUTYFE = PERCENT_FE;
	DUTYACEL = PERCENT_AC;
	DUTYFR = PERCENT_FR;
	NUMBER_OF_FEEDBACKS = FEEDBACK_NUM_STD;
}
/*-----------------------------------------------------------------------------------------------------------------------------------*/


/**-----------------------------------------------------------------------------------------------------------------------------------
  * @brief  Printa o menu.
  *
  * @param  none
  *
  * @retval void
  */
void Print_Menu(){

	escreve_USB("\r\n\r\nMENU\r\n");
	escreve_USB("\n\rSELECT       Shift + key\n\r");
	escreve_USB("\n\rID EQUIPAMENTO                 A");
	escreve_USB("\n\rTIMEOUT PING                   B");
	escreve_USB("\n\rDUTYCYCLE MAX FREIO EMERG      C");
	escreve_USB("\n\rDUTYCYCLE MIN FREIO EMERG      D");
	escreve_USB("\n\rDUTYCYCLE MAX ACEL             E");
	escreve_USB("\n\rDUTYCYCLE MIN ACEL             F");
	escreve_USB("\n\rDUTYCYCLE MAX FREIO RET        G");
	escreve_USB("\n\rDUTYCYCLE MIN FREIO RET        H");
	escreve_USB("\n\rPERCENTUAL PWM FREIO EMERG     I");
	escreve_USB("\n\rPERCENTUAL PWM ACEL            J");
	escreve_USB("\n\rPERCENTUAL PWM FREIO RET       K");
	escreve_USB("\n\rNUMERO FEEDBACKS RADIO:        L");
	escreve_USB("\n\rVER PARAMETROS ATUAIS          V");
	escreve_USB("\n\rSALVAR DADOS->MEMORIA          S");
	escreve_USB("\n\rAPAGAR DADOS DA MEMORIA        Y");
	escreve_USB("\n\rSAIR/LEAVE MENU                X");
	escreve_USB("\r");
}

/*-----------------------------------------------------------------------------------------------------------------------------------*/


/**-----------------------------------------------------------------------------------------------------------------------------------
  * @brief  Salva parametros na memoria flash.
  *
  * @param  none
  *
  * @retval void
  */
void gravar_parametros(){

	strcat(BUFFER_FLASH, (char *) ID);
	strcat(BUFFER_FLASH, DIV);
	strcat(BUFFER_FLASH, TIMEOUT_PING_CHAR);
	strcat(BUFFER_FLASH, DIV);
	strcat(BUFFER_FLASH, DUTY_CYCLE_FE_CHAR);
	strcat(BUFFER_FLASH, DIV);
	strcat(BUFFER_FLASH, DUTY_CYCLE_FE_MIN_CHAR);
	strcat(BUFFER_FLASH, DIV);
	strcat(BUFFER_FLASH, DUTY_CYCLE_FR_CHAR);
	strcat(BUFFER_FLASH, DIV);
	strcat(BUFFER_FLASH, DUTY_CYCLE_FR_MIN_CHAR);
	strcat(BUFFER_FLASH, DIV);
	strcat(BUFFER_FLASH, DUTY_CYCLE_AC_CHAR);
	strcat(BUFFER_FLASH, DIV);
	strcat(BUFFER_FLASH, DUTY_CYCLE_AC_MAX_CHAR);
	strcat(BUFFER_FLASH, DIV);
	strcat(BUFFER_FLASH, DUTYFE_CHAR);
	strcat(BUFFER_FLASH, DIV);
	strcat(BUFFER_FLASH, DUTYACEL_CHAR);
	strcat(BUFFER_FLASH, DIV);
	strcat(BUFFER_FLASH, DUTYFR_CHAR);
	strcat(BUFFER_FLASH, DIV);
	strcat(BUFFER_FLASH, NUMBER_OF_FEEDBACKS_CHAR);

	FLASH_apaga(END_INICIAL, 2); // Page 2
	FLASH_escreve_32bits(END_INICIAL, BUFFER_FLASH);
}
/*-----------------------------------------------------------------------------------------------------------------------------------*/


/**-----------------------------------------------------------------------------------------------------------------------------------
  * @brief  Printa os parametros atuais.
  *
  * @param  none
  *
  * @retval void
  */
void ver_parametros(){

	escreve_USB("\n\r");
	escreve_USB("\r\nID EQUIPAMENTO:");
	escreve_USB((char *)ID);
	escreve_USB("\r\nTIMEOUT PING:");
	escreve_USB(TIMEOUT_PING_CHAR);
	escreve_USB("\r\nDUTYCYCLE MAX FREIO EMERG:");
	escreve_USB(DUTY_CYCLE_FE_CHAR);
	escreve_USB("\r\nDUTYCYCLE MIN FREIO EMERG:");
	escreve_USB(DUTY_CYCLE_FE_MIN_CHAR);
	escreve_USB("\r\nDUTYCYCLE MAX ACEL:");
	escreve_USB(DUTY_CYCLE_AC_MAX_CHAR);
	escreve_USB("\r\nDUTYCYCLE MIN ACEL:");
	escreve_USB(DUTY_CYCLE_AC_CHAR);
	escreve_USB("\r\nDUTYCYCLE MAX FREIO RET:");
	escreve_USB(DUTY_CYCLE_FR_CHAR);
	escreve_USB("\r\nDUTYCYCLE MIN FREIO RET:");
	escreve_USB(DUTY_CYCLE_FR_MIN_CHAR);
	escreve_USB("\r\nPERCENTUAL PWM FREIO EMERG:");
	escreve_USB(DUTYFE_CHAR);
	escreve_USB("\r\nPERCENTUAL PWM ACEL:");
	escreve_USB(DUTYACEL_CHAR);
	escreve_USB("\r\nPERCENTUAL PWM FREIO RET:");
	escreve_USB(DUTYFR_CHAR);
	escreve_USB("\r\nNUMERO FEEDBACKS RADIO:");
	escreve_USB(NUMBER_OF_FEEDBACKS_CHAR);

}
