/*
 * transmissao_dados.c
 *
 *  Created on: 16 de ago de 2022
 *      Author: matheus.gouveia
 */

/**
  ******************************************************************************
  * @file    transmissao_dados.c
  * @author  Matheus Gouveia
  * @brief   Esse arquivo contem todas as funcoes para aplicacoes de
  * 		 transmissoes de dados do SEMA.
  *
  ******************************************************************************
  */

#include <SEMA_definitions.h>
#include "string.h"
#include <stdio.h>
#include <stdlib.h>
#include <transmissao_dados.h>
#include "stm32l4xx_hal_conf.h"
#include "stm32l4xx_it.h"

/**-----------------------------------------------------------------------------------------------------------------------------------
  * @brief Enviar por serial ocorrencias e debug.
  *
  * @param  none
  *
  * @retval void
  */
void COMUNICACAO_SERIAL_DEBUG(){

	/* Transmitir para USB */
	HAL_UART_Transmit(&huart4, BUFFER_SERIAL, strlen(BUFFER_SERIAL), 50);

	/* Apagar Buffer */
	memset(BUFFER_SERIAL,0,strlen(BUFFER_SERIAL));

}
/*-----------------------------------------------------------------------------------------------------------------------------------*/


/**-----------------------------------------------------------------------------------------------------------------------------------
  * @brief  FUNCAO QUE CONVERTE VARIAVEL INT PARA VARIAVEL STRING DE FORMA LITERAL.
  *
  *
  * @param  value:      valor inteiro.
  * @param  *result:	pointer char para armazenar valor como string.
  * @param  base:		base (decimal, binario, hexadecimal, etc).
  *
  * @retval *result
  */
char* IntParaString(uint32_t value, char* result, int base) {

    /* check that the base if valid */
    if (base < 2 || base > 36) { *result = '\0'; return result; }

    char* ptr = result, *ptr1 = result, tmp_char;
    uint32_t tmp_value;

    do {
        tmp_value = value;
        value /= base;
        *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz"
        		[35 + (tmp_value - value * base)];
    } while ( value );

    /* Apply negative sign */
    if (tmp_value < 0) *ptr++ = '-';
    *ptr-- = '\0';
    while(ptr1 < ptr) {
        tmp_char = *ptr;
        *ptr--= *ptr1;
        *ptr1++ = tmp_char;
    }
    return result;

}
/*-----------------------------------------------------------------------------------------------------------------------------------*/


/**-----------------------------------------------------------------------------------------------------------------------------------
  * @brief  Funcao para gerar Buffer do horario de contingencia.
  *
  *
  * @param  millis_time:    tempo de execucao em milissegundos.
  *
  * @retval void
  */
void MILLIS_INFORMAR_HORARIO(uint32_t millis_time){

	char millis_STR_seg [16];
	char millis_STR_min [16];
	char millis_STR_h [16];

	millis_time = millis_time/1000;
	strcat(BUFFER_SERIAL, "\r\n");

	if (millis_time < 60){

		/* Se o tempo for menor que 1 min */
		IntParaString(millis_time, millis_STR_seg, 10);
		strcat(BUFFER_SERIAL, "00:00:");
		if (millis_time < 10){
			/* Menor que 10 segundos */
			strcat(BUFFER_SERIAL, "0");
		}
		strcat(BUFFER_SERIAL, millis_STR_seg);
	}
	else if (millis_time >= 60 &&  millis_time < 3600){

		/* Se o tempo for maior que 1 min e menor que 1h */
		IntParaString(millis_time/60, millis_STR_min, 10);
		IntParaString(millis_time%60, millis_STR_seg, 10);

		strcat(BUFFER_SERIAL, "00:");
		if (millis_time/60 < 10){
			/* Menor que 10 minutos */
			strcat(BUFFER_SERIAL, "0");
		}
		strcat(BUFFER_SERIAL, millis_STR_min);
		strcat(BUFFER_SERIAL, ":");
		if (millis_time%60 < 10){
			/* Menor que 10 segundos */
			strcat(BUFFER_SERIAL, "0");
		}
		strcat(BUFFER_SERIAL, millis_STR_seg);
	}
	else if (millis_time >= 3600){

		/* Se o tempo for maior que 1h */
		IntParaString(millis_time/3600, millis_STR_h, 10);

		if (millis_time/3600 < 10){
			/* Menor que 10 horas */
			strcat(BUFFER_SERIAL, "0");
		}
		strcat(BUFFER_SERIAL, millis_STR_h);
		strcat(BUFFER_SERIAL, ":");

		millis_time = millis_time%3600;
		IntParaString(millis_time/60, millis_STR_min, 10);
		IntParaString(millis_time%60, millis_STR_seg, 10);

		if (millis_time/60 < 10){
			/* Menor que 10 minutos */
			strcat(BUFFER_SERIAL, "0");
		}
		strcat(BUFFER_SERIAL, millis_STR_min);
		strcat(BUFFER_SERIAL, ":");
		if (millis_time%60 < 10){
			/* Menor que 10 segundos */
			strcat(BUFFER_SERIAL, "0");
		}
		strcat(BUFFER_SERIAL, millis_STR_seg);
	}
	strcat(BUFFER_SERIAL, ":");

	/* Apagar tudo */
	memset(millis_STR_h,0,strlen(millis_STR_h));
	memset(millis_STR_min,0,strlen(millis_STR_min));
	memset(millis_STR_seg,0,strlen(millis_STR_seg));

	/* Fim da funcao */
}
/*-----------------------------------------------------------------------------------------------------------------------------------*/


/**-----------------------------------------------------------------------------------------------------------------------------------
  * @brief  FUNCAO ESCREVE NO USB.
  *
  * @param  *data:  dado a ser escrito.
  *
  * @retval void
  */
void escreve_USB(char *data){

	HAL_UART_Transmit(&huart4, data, strlen(data),100);
}

/*-----------------------------------------------------------------------------------------------------------------------------------*/

/* The following functions can be found in https://www.geeksforgeeks.org/ */

// Reverses a string 'str' of length 'len'
void reverse(char* str, int len)
{
    int i = 0, j = len - 1, temp;
    while (i < j) {
        temp = str[i];
        str[i] = str[j];
        str[j] = temp;
        i++;
        j--;
    }
}

// Converts a given integer x to string str[].
// d is the number of digits required in the output.
// If d is more than the number of digits in x,
// then 0s are added at the beginning.
int intToStr(int x, char str[], int d)
{
    int i = 0;
    while (x) {
        str[i++] = (x % 10) + '0';
        x = x / 10;
    }

    // If number of digits required is more, then
    // add 0s at the beginning
    while (i < d)
        str[i++] = '0';

    reverse(str, i);
    str[i] = '\0';
    return i;
}

// Converts a floating-point/double number to a string.
void ftoa(float n, char* res, int afterpoint)
{
    // Extract integer part
    int ipart = (int)n;

    // Extract floating part
    float fpart = n - (float)ipart;

    // convert integer part to string
    int i = intToStr(ipart, res, 0);

    // check for display option after point
    if (afterpoint != 0) {
        res[i] = '.'; // add dot

        // Get the value of fraction part upto given no.
        // of points after dot. The third parameter
        // is needed to handle cases like 233.007
        fpart = fpart * pow(10, afterpoint);

        intToStr((int)fpart, res + i + 1, afterpoint);
    }
}

