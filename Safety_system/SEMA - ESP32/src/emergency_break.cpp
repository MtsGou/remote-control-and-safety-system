#include <emergency_break.hpp>

///////////////////////////////////////////////////////////////////////////////////////
/* ////////////////// FUNCOES PARA ATUACAO E ACIONAMENTO ////////////////////////////*/

/**------------------------------------------------------------------------------------
  * @brief Ajusta o duty cycle que sera usado para gerar PWM (Freio e Acelerador).
  *
  * @OBS O ajuste e feito por interpolacao linear. Por fim, as variaveis globais
  *      PWMFreio e PWMAcel serao usadas para gerar o PWM com ledcwrite.
  *
  * @param none
  * 
  * @retval void
  */
void AJUSTAR_DUTYCYCLE() {

  DUTYCYCLEFreio = (float)DUTYFREIO * (DUTYMAXFreio - DUTYMINFreio);
  DUTYCYCLEFreio += DUTYMINFreio*100;
  DUTYCYCLEFreio = DUTYCYCLEFreio/100;
  
  DUTYCYCLEAcel = (float)DUTYACEL * (DUTYMAXAcel - DUTYMINAcel);
  DUTYCYCLEAcel += DUTYMINAcel*100;
  DUTYCYCLEAcel = DUTYCYCLEAcel/100;

  PWMFreio = DUTYCYCLEFreio * size_bits;
  PWMAcel = DUTYCYCLEAcel * size_bits;
  
}

/**------------------------------------------------------------------------------------
  * @brief Acionamento do rele.
  *
  * @OBS Estados logicos: 0-> Nao atuado. 1-> Atuado. Envia feedbacks 
  * ao CPU e ao STM.
  *
  * @param rele:  Representa o estado real do rele.
  * 
  * @retval void
  */
void Acionamento(int rele)
{
  // FUNCAO APENAS PARA REALIZAR O ACIONAMENTO - DIGITAL PIN E PWM

  digitalWrite(RELE_FREIO_ESP, rele);

  // ENVIO DE FEEDBACKS AO CPU E AO STM
  digitalWrite(OUT_EXTRA_CPU, rele);
  digitalWrite(IO_OUT_ESP_2, rele);

}

///////////////////////////////////////////////////////////////////////////////////////
/* /////////////// IMPLEMENTACAO DO SEMA - SEMA Implementation //////////////////////*/

/**------------------------------------------------------------------------------------
  * @brief Identifica comandos recebidos do CPU e atua. Envia os feedbacks.
  *
  * @OBS Inside main loop.
  * 
  * @param none
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
    static bool stop_report = true;
    relay = 1;
 	  timestamp();
  	if (stop_report){
      Serial.println("ABORTAR COMANDO CPU. FREIO ACIONADO.");
      AddtoReport("ABORTAR COMANDO CPU. FREIO ACIONADO.", false);
      Acionamento(relay);
      SendClients();
      stop_report = false;
    }
	  delay(200);
    PWM_STATUS_Toggle();
    ws.textAll("ABORTAR COMANDO CPU. FREIO ACIONADO.");
    return;
  }

  if (time_last_order != 0){
	  time_interval = millis() - time_last_order;
	  sum_intervals += time_interval;
  }
  if (sum_intervals > 30000){
    sum_intervals = 0;
    count_cpu_orders = 0;
  }
  time_last_order = millis();

  if (digitalRead(IN_EXTRA_CPU) && !relay){
    /* Primeira vez que freia */
    CPU_BREAK_ACT = 1;
    relay = 1;
    count_cpu_orders++;
    Acionamento(relay);
    SendClients();
    timestamp();
    Serial.println("CPU ORDER TO EMERGENCY BREAK");
    AddtoReport("CPU ORDER TO EMERGENCY BREAK", false);
  }

  else if ((!digitalRead(IN_EXTRA_CPU) && relay) && CPU_BREAK_ACT){
    /* Parar freio, ja havia freado */
    CPU_BREAK_ACT = 0;
    relay = 0;
    count_cpu_orders++;
    Acionamento(relay);
    SendClients();
    ws.textAll("CPU ORDER TO RELEASE BREAK");
    timestamp();
    Serial.println("CPU ORDER TO RELEASE BREAK");
    AddtoReport("CPU ORDER TO RELEASE BREAK", false);
  }

  if (CPU_BREAK_ACT){
    /* Permanecer enviando feedback para clientes */
    delay(200);
    PWM_STATUS_Toggle();
    ws.textAll("CPU ORDER TO EMERGENCY BREAK"); 
  }
}

