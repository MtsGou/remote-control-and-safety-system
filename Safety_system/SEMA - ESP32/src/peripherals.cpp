#include <peripherals.h>

///////////////////////////////////////////////////////////////////////////////////////
/* ////////////////////////// FUNCOES PARA PERIFERICOS //////////////////////////////*/

/**------------------------------------------------------------------------------------
  * @brief Toggle pin para geracao de PWM que indica funcionamento do ESP32.
  *
  * @param none
  * 
  * @retval void
  */
void PWM_STATUS_Toggle(){
  TogglePin_STATUS = !TogglePin_STATUS;
  digitalWrite(IN_PWM_ESP, TogglePin_STATUS);
}

/**------------------------------------------------------------------------------------
  * @brief Inicializacao dos pinos GPIO.
  *
  * @param none
  * 
  * @retval void
  */
void Init_Digital(){
  pinMode(RELE_FREIO_ESP, OUTPUT);
  pinMode(ESP_IN_CLIENTE, OUTPUT);
  pinMode(CONNECT_ESP_IN, OUTPUT);
  pinMode(IN_PWM_ESP, OUTPUT);
  pinMode(IO_OUT_ESP_2, OUTPUT);
  pinMode(OUT_EXTRA_CPU, OUTPUT);
  pinMode(IN_EXTRA_CPU, INPUT);
  pinMode(IO_OUT_ESP_1, INPUT);

  digitalWrite(RELE_FREIO_ESP, LOW);
  digitalWrite(CONNECT_ESP_IN, LOW);  
  digitalWrite(ESP_IN_CLIENTE, LOW);
  digitalWrite(IN_PWM_ESP, LOW);
  digitalWrite(IO_OUT_ESP_2, LOW);
  digitalWrite(OUT_EXTRA_CPU, LOW);
}

/**------------------------------------------------------------------------------------
  * @brief Inicializacao dos PWM gerados por timer, nos canais 0 e 1.
  *
  * @param none
  * 
  * @retval void
  */
void Init_PWM(){
  ledcAttachPin(FE_ESP, CHANNEL_FE);
  ledcSetup(CHANNEL_FE, FREQ_FE, RESOLUTION_PWM);
  ledcWrite(CHANNEL_FE, PWMFreio);

  ledcAttachPin(AC_ESP, CHANNEL_AC);
  ledcSetup(CHANNEL_AC, FREQ_AC, RESOLUTION_PWM);
  ledcWrite(CHANNEL_AC, PWMAcel);
}

/**------------------------------------------------------------------------------------
  * @brief Inicializacao das interrupcoes ISR dos pinos para leitura dos sinais
  * de funcionamento do CPU, da UPA e do STM.
  *
  * @OBS  Interrupcao com funcoes de callback Ext_INT(n)_ISR, nas bordas
  * de subida.
  *
  * @param none
  * 
  * @retval void
  */
void Init_Interrupt(){
  pinMode(OUT_PWM_ESP, INPUT);
  pinMode(IN_PWM_UPA, INPUT);
  pinMode(IN_PWM_CPU, INPUT);

  attachInterrupt(OUT_PWM_ESP, Ext_INT1_ISR /*Interrupt Service Routine*/, RISING /*BORDA SUBIDA*/);
  attachInterrupt(IN_PWM_UPA, Ext_INT2_ISR /*Interrupt Service Routine*/, RISING /*BORDA SUBIDA*/);
  attachInterrupt(IN_PWM_CPU, Ext_INT3_ISR /*Interrupt Service Routine*/, RISING /*BORDA SUBIDA*/);
}

/**------------------------------------------------------------------------------------
  * @brief Callback para interrupcao ISR1 - Status STM (IN PWM).
  *
  * @param none
  * 
  * @retval void
  */
void IRAM_ATTR Ext_INT1_ISR()
{
  // STM funcionando Corretamente...
  PERIODO_PWM_STATUS_STM = millis();
  VERIFY_STM = 1;
}

/**------------------------------------------------------------------------------------
  * @brief Callback para interrupcao ISR2 - Status UPA (IN PWM).
  *
  * @param none
  * 
  * @retval void
  */
void IRAM_ATTR Ext_INT2_ISR()
{
  // UPA funcionando Corretamente...
  PERIODO_PWM_STATUS_UPA = millis();
  if (!VERIFY_UPA){
    timestamp();
    Serial.println("Comunicacao com UPA recuperada");
    AddtoReport("Comunicacao com UPA recuperada", false);
  }
  VERIFY_UPA = 1;
}

/**------------------------------------------------------------------------------------
  * @brief Callback para interrupcao ISR3 - Status CPU (IN PWM). 
  *
  * @param none
  * 
  * @retval void
  */
void IRAM_ATTR Ext_INT3_ISR()
{
  // CPU funcionando Corretamente...
  PERIODO_PWM_STATUS_CPU = millis();
  if (!VERIFY_CPU){
    timestamp();
    Serial.println("Comunicacao com CPU recuperada");
    AddtoReport("Comunicacao com CPU recuperada", false);
  }
  VERIFY_CPU = 1;
}
