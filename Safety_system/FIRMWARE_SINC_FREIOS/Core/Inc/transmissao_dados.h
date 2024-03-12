/*
 * transmissao_dados.h
 *
 *  Created on: 16 de ago de 2022
 *      Author: matheus.gouveia
 */

/**
  ******************************************************************************
  * @file    transmissao_dados.h
  * @author  Matheus Gouveia
  * @brief   Esse arquivo contem todas as declaracoes para as aplicacoes de
  * 		 transmissoes de dados.
  *
  *
  ******************************************************************************
  */

#ifndef INC_TRANSMISSAO_DADOS_H_
#define INC_TRANSMISSAO_DADOS_H_

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "stm32l4xx_hal.h"

/* Enviar por serial ocorrencias e debug */
void COMUNICACAO_SERIAL_DEBUG(void);

/* FUNCAO QUE CONVERTE VARIAVEL INT PARA VARIAVEL STRING DE FORMA LITERAL */
char* IntParaString(uint32_t value, char* result, int base);

/* Funcao para gerar Buffer do horario de contingencia */
void MILLIS_INFORMAR_HORARIO(uint32_t millis_time);

/* FUNCAO ESCREVE NO USB */
void escreve_USB(char *data);

/* Funcoes abaixo disponiveis em GeeksforGeeks */

// Reverses a string 'str' of length 'len'
void reverse(char* str, int len);

// Converts a given integer x to string str[].
// d is the number of digits required in the output.
// If d is more than the number of digits in x,
// then 0s are added at the beginning.
int intToStr(int x, char str[], int d);

// Converts a floating-point/double number to a string.
void ftoa(float n, char* res, int afterpoint);

#endif /* INC_TRANSMISSAO_DADOS_H_ */