/**------------------------------------------------------------------------------------
  * @brief Identifica feedbacks recebidos do STM e informa aos clientes.
  *
  * @OBS Inside main loop.
  * 
  * @param none
  * 
  * @retval void
  */
void STM_Feedback_Handler(){
  if (digitalRead(IO_OUT_ESP_1) && !STM_FEEDBACK){
    /* Primeira vez que notifica */
    STM_FEEDBACK = 1;
    timestamp();
    Serial.println("Freio acionado pelo STM.");
    AddtoReport("Freio acionado pelo STM.", false);
  }
  else if (!digitalRead(IO_OUT_ESP_1) && STM_FEEDBACK){
    /* Ja havia freado, notificacao que desativou */
    STM_FEEDBACK = 0;
    ws.textAll("Freio desativado pelo STM.");
    timestamp(); 
    Serial.println("Freio desativado pelo STM.");
    AddtoReport("Freio desativado pelo STM.", false);
  }

  if (STM_FEEDBACK){
    /* Permanecer enviando feedback de freio para clientes */
    delay(200);
    PWM_STATUS_Toggle();
    ws.textAll("Freio acionado pelo STM."); 
  }
}

/**------------------------------------------------------------------------------------
  * @brief Executa ao identificar falha do STM. Atuar prontamente e dar feedbacks
  * ao CPU, a UPA, ao STM e aos clientes.
  *
  * @OBS  Se STM retornar, essa funcao tambem informa os clientes.
  * 
  * @param MODE_STATUS: Status atual do STM. !=0-> FAIL. 0-> NO FAIL.
  * 
  * @retval void
  */
void EMERGENCY_STM_NO_RETURN(int MODE_STATUS)
{
  // FUNCAO QUE EXECUTA AO IDENTIFICAR UMA TRANSICAO DE
  // ESTADO DO STM32. INFORMA QUANDO NAO HA RETORNO DO STM32
  static int OCORRENCIA_FALHA_VALOR_ANTERIOR;
  OCORRENCIA_FALHA_VALOR_ANTERIOR = OCORRENCIA_FALHA_STM;
  if (MODE_STATUS != 0)
  {
    // PERMANECE ENVIANDO AOS CLIENTES SOCKET, PARA ATUALIZAR
    // A PAGINA PARA TODOS:
    ws.textAll("ERRO! PROBLEMA NO HARDWARE STM32. FREIO FOI ACIONADO!");
    delay(200); // NOTE: delay para evitar bug de queue messages

    if (!OCORRENCIA_FALHA_STM)
    {
      // APOS AS TRES TENTATIVAS
      timestamp();
      Serial.println("ERRO! PROBLEMA NO HARDWARE STM32. FREIO FOI ACIONADO!");
      AddtoReport("ERRO! PROBLEMA NO HARDWARE STM32. FREIO FOI ACIONADO!", false);

      // ATESTAR FALHA NAS VARIAVEIS:
      OCORRENCIA_FALHA_STM = 1;
      VERIFY_STM = 0;
    }
  }
  else
  {
    // CHAMAR_EMERGENCIA = 0, logo comunicacao recuperada
    ws.textAll("Comunicacao com STM recuperada.");
    timestamp();
    Serial.println("Comunicacao com STM recuperada.");
    AddtoReport("Comunicacao com STM recuperada.", false);
    if (OCORRENCIA_FALHA_STM)
    {
      // ATUALIZA QUE NAO HA MAIS FALHA NO STM
      OCORRENCIA_FALHA_STM = 0;
      VERIFY_STM = 1;
    }
  }
  if (OCORRENCIA_FALHA_VALOR_ANTERIOR ^ OCORRENCIA_FALHA_STM && (OCORRENCIA_FALHA_STM))
  {
    // SO ENTRA NESTA CONDICAO SE ACONTECER UMA FALHA APOS TUDO ESTAR OK
    relay = 1;
    Acionamento(relay);
    SendClients();
  }
}

