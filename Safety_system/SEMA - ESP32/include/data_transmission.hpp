#include <sys_def.h>


///////////////////////////////////////////////////////////////////////////////////////
/* ////////////////// FUNCOES DE TRANSMISSAO DE DADOS ///////////////////////////////*/

void AddtoReport(String data, bool time);
void Salvar_Dados_Perdidos_Flash(void);
String Checksum(String BUFFER);
String getValue(String data, char separator, int index);
String Verify_TCP_message(String BUFFER_RECEIVE);
String CRC(String BitString);
int strbin_to_dec(String str);
String getBitsFromString(char* bitString);
