#include <UART_handler.hpp>
#include <user_menu.hpp>

///////////////////////////////////////////////////////////////////////////////////////
/* ///////////////////////// UART COMMUNICATION FUNCTIONS ///////////////////////////*/

/**------------------------------------------------------------------------------------
  * @brief Apresentacao serial para monitor, info firmware version.
  *
  * @param  none
  * 
  * @retval void
  */
void FIRMWARE_BEGIN(){
  Serial.println();
  Serial.println("----------------------------------------");
  Serial.println();
  Serial.println("RESET");
  Serial.println();
  Serial.print("WELCOME to ");
  Serial.printf("%s!", SIGLA);
  Serial.printf("\n\r\n\r%s", MEANING_1);
  Serial.printf("\n\r%s\n\r", MEANING_2);
  Serial.printf("\n\rMCU info: %s\n\r", MCU_INFO);
  Serial.print("\n\rEquipment ID: ");
  Serial.print(ID);
  Serial.println();
  Serial.printf("\n\rVersao: %s   %s\n\r", VERSION, FIRMWARE_DATE);
  Serial.printf("\n\r%s", COPYRIGHT);
  Serial.println();
  Serial.println("-----------------------------------------");
  Serial.println();
}

/**------------------------------------------------------------------------------------
  * @brief Gerar Buffer de timestamp do horario de contingencia e enviar serial
  * para monitor.
  *
  * @param  none
  * 
  * @retval void
  */
void timestamp(){

  // String local para print apenas
  String BUFFER_TIMESTAMP = "";

  unsigned int long millis_time = millis()/1000;

  if (millis_time < 60){

    /* Se o tempo for menor que 1 min */
    BUFFER_TIMESTAMP += "[00:00:";
    if (millis_time < 10){
      /* Menor que 10 segundos */
      BUFFER_TIMESTAMP += "0";
    }
    BUFFER_TIMESTAMP += (String) millis_time;
  }
  else if (millis_time >= 60 &&  millis_time < 3600){

    /* Se o tempo for maior que 1 min e menor que 1h */
    uint16_t minutes = millis_time/60;
    uint16_t seconds = millis_time%60;

    BUFFER_TIMESTAMP += "[00:";
    if (minutes < 10){
      /* Menor que 10 minutos */
      BUFFER_TIMESTAMP += "0";
    }
    BUFFER_TIMESTAMP += (String) minutes;
    BUFFER_TIMESTAMP += ":";
    if (seconds < 10){
      /* Menor que 10 segundos */
      BUFFER_TIMESTAMP += "0";
    }
    BUFFER_TIMESTAMP += (String) seconds;
  }
  else if (millis_time >= 3600){

    /* Se o tempo for maior que 1h */
    uint16_t hours = millis_time/3600;
    BUFFER_TIMESTAMP += "[";
    if (hours < 10){
      /* Menor que 10 horas */
      BUFFER_TIMESTAMP += "0";
    }
    BUFFER_TIMESTAMP += (String) hours;
    BUFFER_TIMESTAMP += ":";

    millis_time = millis_time%3600;
    uint16_t minutes = millis_time/60;
    uint16_t seconds = millis_time%60;

    if (minutes < 10){
      /* Menor que 10 minutos */
      BUFFER_TIMESTAMP += "0";
    }
    BUFFER_TIMESTAMP += (String) minutes;
    BUFFER_TIMESTAMP += ":";
    if (seconds < 10){
      /* Menor que 10 segundos */
      BUFFER_TIMESTAMP += "0";
    }
    BUFFER_TIMESTAMP += (String) seconds;
  }
  BUFFER_TIMESTAMP += "]: ";

  Serial.printf("\n\r%s", (char *)BUFFER_TIMESTAMP.c_str());
  AddtoReport(BUFFER_TIMESTAMP, true);

  /* Fim da funcao */
}

/**------------------------------------------------------------------------------------
  * @brief Aguardar se o usuario quiser usar o menu. Ler dados recebidos por UART.
  *
  * @OBS  775 foi obtido experimentalmente para delay aproximado em ms.
  *
  * @param  time_ms:  Tempo em milissegundos
  * 
  * @retval void
  */
void wait_for_serial(uint32_t time_ms){
  uint32_t wait = 775*time_ms;

  delay(500);
  Serial.println();
  Serial.println("Digite Shift + M para configurar");

  do {
    if (Serial.available()){
      Handle_user();
    }
  } while (wait--);
  Serial.println("Iniciando...");
}

/**------------------------------------------------------------------------------------
  * @brief Lidar com dados recebidos por UART do usuario. 
  * Se for 'M', entra no Menu principal.
  *
  * @OBS  Apenas na inicializacao.
  *
  * @param none
  * 
  * @retval void
  */
void Handle_user() {
  char upcommingdata = Serial.read();
  if (upcommingdata == 'M') {
      Menu();
  }
}