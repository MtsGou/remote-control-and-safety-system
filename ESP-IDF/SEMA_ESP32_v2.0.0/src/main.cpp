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
  * All rights reserved.
  *
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
**/

#include "main.h"
#include "esp_log.h"
#include "GPIO_config.h"
#include "uart_handler.h"
#include "flash_handler.h"
#include "parameters.h"

extern "C"
{


    void AJUSTAR_DUTYCYCLE() {

      FINAL_DUTYCYCLE_EMG = (float)PERCENT_EMG * (DUTYCYCLE_MAX_EMG - DUTYCYCLE_MIN_EMG);
      FINAL_DUTYCYCLE_EMG += DUTYCYCLE_MIN_EMG*100;
      FINAL_DUTYCYCLE_EMG = FINAL_DUTYCYCLE_EMG/100;
  
      FINAL_DUTYCYCLE_ACEL = (float)PERCENT_ACEL * (DUTYCYCLE_MAX_ACEL - DUTYCYCLE_MIN_ACEL);
      FINAL_DUTYCYCLE_ACEL += DUTYCYCLE_MIN_ACEL*100;
      FINAL_DUTYCYCLE_ACEL = FINAL_DUTYCYCLE_ACEL/100;

      PWM_EMG = FINAL_DUTYCYCLE_EMG * size_bits;
      PWM_ACEL = FINAL_DUTYCYCLE_ACEL * size_bits;
  
    }

    void setup()
    {

      // INICIAR COMUNICACAO SERIAL
      init_uart();

      AJUSTAR_DUTYCYCLE();

      // INICIAR PWM
      init_PWM_GPIO_pins();

      // INICIALIZACAO PINOS GPIO DIGITAIS
      //init_Digital_GPIO_pins();

      // INICIALIZACAO PINOS GPIO INTERRUPCAO EXTERNA
      //init_ISR_GPIO_pins();

      /*gpio_set_intr_type(OUT_PWM_ESP, GPIO_INTR_ANYEDGE);

      gpio_install_isr_service(0);

      gpio_isr_handler_add(OUT_PWM_ESP, gpio_interrupt_handler_STM, NULL);

      xTaskCreate(Task_ISR_STM, "Task_ISR_STM", 4096, NULL, 10, &ISR);*/

      //printf("\r\nConfigurou com sucesso!");
    
      


    }

    void app_main(void) 
    {
      //static const char *TAG = "MAIN";
      //ESP_LOGD(TAG, "init");

      vTaskDelay(6000 / portTICK_PERIOD_MS);

      // INITIAL SETUP
      setup();

      //const char* mensagem = "\r\nOi Pessoal.";

      //string char_lido;

      // MAIN LOOP
      while(1){
        //printf("\n\rEai galera");
        //if (gpio_get_level(/*IN_EXTRA_CPU*/OUT_PWM_ESP))
        //  printf("\r\nSinal alto...");
        //else
        //  printf("\r\nSinal baixo.");

        //printf("\n\rSetting LED to High...");
        //vTaskDelay(3000 / portTICK_PERIOD_MS);
        //send_uart(mensagem);

        //if (serial_available()){
        //  send_uart("\r\nVoce digitou: ");
        //  send_uart((char *) BUFFER_RX_SERIAL);
        //}
        //gpio_set_level(RELE_FREIO_ESP, HIGH);
        //printf("\n\rSetting LED to Low...");
        //vTaskDelay(1000 / portTICK_PERIOD_MS);
        //gpio_set_level(RELE_FREIO_ESP, LOW);

        //vTaskDelay(1000 / portTICK_PERIOD_MS);

        //if (gpio_get_level(IN_EXTRA_CPU))
        //  printf("\r\nSinal alto...");
        //else
        //  printf("\n\nSinal baixo.");

        //char_lido = serial_read_char();
        //if (char_lido.length() > 0){
        //  send_uart("\rVoce digitou: ");
        //  send_uart((char *) char_lido.c_str());
        //  char_lido = "";
        //}
        //char_lido = serial_read_string_until("\r");
        //send_uart("\rA string completa eh: ");
        //send_uart((char *) char_lido.c_str());
      }
    }
}

