#include <wifi_functions.h>
/**------------------------------------------------------------------------------------
  * @brief Conexao a WiFi nas configuracoes de IP, gateway e Submask.
  *
  * @OBS  Enquanto tenta conectar, o sistema deve funcionar normalmente.
  *
  * @param none
  * 
  * @retval void
  */
void Wifi_connect(){

  // TORNAR IP DA PLACA FIXO - ESCOLHER IP
  IPAddress local_IP(IP[0], IP[1], IP[2], IP[3]);

  // IP DA REDE EM USO - SELECIONAR
  IPAddress gateway(GATE[0], GATE[1], GATE[2], GATE[3]);

  // MASCARA DE REDE
  IPAddress subnet(SUBMASK[0], SUBMASK[1], SUBMASK[2], SUBMASK[3]);
  
  Serial.println("Configurando IP fixo da placa...");

  if (!WiFi.config(local_IP, gateway, subnet))
  {
    timestamp();
    AddtoReport("Falhou na configuracao de IP fixo.", false);
    Serial.println("Falhou na configuracao de IP fixo.");
  }

  if (!(SENHA.length() ^ 0)){ // without password
    WiFi.begin((const char *) LOGIN.c_str());
  }
  else { // with password
    WiFi.begin((const char *) LOGIN.c_str(), (const char *)SENHA.c_str());
  }

  Serial.println();
  delay(400);
  Serial.println("Conectando a ");
  Serial.print(LOGIN);
  while (WiFi.status() != WL_CONNECTED)
  {
    for (int i = 0; i < 5; i++){
        delay(200);
        PWM_STATUS_Toggle(); // continue status
    }
    // continue system main loop functions
    DETECCAO_FALHA();
    CPU_Handler();
    STM_Feedback_Handler();
    // trying to stablish connection...
    Serial.print(".");
  }
  // connected
  digitalWrite(CONNECT_ESP_IN, HIGH);
  SHOW_DADOS_CONEXAO();
}

/**------------------------------------------------------------------------------------
  * @brief Tentar reconectar a WiFi apos perder conexao.
  *
  * @OBS  Enquanto tenta reconectar, o sistema deve funcionar normalmente.
  *
  * @param none
  * 
  * @retval void
  */
void Reconnect_try(){
    // not connected
    digitalWrite(CONNECT_ESP_IN, LOW);
    Salvar_Dados_Perdidos_Flash();
    cont_reconnect = 0;
    // CONEXAO A WIFI
    if (!(SENHA.length() ^ 0)){ //without password
      WiFi.begin((const char *) LOGIN.c_str());
    }
    else { // with password
      WiFi.begin((const char *) LOGIN.c_str(), (const char *)SENHA.c_str());
    }
    Serial.println();
    timestamp();
    Serial.println("Conexao perdida. Tentando reconectar...");
    AddtoReport("Conexao perdida. Tentando reconectar...", false);
    delay(1000);
    Serial.println("Conectando a ");
    Serial.print(LOGIN);
    while (WiFi.status() != WL_CONNECTED)
    {
      for (int i = 0; i < 5; i++){
        delay(200);
        PWM_STATUS_Toggle(); // continue status
      }

      // continue system main loop functions
      DETECCAO_FALHA();
      CPU_Handler();
      STM_Feedback_Handler();
      Serial.print(".");

      cont_reconnect++;

      // Se TimeOut for superado
      if (cont_reconnect > TimeOut)
      {
        Serial.println();
        timestamp();
        Serial.println("TimeOut ultrapassado. Sem conexao.");
        AddtoReport("TimeOut ultrapassado. Sem conexao.", false);
        Serial.println("Verifique a conexao. APERTE RESET OU DESLIGUE A PLACA E TENTE NOVAMENTE.");
        Disconnect = 1;
        break;
      }
    }

    // If connection reestablished
    // INFORMAR O TEMPO DESCONECTADO EM (hr , min, seg)
    if (cont_reconnect < TimeOut)
    {
      digitalWrite(CONNECT_ESP_IN, HIGH);
      Serial.println();
      timestamp();
      Serial.print(" Conexao recuperada.");
      AddtoReport("Conexao recuperada.", false);
      Serial.println("Tempo desconectado: ");

      if (cont_reconnect < 60)
      {
        Serial.print(cont_reconnect);
        Serial.print(" segundos. ");
      }
      else if (cont_reconnect >= 60 && cont_reconnect < 3600)
      {
        Serial.print(cont_reconnect / 60);
        Serial.print("m e ");
        Serial.print(cont_reconnect % 60);
        Serial.print("s. ");
      }
      else if (cont_reconnect >= 3600)
      {
        int horas = cont_reconnect / 3600;
        int resto = cont_reconnect % 3600;
        Serial.print(horas);
        Serial.print("h ");
        Serial.print(resto / 60);
        Serial.print("m e ");
        Serial.print(resto % 60);
        Serial.print("s. ");
      }
      SHOW_DADOS_CONEXAO();

      ws.cleanupClients();
    }
}

/**------------------------------------------------------------------------------------
  * @brief Mostrar dados da conexao apos conectar.
  *
  * @param none
  * 
  * @retval void
  */
void SHOW_DADOS_CONEXAO()
{
  // Printar dados da conexao
  timestamp();
  AddtoReport("Conectado a rede", false);
  Serial.println();
  Serial.println("IP DA PLACA/ENDERECO MAC: ");
  Serial.println(WiFi.localIP());
  Serial.println(WiFi.macAddress());
  Serial.println("HTTP/1.1 200 OK");
  Serial.println("Content-type:text/html");
  Serial.println("Connection: close");
  Serial.println();
}