/**------------------------------------------------------------------------------------
  * @brief Verifica se ocorreu alguma falha do CPU, UPA ou STM.
  *
  * @OBS Inside main loop. Informa os clientes socket.
  * 
  * @param none
  * 
  * @retval void
  */
void DETECCAO_FALHA()
{
  // DETECCAO DE ERRO DO STM
  if (millis() - PERIODO_PWM_STATUS_STM > TIMEOUT_FAIL)
  {
    int CHAMAR_EMERGENCIA = 0;
    // VERIFICA 3 VEZES
    for (int j = 0; j < 3; j++)
    {
      if (millis() - PERIODO_PWM_STATUS_STM > TIMEOUT_FAIL)
      {
        // CHAMAR_EMERGENCIA: CONTA AS TENTATIVAS
        CHAMAR_EMERGENCIA++;
        if (!OCORRENCIA_FALHA_STM)
        {
          ws.textAll("HARDWARE FALHOU. TENTATIVA " + String(CHAMAR_EMERGENCIA));
          timestamp();
          Serial.println("HARDWARE FALHOU. TENTATIVA " + String(CHAMAR_EMERGENCIA));
          AddtoReport("HARDWARE FALHOU. TENTATIVA " + String(CHAMAR_EMERGENCIA), false);
          PWM_STATUS_Toggle();
          delay(800);
        }
      }
      else
      {
        // SE O ERRO FOI CORRIGIDO NO HARDWARE
        CHAMAR_EMERGENCIA = 0;
        break;
      }
    }
    // APOS ERRO DETECTADO DEPOIS DE 3 TENTATIVAS OU APOS 
    // ERRO CORRIGIDO, CHAMA A FUNCAO
    EMERGENCY_STM_NO_RETURN(CHAMAR_EMERGENCIA);
  }

  // SE STM FOI CORRIGIDO APOS UMA FALHA, ATUALIZAR
  else if (VERIFY_STM && OCORRENCIA_FALHA_STM)
  {
    int CHAMAR_EMERGENCIA = 0;
    EMERGENCY_STM_NO_RETURN(CHAMAR_EMERGENCIA);
  }

  // DETECCAO DE ERRO NO CPU
  if (millis() - PERIODO_PWM_STATUS_CPU > TIMEOUT_FAIL){
    ws.textAll("HARDWARE FAIL. FATAL CPU ERROR");
    if (VERIFY_CPU){
      timestamp();
      AddtoReport("HARDWARE FAIL. FATAL CPU ERROR", false);
    }
    Serial.println("HARDWARE FAIL. FATAL CPU ERROR");
    VERIFY_CPU = 0;
    delay(200); // NOTE: delay para evitar bug de queue messages
    PWM_STATUS_Toggle();
  }

  // DETECCAO DE ERRO DA UPA
  if (millis() - PERIODO_PWM_STATUS_UPA > TIMEOUT_FAIL){
    ws.textAll("HARDWARE FAIL. FATAL UPA ERROR");
    if (VERIFY_UPA){
      timestamp();
      AddtoReport("HARDWARE FAIL. FATAL UPA ERROR", false);
    }
    Serial.println("HARDWARE FAIL. FATAL UPA ERROR");
    VERIFY_UPA = 0;
    delay(200); // NOTE: delay para evitar bug de queue messages
    PWM_STATUS_Toggle();
  }
}