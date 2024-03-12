#include <sys_def.h>

///////////////////////////////////////////////////////////////////////////////////////
/* ///////////// WebSocket Implementation to the remote operator /////////////////////*/

void SendClients(void);
void handleSocketMessage(void *arg, uint8_t *data, size_t len);
void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
             void *arg, uint8_t *data, size_t len);
void initSocket(void);
String Substituir(const String &var);
