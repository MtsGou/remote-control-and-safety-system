#include <data_transmission.hpp>
#include <user_menu.hpp>
#include <emergency_break.hpp>

///////////////////////////////////////////////////////////////////////////////////////
/* ////////////////// FUNCOES PARA GERACAO DE RELATORIO /////////////////////////////*/

/**------------------------------------------------------------------------------------
  * @brief Adicionar notificacoes ao relatorio.
  *
  * @param time:  Aplicar 'true' se os dados sao de timestamp.
  * @param data:  String de dados.
  * 
  * @retval void.
  */
void AddtoReport(String data, bool time){
  Buffer_report += data;
  if (!time){
    Buffer_report += "<br>";
    ws.textAll(Buffer_report + "</p>");
  }
  
  if (Buffer_report.length() >= 4000){
    ws.textAll("DOWNLOAD REPORT");
    //Buffer_report.clear();
    Buffer_report = "";
    Buffer_report = "REPORT DATA: ";
    if (cont_clientes <= 1){
      Buffer_report += "Perda de dados anteriores. Houve overflow do buffer.<br>";
      Buffer_report += "Muitas ocorrencias.<br>";
    }
  }

}

/**------------------------------------------------------------------------------------
  * @brief Salvar na memoria flash dados perdidos do relatorio apos perda da conexao.
  *
  * @retval void
  */
void Salvar_Dados_Perdidos_Flash(){

  /* Buffer dados perdidos apos desconexao para salvar na flash */
  String Buffer_lost_data_discnct = "$";
  Buffer_lost_data_discnct += Buffer_report;
  Buffer_lost_data_discnct.replace("<br>"," | ");
  EEPROM.writeString(address_EEPROM, BufferToSave() + Buffer_lost_data_discnct);
  EEPROM.commit();

}

///////////////////////////////////////////////////////////////////////////////////////
/* ////////////////// FUNCOES DE TRANSMISSAO DE DADOS ///////////////////////////////*/

/**------------------------------------------------------------------------------------
  * @brief Calcular checksum 8 bits dos dados.
  *
  * @OBS Retorna valor hexadecimal em Upper Case.
  *
  * @param BUFFER:  String de dados.
  * 
  * @retval String checksum.
  */
String Checksum(String BUFFER)
{
  int checksum_arrived = 0;
  // CALCULO DO CHECKSUM
  for (int index = 0; index < BUFFER.length(); index++)
  {
    checksum_arrived = checksum_arrived ^ BUFFER[index];
  }
  String checksum = String(checksum_arrived, HEX);
  checksum.toUpperCase();
  return checksum;
}

/**------------------------------------------------------------------------------------
  * @brief Captura dados entre divisorias ou separadores.
  *
  * @OBS separador deve ser do tipo char.
  *
  * @param data:        String de dados.
  * @param separator:   Separador.
  * @param index:       Indice do dado que se deseja capturar.
  * 
  * @retval String dado capturado.
  */
String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++)
  {
    if (data.charAt(i) == separator || i == maxIndex)
    {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }

  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

/**------------------------------------------------------------------------------------
  * @brief Verifica a mensagem que chega por TCP do despacho.
  *
  * @OBS Verifica o ID alem do Checksum, e envia feedback aos clientes do front-end
  *      se a mensagem for validada. 
  *
  * @param BUFFER_RECEIVE:    String de dados.
  * 
  * @retval String BUFFER_FEEDBACK.
  */
String Verify_TCP_message(String BUFFER_RECEIVE){
  String BUFFER_FEEDBACK = "";
  String Checksum_RECEBIDO = BUFFER_RECEIVE.substring(BUFFER_RECEIVE.indexOf(CHECKSUM_DIV));
  Checksum_RECEBIDO.remove(0, 1);
  BUFFER_RECEIVE = BUFFER_RECEIVE.substring(0, BUFFER_RECEIVE.indexOf(CHECKSUM_DIV));
  if (Checksum_RECEBIDO.equals(Checksum(BUFFER_RECEIVE)) && getValue(BUFFER_RECEIVE, ',', 0).equals(ID)){
    if (getValue(BUFFER_RECEIVE, ',', 2).startsWith("1")){
      relay = 1;
    }
    else if (getValue(BUFFER_RECEIVE, ',', 2).startsWith("0")){
      relay = 0;
    }

    Acionamento(relay);
    SendClients();
    timestamp();
    Serial.println("Comando executado pelo despacho.");
    AddtoReport("Comando executado pelo despacho.", false);
        
    // Feedback
    BUFFER_FEEDBACK += getValue(BUFFER_RECEIVE, ',', 0);
    BUFFER_FEEDBACK += ',' + getValue(BUFFER_RECEIVE, ',', 1);
    BUFFER_FEEDBACK += ',';
    BUFFER_FEEDBACK += "OK,";
    BUFFER_FEEDBACK += String(relay);
    String checksum_feedback_TCP = Checksum(BUFFER_FEEDBACK);
    BUFFER_FEEDBACK += CHECKSUM_DIV;
    BUFFER_FEEDBACK += checksum_feedback_TCP;
  }
  return BUFFER_FEEDBACK;
}

/**------------------------------------------------------------------------------------
  * @brief CRC 8 bits da mensagem em string de bits.
  *
  * @OBS CRC polinomio divisor [100110001]. Retorna resultado CRC em hexadecimal.
  *
  * @param BitString: String de bits da mensagem.
  * 
  * @retval String Res
  */
String CRC(String BitString){
   String Res; // Armazena resultado CRC
   char CRC[8];
   int  i;
   char DoInvert;
   char *getbitstring = (char *) BitString.c_str();
   char res[9];
   
   for (i=0; i<8; ++i)  CRC[i] = 0;
   
   for (i=0; i<strlen(getbitstring); ++i)
      {
      DoInvert = ('1'==getbitstring[i]) ^ CRC[7]; // fazer operacao XOR

      CRC[7] = CRC[6];
      CRC[6] = CRC[5];
      CRC[5] = CRC[4] ^ DoInvert;
      CRC[4] = CRC[3] ^ DoInvert;
      CRC[3] = CRC[2];
      CRC[2] = CRC[1];
      CRC[1] = CRC[0];
      CRC[0] = DoInvert;
      }
      
   for (i=0; i<8; ++i)  res[7-i] = CRC[i] ? '1' : '0'; // converter binario para ASCII
   res[8] = 0; // fim da string

   Res = String(res);
   return Res;
}

/**------------------------------------------------------------------------------------
  * @brief Recebe uma string em binario e retorna o inteiro correspondente.
  *
  * @param str: String de bits da mensagem.
  * 
  * @retval int result
  */
int strbin_to_dec(String str) {
    uint result = 0;
    for (int i = str.length() - 1, j = 0; i >= 0; i--, j++) {
        byte k = str[i] - '0'; // we assume ASCII coding
        k <<= j;
        result += k;
    }
    return result;
}

/**------------------------------------------------------------------------------------
  * @brief Captura uma string tipo char* e retorna a string correspondente
  *  em binario (ASCII).
  *
  * @param bitString: String de dados ASCII.
  * 
  * @retval String Resultado
  */
String getBitsFromString(char* bitString){

    String Resultado;

    char *ptr = bitString;
    int i;

    for(; *ptr != 0; ++ptr)
    {

        /* perform bitwise AND for every bit of the character */
        for(i = 7; i >= 0; --i) 
            (*ptr & 1 << i) ? Resultado += "1": Resultado += "0";
            

    }
    return Resultado;
}