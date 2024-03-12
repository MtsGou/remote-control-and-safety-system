#include <TCP_server_handler.hpp>

///////////////////////////////////////////////////////////////////////////////////////
/* ////////////////////////// SERVIDOR TCP NA PORTA 8080 ////////////////////////////*/

/**------------------------------------------------------------------------------------
  * @brief Callback para lidar com novos clientes que conectam.
  *
  * @OBS static void para funcao restrita.
  *
  * @param  *arg:	      Argumento.
  * @param  *client:    Cliente da classe AsyncClient.
  *
  * @retval void
  */
static void handleNewClient(void *arg, AsyncClient *client)
{
  timestamp();
  Serial.printf("\n\rDespacho conectado no IP: %s", client->remoteIP().toString().c_str());

  AddtoReport("Despacho conectado no IP: " + client->remoteIP().toString(), false);
	
  // Funcoes callback para events
  client->onData(&handleData, NULL); // dados chegando
  client->onDisconnect(&handleDisconnect, NULL); // cliente desconectado
}

/**------------------------------------------------------------------------------------
  * @brief Callback para lidar com dados dos clientes.
  *
  * @OBS static void para funcao restrita.
  *
  * @param  *arg:	      Argumento.
  * @param  *client:    Cliente da classe AsyncClient.
  * @param  *data:      Dados.
  * @param  len:        Tamanho da mensagem
  * 
  * @retval void
  */
static void handleData(void *arg, AsyncClient *client, void *data, size_t len){
  uint8_t *dado = (uint8_t *)data;
  timestamp();
  Serial.println("\r\nDado recebido do despacho: ");
  Serial.write(dado, len);

  char * commingdata = (char *)dado;
  String BUFFER_TCP = (String) commingdata;

  AddtoReport("Dado recebido do despacho: " + BUFFER_TCP, false);

  BUFFER_TCP = BUFFER_TCP.substring(BUFFER_TCP.indexOf(start_byte), BUFFER_TCP.indexOf(end_byte));
  BUFFER_TCP.remove(0, 1);

  String msg_feedback = Verify_TCP_message(BUFFER_TCP);

  client->add(msg_feedback.c_str(), msg_feedback.length());
  client->send();
}

/**------------------------------------------------------------------------------------
  * @brief Callback para desconexao dos clientes.
  *
  * @OBS static void para funcao restrita.
  *
  * @param  *arg:	      Argumento.
  * @param  *client:    Cliente da classe AsyncClient.
  * 
  * @retval void
  */
static void handleDisconnect(void *arg, AsyncClient *client)
{
  timestamp();
  Serial.printf("\n\rDespacho desconectado");
  AddtoReport("Despacho desconectado", false);
}