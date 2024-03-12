/**
  ******************************************************************************
  * @file    main.cpp
  * @author  Matheus Gouveia.
  * @brief   Sistema de Emergencia e Monitoramento do autonomo (SEMA)
  *          This software is for Microcontroller ESP32 DEV-KIT-V1 DO IT
  *          Comunicacao pela rede do despacho.
  *          Atua remotamente por um cliente conectado.
  *          Informa no front-end as eventualidades
  *          Atua em conjunto com o microcontrolador STM32L476RG
  *          Linguagens de programacao utilizadas: C++, JavaScript, C.
  *          Linguagens de outra funcao: CSS e HTML.
  *
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022. Matheus Gouveia
  * All rights reserved.
  *
  *  Sistema de Emergencia e Monitoramento do Autonomo (SEMA)
  *  Created on: 30/08/2022
  *  Os direitos autorais devem ser incluidos
  *  em todas as copias ou partes substanciais do Software.
  *      Author: Matheus Gouveia.
 **/

 /*===============================================================================
                        ##### EXPLICACAO DO SOFTWARE #####
   ===============================================================================
  [..]
    
    PINOUT:

    (#) CPU communication PINS:
      (*) IN_EXTRA_CPU: Recebimento de comandos do CPU para frear ou desativar freio.
                          1 -> Freio atuado. 0-> Freio nao atuado.
      (*) OUT_EXTRA_CPU: Envio de feedbacks para CPU apos atuacao.
                          1 -> Freio atuado. 0-> Freio nao atuado.
      (*) IN_PWM_CPU: Recebe o PWM indicativo do estado de funcionamento do CPU.

    (#) UPA communication PINS:
      (*) IN_PWM_UPA: Recebe o PWM indicativo do estado de funcionamento da UPA.

    (#) STM communication PINS:
      (*) IO_OUT_ESP_1: Recebimento de feedbacks do STM apos atuacao deste.
                          1 -> Freio atuado. 0-> Freio nao atuado.
      (*) IO_OUT_ESP_2: Envio de feedbacks para STM apos atuacao.
                          1 -> Freio atuado. 0-> Freio nao atuado.
      (*) OUT_PWM_ESP: Recebe o PWM indicativo do estado de funcionamento do STM.
      (*) IN_PWM_ESP: Envia um PWM indicativo do estado de funcionamento do ESP.
      (*) CONNECT_ESP_IN: Envia um sinal digital para informar a conexao do ESP.
                          1 -> conectado. 0-> nao conectado.
      (*) ESP_IN_CLIENTE: Envia um sinal digital indicando conexao dos clientes.
                        1 -> ao menos 1 cliente conectado. 0-> nao conectado.

    (#) Actuation pins:
      (*) RELE_FREIO_ESP: Saida digital responsavel por atuar o rele de freio.
      (*) FE_ESP: Geracao do PWM do freio de emergencia para a ECM do caminhao.
      (*) AC_ESP: Geracao do PWM do acelerador para a ECM do caminhao.

    (#) RESUMO:
        (+) Em condicoes normais, o rele nao esta atuado.
        (+) Recebimento de PWM indicativo de estado de funcionamento:
          - Feito por meio de pinos de interrupcao externa.
          - A interrupcao zera contadores.
          - Se os contadores ultrapassarem um limite,
            e entendido como falha.
        (+) Condicoes de atuacao:
          - Com falha do STM, ou
          - Com ordem direta do operador (cliente), ou
          - Com ordem direta do CPU, ou
          - Pelo despacho, ao receber comando em protocolo TCP
        (+) Na atuacao, o rele atua e envia para ECM os PWM gerados.
        (+) Um menu de configuracao pode ser acessado 
            na inicializacao (apenas).
          - Os parametros configurados podem ser salvos na flash.
          - Os parametros configurados podem ser apagados da flash.
          - A flash e manipulada com uso da biblioteca EEPROM.
        (+) Eventualidades sao informadas a todo momento por UART,
            contendo timestamp.
        (+) Conexao:
          - Tenta conectar a um AP WiFi na rede salva.
          - Deve estar na mesma rede do despacho.
          - O sistema funciona mesmo sem rede, porem, sem os
            recursos de freio remoto e monitoramento remoto.
          - Este software oferece dois servidores.
          - Servidor 1: porta 80. Comunica via socket com front-end.
          - Servidor 2: porta 8080. Comunica com despacho via TCP.
  */

#include <sys_def.h>
#include <TCP_server_handler.hpp>
#include <front_page.h>
#include <user_menu.hpp>
#include <wifi_functions.h>

/* Parte central do codigo - setup e loop principal */
void setup()
{
  // INICIAR EEPROM 
  EEPROM.begin(EEPROM_SIZE);

  // INICIAR MONITOR COMUNICACAO SERIAL
  Serial.begin(BAUD_RATE);

  // INICIAR PARAMETROS (CHECAR FLASH)
  Set_initial_config();

  // CHECAR SE O USUARIO QUER CONFIGURAR. Aguardar CPU
  wait_for_serial(28000);

  // PRESENT FIRMWARE INFORMATION
  FIRMWARE_BEGIN();

  // GPIO PIN SET
  Init_Digital();

  // Ajustar duty cicle PWMs
  AJUSTAR_DUTYCYCLE();

  // INICIALIZAR PWMs
  Init_PWM();

  // INICIALIZAR INTERRUPCOES
  Init_Interrupt();

  // CONEXAO A WIFI
  Wifi_connect();

  // INICIALIZAR O SOCKET
  initSocket();

  // SERVIDOR SOCKET METODO HTTP - HTTP Server
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send_P(200, "text/html", index_html, Substituir); });

  // Start socket server
  server.begin();

  // Start server TCP
  server_TCP->onClient(&handleNewClient, server_TCP);
  server_TCP->begin();

}

void loop()
{
  // TOGGLE PIN STATUS
  delay(100); // 100 ms, 10 Hz
  PWM_STATUS_Toggle();

  DETECCAO_FALHA();
  CPU_Handler();
  STM_Feedback_Handler();

  // GARANTIR A RECONEXAO CASO DESCONECTE
  if (WiFi.status() != WL_CONNECTED && Disconnect != 1)
  {
    Reconnect_try();
  }

  // CALL LIMPAR LISTA DE CLIENTES
  ws.cleanupClients();
}


/**********************************************************************************************
  Outubro, 2021.
  Codigo para implementacao do SEMA (parte ESP32) do caminhao autonomo
  Os direitos autorais devem ser incluidos em todas as copias ou partes substanciais do Software.
  Author:  Gouveia, Matheus.

***********************************************************************************************/
