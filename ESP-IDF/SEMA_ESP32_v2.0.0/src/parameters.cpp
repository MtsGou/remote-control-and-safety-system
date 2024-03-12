#include "parameters.h"

/* ================================= FUNCOES EXTRAS ======================================*/

double power(double a, double b)
{
    double result = 1;
    for(int i = 0; i < b; i++)
    {
        result *= a;
    }
    return result;
}

/* ======================= ELEMENTOS PARA INTERACAO E DEPURACAO ===========================*/

uint8_t *BUFFER_RX_SERIAL;

/* ======================= VARIAVEIS PARA GERACAO DOS SINAIS PWM ==========================*/

const uint32_t size_bits = power(2, RESOLUTION_PWM);

// DUTY CYCLE DO FREIO E DO ACELERADOR

// FREIO
volatile float DUTYCYCLE_MAX_EMG = 0.85;     // limite maximo Duty Cycle Freio
volatile float DUTYCYCLE_MIN_EMG = 0.174;    // limite minimo Duty Cycle Freio
volatile uint8_t PERCENT_EMG = 100;          // % do duty cycle dentro dos limites
volatile float FINAL_DUTYCYCLE_EMG;          // Duty cycle efetivo
volatile uint32_t PWM_EMG;                   // PWM efetivo para funcao ledcwrite

// ACELERADOR
volatile float DUTYCYCLE_MAX_ACEL = 0.848;   // limite maximo Duty Cycle Acel
volatile float DUTYCYCLE_MIN_ACEL = 0.158;   // limite minimo Duty Cycle Freio
volatile uint8_t PERCENT_ACEL = 0;           // % do dc dentro nos limites
volatile float FINAL_DUTYCYCLE_ACEL;         // Duty cycle efetivo
volatile uint32_t PWM_ACEL;                  // PWM efetivo para funcao ledcwrite


/* ================================= DADOS DE CONEXAO ====================================*/

// PREENCHA COM O LOGIN DA REDE
string LOGIN = "USR-WIFI232-610_3808";

// PREENCHA COM A SENHA DA REDE
string SENHA = "";

// ESPECIFICACOES REDE
uint8_t IP[4] = {10, 0, 20, 51};
uint8_t GATE[4] = {10, 0, 20, 23};
uint8_t SUBMASK[4] = {255, 255, 255, 0};

/* ================================= DADOS DE PROTOCOLO ==================================*/

// IDENTIFICACAO DO CAMINHAO
string ID = "CR2100";
// Divisoria para capturar Checksum
string CHECKSUM_DIV = "*";
// Start byte
string START_BYTE = "@";
// End byte
string END_BYTE = "?";

// =============================== VARIAVEIS DE SISTEMA ==================================*/

// VARIAVEL DE ACIONAMENTO DO RELE
volatile bool ESTADO_RELE = OFF;

// CONTADOR DE CLIENTES CONECTADOS
int NUM_CLIENTES_CONECTADOS = 0;

// CONTADOR DE TEMPO DE RECONEXAO APOS PERDER CONEXAO
int COUNT_RECONNECT = 0;

// TEMPO TIMEOUT DE RECONEXAO
//(SE PERDER CONEXAO, O SISTEMA TENTA RECONECTAR DURANTE ALGUM TEMPO.
// SE SUPERAR TIMEOUT, A PLACA DEVE SER RESETADA PARA RECONECTAR.)
// Em segundos (SELECIONE O DESEJADO). 3600s = 1h; 86400s = 1d
unsigned int long TIMEOUT_NETWORK_FAIL = 3600;

// CASO A CONEXAO FALHE E NAO CONSIGA RECONECTAR APOS TIMEOUT
bool ABORT_RECONNECT = false;

// VERIFICACAO DOS SINAIS QUE VEM DO STM, UPA e CPU
unsigned int long TIME_LAST_SWITCH_ALIVE_STM = 0;
bool STM_OK = true;

unsigned int long TIME_LAST_SWITCH_ALIVE_CPU = 0;
bool CPU_OK = true;

unsigned int long TIME_LAST_SWITCH_ALIVE_UPA = 0;
bool UPA_OK =  true;

// VARIAVEL PARA TOGGLE PIN STATUS DO PROPRIO ESP32
bool TOGGLE_STATUS = LOW;

// HISTORICO DE OCORRENCIA DE FALHA NO STM
bool HISTORICO_FALHA_STM = false;

// HISTORICO DE ACIONAMENTO PELO CPU
bool HISTORICO_CPU_ORDEM_FREIO = false;

// HISTORICO DE FEEDBACK STM
bool HISTORICO_STM_FEEDBACK =  false;

// Buffer para o relatorio 
string BUFFER_REPORT = "REPORT DATA: <br><br>";

/* ====================================================================================*/
