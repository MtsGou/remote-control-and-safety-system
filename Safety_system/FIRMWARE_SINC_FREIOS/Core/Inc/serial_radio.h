/*
 * serial_radio.h
 *
 *  Created on: 16 de ago de 2022
 *      Author: Matheus Gouveia
 */

/**
  ******************************************************************************
  * @file    serial_radio.h
  * @author  Matheus Gouveia
  * @brief   Esse arquivo contem as declaracoes das funcoes para a
  * 		 comunicacao serial com o radio.
  *
  ******************************************************************************
  */

#ifndef INC_SERIAL_RADIO_H_
#define INC_SERIAL_RADIO_H_

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "stm32l4xx_hal.h"

/* Gera frames a serem enviados por serial */
void GerarFrames(void);

/* PREENCHE O BUFFER E CALCULA
 * O RESPECTIVO CHECKSUM, QUE SERA ENVIADO POR SERIAL*/
void PreencherBuffer(int MODE);

/* RECEBE O COMANDO DO TRANSMISSOR
 * E VERIFICA A MENSAGEM E O CHECKSUM. Envia feedbacks apos atuar.*/
void VerificarComando(void);


#endif /* INC_SERIAL_RADIO_H_ */
