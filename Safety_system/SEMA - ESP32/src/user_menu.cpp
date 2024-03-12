#include <user_menu.hpp>

///////////////////////////////////////////////////////////////////////////////////////
/* /////////////// MENU DO USUARIO PARA CONFIGURACAO DOS PARAMETROS /////////////////*/

/**------------------------------------------------------------------------------------
  * @brief Reseta os dados da flash.
  *
  * @OBS Apos resetar, o microcontrolador deve ser resetado ou desligado 
  * para retornar aos valores padrao.
  * 
  * @param none
  * 
  * @retval void
  */
void reset_dados(){
   for (int i = 0; i < EEPROM_SIZE; i++){
    EEPROM.write(i, EMPTY_VALUE);
   }
   EEPROM.commit();
   Serial.println("Memoria Flash apagada. Aperte reset para atualizar");
}

/**------------------------------------------------------------------------------------
  * @brief Verifica se ha dados salvos na memoria flash
  * Se houver, verifica se nao ha dados corrompidos
  * Se os dados estiverem seguros, os captura e inicializa
  * com os dados salvos na memoria.
  *
  * @OBS Apenas durante a inicializacao.
  * 
  * @param none
  * 
  * @retval void
  */
void Set_initial_config() {

  String getIP, getGATE, getSUBMASK;

  /* Capturar string salva na memoria flash */
  String getEEPROM = EEPROM.readString(address_EEPROM);

  if (getEEPROM.length() > 0){

    Serial.println("DADOS ARMAZENADOS NA MEMORIA: ");
    Serial.print(getEEPROM);

    /* Se houver dados salvos na memoria, obter CRC */
    String data_mem = getEEPROM.substring(0,getEEPROM.indexOf(CHECKSUM_DIV));
    String CRC_arrived = getEEPROM.substring(getEEPROM.indexOf(CHECKSUM_DIV), getEEPROM.indexOf('$'));
    CRC_arrived.remove(0, 1); 
    char* DataEEPROM = (char *)data_mem.c_str();
    String CRC_str = CRC(getBitsFromString(DataEEPROM));
    uint8_t CRC_int = strbin_to_dec(CRC_str);
    String CRC_verify = String(CRC_int, HEX);
    CRC_verify.toUpperCase();

    /* Verificar CRC */
    if (CRC_arrived.equals(CRC_verify)){
      /* CRC bateu, capturar dados e configurar parametros */
      
      DUTYMAXFreio = getValue(data_mem,',',0).toFloat();
      DUTYMINFreio = getValue(data_mem,',',1).toFloat();
      DUTYMAXAcel =  getValue(data_mem,',',2).toFloat();
      DUTYMINAcel =  getValue(data_mem,',',3).toFloat();
      DUTYFREIO = getValue(data_mem,',',4).toInt();
      DUTYACEL = getValue(data_mem,',',5).toInt();
      TimeOut = 3600*(getValue(data_mem,',',6).toFloat());
      LOGIN = getValue(data_mem, ',', 7);
      SENHA = getValue(data_mem, ',', 8);
      getIP = getValue(data_mem, ',', 9);
      getGATE = getValue(data_mem, ',', 10);
      getSUBMASK = getValue(data_mem, ',', 11);
      ID = getValue(data_mem, ',', 12);

      IP[0] = getValue(getIP,'.',0).toInt();
      IP[1] = getValue(getIP,'.',1).toInt();
      IP[2] = getValue(getIP,'.',2).toInt();
      IP[3] = getValue(getIP,'.',3).toInt();
      GATE[0] = getValue(getGATE,'.',0).toInt();
      GATE[1] = getValue(getGATE,'.',1).toInt();
      GATE[2] = getValue(getGATE,'.',2).toInt();
      GATE[3] = getValue(getGATE,'.',3).toInt();
      SUBMASK[0] = getValue(getSUBMASK,'.',0).toInt();
      SUBMASK[1] = getValue(getSUBMASK,'.',1).toInt();
      SUBMASK[2] = getValue(getSUBMASK,'.',2).toInt();
      SUBMASK[3] = getValue(getSUBMASK,'.',3).toInt();   
    }
    
  }

}

/**------------------------------------------------------------------------------------
  * @brief Apenas printar no monitor serial o Menu.
  * 
  * @param none
  * 
  * @retval void
  */
