/*
 * Menu_SEMA.h
 *
 *  Created on: 16 de ago de 2022
 *      Author: matheus.gouveia
 */

/**
  ******************************************************************************
  * @file    Menu_SEMA.h
  * @author  Matheus Gouveia
  * @brief   Esse arquivo contem as declaracoes das funcoes utilizadas para
  * 		 o menu do SEMA.
  *
  ******************************************************************************
  * @attention
  *
  * Todos os direitos preservados.
  *
  *
  ******************************************************************************
  */

#ifndef INC_MENU_SEMA_H_
#define INC_MENU_SEMA_H_

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "stm32l4xx_hal.h"

/* Aguarda um dado serial do teclado pelo tempo especificado.
 * Se chegar um dado, verifica o dado. Se for 'M', entra no Menu.
 * Apos entrar no Menu, atende as requisicoes do usuario.*/
void Handle_user(int ms_time);

/* Edita um valor inteiro de forma interativa.*/
uint32_t edit_int(uint32_t value, int type);

/* Edita um valor flutuante de forma interativa.*/
float edit_float(float value);

/* Edita uma string, aguardando o usuario digitar. */
void edit_string(void);

/* Apaga dados da memoria flash.*/
void reset_dados(void);

/* Recebe uma string e elimina dela qualquer dado
 * vazio. Retorna a nova string*/
char* eliminar_nao_char(char* string);

/* Apaga a tela do monitor serial. */
void apagar_tela(void);

/* Atribui aos parametros os valores padrao. */
void usar_valores_padrao(void);

/* Printa o Menu. */
void Print_Menu(void);

/* Salva parametros na memoria flash. */
void gravar_parametros(void);

/* Printa os parametros atuais. */
void ver_parametros(void);

#endif /* INC_MENU_SEMA_H_ */
