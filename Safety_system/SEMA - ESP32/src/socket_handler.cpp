#include <socket_handler.hpp>

///////////////////////////////////////////////////////////////////////////////////////
/* ///////////// WebSocket Implementation to the remote operator /////////////////////*/

/**------------------------------------------------------------------------------------
  * @brief Envia dados a todos os clientes inscritos no socket (front-end).
  *
  * @param none
  * 
  * @retval void
  */
void SendClients()
{
  // Funcao para enviar feedback aos clientes.
  ws.textAll(String(relay));
}

/**------------------------------------------------------------------------------------
  * @brief Callback ao receber mensagens pelo socket (porta 80).
  *
  * @OBS  Envia os feedbacks para os clientes e atualiza as paginas.
  * 
  * @param *arg:    Argumento
  * @param *data:   Mensagem
  * @param len:     Tamanho da mensagem
  * 
  * @retval void
  */
void handleSocketMessage(void *arg, uint8_t *data, size_t len)
{
  // Callback ao receber mensagens dos clientes.
  AwsFrameInfo *info = (AwsFrameInfo *)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT)
  {

    data[len] = 0;
    if (strcmp((char *)data, "ON") == 0)
    {
      relay = 1;
      Acionamento(relay);
      SendClients();
      timestamp();
      Serial.println("\nFreio Acionado!");
      AddtoReport("Freio Acionado!", false);
    }

    else if (strcmp((char *)data, "OFF") == 0)
    {
      relay = 0;
      Acionamento(relay);
      SendClients();
      timestamp();
      Serial.println("\nFreio desarmado.");
      AddtoReport("Freio desarmado.", false);
    }
  }
}

/**------------------------------------------------------------------------------------
  * @brief Callback para qualquer evento socket (porta 80).
  *
  * @OBS  WS_EVT_CONNECT:     Conexao de novo cliente.
  *       WS_EVT_DISCONNECT:  Desconexao de cliente.
  *       WS_EVT_DATA:        Dados transmitidos.
  *       WS_EVT_PONG:        Resposta para PING.
  *       WS_EVT_ERROR:       Erro ocorrido.
  * 
  * @param *server:    Servidor da classe AsyncWebSocket.
  * @param *client:    Cliente da classe AsyncWebSocketClient.
  * @param type:       Tipo do evento, da classe AwsEventType.
  * @param *arg:       Argumento.
  * @param *data:      Dados do evento.
  * @param len:        Tamanho dos dados.
  * 
  * @retval void
  */
void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
             void *arg, uint8_t *data, size_t len)
{
  // FUNCAO HANDLE WEB SOCKET EVENT - SERVIDOR
  switch (type)
  {
  case WS_EVT_CONNECT: // Quando novo cliente loga
    timestamp();
    Serial.printf("\n\rCliente %u conectado do IP %s", client->id(), client->remoteIP().toString().c_str());
    AddtoReport("Cliente " + (String) client->id() + " conectado do IP: " + client->remoteIP().toString(), false);
    Serial.println();
    cont_clientes++;
    SendClients();
    // VERIFICAR SE O PRIMEIRO CLIENTE CONECTOU
    if (cont_clientes == 1)
    { /* executar acao */
      digitalWrite(ESP_IN_CLIENTE, HIGH);
    }
    break;
  case WS_EVT_DISCONNECT: // quando um cliente desloga
    timestamp();
    Serial.printf("\n\rCliente %u desconectado", client->id());
    AddtoReport("Cliente " + (String) client->id() + " desconectado.", false);
    Serial.println();
    cont_clientes--;
    // VERIFICAR SE NAO HA CLIENTES OU SE A CONEXAO FOI PERDIDA
    if (cont_clientes == 0)
    { /*executar acao*/
      digitalWrite(ESP_IN_CLIENTE, LOW);
      Serial.println("Clientes desconectados...");
      AddtoReport("Clientes desconectados...", false);
      Serial.println();
    }
    break;
  case WS_EVT_DATA: // quando dados sao recebidos do cliente
    handleSocketMessage(arg, data, len);
    break;
  case WS_EVT_PONG: // resposta do cliente para um request ping
    break;
  case WS_EVT_ERROR: // evento erro
    break;
  }
}

/**------------------------------------------------------------------------------------
  * @brief Inicializacao do Socket e da funcao callback onEvent.
  * 
  * @param none
  * 
  * @retval void
  */
void initSocket()
{
  // INICIALIZACAO DO SOCKET
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}

/**------------------------------------------------------------------------------------
  * @brief Processor, handler to front-end pages.
  *
  * @OBS  STATE in between '%' characters
  * 
  * @param var:    String text in page format (HTML)
  * 
  * @retval String STOP/DESARMAR
  */
String Substituir(const String &var)
{
  // FUNCAO PARA DAR FEEDBACK DA ACAO E ATUALIZAR PAGINAS 
  // DE TODOS OS CLIENTES DENTRO DA MESMA COMUNICACAO
  // INFORMAR OS OUTROS CLIENTES CASO OCORRA ALGUM EVENTO
  String ans_return = "";
  if (var == "STATE")
  {
    ans_return = relay ? "DESARMAR" : "STOP";
  }
  if (var == "ReportCont")
  {
    ans_return = Buffer_report == ""? "SEM NOTIFICACOES": Buffer_report;
  }

  return ans_return;
}