void Print_menu() {
  Serial.println("\f");
  Serial.println("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
  Serial.print("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
  Serial.println("\n\rMENU\r\n");
  Serial.print("\n\rSELECT       Shift + key\n\r");
  Serial.print("\n\rID DO EQUIPAMENTO           I");
  Serial.print("\n\rDUTYCYCLE MAX FREIO         F");
  Serial.print("\n\rDUTYCYCLE MIN FREIO         G");
  Serial.print("\n\rDUTYCYCLE MAX ACEL          A");
  Serial.print("\n\rDUTYCYCLE MIN ACEL          S");
  Serial.print("\n\rPERCENTUAL PWM FREIO        P");
  Serial.print("\n\rPERCENTUAL PWM ACEL         O");
  Serial.print("\n\rTIMEOUT NET                 T");
  Serial.print("\n\rCONFIGURACOES NET           C");
  Serial.print("\n\rVER PARAMETROS ATUAIS       V");
  Serial.print("\n\rSALVAR DADOS->MEMORIA       M");
  Serial.print("\n\rAPAGAR DADOS DA MEMORIA     R");
  Serial.println("\n\rSAIR/LEAVE MENU             X");
}

/**------------------------------------------------------------------------------------
  * @brief Apenas printar no monitor serial o Menu de configuracao de Rede.
  * 
  * @param none
  * 
  * @retval void
  */
void Print_menu_NET() {
  Serial.println("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
  Serial.print("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
  Serial.print("\n\rALTERAR LOGIN WIFI          L");
  Serial.print("\n\rALTERAR SENHA WIFI          S");
  Serial.print("\n\rALTERAR IP                  I");
  Serial.print("\n\rALTERAR GATEWAY             G");
  Serial.print("\n\rALTERAR MASCARA SUB REDE    M");
  Serial.println("\n\rVOLTAR AO MENU PRINCIPAL    X");
}

/**------------------------------------------------------------------------------------
  * @brief Printar os dados salvos atualmente.
  * 
  * @param none
  * 
  * @retval void
  */
void Print_parameters() {
  Serial.print("\r\nID DO EQUIPAMENTO:    ");
  Serial.print(ID);
  Serial.print("\r\nDUTY CYCLE MAX Freio: ");
  Serial.print(DUTYMAXFreio,3);
  Serial.print("\r\nDUTY CYCLE MIN Freio: ");
  Serial.print(DUTYMINFreio,3);
  Serial.print("\r\nDUTY CYCLE MAX Acel: ");
  Serial.print(DUTYMAXAcel,3);
  Serial.print("\r\nDUTY CYCLE MIN Acel: ");
  Serial.print(DUTYMINAcel,3);
  Serial.print("\r\nPWM Freio: ");
  Serial.print(DUTYFREIO);
  Serial.print("\r\nPWM Acel: ");
  Serial.print(DUTYACEL);
  Serial.print("\r\nTimeout NET: ");
  Serial.print((float)TimeOut/3600);
  Serial.print("\r\nLOGIN WiFi: ");
  Serial.print(LOGIN);
  Serial.print("\r\nSENHA WiFi: ");
  Serial.print(SENHA);
  Serial.print("\r\nIP: ");
  Serial.printf("%d.%d.%d.%d", IP[0], IP[1], IP[2], IP[3]);
  Serial.print("\r\nGateway: ");
  Serial.printf("%d.%d.%d.%d", GATE[0], GATE[1], GATE[2], GATE[3]);
  Serial.print("\r\nSubmask: ");
  Serial.printf("%d.%d.%d.%d", SUBMASK[0], SUBMASK[1], SUBMASK[2], SUBMASK[3]);
  Serial.println();
}

/**------------------------------------------------------------------------------------
  * @brief Loop principal do Menu do usuario.
  *
  * @OBS Apenas durante a inicializacao.
  * 
  * @param none
  * 
  * @retval void
  */
void Menu() {

  Print_menu();

  while (1) {
    if (Serial.available()) {
      char userdata = Serial.read();
      if (userdata == 'I') {
        Serial.println();
        Serial.println("\r\nID atual: ");
        Serial.println(ID);
        Serial.println("Digite o novo ID: (x -> cancelar) ");
        ID = Change_parameters_char(ID);
        Print_menu();
        Serial.println("\r\nID salvo: ");
        Serial.print(ID);
      }
      else if (userdata == 'F') {
        user_interface(1);
      }
      else if (userdata == 'G') {
        user_interface(2);
      }
      else if (userdata == 'A') {
        user_interface(3);
      }
      else if (userdata == 'S') {
        user_interface(4);
      }
      else if (userdata == 'P') {
        user_interface(5);
      }
      else if (userdata == 'O') {
        user_interface(6);
      }
      else if (userdata == 'T') {
        user_interface(7);
      }
      else if (userdata == 'C') {
        user_interface(8);
      }
      else if (userdata == 'V') {
        Print_parameters();
      }
      else if (userdata == 'M') {
        EEPROM.writeString(address_EEPROM, BufferToSave());
        EEPROM.commit();
        Serial.println();
        Serial.println("\rSalvo");
      }
      else if (userdata == 'R') {
        reset_dados();
      }
      else if (userdata == 'X') {
        break;
      }
    }
  }
  Serial.print("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
  Serial.print("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
}

/**------------------------------------------------------------------------------------
  * @brief Loop Menu de configuracoes de rede.
  *
  * @OBS Apenas durante a inicializacao.
  * 
  * @param none
  * 
  * @retval void
  */
void Menu_NET() {
  Print_menu_NET();

  while (1) {
    if (Serial.available()) {
      char userdata = Serial.read();
      if (userdata == 'L') {
        Serial.println();
        Serial.println("\r\nLOGIN atual: ");
        Serial.println(LOGIN);
        Serial.println("Digite o novo LOGIN WiFi: (x -> cancelar) ");
        LOGIN = Change_parameters_char(LOGIN);
        Print_menu_NET();
        Serial.println("\r\nLOGIN salvo: ");
        Serial.print(LOGIN);
        Serial.println("\r\nSENHA atual: ");
        Serial.print(SENHA);
      }
      else if (userdata == 'S') {
        Serial.println();
        Serial.println("\r\nSENHA atual: ");
        Serial.println(SENHA);
        Serial.println("Digite a nova SENHA WiFi: (x -> cancelar)");
        SENHA = Change_parameters_char(SENHA);
        Print_menu_NET();
        Serial.println("\r\nSENHA salva: ");
        Serial.print(SENHA);
        Serial.println("\r\nLOGIN atual: ");
        Serial.println(LOGIN);
      }
      else if (userdata == 'I') {
        for (int index = 0; index < 4; index++) {
          int feedback = 1;
          Serial.println();
          Serial.printf("%do digito IP (0 a 255):", index + 1);
          feedback = Change_parameters_simple(1, IP[index]);
          if (feedback < 0) {
            break;
          }
          IP[index] = feedback;
        }

        Print_menu_NET();
        Serial.println();
        Serial.println("IP Salvo:");
        Serial.printf("%d.%d.%d.%d", IP[0], IP[1], IP[2], IP[3]);
        Serial.println();
      }
      else if (userdata == 'G') {
        for (int index = 0; index < 4; index++) {
          int feedback = 1;
          Serial.println();
          Serial.printf("%do digito Gateway (0 a 255):", index + 1);
          feedback = Change_parameters_simple(1, GATE[index]);
          if (feedback < 0) {
            break;
          }
          GATE[index] = feedback;
        }

        Print_menu_NET();
        Serial.println();
        Serial.println("Gateway salvo:");
        Serial.printf("%d.%d.%d.%d", GATE[0], GATE[1], GATE[2], GATE[3]);
        Serial.println();
      }
      else if (userdata == 'M') {
        int feedback = 1;
        for (int index = 0; index < 4; index++) {
          Serial.println();
          Serial.printf("%do digito Sub-mask (0 a 255):", index + 1);
          feedback = Change_parameters_simple(1, SUBMASK[index]);
          if (feedback < 0) {
            break;
          }
          SUBMASK[index] = feedback;
        }

        Print_menu_NET();
        Serial.println();
        Serial.println("Mascara Sub rede Salva:");
        Serial.printf("%d.%d.%d.%d", SUBMASK[0], SUBMASK[1], SUBMASK[2], SUBMASK[3]);
        Serial.println();
      }
      else if (userdata == 'X') {
        break;
      }
    }
  }

  Serial.print("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
  Serial.print("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
  Print_menu();

}

/**------------------------------------------------------------------------------------
  * @brief Altera parametros float de forma interativa.
  *
  * @OBS Caso o usuario cancele, 'var' sera o valor padrao 'Stan',
  * que sera retornado.
  * 
  * @param var: variavel float a ser configurada.
  * 
  * @retval float var;
  */
float Change_parameters_float(float var) {

  /* Casa decimal */
  int house = 3;

  /* Valor correspondente a casa decimal */ 
  float decimal = 0.001;

  /* Valor padrao, caso cancelar */
  float Stan = var;

  while (1)
  {
    if (Serial.available()) {
      char command = Serial.read();
      if (command == '\n' || command == '\r') {}
      else {
        if (command == 'x' || command == 'X') {
          /* Cancelar */
          Print_menu();
          var = Stan;
          break;
        }

        if (command == 'S') {
          /* Salvar */
          Print_menu();
          break;
        }

        if (command == 'd') {
          house++;
          if (house > 3) {
            house = 3;
          }
        }
        else if (command == 'a') {
          house--;
          if (house < 1) {
            house = 1;
          }
        }

        switch (house) {
          case 1:
            decimal = 0.1;
            break;

          case 2:
            decimal = 0.01;
            break;

          case 3:
            decimal = 0.001;
            break;
        }

        if (command == 'w') {
          var += decimal;
        }
        else if (command == 's') {
          var -= decimal;
        }


        if (var > 1) {
          var = 1;
        }
        else if (var < 0) {
          var = 0;
        }
        Serial.print("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
        Serial.print("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
        Serial.println(var, 3);
      }
    }
  }
  return var;
}

/**------------------------------------------------------------------------------------
  * @brief Altera parametros int de forma interativa.
  *
  * @OBS Caso o usuario cancele, 'variable' sera o valor padrao 'Standard',
  * que sera retornado.
  * 
  * @param variable: variavel int a ser configurada.
  * 
  * @retval int variable;
  */
int Change_parameters_int(int variable) {

  /* Algarismo significativo */
  int house = 1;

  /* Correspondente ao algarismo */
  int decimal = 1;

  /* Valor padrao, caso cancelar */
  int Standard = variable;

  while (1)
  {
    if (Serial.available()) {
      char command = Serial.read();

      if (command == '\n' || command == '\r') {}

      else {

        if (command == 'x' || command == 'X') {
          /* Cancelar */
          Print_menu();
          variable = Standard;
          break;
        }

        if (command == 'S') {
          /* Salvar */
          Print_menu();
          break;
        }

        if (command == 'a') {
          house--;
          if (house < 1) {
            house = 1;
          }
        }
        else if (command == 'd') {
          house++;
          if (house > 2) {
            house = 2;
          }
        }

        switch (house) {
          case 1:
            decimal = 10;
            break;

          case 2:
            decimal = 1;
            break;
        }

        if (command == 'w') {
          variable += decimal;
        }
        else if (command == 's') {
          variable -= decimal;
        }


        if (variable > 100) {
          variable = 100;
        }
        else if (variable < 0) {
          variable = 0;
        }
        Serial.print("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
        Serial.print("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
        Serial.println(variable);
      }
    }
  }
  return variable;
}

/**------------------------------------------------------------------------------------
  * @brief Captura string para salvar no lugar da string salva.
  *
  * @OBS Caso o usuario cancele, 'digit' sera o valor padrao 'enter',
  * que sera retornado.
  * 
  * @param enter: String a ser configurada.
  * 
  * @retval String digit;
  */
String Change_parameters_char(String enter) {
  String digit = "";
  while (1) {
    if (Serial.available()) {
      digit = Serial.readStringUntil('\n');
      if (digit.equals("x") || digit.equals("X")) {
        /* Cancelar */
        digit = enter;
        break;
      }
      if (!digit.length()){
      }
      else{
        break;
      }
    }
  }
  return digit;
}

/**------------------------------------------------------------------------------------
  * @brief Captura inteiro para salvar.
  *
  * @OBS Caso o usuario cancele, o valor retornado sera 'STANDARD'.
  * 
  * @param ID:        Identifica o caso. 0 -> TimeOut config. 1-> 0 a 255.
  * @param STANDARD:  Valor int a ser capturado. 
  * 
  * @retval (int) multiplier_output * reading
  */
int32_t Change_parameters_simple(int ID, int STANDARD) {
  String getuserdata = "";
  float reading;
  float min, max, multiplier_output;

  switch (ID) {
    case 0:
    /* Caso config timeout */
      min = 0.01;
      max = 100;
      multiplier_output = 3600;
      break;

    case 1:
    /* Caso 0 a 255 */
      min = 0;
      max = 255;
      multiplier_output = 1;
      break;
  }

  while (1) {
    if (Serial.available()) {
      getuserdata = "";
      getuserdata = Serial.readStringUntil('\n');
      reading = getuserdata.toFloat();

      if (getuserdata.equals("X") || getuserdata.equals("x")) {
        /* Cancelar */
        if (!ID) {
          /* Voltar para menu */
          Print_menu();
          reading = STANDARD / 3600;
        }
        else {
          /* Voltar para menu de rede */
          reading = -1;
        }
        break;
      }

      if ((reading < min || reading > max)&&(reading != 0)) {
        /* Valor invalido. tentar de novo */
        Serial.println();
        Serial.println("\rValor invalido. Digite novamente.");
      }
      else if ((ID && (getuserdata.length() > 0)) || (!ID && reading != 0)){
        /* Salvar */
        Serial.println();
        Serial.println("\rValor atualizado:");
        if (ID) {
          Serial.print((int)reading);
          Serial.println();
        }
        else {
          Serial.print(reading);
          Serial.println();
          Print_menu();
        }
        break;
      }
    }

  }
  return (int) multiplier_output * reading;
}

/**------------------------------------------------------------------------------------
  * @brief Camada secundaria do menu principal, que define o que fazer
  * em cada caso do Menu principal.
  * 
  * @param ident:   Identificador do caso.
  * 
  * @retval void
  */
void user_interface(int ident) {

  Serial.print("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
  Serial.print("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");

  if (ident == 1 || ident == 2 || ident == 3 || ident == 4) {

    Serial.println("\rEscolha entre 0 e 1 (w -> UP, s-> DOWN, a-> LEFT, d-> RIGHT)");
    Serial.println("\rMAX: min 0.6; MIN: max 0.4; Pressione Shift + S para salvar.");

    if (ident == 1) {
      Serial.println(DUTYMAXFreio, 3); DUTYMAXFreio = Change_parameters_float(DUTYMAXFreio);
      if (DUTYMAXFreio > 1) {
        DUTYMAXFreio = 1;
      } else if (DUTYMAXFreio < 0.6) {
        DUTYMAXFreio = 0.6;
      }
      Serial.println("\r\nSalvo: DUTYMAXFreio = "); Serial.print(DUTYMAXFreio, 3);
    }

    else if (ident == 2) {
      Serial.println(DUTYMINFreio, 3); DUTYMINFreio = Change_parameters_float(DUTYMINFreio);
      if (DUTYMINFreio > 0.4) {
        DUTYMINFreio = 0.4;
      } else if (DUTYMINFreio < 0) {
        DUTYMINFreio = 0;
      }
      Serial.println("\r\nSalvo: DUTYMINFreio = "); Serial.print(DUTYMINFreio, 3);
    }

    else if (ident == 3) {
      Serial.println(DUTYMAXAcel, 3); DUTYMAXAcel = Change_parameters_float(DUTYMAXAcel);
      if (DUTYMAXAcel > 1) {
        DUTYMAXAcel = 1;
      } else if (DUTYMAXAcel < 0.6) {
        DUTYMAXAcel = 0.6;
      }
      Serial.println("\r\nSalvo: DUTYMAXAcel = "); Serial.print(DUTYMAXAcel, 3);
    }

    else if (ident == 4) {
      Serial.println(DUTYMINAcel, 3); DUTYMINAcel = Change_parameters_float(DUTYMINAcel);
      if (DUTYMINAcel > 0.4) {
        DUTYMINAcel = 0.4;
      } else if (DUTYMINAcel < 0) {
        DUTYMINAcel = 0;
      }
      Serial.println("\r\nSalvo: DUTYMINAcel = "); Serial.print(DUTYMINAcel, 3);
    }

  }

  else if (ident == 5 || ident == 6) {
    Serial.println("\r\nEscolha entre 0 e 100 (w -> UP, s-> DOWN, a-> LEFT, d-> RIGHT)");
    Serial.println("\r\nPressione Shift + S para salvar");

    if (ident == 5) {
      Serial.println(DUTYFREIO); DUTYFREIO = Change_parameters_int(DUTYFREIO);
      if (DUTYFREIO > 100) {
        DUTYFREIO = 100;
      } else if (DUTYFREIO < 0) {
        DUTYFREIO = 0;
      }
      Serial.println("\r\nSalvo: PWM_FREIO = "); Serial.print(DUTYFREIO);
    }
    else if (ident == 6) {
      Serial.println(DUTYACEL); DUTYACEL = Change_parameters_int(DUTYACEL);
      if (DUTYACEL > 100) {
        DUTYACEL = 100;
      } else if (DUTYACEL < 0) {
        DUTYACEL = 0;
      }
      Serial.println("\r\nSalvo: PWM_ACEL = "); Serial.print(DUTYACEL);
    }
  }

  else if (ident == 7) {
    Serial.println("Padrao = 1 [h].");
    float TIME = (float)TimeOut / (float)3600;
    Serial.println("\n\rValor atual: ");
    Serial.print(TIME);
    Serial.println("\r\nDigite um valor entre 0.01 e 100 [horas] no formato X.XX");
    TimeOut = Change_parameters_simple(0, TimeOut);
    Serial.println("\r\nValor salvo:");
    Serial.print((float)(TimeOut / (float)3600));
  }

  else if (ident == 8) {
    Menu_NET();
  }

}

/**------------------------------------------------------------------------------------
  * @brief Preenche buffer a ser salvo na memoria flash (com CRC).
  * 
  * @param none
  * 
  * @retval String BUFFER_EEPROM
  */
String BufferToSave(){

  String BUFFER_EEPROM = "";
  BUFFER_EEPROM += String(DUTYMAXFreio,3);
  BUFFER_EEPROM += ",";
  BUFFER_EEPROM += String(DUTYMINFreio,3);
  BUFFER_EEPROM += ",";
  BUFFER_EEPROM += String(DUTYMAXAcel,3);
  BUFFER_EEPROM += ",";
  BUFFER_EEPROM += String(DUTYMINFreio,3);
  BUFFER_EEPROM += ",";
  BUFFER_EEPROM += (String)DUTYFREIO;
  BUFFER_EEPROM += ",";
  BUFFER_EEPROM += (String)DUTYACEL;
  BUFFER_EEPROM += ",";
  float auxerase = (float)TimeOut/3600;
  BUFFER_EEPROM += (String)auxerase;
  BUFFER_EEPROM += ",";
  BUFFER_EEPROM += LOGIN;
  BUFFER_EEPROM += ",";
  BUFFER_EEPROM += SENHA;
  BUFFER_EEPROM += ",";
  BUFFER_EEPROM += (String)IP[0];
  BUFFER_EEPROM += ".";
  BUFFER_EEPROM += (String)IP[1];
  BUFFER_EEPROM += ".";
  BUFFER_EEPROM += (String)IP[2];
  BUFFER_EEPROM += ".";
  BUFFER_EEPROM += (String)IP[3];
  BUFFER_EEPROM += ",";
  BUFFER_EEPROM += (String)GATE[0];
  BUFFER_EEPROM += ".";
  BUFFER_EEPROM += (String)GATE[1];
  BUFFER_EEPROM += ".";
  BUFFER_EEPROM += (String)GATE[2];
  BUFFER_EEPROM += ".";
  BUFFER_EEPROM += (String)GATE[3];
  BUFFER_EEPROM += ",";
  BUFFER_EEPROM += (String)SUBMASK[0];
  BUFFER_EEPROM += ".";
  BUFFER_EEPROM += (String)SUBMASK[1];
  BUFFER_EEPROM += ".";
  BUFFER_EEPROM += (String)SUBMASK[2];
  BUFFER_EEPROM += ".";
  BUFFER_EEPROM += (String)SUBMASK[3];
  BUFFER_EEPROM += ",";
  BUFFER_EEPROM += ID;

  String CRC_put = CRC(getBitsFromString((char*) BUFFER_EEPROM.c_str()));
  uint8_t CRC_put_int = strbin_to_dec(CRC_put);
  String CRC_put_STR = String(CRC_put_int, HEX);
  CRC_put_STR.toUpperCase();

  BUFFER_EEPROM += "*";
  BUFFER_EEPROM += CRC_put_STR;

  return BUFFER_EEPROM;
}