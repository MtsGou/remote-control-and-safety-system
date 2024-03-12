#include <sys_def.h>

// BAUD RATE PARA COMUNICACAO SERIAL
const unsigned int long BAUD_RATE = 115200;

// BITS DO PWM PARA USAR A FUNCAO LEDCWRITE
const uint32_t size_bits = pow(2, RESOLUTION_PWM);

/* ================== DECLARACAO DE VARIAVEIS GLOBAIS ========================*/

// ################## VARIAVEIS PARA GERACAO DOS SINAIS PWM ###################

// DUTY CYCLE DO FREIO E DO ACELERADOR

// FREIO
volatile float DUTYMAXFreio = 0.85;   // limite maximo Duty Cycle Freio
volatile float DUTYMINFreio = 0.174;  // limite minimo Duty Cycle Freio
volatile uint32_t PWMFreio;           // PWM efetivo para funcao ledcwrite
volatile float DUTYCYCLEFreio;        // Duty cycle efetivo
volatile uint8_t DUTYFREIO = 100;     // % do duty cycle dentro dos limites

// ACELERADOR
volatile float DUTYMAXAcel = 0.848;   // limite maximo Duty Cycle Acel
volatile float DUTYMINAcel = 0.158;   // limite minimo Duty Cycle Freio
volatile uint32_t PWMAcel;            // PWM efetivo para funcao ledcwrite
volatile float DUTYCYCLEAcel;         // Duty cycle efetivo
volatile uint8_t DUTYACEL = 0;        // % do dc dentro nos limites


// ########################## DADOS DA CONEXAO ################################

// PREENCHA COM O LOGIN DA REDE
String LOGIN = "NETWORK_SSID";

// PREENCHA COM A SENHA DA REDE
String SENHA = "NETWORK_PASSWORD";

// ESPECIFICACOES REDE
uint8_t IP[4] = {10, 0, 20, 51};
uint8_t GATE[4] = {10, 0, 20, 23};
uint8_t SUBMASK[4] = {255, 255, 255, 0};

// ########################### DADOS DE PROTOCOLO #############################

// IDENTIFICACAO DO CAMINHAO
String ID = "CR2100";
// Divisoria para capturar Checksum
String CHECKSUM_DIV = "*";
// Start byte
String start_byte = "@";
// End byte
String end_byte = "?";

// ########################## VARIAVEIS DO SISTEMA ############################

// VARIAVEL DE ACIONAMENTO DO RELE
volatile bool relay = 0;

// CONTADOR DE CLIENTES CONECTADOS
int cont_clientes = 0;

// CONTADOR DE TEMPO DE RECONEXAO APOS PERDER CONEXAO
int cont_reconnect = 0;

// TEMPO TIMEOUT DE RECONEXAO
//(SE PERDER CONEXAO, O SISTEMA TENTA RECONECTAR DURANTE ALGUM TEMPO.
// SE SUPERAR TimeOut, A PLACA DEVE SER RESETADA PARA RECONECTAR.)
// Em segundos (SELECIONE O DESEJADO). 3600s = 1h; 86400s = 1d
unsigned int long TimeOut = 3600;

// CASO A CONEXAO FALHE E NAO CONSIGA RECONECTAR APOS TimeOut, Disconnect = 1.
bool Disconnect = 0;

// VERIFICACAO DOS SINAIS QUE VEM DO STM, UPA e CPU
unsigned int long PERIODO_PWM_STATUS_STM = 0;
bool VERIFY_STM = 1; // STM OK - debug

unsigned int long PERIODO_PWM_STATUS_CPU = 0;
bool VERIFY_CPU = 1; // CPU OK - debug

unsigned int long PERIODO_PWM_STATUS_UPA = 0;
bool VERIFY_UPA = 1; // UPA OK - debug

// VARIAVEL PARA TOGGLE PIN STATUS DO PROPRIO ESP32
bool TogglePin_STATUS = 0;

// NOTIFICACAO DE OCORRENCIA DE FALHA NO STM
bool OCORRENCIA_FALHA_STM = 0;

// HISTORICO DE ACIONAMENTO PELO CPU
bool CPU_BREAK_ACT = 0;

// HISTORICO DE FEEDBACK STM
bool STM_FEEDBACK = 0;

String Buffer_report = "REPORT DATA: <br><br>";

char empty[EEPROM_SIZE];